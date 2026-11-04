#ifndef	CLIENTSOCKET_H
#define	CLIENTSOCKET_H

#ifndef FD_SETSIZE
#define	FD_SETSIZE					2048
#endif
#include <winsock2.h>
#include <list>
#include <memory>
#include "typedef.h"
#include "EncryptClient.h"

const int	RECV_BUFFER_SIZE	= 2048;
const int	SEND_BUFFER_SIZE	= 4048;

#include "common.h"

struct CSendPacket
{
	CSendPacket(const char* pack, int nLen);
	int		m_nLen;
	int		m_nSent;
	std::shared_ptr<char>	m_buf;
};

class	CClientSocket
{
public:
	CClientSocket();
	~CClientSocket();
public:
	bool		Open(const char* szName, int nPort, int nBlockSecs = 0, int nSndBuf = 0, int nRcvBuf = 0);
	bool		Open(int nBlockSecs = 0);

	LPCTSTR		GetPacket(int *pBufSize, bool bFromNet = true);

	bool		ClearPacket(int nPacketSize);

	void		Close();

public:
	bool		SendPacket(const char* pack, int nLen, bool bFlush = false);

	bool		Flush();

public:
	bool		IsOpen();

protected:
	IPSTR		m_szIP;
	int			m_nPort;
	int			m_nSndSize;
	int			m_nRcvSize;

	SOCKET		m_sock;
	char		m_bufMsg[RECV_BUFFER_SIZE];
	int			m_nLen;
	int			m_nBufferedSendBytes;
	int			m_nMaxBufferedSendBytes;
	int			m_nMaxBufferedSendPackets;
	std::list<CSendPacket>	m_setSendPacket;

	enum { STATE_CLOSED = 0, STATE_CONNECT, STATE_OPEN, };
	int			m_nState;
public:
	void	ChangeCode(DWORD dwData) { m_cSendEncrypt.ChangeCode(dwData); }
protected:
	typedef	CEncryptClient<0xFA, 0x0F, 0x13, 0x9D, 0xA4, 0x79, 0x6D, 0x62>	ENCRYPTCLIENT;
	ENCRYPTCLIENT	m_cSendEncrypt, m_cRecvEncrypt;
};

#endif
