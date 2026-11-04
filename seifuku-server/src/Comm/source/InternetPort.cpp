#include "InternetPort.h"

using namespace message_port;

CInternetPort::CInternetPort(int nPort)
{
	m_nPortSize	= 0;

	m_id		= nPort;
	m_nState	= STATE_CLOSED;
	m_sockMain	= INVALID_SOCKET;
	m_szKey[0]	= 0;
	m_bWinStartup	= false;
}

bool CInternetPort::Open	()
{
	if(IsOpen())
		return true;

	if(m_id == 0)
	{
		m_nState = STATE_CLOSED;
		m_sockMain = CreateServerSocket();
		if(m_sockMain == INVALID_SOCKET)
			return false;
	}
	else
	{
		if(m_nState != STATE_OPENING)
		{
			m_nState = STATE_CLOSED;
			m_sockMain = CreateClientSocket();
			if(m_sockMain == INVALID_SOCKET)
				return false;

			m_nState = STATE_OPENING;
		}

		timeval timeout;
		timeout.tv_sec		= CONNECT_OVERTIMESECS/1000;
		timeout.tv_usec		= (CONNECT_OVERTIMESECS%1000) * 1000;
		fd_set readset, writeset, exceptset;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&exceptset);
		FD_SET(m_sockMain, &readset);
		FD_SET(m_sockMain, &writeset);
		FD_SET(m_sockMain, &exceptset);

		int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
		if (ret == 0)
			return false;
		else if(ret < 0 || FD_ISSET(m_sockMain, &exceptset))
		{
			Close();
			return false;
		}

		for(int i = 0; i < m_nPortSize; i++)
			m_setSocket[i] = m_sockMain;

		m_nState	= STATE_OK;
		Send(MASTER_PORT_ID, SYS_PACKET_LOGINKEY, STRING_TYPE(m_szKey), m_szKey);
		Send(MASTER_PORT_ID, SYS_PACKET_PORTID, VARTYPE_INT, &m_id);
	}

	m_nState	= STATE_OK;
	for(int i = 0; i < m_setRecvBuf.size(); i++)
		m_setRecvBuf[i].Reset();
	return true;
}

SOCKET CInternetPort::CreateServerSocket()
{
	SOCKET	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		LOGERROR("socket() failed[%d].", err);
		return INVALID_SOCKET;
	}

	int		optval = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("setsockopt(SO_REUSEADDR) failed[%d].", err);
		return INVALID_SOCKET;
	}

	optval = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("setsockopt(SO_KEEPALIVE) failed[%d].", err);
		return INVALID_SOCKET;
	}

	optval = SNDBUF_SIZE;
	if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("setsockopt(SO_SNDBUF) failed[%d].", err);
		return INVALID_SOCKET;
	}

	SOCKADDR_IN		sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= 0;
	sin.sin_port		= htons(m_nMasterPort);
	if(bind(sock, (LPSOCKADDR) &sin, sizeof(sin)))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("bind() failed[%d].", err);
		return INVALID_SOCKET;
	}

	unsigned long	a = 1;
	if(ioctlsocket(sock, FIONBIO, &a))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("ioctlsocket() failed[%d].", err);
		return INVALID_SOCKET;
	}

	if(listen(sock, SOMAXCONN))
	{
		int err = WSAGetLastError();
		closesocket(sock);
		LOGERROR("listen() failed[%d].", err);
		return INVALID_SOCKET;
	}

	return sock;
}

SOCKET CInternetPort::CreateClientSocket()
{
	SOCKET	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		LOGERROR("socket() failed[%d].", err);
		return INVALID_SOCKET;
	}

	int		optval=1;
	if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt(SO_KEEPALIVE) failed[%d].", err);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	optval = SNDBUF_SIZE;
	if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *) &optval, sizeof(optval)))
	{
		int err = WSAGetLastError();
		LOGERROR("setsockopt(SO_SNDBUF) failed[%d].", err);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	unsigned long	a = 1;
	if(ioctlsocket(sock, FIONBIO, &a))
	{
		int err = WSAGetLastError();
		LOGERROR("ioctlsocket() failed[%d].", err);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	UINT	nAddr = inet_addr(m_szMasterIP);
	if(nAddr == INADDR_NONE)
	{
		hostent	* hp;
		hp = gethostbyname(m_szMasterIP);
		if(hp == 0)
		{
			int	err = WSAGetLastError();
			LOGERROR("Invalid ip addr[%s], err[%d].", m_szMasterIP, err);
			closesocket(sock);
			return INVALID_SOCKET;
		}
		nAddr = inet_addr(hp->h_addr_list[0]);
	}

	sockaddr_in	addr_in;
	memset((void *)&addr_in, sizeof(addr_in), 0);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(m_nMasterPort);
	addr_in.sin_addr.s_addr = nAddr;
	if(connect(sock, (sockaddr *)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if(err != WSAEWOULDBLOCK)
		{
			LOGERROR("Connect() failed[%d].", err);
			closesocket(sock);
			return INVALID_SOCKET;
		}
		else
		{
			return sock;
		}
	}

	return sock;
}

bool CInternetPort::Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf)
{
	if(!IsOpen())
		return false;

	if(nPort == INVALID_PORT)
		return true;

	return SendFrom(m_id, nPort, nPacket, nVarType, buf);
}

bool CInternetPort::SendFrom(int nPortFrom, int nPort, int nPacket, VAR_TYPE nVarType, const void* buf)
{
	CHECKF(buf);

	if(nPort < 0 || nPort >= m_nPortSize)
	{
		MYASSERT(!"nPort");
		return false;
	}

	if(nPort == m_id)
	{
		PushMsg(m_id, nPacket, nVarType, buf);
		return true;
	}

	if(m_setSocket[nPort] == INVALID_SOCKET)
	{
		LOGERROR("port[%d] send(%d, %d, %d, %s) failed.", m_id, nPort, nPacket, nVarType, buf);
		return false;
	}

	int		nLen = MSG_HEAD_SIZE;
	CMessageHead	cMsgHead;
	char*	pBuf = (char *)&cMsgHead;
	if(m_id == MASTER_PORT_ID)
		cMsgHead.m_nPort	= nPortFrom;
	else
		cMsgHead.m_nPort	= nPort;
	cMsgHead.m_nPacket		= nPacket;
	cMsgHead.m_nVarType		= nVarType;

	int ret = send(m_setSocket[nPort], pBuf, nLen, 0);
	if(ret == nLen)
	{
		nLen = SIZE_OF_TYPE(nVarType);
		pBuf = (char *)buf;

		ret = send(m_setSocket[nPort], pBuf, nLen, 0);
		if(ret == nLen)
			return true;
	}

	int err = WSAGetLastError();
	if(ret > 0 || err == WSAEWOULDBLOCK)
		LOGERROR("port[%d] overflow.", m_id);
	else
		LOGERROR("port[%d] error[%d].", m_id, err);

	if(m_id == 0)
	{
		closesocket(m_setSocket[nPort]);
		m_setSocket[nPort]	= INVALID_SOCKET;
	}
	else
	{
		Close();
		Open();
	}

	return false;
}

bool CInternetPort::Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus* pStatus )
{
	CHECKF(buf);

	if(!IsOpen())
	{
		if(pStatus)
			pStatus->m_nError	= STATUS_FLAG_CLOSE;
		return false;
	}

	RecvMsg(0);

	int		nRcvPort = nPort, nRcvPacket = nPacket;
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
	{
		if(IsOpen())
			pStatus->m_nError	= STATUS_FLAG_OK;
		else
			pStatus->m_nError	= STATUS_FLAG_ERROR;
	}
	return false;
}

bool CInternetPort::RecvMsg(int nMilliSec)
{
	if(m_nState == STATE_CLOSED)
		return false;

	timeval timeout;
	timeout.tv_sec		= nMilliSec / 1000;
	timeout.tv_usec		= (nMilliSec%1000) * 1000;
	fd_set readset;
	FD_ZERO(&readset);
	if(m_sockMain != INVALID_SOCKET)
		FD_SET(m_sockMain, &readset);
	if(m_id == 0)
	{
		for(int i = 0; i < m_setSocket.size(); i++)
		{
			if(m_setSocket[i] != INVALID_SOCKET)
				FD_SET(m_setSocket[i], &readset);
		}
	}

	int ret = select(FD_SETSIZE, &readset, NULL, NULL, &timeout);
	if (ret > 0)
	{
		if(m_id == 0)
		{
			if(FD_ISSET(m_sockMain, &readset))
			{
				struct sockaddr_in	addr;
				memset(&addr, 0, sizeof(addr));
				int	   len = sizeof(addr);
				SOCKET newsock = accept(m_sockMain, (sockaddr *) &addr, (int *) &len);
				if(newsock != INVALID_SOCKET)
				{
					m_setSocket.push_back(newsock);
					m_setRecvBuf.push_back(CRecvBuffer());
				}
			}

			for(int i = m_setSocket.size() - 1; i >= 0; i--)
			{
				if(m_setSocket[i] == INVALID_SOCKET || !FD_ISSET(m_setSocket[i], &readset))
					continue;

				if(RecvToBuf(&m_setRecvBuf[i], m_setSocket[i]))
				{
					BufferToList(i);
				}
				else
				{
					closesocket(m_setSocket[i]);
					m_setSocket[i]	= INVALID_SOCKET;
					if(i >= m_nPortSize)
					{
						m_setSocket.erase(m_setSocket.begin() + i);
						m_setRecvBuf.erase(m_setRecvBuf.begin() + i);
					}
				}
			}
		}
		else
		{
			if(RecvToBuf(&m_setRecvBuf[0], m_sockMain))
			{
				BufferToList(0);
			}
			else
			{
				Close();
				Open();
			}
		}
	}

	return true;
}

bool CInternetPort::RecvToBuf(CRecvBuffer* pRcvBuf, SOCKET sock)
{
	char*	pBuf = pRcvBuf->GetTail();
	int		nLen = message_port::RECV_BUFFER_SIZE - pRcvBuf->GetLen();
	int	ret = recv(sock, pBuf, nLen, 0);
	if(ret > 0)
	{
		pRcvBuf->AddLen(ret);
		return true;
	}

	if(ret == 0)
	{
		LOGERROR("message: socket close.");
	}
	else if(ret < 0)
	{
		int err = WSAGetLastError();
		if(err == WSAEWOULDBLOCK)
			return true;
		LOGERROR("message: socket error[%d], recv() failed.", err);
	}

	return false;
}

bool CInternetPort::BufferToList(int idx)
{
	if(m_id == 0)
	{
		while(true)
		{
			if(m_setRecvBuf[idx].GetLen() < MSG_HEAD_SIZE)
				break;
			CMessageHead*	pHead = m_setRecvBuf[idx].GetHead();
			int nLen = SIZE_OF_TYPE(pHead->m_nVarType);
			if(m_setRecvBuf[idx].GetLen() < MSG_HEAD_SIZE + nLen)
				break;
			if(pHead->m_nPacket >= SYS_PACKET_BASE)
			{
				if(pHead->m_nPacket == SYS_PACKET_LOGINKEY)
				{
					LPCTSTR	pszKey = (LPCTSTR)pHead->m_bufData;
					IF_NOT(strlen(m_szKey) && strcmp(pszKey, m_szKey) == 0)
					{
						closesocket(m_setSocket[idx]);

					}
				}
				else if(pHead->m_nPacket == SYS_PACKET_PORTID)
				{
					IF_OK(pHead->m_nVarType == VARTYPE_INT)
					{
						int	nLoginPort = *((int*)pHead->m_bufData);

						IF_OK(nLoginPort > MASTER_PORT_ID && nLoginPort < m_nPortSize && idx >= m_nPortSize)
						{
							if(m_setSocket[nLoginPort] != INVALID_SOCKET)
							{
								closesocket(m_setSocket[nLoginPort]);
							}
							m_setSocket[nLoginPort]		= m_setSocket[idx];
							m_setRecvBuf[nLoginPort]	= m_setRecvBuf[idx];
							m_setSocket.erase(m_setSocket.begin() + idx);
							m_setRecvBuf.erase(m_setRecvBuf.begin() + idx);

							idx		= nLoginPort;
						}
					}
					SendFrom(m_id, idx, SYS_PACKET_PORTSIZE, VARTYPE_INT, &m_nPortSize);
				}
				else 
				{
					MYASSERT(!"Unknown system packet!");
				}
			}
			else
			{
				if(pHead->m_nPort != MASTER_PORT_ID)
				{
					IF_OK(idx < m_nPortSize)
						SendFrom(idx, pHead->m_nPort, pHead->m_nPacket, (VAR_TYPE)pHead->m_nVarType, pHead->m_bufData);
				}
				else
				{
					IF_OK(idx < m_nPortSize)
						PushMsg(idx, pHead->m_nPacket, (VAR_TYPE)pHead->m_nVarType, pHead->m_bufData);
				}
			}

			m_setRecvBuf[idx].DelMsg(MSG_HEAD_SIZE + nLen);

		}
	}
	else
	{
		while(true)
		{
			if(m_setRecvBuf[0].GetLen() < MSG_HEAD_SIZE)
				break;
			CMessageHead*	pHead = m_setRecvBuf[0].GetHead();
			int nLen = SIZE_OF_TYPE(pHead->m_nVarType);
			if(m_setRecvBuf[0].GetLen() < MSG_HEAD_SIZE + nLen)
				break;

			if(pHead->m_nPacket == SYS_PACKET_PORTSIZE)
			{
				IF_OK(pHead->m_nVarType == VARTYPE_INT)
				{
					int	nPortSize = *((int*)pHead->m_bufData);
					IF_OK(idx == MASTER_PORT_ID)
					{
						if(nPortSize != m_nPortSize)
						{
							m_nPortSize = nPortSize;
							m_setSocket.resize(m_nPortSize, m_sockMain);
						}
					}
				}
			}
			else
			{
				PushMsg(pHead->m_nPort, pHead->m_nPacket, (VAR_TYPE)pHead->m_nVarType, pHead->m_bufData);
			}

			m_setRecvBuf[0].DelMsg(MSG_HEAD_SIZE + nLen);
		}
	}

	return (m_setMsg.size() != 0);
}

bool CInternetPort::WaitMsg(int nMilliSec)
{
	if(!IsOpen())
		return false;

	if(m_setMsg.size())
		return true;

	RecvMsg(nMilliSec);

	return	(m_setMsg.size() != 0);
}

bool CInternetPort::Close()
{
	if(m_nState == STATE_CLOSED)
		return true;

	m_nState = STATE_CLOSED;
	if(m_id == 0)
	{
		for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
			delete(*iter);
		m_setMsg.clear();
		m_setRecvBuf.resize(m_nPortSize);
		for(int i = 0; i < m_setRecvBuf.size(); i++)
			m_setRecvBuf[i].Reset();

		if(m_sockMain != INVALID_SOCKET)
		{
			closesocket(m_sockMain);
			m_sockMain	= INVALID_SOCKET;
		}
		m_setSocket.resize(m_nPortSize);
		for(int i = 0; i < m_setSocket.size(); i++)
		{
			if(m_setSocket[i] != INVALID_SOCKET)
			{
				closesocket(m_setSocket[i]);
				m_setSocket[i]	= INVALID_SOCKET;
			}
		}
	}
	else
	{
		for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
			delete(*iter);
		m_setMsg.clear();
		m_setRecvBuf[0].Reset();

		if(m_sockMain != INVALID_SOCKET)
		{
			closesocket(m_sockMain);
			m_sockMain	= INVALID_SOCKET;
		}
		for(int i = 0; i < m_setSocket.size(); i++)
			m_setSocket[i] = INVALID_SOCKET;
	}

	return true;
}

int CInternetPort::SIZE_OF_TYPE		(int type)
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

bool CInternetPort::PushMsg(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf)
{
	CHECKF(buf);

	if(m_nState != STATE_OK)
		return false;

	if(nPort < 0 || nPort >= m_nPortSize)
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

	CMessagePacket*	pMsg = new CMessagePacket;
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

	if((m_setMsg.size()%100) == 0)
		LOGERROR("CInternetPacket[%d] interface count reached [%u]", m_id, m_setMsg.size());

	if(m_setMsg.size() > 10000)
	{
		LOGERROR("ERROR: message heap overflow, InternetPort[%d] closed!", m_id);
		Close();
	}

	return m_setMsg.size() <= 1000;
}

bool CInternetPort::PopMsg(int *pPort, int *pPacket, VAR_TYPE *pVarType, void* buf)
{
	CHECKF(pPort && pPacket && pVarType && buf);

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

			delete(pMsg);
			m_setMsg.erase(iter);
			return true;
		}
	}

	return false;
}

bool CInternetPort::Init()
{
	m_setSocket.resize(m_nPortSize, INVALID_SOCKET);

	if(m_id == MASTER_PORT_ID)
	{
		for(int i = 0; i < m_nPortSize; i++)
			m_setRecvBuf.push_back(CRecvBuffer());
	}
	else
		m_setRecvBuf.push_back(CRecvBuffer());

	return true;
}

void CInternetPort::Clear()
{
	Close();

	for(MSG_SET::iterator iter = m_setMsg.begin(); iter != m_setMsg.end(); iter++)
		delete(*iter);
	m_setMsg.clear();
}

bool CInternetPort::InitPortSet(int nPortSize, LPCTSTR pszMasterIP, int nMasterPort)
{
	if(!pszMasterIP || pszMasterIP[0] == 0 || strlen(pszMasterIP) >= IPSTR_SIZE)
		return false;

	if(nPortSize <= m_id)
		nPortSize = m_id + 1;

	if(nPortSize < 1 || nPortSize > MAX_PORT_SIZE)
	{
		MYASSERT(!"网络节点数错误! 请修改FD_SETSIZE值并重新编译程序。");
		return false;
	}

	m_nPortSize	= nPortSize;
	strcpy(m_szMasterIP, pszMasterIP);
	m_nMasterPort	= nMasterPort;

	WSADATA		wsaData;
	int	err;
	if((err = WSAStartup(SOCKET_VERSION, &wsaData)) != 0)
	{
		LOGERROR("WSAStartup failed[%d].", err);
		return false;
	}
	m_bWinStartup = true;

	if(wsaData.wVersion != 0x0002)
	{
		WSACleanup();
		LOGERROR("wsaData version unmatch[0x%04X].", wsaData.wVersion);
		return false;
	}

	Init();
	return true;
}

void CInternetPort::ClearPortSet()
{
	Clear();

	if(m_bWinStartup)
		WSACleanup();
}

IMessagePort* CInternetPort::CreateNew(int nPortID, int nPortSize, LPCTSTR szMasterIP, int nMasterPort, LPCTSTR szKey)
{
	CHECKF(szMasterIP && szKey);
	CHECKF(nPortID >= 0 && nPortSize >=0 && nMasterPort >= 0);

	if(nPortID == 0 && nPortSize == 0 || nPortID != 0 && strlen(szMasterIP) == 0)
		return NULL;

	CInternetPort* pPort = new CInternetPort(nPortID);
	if(pPort)
	{
		if(pPort->InitPortSet(nPortSize, szMasterIP, nMasterPort))
		{
			SafeCopy(pPort->m_szKey, szKey, MAX_KEYSIZE);
			return (IMessagePort*)pPort;
		}

		delete pPort;
	}

	return NULL;
}
