#if !defined(AFX_MSGNPCINFO_H__6E225150_E2CE_4437_8045_606051EFAEC8__INCLUDED_)
#define AFX_MSGNPCINFO_H__6E225150_E2CE_4437_8045_606051EFAEC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"

class CMsgNpcInfo : public CNetMsg    
{
	friend struct MsgNpcInfoProbe;
public:
	CMsgNpcInfo();
	virtual ~CMsgNpcInfo();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(CMyPos posNpc, int nDir, int nFrame, int nAction);
	BOOL			Create		(CMyPos posNpc, int nLook);

protected:
	typedef struct
	{
		USHORT		unMsgSize;
		USHORT		unMsgType;
		OBJID		id;
		unsigned	short usCellX;			
		unsigned	short usCellY;
		USHORT		usLook;
		USHORT		usType;
		USHORT		usSort;
		char		szName[1];
	}MSG_Info;
	MSG_Info*	m_pInfo;
private:
	CNetStringPacker	m_StrPacker;
};

#endif
