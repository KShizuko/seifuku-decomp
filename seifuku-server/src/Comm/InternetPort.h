#ifndef	INTERNETPORT_H
#define INTERNETPORT_H

#pragma warning(disable:4786)

#define	_WINSOCKAPI_
#include <afxmt.h>
#ifndef FD_SETSIZE
#define	FD_SETSIZE					2048
#endif
#include "winsock2.h"
#include "I_MessagePort.h"
#include <list>
#include <vector>
using namespace std;

#include "MessagePort.h"

MSGPORT_BEGIN
#define	CONNECT_OVERTIMESECS		10
#define	SOCKET_VERSION				0x0002
#define	SNDBUF_SIZE					(32*1024)

#define	IPSTR_SIZE	16
#define	MAX_KEYSIZE	256

#define	MASTER_PORT_ID				0
#define	MAX_PORT_SIZE				FD_SETSIZE-2

enum {	SYS_PACKET_BASE		= MAX_PACKET_ID+1,
		SYS_PACKET_PORTID	= SYS_PACKET_BASE+1,
		SYS_PACKET_PORTSIZE	= SYS_PACKET_BASE+2,
		SYS_PACKET_LOGINKEY	= SYS_PACKET_BASE+3,
};
MSGPORT_END

MSGPORT_BEGIN
const int	MSG_HEAD_SIZE		= 6;
const int	RECV_BUFFER_SIZE	= 16*1024;
struct	CMessageHead
{
	unsigned short		m_nPort;
	unsigned short		m_nPacket;
	unsigned short		m_nVarType;
	char				m_bufData[1];
};
MSGPORT_END

class	CInternetPort : private IMessagePort
{
private:
	struct	CRecvBuffer{
	public:
		CRecvBuffer()	{ Reset(); }
		void Reset()	{ m_nMsgLen = 0; }
		bool DelMsg(int nLen){
						if(nLen <= m_nMsgLen){
							if(nLen < m_nMsgLen)
								memcpy(m_bufMsg, m_bufMsg + nLen, m_nMsgLen - nLen);
							m_nMsgLen -= nLen;
							return true;
						}
						m_nMsgLen	= 0;
						return false;
		}
		message_port::CMessageHead* GetHead()	{ return (message_port::CMessageHead*)m_bufMsg; }
		char* GetTail()	{ return m_bufMsg + m_nMsgLen; }
		int	  GetLen()	{ return m_nMsgLen; }
		void  AddLen(int nLen)	{ m_nMsgLen += nLen; }
	protected:
		char		m_bufMsg[message_port::RECV_BUFFER_SIZE];
		int			m_nMsgLen;
	};
public:
	CInternetPort			(int nPort);
	virtual ~CInternetPort	() { ClearPortSet(); }

	IMessagePort*	GetInterface() { return (IMessagePort*)this; }

protected:
	virtual int		GetSize	() { return m_nPortSize; }
	virtual int		GetID	() { return m_id; }

	virtual bool	Open	();
	virtual bool	Close	();

	virtual bool	Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
	virtual bool	Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus *pStatus = NULL);

	virtual bool	WaitMsg	(int nMilliSec);
	virtual bool	IsOpen	() { return m_nState == STATE_OK; }

protected:
	bool	PushMsg		(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
	bool	PopMsg		(int *nPort, int *nPacket, VAR_TYPE *nVarType, void* buf);
	bool	SendFrom	(int nPortFrom, int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
	bool	RecvMsg		(int nMilliSec);
	bool	RecvToBuf	(CRecvBuffer* pBuf, SOCKET sock);
	bool	BufferToList	(int idx);
	static int	SIZE_OF_TYPE		(int type);
	SOCKET	CreateServerSocket		();
	SOCKET	CreateClientSocket		();

	bool	Init();
	void	Clear();

protected:
	int			m_id;
	enum { STATE_OK, STATE_CLOSED, STATE_OPENING };
	int			m_nState;
	typedef	list<message_port::CMessagePacket*>	MSG_SET;
	MSG_SET		m_setMsg;

public:
	bool	InitPortSet(int nPortSize, LPCTSTR pszMasterIP, int nMasterPort);
	void	ClearPortSet();

public:
	static IMessagePort* CreateNew(int nPortID, int nPortSize, LPCTSTR szMasterIP, int nMasterPort, LPCTSTR szKey);

protected:
	int				m_nPortSize;
	char			m_szMasterIP[IPSTR_SIZE];
	int				m_nMasterPort;
	char			m_szKey[MAX_KEYSIZE];

	typedef	vector<SOCKET> PORT_SET;
	PORT_SET		m_setSocket;
	SOCKET			m_sockMain;

	typedef	vector<CRecvBuffer> BUFFER_SET;
	BUFFER_SET		m_setRecvBuf;

	bool			m_bWinStartup;
};

#endif
