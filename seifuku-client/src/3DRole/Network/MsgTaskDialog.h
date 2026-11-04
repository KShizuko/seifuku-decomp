#if !defined(AFX_MSGTASKDIALOG_H__0D55D316_2027_4D86_81EC_5BEA5B351F69__INCLUDED_)
#define AFX_MSGTASKDIALOG_H__0D55D316_2027_4D86_81EC_5BEA5B351F69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"
#include "NetStringPacker.h"
enum
{
		MSGDIALOG_TEXT		= 1,		
		MSGDIALOG_LINK		= 2,		
		MSGDIALOG_EDIT		= 3,		
		MSGDIALOG_PIC		= 4,	
		MSGDIALOG_LISTLINE	= 5,
		MSGDIALOG_CREATE	= 100,		
		MSGDIALOG_ANSWER	= 101,		
		MSGDIALOG_TASKID	= 102,
};

class CMsgTaskDialog : public CNetMsg  
{
	friend struct MsgTaskDialogProbe;
public:
	CMsgTaskDialog();
	virtual ~CMsgTaskDialog();

public:
	BOOL			Create		(char* pbufMsg, DWORD dwMsgSize);
	void			Process		(void *pInfo);

public:
	BOOL			Create		(int nIndex, char* pszStr, OBJID idAction=ID_NONE, int nAction=MSGDIALOG_ANSWER);

private:
	typedef struct 
	{
		unsigned short	unMsgSize;
		unsigned short	unMsgType;

		union{
			struct{
				USHORT	usPosX;
				USHORT	usPosY;
			};
			DWORD	idTask;
		};
		USHORT	usData;
		UCHAR	idxTask;
		UCHAR	ucAction;
		char	szBuf[1];
	}MSG_Info;
	MSG_Info*	m_pInfo;

private:
	CNetStringPacker	m_StrPacker;	
};

#endif
