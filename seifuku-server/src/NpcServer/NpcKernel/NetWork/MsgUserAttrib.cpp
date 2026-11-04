#include "AllMsg.h"
#include "NpcWorld.h"

CMsgUserAttrib::CMsgUserAttrib()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgUserAttrib::~CMsgUserAttrib()
{

}

BOOL CMsgUserAttrib::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;

	if(_MSG_USERATTRIB != this->GetType())
		return false;

	return true;
}

void CMsgUserAttrib::Process(void* pSocket)
{
	IRole* pRole = UserManager()->QueryRole(m_pInfo->idUser);
	if(!pRole)
		return ;

	for(int i = 0; i < (int)m_pInfo->dwAttributeNum; i++)
	{
		switch(m_pInfo->setUserAttrib[i].ucAttributeType)
		{
		case	_USERATTRIB_LIFE:
			pRole->SetLife(m_pInfo->setUserAttrib[i].dwAttributeData);
			break;
		case	_USERATTRIB_MANA:
			pRole->SetPower(m_pInfo->setUserAttrib[i].dwAttributeData);
			break;
		case	_USERATTRIB_KEEPEFFECT:
			{
				pRole->SetEffect(m_pInfo->setUserAttrib[i].dwAttributeData);
			}
			break;
		case	_USERATTRIB_SIZEADD:
			pRole->SetSizeAdd(m_pInfo->setUserAttrib[i].dwAttributeData);
			break;
		}
	}
}
