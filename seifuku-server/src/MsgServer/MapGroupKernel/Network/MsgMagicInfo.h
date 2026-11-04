#if !defined(AFX_MSGMAGICINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_)
#define AFX_MSGMAGICINFO_H__683B9A1E_9C3D_41E1_B90D_8DF5ABE79632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "Magic.h"

class CMsgMagicInfo : public CNetMsg  
{
public:
	CMsgMagicInfo();
	virtual ~CMsgMagicInfo();

	BOOL	Create		(int nType, int nLevel, int nExp);
	BOOL	Create		(char* pMsgBuf, DWORD dwSize);
	void	Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		DWORD		dwExp;
		USHORT		usType;
		USHORT		usLevel;
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
