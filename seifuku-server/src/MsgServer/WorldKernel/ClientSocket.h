#ifndef	CLIENTSOCKET_H
#define	CLIENTSOCKET_H

#include <winsock2.h>
#include "define.h"
#include "EncryptClient.h"

#define	ENCRYPT_ENABLE

const int	RECV_BUFFER_SIZE	= 2048;
const int	SEND_BUFFER_SIZE	= 4048;

#include "common.h"

class	CClientSocket
{
public:
	CClientSocket();
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
	NAMESTR		m_szIP;
	int			m_nPort;
	int			m_nSndSize;
	int			m_nRcvSize;

	SOCKET		m_sock;
	char		m_bufMsg[RECV_BUFFER_SIZE];
	int			m_nLen;
	char		m_bufSendMsg[SEND_BUFFER_SIZE];
	int			m_nSendLen;

	enum { STATE_CLOSED = 0, STATE_CONNECT, STATE_OPEN, };
	int			m_nState;

	typedef	CEncryptClient<0x12, 0x97, 0x19, 0x7E, 0x48, 0x51, 0x1B, 0xD3>	EncryptClient;
	EncryptClient	m_cEncryptSend;
	EncryptClient	m_cEncryptRecv;
};

#endif
