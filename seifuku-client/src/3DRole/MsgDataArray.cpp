#include "MsgDataArray.h"
#include "Hero.h"

CMsgDataArray::CMsgDataArray()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgDataArray::~CMsgDataArray()
{

}

BOOL CMsgDataArray::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_DATAARRAY != this->GetType())
		return false;

	return true;
}

void CMsgDataArray::Process(void* pInfo)
{
	if (m_pInfo->ucAction == _DATAARRAY_ACTION_TABLE)
	{
		g_objHero.SetTableParam0(m_pInfo->dwLong[0]);
		g_objHero.AddTableRecord(m_pInfo->dwLong[0], m_pInfo->dwLong[3], m_pInfo->dwLong[1], _TABLERECORD_NONE, false);
		for (int i = 4; i < m_pInfo->ucAmount; i += 3)
			g_objHero.AddTableRecord(m_pInfo->dwLong[0], m_pInfo->dwLong[i+2], m_pInfo->dwLong[i+1], m_pInfo->dwLong[i], true);
		::PostCmd(CMD_NORMAL + 92, 1);
	}
}

BOOL CMsgDataArray::Create(DWORD dwData0, DWORD dwData1, DWORD dwData2, DWORD dwData3, DWORD dwData4)
{
	m_pInfo->ucAction	= 0;
	m_pInfo->ucAmount	= 5;
	m_pInfo->dwLong[0]	= dwData0;
	m_pInfo->dwLong[1]	= dwData1;
	m_pInfo->dwLong[2]	= dwData2;
	m_pInfo->dwLong[3]	= dwData3;
	m_pInfo->dwLong[4]	= dwData4;
	m_pInfo->unMsgType		= _MSG_DATAARRAY;
	m_pInfo->unMsgSize		= sizeof(MSG_Info)-sizeof(unsigned long) + 5*sizeof(unsigned long);
	return true;
}
