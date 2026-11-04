#if !defined(AFX_GAMEMSG_H__8DCDC868_325E_4751_9C6B_4277BC598AEA__INCLUDED_)
#define AFX_GAMEMSG_H__8DCDC868_325E_4751_9C6B_4277BC598AEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif


#include <deque>
using namespace std;

#include "BaseFunc.h"
#include "TxtMsgSet.h"
#include "NetworkDef.h"
#include "ChatLog.h"
const int _MAX_EMOTIONSIZE			= 16;

const int _AREA_NUMBER				= 3 ;
const int _NORAREA_MSG_LINE			= 20 ;
const int _SYSAREA_MSG_LINE			= 3 ;
const int _MANAREA_MSG_LINE			= 1 ;
const DWORD _COLOR_SYSTEM_SPEAKER     = 0XFFFE00FB;
const DWORD _COLOR_SELF				  = 0XFF00FF;	
const DWORD _COLOR_SYSTEM_MESSAGE     = 0XFF0000;
const DWORD _COLOR_PLAYER_SPEAKER     = 0XAAFFAA;
const DWORD _COLOR_RECEIVER           = 0XAAFFAA;
const DWORD _COLOR_EMOTION            = 0X00FF00;
const DWORD _COLOR_NORMAL             = 0XFFFFFF;
const DWORD _COLOR_TIME               = 0XFFFFFF;
class CMyBitmap;
class CGameMsgUnit
{
public:
	CGameMsgUnit(void);
	virtual ~CGameMsgUnit(void);
	char*	Show(int nStartX, int nStartY, BOOL bCapture);
	int		GetLines(void);
	void	SetStartX(int nPosX) { m_nStartX = nPosX; }
	BOOL	GetText(char* pszText);
	const char* GetMsg(void) const {return m_szMsg;}
	CMyPos ShowColorStr(int nPosX, int nPosY, DWORD dwColor, const char* pszString);

private:
	int		Scan(void);
	void	GetDateTimeStr(DWORD dwDateTime, char* pszStr);
public:
	char	m_szSender[_MAX_WORDSSIZE];
	char	m_szReceiver[_MAX_WORDSSIZE];
	char	m_szEmotion[_MAX_WORDSSIZE];
	char	m_szMsg[_MAX_WORDSSIZE];
	unsigned short	m_unTxtAttribute;
	unsigned short	m_unTxtStyle;
	DWORD	m_dwWordsColor;
	int		m_nStartX;
	int		m_nDragStep;
	int		m_nQuakeFlag;
	BOOL	m_bSuper;
	DWORD	m_dwTime, m_dwBeginTime;
private:
	CMySize m_sizeFont;
	CMySize m_sizeScreen;
	int		m_nFlashFlag;
	int		m_nStrReturnCounter;
	DWORD	m_dwWordsColorFlash;
	DWORD	m_dwSystemColor;
	DWORD	m_dwSystemMessageColor;
	DWORD	m_dwSenderColor;
	DWORD	m_dwNormalColor;
	DWORD	m_dwReceiverColor;
	DWORD	m_dwEmotionColor;
	int		m_nPrevPosX, m_nPrevPosY;

};
typedef deque<CGameMsgUnit*> DEQUE_MSGUNIT;
typedef deque<char*> DEQUE_BLACKNAME;

class CGameMsg {
public:
	CGameMsg(void);
	virtual ~CGameMsg(void);
	BOOL	AddMsg(const char* pszSender, char* pszReceiver, char* pszEmotion, char* pszMsg, DWORD dwWordsColor, unsigned short unTxtStyle, unsigned short unTxtAttribute, DWORD dwTime);
	BOOL	AddMsg(const char* pszMsg, DWORD dwChannel=_TXTATR_SYSTEM, DWORD dwWordsColor=_COLOR_SYSTEM_MESSAGE, DWORD dwStyle=_TXT_NORMAL, BOOL bSuper=false);
	BOOL	AddMsg(DWORD dwChannel, const char* fmt, ...);
	void    ClearMsg(void);
	void	Show(BOOL bCapture = false);
	void	SetShowLines(UINT uMaxLines) {m_nMaxLine = (int)uMaxLines;}
	UINT	GetShowLines(void)	{return m_nMaxLine;}

	void	ProcessArea ( int& nCurAmount, int nMsgAmount, int nDequeIndex, int nDequeSize, int* pIndex, BOOL bFull ) ;

	CTxtMsgSet		m_setMsg;

private:
	DEQUE_MSGUNIT	m_dequeMsg[14] ;
	int				m_nMaxMsgLine[14] ;

	int				m_nNorAreaShowIndex[_NORAREA_MSG_LINE * 2] ;
	int				m_nNorAreaAmount ;
	int				m_nSysAreaShowIndex[_SYSAREA_MSG_LINE * 2] ;
	int				m_nSysAreaAmount ;
	int				m_nManAreaShowIndex[_MANAREA_MSG_LINE * 2] ;
	int				m_nManAreaAmount ;

	int				m_nMaxLine;

	DWORD			m_dwTime;

	BOOL			m_bChannelTalk;
	BOOL			m_bChannelSyndicate;
	BOOL			m_bChannelFriend;
	BOOL			m_bChannelTeam;
	BOOL			m_bChannelAction;
	BOOL			m_bChannelServe;
	BOOL			m_bChannelPrivateExceptFriend;
	BOOL			m_bChannelPrivateFriend;
	
	DWORD			m_colorChannelTalk; 
	DWORD			m_colorChannelSyndicate;
	DWORD			m_colorChannelFriend;
	DWORD			m_colorChannelTeam;
	DWORD			m_colorChannelPrivate;
	DWORD			m_colorChannelAction;
	DWORD			m_colorChannelServe ;

	char			m_szCaptureName[_MAX_NAMESIZE];
private:
	int m_nWarningTime;
	DEQUE_BLACKNAME	m_setBlackName;

public:
	void	DestroyBlackName();
	void	AddBlackName(char* pszName);
	void	DelBlackName(int nIndex);
	int		GetBlackNameAmount();
	char*	GetBlackNameByIndex(int nIndex);
	BOOL	IsBlackName(char* pszName);
public:
	void	SetChannelColor(unsigned short usChannel, DWORD dwColor);
	void	GetChannelColor(unsigned short usChannel, DWORD& dwColor);

	void	SetChannelStatus(unsigned short usChannel, BOOL bOn);
	void	SetPrivateChannelStatusExceptFriend(BOOL bOn);
	void	SetPrivateChannelStatusFriendOnly(BOOL bOn);
	BOOL	TestPriveteChannel(char* pszName);
	BOOL	GetChannelStatus(unsigned short usChannel);

	char*	GetCaptureName(){return m_szCaptureName;}

	CChatLog		m_objChatLog;
};
extern CGameMsg		g_objGameMsg;
#ifdef _ANALYSIS_ON
extern DWORD g_dwTxtMsgTimeCost;
#endif
#endif
