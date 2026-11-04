#if !defined(AFX_MSGBOARD_H__C23774D2_1097_49C1_BBBF_CAA16D4F6C94__INCLUDED_)
#define AFX_MSGBOARD_H__C23774D2_1097_49C1_BBBF_CAA16D4F6C94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"
enum
{
		MESSAGEBOARD_DEL				= 1,
		MESSAGEBOARD_GETLIST			= 2,
		MESSAGEBOARD_LIST				= 3,
		MESSAGEBOARD_GETWORDS			= 4,
};

class CMsgMessageBoard : public CNetMsg 
{
	friend struct MsgBoardProbe;
public:
	CMsgMessageBoard();
	virtual ~CMsgMessageBoard();


public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);
	BOOL			Create		(unsigned short usIndex, unsigned char ucAction, unsigned short usChannel, char* pszStr = NULL);

protected:
	typedef struct {
		USHORT			unMsgSize;
		USHORT			unMsgType;

		USHORT	usIndex;
		USHORT	usChannel;
		UCHAR	ucAction;
		char	szBuf[1];
	}MSG_Info;
	MSG_Info*	m_pInfo;
private:
	CNetStringPacker	m_StrPacker;

};

#endif
