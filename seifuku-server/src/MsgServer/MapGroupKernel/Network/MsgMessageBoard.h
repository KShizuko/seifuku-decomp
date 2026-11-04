#ifndef MESSAGEBOARD_H
#define MESSAGEBOARD_H

#include "NetMsg.h"
#include "NetStringPacker.h"

enum{
		MESSAGEBOARD_DEL				= 1,
		MESSAGEBOARD_GETLIST			= 2,
		MESSAGEBOARD_LIST				= 3,
		MESSAGEBOARD_GETWORDS			= 4,
};

class CMsgMessageBoard : public CNetMsg  
{
public:
	CMsgMessageBoard();
	virtual ~CMsgMessageBoard();

public:
	void			Process		(void *pInfo);

private:
	typedef struct {
		MSGHEAD_DEFINE

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
