#include	"MsgPetInfo.h"
#include	"Pet.h"
#include	"GamePlayerSet.h"
#include	"Hero.h"

CMsgPetInfo::CMsgPetInfo()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;

}

CMsgPetInfo::~CMsgPetInfo()
{

}

BOOL CMsgPetInfo::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;

	if(_MSG_PETINFO != this->GetType())
		return false;

	return true;
}
void CMsgPetInfo::Process(void* pInfo)
{
	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->id);
	if(pPlayer)
		return;

	CPet* pPet = CPet::CreateNewPet(m_pInfo->id, m_pInfo->nAIType);
	pPet->SetLook(m_pInfo->dwLookFace);
	pPet->SetRoleType(_ROLE_CALL_PET);
	pPet->Create(m_pInfo->dwLookFace, _ROLE_VIEW_ROLE);
	CMyPos posPet;
	posPet.x = m_pInfo->usPosX;
	posPet.y = m_pInfo->usPosY;

	pPet->SetPos(posPet);
	pPet->SetAlignPos(posPet.x, posPet.y);

	g_objHero.AddPet(m_pInfo->id);
	g_objPlayerSet.AddPlayer((CPlayer*)pPet);

	pPet->SetName(m_pInfo->szName);
}
