#if !defined(AFX_MSGNPCINFO_H__50F4AAD1_342A_4D6C_A161_33CDE0158D5B__INCLUDED_EX)
#define AFX_MSGNPCINFO_H__50F4AAD1_342A_4D6C_A161_33CDE0158D5B__INCLUDED_EX

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "Npc.h"
#include "NetStringPacker.h"

class CNpc;
class CMsgNpcInfoEx  : public CNetMsg
{
public:
	CMsgNpcInfoEx();
	virtual ~CMsgNpcInfoEx();

	BOOL	Create	(CNpc* pNpc);
	BOOL	Create(OBJID id, DWORD dwMaxLife, DWORD dwLife, int nType, int nSort,
					int nLookType, int nCellX, int nCellY, const char* pszName=NULL);
public:
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID		id;
		DWORD		dwMaxLife;
		DWORD		dwLife;
		unsigned	short usCellX;
		unsigned	short usCellY;
		USHORT		usLook;
		USHORT		usType;
		USHORT		usSort;
		char		szBuf[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;
};

#endif
