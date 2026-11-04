#if !defined(AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_)
#define AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"
#include "AiNpc.h"
#include <stddef.h>

const DWORD	MASK_SYNID			= 0x00FFFFFF;
const DWORD	MASK_RANK_SHIFT		= 24;

class CUser;
class CMsgPlayer : public CNetMsg  
{
public:
	CMsgPlayer();
	virtual ~CMsgPlayer();

public:	
	BOOL			Create		(IRole* pRole, bool bQuery = false);

	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		DWORD dwLookFace;
		DWORD	dwStatus[2];
		DWORD	dwSynID_Rank;
		DWORD	dwMountType;
		DWORD	dwHelmetType;
		DWORD	dwArmorType;
		DWORD	dwWeaponRType;
		DWORD	dwWeaponLType;
		union
		{
			struct {
				USHORT	usWeaponRAntiMonster;
				USHORT	usWeaponLAntiMonster;
			};
			DWORD	dwSpriteType;
		};
		union
		{
			DWORD	dwMantleType;
			struct {
				USHORT	usLife;
				USHORT	usLevel;
			};
			struct
			{
				USHORT usStatuaryLife;
				USHORT usStatuaryFrame;
			};
		};

		USHORT usPosX, usPosY;
		USHORT usHair;

		UCHAR ucDir;
		UCHAR ucPose;
		UCHAR ucMetempsychosis;
		UCHAR ucReserved;
		char szBuf[1];
	}MSG_Info;
	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;

};

#endif
