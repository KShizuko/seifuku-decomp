#if !defined(AFX_MSGUSERINFO_H__DFC94B21_4F65_11D5_A212_0000E8E7F527__INCLUDED_)
#define AFX_MSGUSERINFO_H__DFC94B21_4F65_11D5_A212_0000E8E7F527__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"

struct UserInfoStruct;
class CMsgUserInfo : public CNetMsg
{
public:
	CMsgUserInfo();
	virtual ~CMsgUserInfo();

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void *pInfo);

protected:
#pragma pack(push)
#pragma pack(1)
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		DWORD dwLookFace;
		USHORT usHair;
		DWORD dwMoney;
		LONG nExp;

		int    nPotential;
		DWORD	dwReserved16;
		int		nDeed;
		DWORD	dwMedal;
		DWORD	dwMedalSelect;
		DWORD	dwVirtue;
		USHORT usForce;
		USHORT usDexterity;
		USHORT usHealth;
		USHORT usSoul;

		USHORT usAdditionalPoint;

		USHORT usLife;
		USHORT usMana;
		SHORT sPk;

		UCHAR ucLevel;
		UCHAR ucProfession;
		UCHAR ucNobility;
		UCHAR ucMetempsychosis;
		UCHAR ucAutoAllot;

		char			szBuf[1];
	}MSG_Info;
#pragma pack(pop)
	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;
};

#endif
