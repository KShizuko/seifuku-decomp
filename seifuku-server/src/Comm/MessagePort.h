#ifndef	MESSAGEPORT_H
#define MESSAGEPORT_H

#pragma warning(disable:4786)

#include <afxmt.h>
#include "winsock2.h"
#include "I_MessagePort.h"
#include "T_MyQueue.h"
#include <list>
#include <vector>
using namespace std;

MSGPORT_BEGIN
const int	MAX_MSGPACKSIZE = 2048;
struct	CMessagePacket
{
	unsigned int		m_nPortFrom;
	unsigned int		m_nPacket;
	unsigned int		m_nVarType;
	char	m_bufData[MAX_MSGPACKSIZE];
};

#define	CRITSECT	CCriticalSection
#define LOCKOBJ		CSingleLock xLock(&m_xCtrl, true)
#define UNLOCKOBJ	xLock.Unlock()
MSGPORT_END

class	CMessagePort : private IMessagePort
{
protected:
	CMessagePort			(int nPort) { LOCKOBJ; m_id = nPort; m_nState = STATE_CLOSED; m_hHaveMsg = NULL; }
	virtual ~CMessagePort	() { Clear(); }

	IMessagePort*	GetInterface() { LOCKOBJ; return (IMessagePort*)this; }

protected:
	virtual bool	IsOpen	() { LOCKOBJ; return m_nState == STATE_OK; }

	virtual int		GetID	() { LOCKOBJ; return m_id; }
	virtual int		GetSize	() { return m_setPort.size(); }

	virtual bool	Open	();
	virtual bool	Close	();

	virtual bool	Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
	virtual bool	Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus *pStatus = NULL);

	virtual bool	WaitMsg	(int nMilliSec);

protected:
	bool		PushMsg		(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf);
private:
	bool		PopMsg		(int *nPort, int *nPacket, VAR_TYPE *nVarType, void* buf);
	static int	SIZE_OF_TYPE		(int type);

	bool		Init();
	void		Clear();

protected:
	int			m_id;
	enum { STATE_OK, STATE_CLOSED };
	int			m_nState;
	typedef	list<message_port::CMessagePacket*>	MSG_SET;
	MSG_SET		m_setMsg;
	CRITSECT	m_xCtrl;
	HANDLE		m_hHaveMsg;
	int			m_debugPackets;
	CMyQueue<message_port::CMessagePacket*>		m_setRecycle;

public:
	static bool	InitPortSet(int nPortNum);
	static void	ClearPortSet();
	static IMessagePort*	GetInterface(int nPort) {
					if(nPort>=0&&nPort<m_setPort.size()) return m_setPort[nPort]->GetInterface(); else return NULL;
				}
	static int	GetQueueSize(int nPort)				{
					if(nPort>=0&&nPort<m_setPort.size()) return m_setPort[nPort]->m_setMsg.size(); else return -1;
				}

protected:
	typedef	vector<CMessagePort*> PORT_SET;
	static PORT_SET		m_setPort;
	static CRITSECT		m_xSetCtrl;
};

#endif
