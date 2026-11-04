template <unsigned long key1, unsigned long key2>
CServerSocket<key1, key2>::CServerSocket( )
{
	m_sockServer	= INVALID_SOCKET;
	m_bufPeerIP[0]	= 0;
	m_tLastRecv		= 0;

	m_bState		= c_stateNone;
}

template <unsigned long key1, unsigned long key2>
CServerSocket<key1, key2>::~CServerSocket()
{
	Close();
}

template <unsigned long key1, unsigned long key2>
bool CServerSocket<key1, key2>::Open(SOCKET sockServer, u_long nClientIP)
{
	if(m_bState)
	{
		LOGWARNING("CServerSocket<key1, key2>::Open()前没有CLOSE");
		Close(true);
	}

	if(sockServer == INVALID_SOCKET)
	{
		LOGERROR("CServerSocket<key1, key2>::Open()参数错误，SOCKET号无效");
		return	false;
	}

	if(m_sockServer != INVALID_SOCKET)
	{
		LOGERROR("CServerSocket<key1, key2>::Open()复盖了前一个SOCKET");
	}

	m_sockServer = sockServer;
#ifdef	ENCRYPT
	m_cEncryptRecv.Init();
	m_cEncryptSend.Init();
#endif
	m_bufPeerIP[0] = 0;
	struct in_addr	in;
	in.S_un.S_addr = nClientIP;
	char * pClientIP = inet_ntoa(in);
	if(pClientIP)
		strcpy(m_bufPeerIP, pClientIP);
	m_tLastRecv		= -clock();

	m_bState		= c_stateNormal;

	return true;
}
template <unsigned long key1, unsigned long key2>
bool CServerSocket<key1, key2>::Recv(char * pBuf, int &nLen)
{
	int		ret;
	u_short	nMessageSize = 0;
	nLen = 0;

	if(!m_bState)
		return false;

	if((ret = recv(m_sockServer, (char *)&nMessageSize, 2, MSG_PEEK)) == 2)
	{
#ifdef	ENCRYPT
		if(key1 || key2)
			m_cEncryptRecv.Encrypt((unsigned char *)&nMessageSize, 2, false);
#endif
		u_long	nRcvBufLen;
		ioctlsocket(m_sockServer, FIONREAD, &nRcvBufLen);

		if(!(nMessageSize > 0 && nMessageSize <= _MAX_MSGSIZE))
		{
			LOGWARNING("接收到一个错误长度[%d]的消息。这时RCVBUF中有[%d]个字节的数据。客户端的IP地址为[%s]", 
							nMessageSize, nRcvBufLen, m_bufPeerIP);
			Close(true);
			return false;
		}

		if(nRcvBufLen >= nMessageSize)
		{
			int	nGotBytes	= 0;
			int	nCount		= 0;
			while(nGotBytes < nMessageSize)
			{
				ret = recv(m_sockServer, (char *)pBuf + nGotBytes, nMessageSize - nGotBytes, 0);
				if(ret > 0)
				{
					nGotBytes += ret;
				}
				else
				{
					break;
				}

				if(++nCount > RECVREPEATTIMES)
				{
					LOGWARNING("系统效率严重下降，recv()函数无法正常读出数据。一个连接被强行断开。");
					Close(true);
					return false;
				}
			}

			if(nGotBytes == nMessageSize)
			{
				if(nCount >= 2)
				{
					if(nCount >= 3)
					{
						LOGMSG("系统效率严重下降，recv()运行了[%d]次才取到足够的数据。", nCount);
					}

#ifdef	LOCKACCEPT
					if(InterlockedExchange(&s_nLockAcceptTime, clock() + LOCKACCEPTSECOND*CLOCKS_PER_SEC) == 0)
					{
						LOGWARNING("系统太繁忙，已暂停接收新连接...");
					}
#endif
				}

				m_tLastRecv = clock();

				nLen = nMessageSize;
#ifdef	ENCRYPT
				if(key1 || key2)
					m_cEncryptRecv.Encrypt((unsigned char *)pBuf, nLen);
#endif
#ifdef	DUMP
				char buf[4096] = "";
				char buf3[4];
				for(int i = 0; i < nLen; i++)
				{
					sprintf(buf3, "%02X ", (unsigned char)pBuf[i]);
					strcat(buf, buf3);
					if(i%16 == 15)
						strcat(buf, "\n");
				}
				strcat(buf, "\n");
				LOGMSG("Send: \n%s", buf);
#endif
				return true;
			}
			else if(ret == 0)
			{
#ifdef	ALPHA_X
				LOGMSG("接收到客户端的关闭SOCKET消息。");
#endif
				Close();
				return false;
			}
			else if(ret < 0)
			{
				int err = WSAGetLastError();
				if(err == WSAEWOULDBLOCK)
				{
					LOGWARNING("系统效率严重下降，recv()函数出现阻塞。一个连接被强行断开。系统有[%d]字节，", nRcvBufLen);
					LOGWARNING("打算读入[%d]个字节，但只能读入[%d]个字节。", nMessageSize, nGotBytes);
					return false;
				}
				else
				{
#ifdef	ALPHA_X
					LOGERROR("recv()错误[%d]", err);
#endif
					Close(true);
					return false;
				}
			}
			else
			{
				LOGERROR("CServerSocket<key1, key2>::Recv()函数异常");
				return false;
			}

		}
		else
		{
			nLen = 0;
			return true;
		}
	}
	else if(ret > 0)
	{
		nLen = 0;
		return true;
	}
	else if(ret == 0)
	{
#ifdef	ALPHA_X
		LOGMSG("接收到客户端的关闭SOCKET消息。");
#endif
		Close();
		return false;
	}
	else
	{
		int err = WSAGetLastError();
		if(err == WSAEWOULDBLOCK)
		{
			nLen = 0;
			return true;
		}
		else
		{
#ifdef	ALPHA_X
			LOGERROR("recv()错误[%d]", err);
#endif
			Close(true);
			return false;
		}
	}

}

template <unsigned long key1, unsigned long key2>
bool	CServerSocket<key1, key2>::Send(const char * pBuf, int nLen)
{
	if(m_bState == c_stateNone)
		return false;
	if(m_bState == c_stateShutDown)
		return true;

	if(!(pBuf && nLen > 0 && nLen <= _MAX_MSGSIZE))
	{
		LOGERROR("CServerSocket<key1, key2>::Send([0x%04X], [%d])参数异常。", pBuf, nLen);
		Close(true);
		return false;
	}
	int	nMsgLen;
	try{ nMsgLen = *(unsigned short *)pBuf;	}catch(...) {
		LOGCATCH("CServerSocket<key1, key2>::Send()指针参数无效。一个连接被断开。");
		Close(true);
		return false;
	}
	if(!(nMsgLen > 0 && nMsgLen <= _MAX_MSGSIZE && nMsgLen == nLen))
	{
		LOGERROR("内核SEND的一个消息的长度值[%d]错误，或与消息长度[%d]不符。一个连接被断开，", nMsgLen, nLen);
		LOGERROR("该消息ID为[%d]", *(const unsigned short *)(pBuf + 2));
		Close(true);
		return false;
	}
	
#ifdef	PROFILE_X
	InterlockedExchangeAdd(&s_nPackSum, nLen);
	InterlockedIncrement(&s_nPackCount);
#endif
	
#ifdef	DUMP
	char buf[4096] = "";
	char buf3[4];
	for(int i = 0; i < nLen; i++)
	{
		sprintf(buf3, "%02X ", (unsigned char)pBuf[i]);
		strcat(buf, buf3);
		if(i%16 == 15)
			strcat(buf, "\n");
	}
	strcat(buf, "\n");
	LOGMSG("Send: \n%s", buf);
#endif
#ifdef	ENCRYPT
	char	buf[_MAX_MSGSIZE];
	memcpy(buf, pBuf, nLen);
	pBuf = buf;
	if(key1 || key2)
		m_cEncryptSend.Encrypt((unsigned char *)pBuf, nLen);
#endif
	
	int		nPutBytes = 0;
	int		nCount = 0;
	int		ret = 0;
	while(nPutBytes < nLen)
	{
		ret = send(m_sockServer, (char *)pBuf + nPutBytes, nLen - nPutBytes, 0);
		if(ret > 0)
		{
			nPutBytes += ret;
		}
		else
		{
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK)
			{
#ifdef	ALPHA_X
				LOGWARNING("服务SOCKET的send()错误[%d]，可能客户端关闭了SOCKET", err);
#endif
				Close(true);
				return false;
			}
			
			ret = send(m_sockServer, (char *)pBuf + nPutBytes, nLen - nPutBytes, 0);
			if(ret > 0)
			{
				nPutBytes += ret;
				LOGERROR("首次SEND不成功，再次SEND成功，原因未知。IP地址为[%s]", m_bufPeerIP);
			}
			else
			{
				int err = WSAGetLastError();
				if(err == WSAEWOULDBLOCK)
				{
					if(m_tLastRecv > 0)
					{
						if((clock()-m_tLastRecv) / CLOCKS_PER_SEC >= 25)
							LOGERROR("send()溢出，可能服务器已断线。IP地址为[%s]", m_bufPeerIP);
						else
							LOGERROR("send()溢出，玩家网速太慢。IP地址为[%s]", m_bufPeerIP);
					}
					else if(m_tLastRecv < 0)
					{
						if((clock()+m_tLastRecv) / CLOCKS_PER_SEC >= 25)
							LOGERROR("send()溢出，可能服务器已断线。IP地址为[%s]。", m_bufPeerIP);
						else
							LOGERROR("send()溢出，玩家网速太慢。IP地址为[%s]。", m_bufPeerIP);
					}
					else
					{
						LOGERROR("内部异常，m_tLastRecv 为 0");
					}
				}
#ifdef	ALPHA_X
				else
					LOGWARNING("服务SOCKET的send()错误[%d]，可能客户端关闭了SOCKET.", err);
#endif

				Close(true);
				return false;
			}
		}

		if(++nCount > RECVREPEATTIMES)
		{
			LOGWARNING("系统效率严重下降，send()函数无法正常写出数据。一个连接被强行断开。");
			Close(true);
			return false;
		}
	}

	ASSERT(ret == nLen);

	if(nCount >= 2)
	{
		LOGMSG("系统效率严重下降，send()运行了[%d]次才发送完数据。", nCount);
	}

#ifdef	PROFILE_XXXXX
	if(g_aServerShare[nIndex].m_queOutput.Len() > OUTBUFSIZE*4/5)
	{
		LOGWARNING("连接[%d]OUTBUF使用太多[%d]，即将溢出", nIndex, 
					g_aServerShare[nIndex].m_queOutput.Len());
	}

#endif

	return true;
}

template <unsigned long key1, unsigned long key2>
bool	CServerSocket<key1, key2>::ShutDown()
{
	if(!m_bState)
		return false;

	m_bState = c_stateShutDown;
	if(shutdown(m_sockServer, 1))
	{
		int err = WSAGetLastError();
		LOGWARNING("服务SOCKET的shutdown()错误[%d]，可能客户端关闭了SOCKET", err);
		Close(true);
		return false;
	}
	else
	{
		return true;
	}
}

template <unsigned long key1, unsigned long key2>
void	CServerSocket<key1, key2>::Close(bool bLinger0 )
{
	if(m_bState)
	{
		if(bLinger0)
		{
			struct linger ling = { 1, 0 };
			setsockopt(m_sockServer, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
		}

		m_bState = c_stateNone;
		closesocket(m_sockServer);
		InterlockedDecrement(&s_nSocketCount);
		m_sockServer	= INVALID_SOCKET;
		m_bufPeerIP[0]	= 0;

	}
}

template <unsigned long key1, unsigned long key2>
char *	CServerSocket<key1, key2>::GetPeerIP()
{
	if(!m_bState)
		return NULL;

	if(strlen(m_bufPeerIP))
	{
		return m_bufPeerIP;
	}
	else
	{
		LOGERROR("调用了GetPeerIP()中的无效代码。并无错误：)");
		sockaddr_in	inAddr;
		memset(&inAddr, 0, sizeof(inAddr));
		int		nLen = sizeof(inAddr);
		if(getpeername(m_sockServer, (sockaddr *)&inAddr, &nLen))
		{
			int err = WSAGetLastError();
			LOGERROR("服务端SOCKET的getpeername()错误[%d].", err);
			Close(true);
			return NULL;
		}
		char *	pAddr = inet_ntoa(inAddr.sin_addr);
		if(pAddr)
		{
			strcpy(m_bufPeerIP, pAddr);
			return m_bufPeerIP;
		}
		else
		{
			LOGERROR("服务端SOCKET的inet_ntoa()错误.");
			Close(true);
			return NULL;
		}
	}
}
