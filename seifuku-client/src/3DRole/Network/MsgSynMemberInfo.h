#if !defined(AFX_MSGSYNMEMBERINFO_H__CB607DAF_CB16_40F2_A459_E8963D4B91AD__INCLUDED_)
#define AFX_MSGSYNMEMBERINFO_H__CB607DAF_CB16_40F2_A459_E8963D4B91AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "NetMsg.h"


class CMsgSynMemberInfo  : public CNetMsg  
{
	friend struct MsgSynMemberInfoProbe;
public:
	CMsgSynMemberInfo();
	virtual ~CMsgSynMemberInfo();
public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(char* pszName);
protected:
	typedef struct {
		USHORT	unMsgSize;
		USHORT	unMsgType;
		int		nProffer;
		UCHAR	ucRank;
		char	szName[_MAX_NAMESIZE];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
