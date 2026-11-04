#if !defined(AFX_MSGSYNDICATEATTRIBUTEINFO_H__BEACEA82_5569_4645_BC2D_A5BB94E67C57__INCLUDED_)
#define AFX_MSGSYNDICATEATTRIBUTEINFO_H__BEACEA82_5569_4645_BC2D_A5BB94E67C57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"


class CMsgSyndicateAttributeInfo  : public CNetMsg  
{
public:
	CMsgSyndicateAttributeInfo();
	virtual ~CMsgSyndicateAttributeInfo();
public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
protected:
	typedef struct {
		USHORT	unMsgSize;
		USHORT	unMsgType;
		OBJID	idSyndicate;
		int		nProffer;
		DWORD	dwSyndicateFund;
		DWORD	dwSyndicatePopulation;
		UCHAR	ucRank;
		char	szLeader[_MAX_NAMESIZE];
		}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
