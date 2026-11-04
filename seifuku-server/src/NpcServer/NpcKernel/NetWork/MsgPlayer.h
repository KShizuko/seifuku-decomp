#if !defined(AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_)
#define AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"
#include "User.h"

const DWORD	MASK_SYNID			= 0x00FFFFFF;
const DWORD	MASK_RANK_SHIFT		= 24;

class CMsgPlayer : public CNetMsg  
{
public:
	CMsgPlayer();
	virtual ~CMsgPlayer();

public:	

	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		DWORD dwLookFace;
		union
		{
			DWORD dwEffect[2];
			struct
			{
				USHORT usStatuaryLife;
				USHORT usStatuaryFrame;
			};
		};
		
		DWORD dwSynID_Rank;
		DWORD dwReserved14;
		DWORD dwHelmetType;
		DWORD dwArmorType;
		DWORD dwWeaponRType;
		DWORD dwWeaponLType;
		union
		{
			DWORD dwMountType;
			struct
			{
				USHORT usLife;
				USHORT usLevel;
			};
		};
		DWORD dwMedalSelect;

		USHORT usPosX, usPosY;
		USHORT usHair;

		UCHAR ucDir;
		UCHAR ucPose;

		char	cReserved38[0x50-0x38];

		char			szBuf[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;

};

#endif
