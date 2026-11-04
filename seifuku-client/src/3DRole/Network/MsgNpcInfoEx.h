#if !defined(AFX_MSGNPCINFOEX_H__8CE4D354_B8B9_44BC_B2D2_D2776C160CEF__INCLUDED_)
#define AFX_MSGNPCINFOEX_H__8CE4D354_B8B9_44BC_B2D2_D2776C160CEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"

class CMsgNpcInfoEX  : public CNetMsg   
{
	friend struct MsgNpcInfoExProbe;
public:
	CMsgNpcInfoEX();
	virtual ~CMsgNpcInfoEX();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct
	{
		USHORT		unMsgSize;
		USHORT		unMsgType;
		OBJID		id;
		DWORD		dwMaxLife;
		DWORD		dwLife;
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
