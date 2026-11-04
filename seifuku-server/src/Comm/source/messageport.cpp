#include "MessagePort.h"
#include "BaseFunc.h"
#include <stdlib.h>

using namespace message_port;

long	g_nMessagePortErrorCount;
bool CMessagePort::Open	()
{
	LOCKOBJ;

	m_nState = STATE_OK;
	for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
		m_setRecycle.push(*iter);
	m_setMsg.clear();
	::ResetEvent(m_hHaveMsg);

	return true;
}

bool CMessagePort::Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf)
{
	LOCKOBJ;

	MYASSERT(buf);
	if(!IsOpen())
		return false;

	if(nPort == INVALID_PORT)
		return true;

	if(nPort < 0 || nPort >= m_setPort.size())
	{
		MYASSERT(!"nPort");
		return false;
	}

	UNLOCKOBJ;

	return m_setPort[nPort]->PushMsg(m_id, nPacket, nVarType, buf);
}

bool CMessagePort::Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus* pStatus )
{
	LOCKOBJ;

	MYASSERT(buf);

	if(m_nState == STATE_CLOSED)
	{
		if(pStatus)
			pStatus->m_nError	= STATUS_FLAG_CLOSE;
		return false;
	}

	int	nRcvPort = nPort, nRcvPacket = nPacket;
	VAR_TYPE	nRcvVarType;
	if(PopMsg(&nRcvPort, &nRcvPacket, &nRcvVarType, buf))
	{
		if(SIZE_OF_TYPE(nRcvVarType) > SIZE_OF_TYPE(nVarType))
		{
			MYASSERT(!"VarType");
			if(pStatus)
				pStatus->m_nError		= STATUS_FLAG_ERROR;
			return false;
		}

		if(pStatus)
		{
			pStatus->m_nPortFrom	= nRcvPort;
			pStatus->m_nPacket		= nRcvPacket;
			pStatus->m_nVarType		= nRcvVarType;
			pStatus->m_nError		= STATUS_FLAG_OK;
		}
		return true;
	}

	if(pStatus)
		pStatus->m_nError	= STATUS_FLAG_OK;
	return false;
}

bool CMessagePort::WaitMsg(int nMilliSec)
{
	LOCKOBJ;

	if(m_nState == STATE_CLOSED)
		return false;

	if(m_setMsg.size())
		return true;
	::ResetEvent(m_hHaveMsg);

	UNLOCKOBJ;

	DWORD	ret = ::WaitForSingleObject(m_hHaveMsg, nMilliSec);
	if(ret == WAIT_OBJECT_0)
	{
		return true;
	}
	if(ret == WAIT_ABANDONED)
	{
		LOCKOBJ;
		Close();
		return false;
	}
	else
	{
		return false;
	}
}

bool CMessagePort::Close()
{
	LOCKOBJ;

	m_nState = STATE_CLOSED;

	return true;
}

int CMessagePort::SIZE_OF_TYPE		(int type)
{
	const int	fixlen_type_size = 10;
	const int	size_of[fixlen_type_size] = {1,1,2,2, 4,4,4,4, 4,8};
	if(type < VARTYPE_NONE)
		return type;
	else if(type == VARTYPE_NONE)
	{
		MYASSERT(!"SIZE_OF_TYPE");
		return 0;
	}
	else if(type - (VARTYPE_NONE+1) < fixlen_type_size)
		return size_of[type - (VARTYPE_NONE+1)];

	MYASSERT(!"SIZE_OF_TYPE.");
	return 0;
}

bool CMessagePort::PushMsg(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf)
{
	LOCKOBJ;

	MYASSERT(buf);
	if(m_nState != STATE_OK)
		return false;

	if(nPort < 0 || nPort >= m_setPort.size())
	{
		MYASSERT(!"PushMsg(nPort)");
		return false;
	}

	int	nSize = SIZE_OF_TYPE(nVarType);
	if(nSize == 0 || nSize > MAX_MSGPACKSIZE)
	{
		MYASSERT(!"PushMsg(nSize)");
		return false;
	}

	CMessagePacket*	pMsg = m_setRecycle.pop();
	if(!pMsg)
		pMsg = new CMessagePacket;
	if(!pMsg)
	{
		MYASSERT(!"new");
		return false;
	}
	pMsg->m_nPortFrom		= nPort;
	pMsg->m_nPacket			= nPacket;
	pMsg->m_nVarType		= nVarType;
	memcpy(pMsg->m_bufData, buf, nSize);

	m_setMsg.push_back(pMsg);
	::SetEvent(m_hHaveMsg);

	int nSetSize = m_setMsg.size();
	if(nSetSize >= 5000 && (nSetSize%1000) == 0 && nSetSize != m_debugPackets)
	{
		m_debugPackets = nSetSize;
		LOGERROR("CMessagePacket[%d] node count reached [%u]. [%d][%d][%s]", m_id, nSetSize, nPort, nPacket, (LPCTSTR)DumpBuffer((char*)buf, __min(16,nSize)));
		InterlockedExchange(&g_nMessagePortErrorCount, nSetSize);
	}

	if (::RandGet(300) == 123)
		InterlockedExchange(&g_nMessagePortErrorCount, nSetSize);

	if(m_setMsg.size() > 100000)
	{
		LOGERROR("ERROR: message heap overflow, MessagePort[%d] closed!", m_id);
		Close();
	}

	return m_setMsg.size() <= 1000;
}

bool CMessagePort::PopMsg(int *pPort, int *pPacket, VAR_TYPE *pVarType, void* buf)
{

	MYASSERT(pPort && pPacket && pVarType && buf);

	if(m_nState != STATE_OK)
		return false;

	if(m_setMsg.size() == 0)
		return false;

	for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
	{
		CMessagePacket* pMsg = *iter;
		if((*pPort == PORT_ANY || *pPort == pMsg->m_nPortFrom) && (*pPacket == PACKET_ANY || *pPacket == pMsg->m_nPacket))
		{
			*pPort		= pMsg->m_nPortFrom;
			*pPacket	= pMsg->m_nPacket;
			*pVarType	= (VAR_TYPE)pMsg->m_nVarType;

			int	nSize = SIZE_OF_TYPE(*pVarType);
			if(nSize)
			{
				memcpy(buf, pMsg->m_bufData, nSize);
			}

			m_setRecycle.push(pMsg);
			m_setMsg.erase(iter);
			return true;
		}
	}

	return false;
}

bool CMessagePort::Init()
{
	if((m_hHaveMsg	= ::CreateEvent(NULL, false, false, NULL)) == NULL)
	{
		LOGERROR("Cannot create CMessagePort handle!");
		return false;
	}
	return true;
}

void CMessagePort::Clear()
{
	for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
		delete(*iter);
	m_setMsg.clear();

	while(m_setRecycle.size())
		delete(m_setRecycle.pop());

	if(m_hHaveMsg != NULL)
		::CloseHandle(m_hHaveMsg);
	m_hHaveMsg = NULL;
}

bool CMessagePort::InitPortSet(int nPortNum)
{
	MYASSERT(nPortNum >= 1);

	for(int i = 0; i < nPortNum; i++)
	{
		CMessagePort*	pPort = new CMessagePort(i);
		if(!pPort)
			return false;
		m_setPort.push_back(pPort);
		if(!pPort->Init())
			return false;
	}

	return true;
}

void CMessagePort::ClearPortSet()
{
	for(int i = 0; i < m_setPort.size(); i++)
	{
		delete(m_setPort[i]);
	}

	m_setPort.clear();
}

CMessagePort::PORT_SET CMessagePort::m_setPort;
CRITSECT CMessagePort::m_xSetCtrl;
