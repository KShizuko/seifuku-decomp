#if !defined(AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_)
#define AFX_MSGPLAYER_H__298DE32A_21B9_41E0_8728_CD895F36A959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"

class CMsgPlayer : public CNetMsg  
{
public:
	CMsgPlayer();
	virtual ~CMsgPlayer();

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		unsigned short unMsgSize;
		unsigned short unMsgType;
		OBJID id;
		DWORD dwLookFace;
		DWORD dwStatus[2];
		union
		{
			DWORD dwSynID_Rank;
			OBJID idOwner;
		};
		DWORD dwGarmentType;
		DWORD dwArmetType;
		DWORD dwArmorType;
		DWORD dwWeaponRType;
		DWORD dwWeaponLType;
		USHORT usRWeaponRestrain;
		USHORT usLWeaponRestrain;
		union
		{
			int nPotential;
			struct
			{
				USHORT usLife;
				USHORT usLevel;
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
	friend struct MsgPlayerProbe;

};

#endif
