#if !defined(AFX_MSGSYNMEMBERINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_)
#define AFX_MSGSYNMEMBERINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

class CMsgSynMemberInfo : public CNetMsg  
{
public:
	CMsgSynMemberInfo();
	virtual ~CMsgSynMemberInfo();

	BOOL	Create		(LPCTSTR szName, int nRankShow, int nProffer);
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		int 	nProffer;
		UCHAR	ucRank;
		char	szName[_MAX_NAMESIZE];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
