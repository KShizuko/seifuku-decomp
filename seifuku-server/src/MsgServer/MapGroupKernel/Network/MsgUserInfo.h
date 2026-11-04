#if !defined(AFX_MSGUSERINFO_H__DFC94B21_4F65_11D5_A212_0000E8E7F527__INCLUDED_)
#define AFX_MSGUSERINFO_H__DFC94B21_4F65_11D5_A212_0000E8E7F527__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"

struct UserInfoStruct;
class  ISocket;
class CMsgUserInfo : public CNetMsg
{
public:
	CMsgUserInfo();
	virtual ~CMsgUserInfo();

public:
	BOOL			Create		(OBJID idUser, const UserInfoStruct* pInfo);
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		OBJID id;
		DWORD dwLookFace;
		USHORT usHair;
		DWORD dwMoney;
		DWORD dwEmoney;
		LONG nExp;

		int		nDeed;
		DWORD	dwMedal;
		DWORD	dwMedal_select;
		DWORD	dwVirtue;
		USHORT	usForce;
		USHORT	usDexterity;
		USHORT	usHealth;
		USHORT	usSoul;
		USHORT	usAdditionalPoint;
		USHORT	usLife;
		USHORT	usMana;

		SHORT	sPk;
		UCHAR	ucLevel;
		UCHAR	ucProfession;

		UCHAR	ucNobility;
		UCHAR	ucMetempsychosis;
		UCHAR	ucAutoAllot;
		char szBuf[1];
	}MSG_Info;
	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;
};

#endif
