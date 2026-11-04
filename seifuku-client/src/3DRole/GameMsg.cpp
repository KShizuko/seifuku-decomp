#include "BaseFunc.h"
#include "nd_bitmap.H"
#include "GameMsg.h"
#include "Player.h"
#include "Hero.h"
#include "GameDataSet.h"
#include "3droledata.h"
#include <ctype.h>
const int  FONT_EDGE_COLOR				= 0x000000;

const OBJID STRING_PREFIX_GLOBAL		= 100005;
const OBJID STRING_PREFIX_SYSTEM		= 100006;
const OBJID STRING_PREFIX_ACTION		= 100007;
const OBJID STRING_PREFIX_WHISPER		= 100008;
const OBJID STRING_PREFIX_TEAM			= 100009;
const OBJID STRING_PREFIX_CLAN			= 100010;
const OBJID STRING_PREFIX_FAMILY		= 100011;
const OBJID STRING_PREFIX_TALK			= 100012;
const OBJID STRING_PREFIX_YELP			= 100013;
const OBJID STRING_PREFIX_FRIEND		= 100014;
const OBJID STRING_PREFIX_GM			= 100015;
const OBJID STRING_PREFIX_GHOST			= 100016;
const OBJID STRING_PREFIX_LEAVEWORD		= 100119;
const OBJID STRING_PREFIX_SERVE			= 100157;

const OBJID STRING_TO					= 100017;
const OBJID STRING_SAY					= 100018;

const OBJID STR_SYSTEM					= 100020;
const OBJID STR_ALLUSERS				= 100026;

const char STRING_SYSTEM[]				= "SYSTEM";
const char STRING_ALL_USERS[]			= "ALLUSERS";

const int	LAST_SHOWLINE_INDEX			= 17;
const int	FONT_NAME					= 10049;
const int  MSGFONT_WIDTH				= 7;
const int  MSGFONT_HEIGHT				= 14;
const int  WARNING_CHAR1				= 100300;
const int  WARNING_CHAR2				= 100301;
const int  WARNING_CHAR3				= 100302;
const int  WARNING_CHAR4				= 100303;

CGameMsg	g_objGameMsg;
#ifdef _ANALYSIS_ON
DWORD g_dwTxtMsgTimeCost	= 0;
#endif

CGameMsgUnit::CGameMsgUnit(void)
{
	this->m_nPrevPosX	= 0;
	this->m_nPrevPosY	= 0;
	this->m_dwTime		= 2500;
	this->m_nStartX		= 0;
	this->m_nDragStep	= 0;
	this->m_nQuakeFlag	= 1;
	this->m_bSuper		= false;

	memset(m_szSender, 0L, sizeof(m_szSender));
	memset(m_szReceiver, 0L, sizeof(m_szReceiver));
	memset(m_szEmotion, 0L, sizeof(m_szEmotion));
	memset(m_szMsg, 0L, sizeof(m_szMsg));

	CMyBitmap::GetFontSize(m_sizeFont);
	CMyBitmap::GetScreenSize(m_sizeScreen.iWidth, m_sizeScreen.iHeight);
}
CGameMsgUnit::~CGameMsgUnit(void)
{
	return;
}
void CGameMsgUnit::GetDateTimeStr(DWORD dwDateTime, char* pszStr)
{
	char szTemp[64];
	int nYear, nMonth, nDay, nHour, nMinute;
	nYear	= dwDateTime/100000000;
	nMonth	= (dwDateTime/1000000)%100;
	nDay	= (dwDateTime/10000)%100;
	nHour	= (dwDateTime/100)%100;
	nMinute	= dwDateTime%100;
	sprintf(szTemp,"[%d:%d]", nHour, nMinute);
	strcpy(pszStr, szTemp);
}
static int s_nFlashDir = 0;
char* CGameMsgUnit::Show(int nStartX, int nStartY, BOOL bCapture)
{
	if(m_nDragStep == 0)
	{
		m_nStartX = m_sizeScreen.iWidth;
		m_nDragStep = 1;
	}
	if(this->m_unTxtStyle & 0x0001)
	{
		if(nStartX < m_nStartX)
			m_nStartX = m_nStartX - 6;
		else
			m_nStartX = nStartX;
	}
	else
	{
		m_nStartX = nStartX;
	}
	if(this->m_unTxtStyle & 0x0002)
	{
		if(this->m_unTxtAttribute == _TXTATR_SYSTEM)
		{
			this->m_dwSystemColor			= 0x00FF00;
			this->m_dwSystemMessageColor	= 0xFF0000;
			this->m_dwSenderColor			= 0xAAFFAA;
			this->m_dwEmotionColor			= 0x00FF00;
			this->m_dwNormalColor			= 0xFFFFFF;
			this->m_dwReceiverColor			= 0xAAFFAA;
			if(::TimeGet() > this->m_dwBeginTime + 3000)
			{
				this->m_unTxtStyle ^= 0x0002;
				this->m_dwWordsColor = 0xFF0000;
			}
			else
			{
				if(s_nFlashDir)
					this->m_dwWordsColorFlash += 0x00080000;
				else
					this->m_dwWordsColorFlash -= 0x00080000;
				if(this->m_dwWordsColorFlash > 0xFF0000)
				{
					this->m_dwWordsColorFlash = 0xFF0000;
					s_nFlashDir = 0;
				}
				else if(this->m_dwWordsColorFlash < 0x770000)
				{
					this->m_dwWordsColorFlash = 0x770000;
					s_nFlashDir = 1;
				}
				this->m_dwWordsColor = this->m_dwWordsColorFlash;
			}
		}
		else
		{
			this->m_dwWordsColorFlash += 0x00000500;
			if(this->m_dwWordsColorFlash > 0xFF00)
				this->m_dwWordsColorFlash = 0x7700;
			this->m_dwSystemColor			= this->m_dwWordsColorFlash;
			this->m_dwSystemMessageColor	= this->m_dwWordsColorFlash;
			this->m_dwSenderColor			= this->m_dwWordsColorFlash;
			this->m_dwEmotionColor			= this->m_dwWordsColorFlash;
			this->m_dwNormalColor			= this->m_dwWordsColorFlash;
			this->m_dwReceiverColor			= this->m_dwWordsColorFlash;
			this->m_dwWordsColor			= this->m_dwWordsColorFlash;
		}
	}
	else
	{
		this->m_dwSystemColor			= 0x00FF00;
		this->m_dwSystemMessageColor	= 0xFF0000;
		this->m_dwSenderColor			= 0xAAFFAA;
		this->m_dwEmotionColor			= 0x00FF00;
		this->m_dwNormalColor			= 0xFFFFFF;
		this->m_dwReceiverColor			= 0xAAFFAA;
	}
	if(this->m_unTxtStyle & 0x0008)
	{
		if(this->m_nQuakeFlag<0)
		{
			if(this->m_nQuakeFlag++>0)
				this->m_nStartX += 2;
		}
		else
		{
			this->m_nQuakeFlag = (-10);
			this->m_nStartX -= 2;
		}
	}
	DWORD dwColorTitle = this->m_dwSystemColor;
	if(strstr(this->m_szMsg, g_objHero.GetName()))
	{
		dwColorTitle = _COLOR_SELF;
	}
	else if(strstr(this->m_szSender, g_objHero.GetName()))
	{
		dwColorTitle = _COLOR_SELF;
	}
	else if(strstr(this->m_szReceiver, g_objHero.GetName()))
	{
		dwColorTitle = _COLOR_SELF;
	}
	CMyPos pos;
	pos.x = m_nStartX;
	pos.y = nStartY;
	switch(this->m_unTxtAttribute)
	{
	case _TXTATR_SYSTEM:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_SYSTEM));
		break;
	case _TXTATR_ACTION:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_ACTION));
		break;
	case _TXTATR_PRIVATE:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_WHISPER));
		break;
	case _TXTATR_TEAM:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_TEAM));
		break;
	case _TXTATR_SYNDICATE:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_CLAN));
		break;
	case _TXTATR_FAMILY:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_FAMILY));
		break;
	case _TXTATR_NORMAL:
	case _TXTATR_TALK:
		if (0 != strcmp(m_szSender, STRING_SYSTEM))
			pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_TALK));
		else
			pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_SYSTEM));
		break;
	case _TXTATR_YELP:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_YELP));
		break;
	case _TXTATR_FRIEND:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_FRIEND));
		break;
	case _TXTATR_GLOBAL:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_GLOBAL));
		break;
	case _TXTATR_GM:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_GM));
		break;
	case _TXTATR_GHOST:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_GHOST));
		break;
	case _TXTATR_LEAVEWORD:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_LEAVEWORD));
		break;
	case _TXTATR_SERVE:
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_SERVE));
		break;
	default:
		this->m_unTxtAttribute = _TXTATR_SYSTEM;
		pos = ShowColorStr(pos.x, pos.y, dwColorTitle, GameDataSetQuery()->GetStr(STRING_PREFIX_SYSTEM));
		break;
	}
	const OBJID idStrSystem=100020;
	if (_TXTATR_ACTION != m_unTxtAttribute &&
			_TXTATR_SYSTEM != m_unTxtAttribute &&
				0 != strcmp(m_szSender, GameDataSetQuery()->GetStr(idStrSystem)) &&
					0 != strcmp(m_szSender, STRING_SYSTEM))
	{
		pos = ShowColorStr(pos.x, pos.y, m_dwSenderColor, m_szSender);
		pos = ShowColorStr(pos.x, pos.y, m_dwEmotionColor, m_szEmotion);
		pos = ShowColorStr(pos.x, pos.y, m_dwNormalColor, GameDataSetQuery()->GetStr(STRING_TO));
		pos = ShowColorStr(pos.x, pos.y, m_dwReceiverColor, m_szReceiver);
		pos = ShowColorStr(pos.x, pos.y, m_dwNormalColor, GameDataSetQuery()->GetStr(STRING_SAY));
	}
	pos = ShowColorStr(pos.x, pos.y, this->m_dwWordsColor, this->m_szMsg);
	CMyPos posMouse;
	::MouseCheck(posMouse.x, posMouse.y);
	OBJID idStrAllUser = 100026;
	if(this->m_unTxtAttribute != _TXTATR_SYSTEM && 0 != strcmp(m_szSender, STRING_SYSTEM))
	{
		if ( 0 == strcmp(g_objHero.GetName(), this->m_szSender) && 0 == strcmp(this->m_szReceiver, GameDataSetQuery()->GetStr(idStrAllUser)))
			return NULL;
		CMySize sizeFont;
		CMyBitmap::GetFontSize(sizeFont);
		CMyPos posRectBegin, posRectEnd;
		posRectBegin.x = m_nStartX;
		posRectBegin.y = nStartY;
		posRectEnd.x = CMyBitmap::GetScreenWidth() - 2;
		posRectEnd.y = pos.y+sizeFont.iHeight+1;
		if(posMouse.x > posRectBegin.x && posMouse.y > posRectBegin.y && posMouse.x < posRectEnd.x && posMouse.y < posRectEnd.y && bCapture)
		{
			static DWORD dwFrame = 0;
			dwFrame ++;
			DWORD dwCurrentFrame = dwFrame%31;
			DWORD dwR, dwG, dwB;
			if(dwCurrentFrame <= 15)
			{
				dwR = 0x11*dwCurrentFrame;
				dwG = 0x11*dwCurrentFrame;
				dwB = 0xff - 0x11*dwCurrentFrame;
			}
			else
			{
				dwR = 0xff - 0x11*(31-dwCurrentFrame);
				dwG = 0xff - 0x11*(31-dwCurrentFrame);
				dwB = 0x11*(31-dwCurrentFrame);
			}
			DWORD dwColor = 0xff000000 + (dwR << 16) + (dwG << 8) + dwB;
			CMyBitmap::ShowRect(posRectBegin.x, posRectBegin.y, posRectEnd.x, posRectEnd.y, dwColor);
			if ( 0 != strcmp(g_objHero.GetName(), this->m_szSender) )
				return this->m_szSender;
			else
				return this->m_szReceiver ;
		}
	}
	return NULL;
}
int CGameMsgUnit::GetLines(void)
{
	return Scan();
}
CMyPos CGameMsgUnit::ShowColorStr(int nPosX, int nPosY
								, DWORD dwColor, const char* pszString)
{
	CMyPos posReturn;
	posReturn.x = nPosX;
	posReturn.y = nPosY;
	if (!pszString || strlen(pszString) < 1)
		return posReturn;
	const char* buf = pszString;
	while(true)
	{
		int nLineLen = (m_sizeScreen.iWidth - m_sizeFont.iWidth * 2 - posReturn.x) / m_sizeFont.iWidth;
		if (nLineLen <= 0)
		{
			posReturn.x = m_nStartX;
			posReturn.y += m_sizeFont.iHeight + 2;
			continue;
		}
		int nStrLen = strlen(buf);
		if (nLineLen >= nStrLen)
		{
			CMyPos posDrawn;
			CMyBitmap::ShowEmotionString(posReturn.x, posReturn.y, dwColor, buf, (IDataIcon*) GameDataSetQuery()->QueryInterface(0), NULL, 0, 0, 0, 0, RENDER_TEXT_OUTLINE, 0xAA444444, posDrawn);
			posReturn.x += m_sizeFont.iWidth * nStrLen;
			break;
		}
		if (CMyBitmap::IsDBCSLeadByte(buf, nLineLen - 1))
			nLineLen --;
		if ( strcmp( GameDataSetQuery()->GetVersionInfo(), "English" ) == 0 )
		{
			for ( int i = nLineLen - 1; i >= 0; i-- )
			{
				if ( buf[i] == ' ' )
				{
					nLineLen -= nLineLen - i - 1;
					break;
				}
			}
		}
		char szMsg[2048] = "";
		strncpy(szMsg, buf, nLineLen);
		CMyPos posDrawn;
		CMyBitmap::ShowEmotionString(posReturn.x, posReturn.y, dwColor, szMsg, (IDataIcon*) GameDataSetQuery()->QueryInterface(0), NULL, 0, 0, 0, 0, RENDER_TEXT_OUTLINE, 0xAA444444, posDrawn);
		buf += nLineLen;
		posReturn.x = m_nStartX;
		posReturn.y += m_sizeFont.iHeight + 2;
	}
	return posReturn;
}
BOOL CGameMsgUnit::GetText(char* pszText)
{
	if(pszText == NULL)
		return false;
	char szTemp[_MAX_WORDSSIZE * 2]="";
	switch(m_unTxtAttribute)
	{
	case _TXTATR_SYSTEM:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_SYSTEM));
		break;
	case _TXTATR_ACTION:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_ACTION));
		break;
	case _TXTATR_PRIVATE:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_WHISPER));
		break;
	case _TXTATR_TEAM:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_TEAM));
		break;
	case _TXTATR_SYNDICATE:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_CLAN));
		break;
	case _TXTATR_FAMILY:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_FAMILY));
		break;
	case _TXTATR_NORMAL:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_TALK));
		break;
	case _TXTATR_TALK:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_TALK));
		break;
	case _TXTATR_YELP:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_YELP));
		break;
	case _TXTATR_FRIEND:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_FRIEND));
		break;
	case _TXTATR_GLOBAL:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_GLOBAL));
		break;
	case _TXTATR_GM:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_GM));
	break;
	case _TXTATR_GHOST:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_GHOST));
		break;
	case _TXTATR_LEAVEWORD:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_LEAVEWORD));
		break;
	case _TXTATR_SERVE:
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_SERVE));
		break ;
	default:
		this->m_unTxtAttribute = _TXTATR_SYSTEM;
		strcpy(szTemp, GameDataSetQuery()->GetStr(STRING_PREFIX_SYSTEM));
		break;
	}
	if(m_unTxtAttribute != _TXTATR_ACTION && m_unTxtAttribute!= _TXTATR_SYSTEM)
		sprintf(szTemp, "%s%s%s%s%s%s",
			szTemp,
			this->m_szSender,
			this->m_szEmotion,
			GameDataSetQuery()->GetStr(STRING_TO),
			this->m_szReceiver,
			GameDataSetQuery()->GetStr(STRING_SAY));
	sprintf(szTemp, "%s%s", szTemp, this->m_szMsg);
	szTemp[_MAX_WORDSSIZE - 1] = '\0' ;
	strcpy(pszText, szTemp);
	GetDateTimeStr(m_dwTime, szTemp);
	sprintf(pszText, "%s%s", pszText, szTemp);
	return true;
}
int CGameMsgUnit::Scan(void)
{
	char szTemp[_MAX_WORDSSIZE * 2];
	GetText(szTemp);
	int nLines = 1;
	char* pszTemp = szTemp;
	unsigned char ucSingle[2] = {'\0', '\0'};
	unsigned char ucDouble[3] = {'\0', '\0', '\0'};
	int nPosX = m_nStartX;
	while((ucSingle[0]=*pszTemp))
	{
		if(ucSingle[0]<0x0A)
		{
			nPosX += m_sizeFont.iWidth;
			if((nPosX - m_nStartX) > (m_sizeScreen.iWidth - m_sizeFont.iWidth * 2))
			{
				nPosX = m_nStartX;
				nLines++;
			}
		}
		else
		{
			if(ucDouble[0]=='\0')
			{
				ucDouble[0] = ucSingle[0];
			}
			else if(ucDouble[1]=='\0')
			{
				ucDouble[1] = ucSingle[0];
				nPosX = nPosX + m_sizeFont.iWidth * 2;
				if((nPosX - m_nStartX) > (m_sizeScreen.iWidth - m_sizeFont.iWidth * 2))
				{
					nPosX = m_nStartX;
					nLines++;
				}
				ucDouble[0]='\0';
				ucDouble[1]='\0';
			}
		}
		pszTemp++;
	}
	return nLines;
}
CGameMsg::CGameMsg()
{
	m_setMsg.SetMaxMsg(2048);
	m_dwTime	= 0;
	m_bChannelTalk			=	true;
	m_bChannelSyndicate		=	true;
	m_bChannelFriend		=	true;
	m_bChannelTeam			=	true;
	m_bChannelAction        =   true;
	m_bChannelServe			=   true;
	m_bChannelPrivateExceptFriend	=	true;
	m_bChannelPrivateFriend			=	true;
	strcpy(m_szCaptureName, "");
	m_colorChannelTalk			=	0xffffff00;
	m_colorChannelSyndicate		=	0xffffff00;
	m_colorChannelFriend		=	0xffffff00;
	m_colorChannelTeam			=	0xffffff00;
	m_colorChannelPrivate		=	0xffffff00;
	m_colorChannelAction		=	0xffffff00;
	m_colorChannelServe			=	0xffffff00;
	m_nMaxMsgLine[0] = 20 ;
	m_nMaxMsgLine[1] = 16 ;
	m_nMaxMsgLine[2] = 20 ;
	m_nMaxMsgLine[3] = 20 ;
	m_nMaxMsgLine[4] = 3 ;
	m_nMaxMsgLine[5] = 16 ;
	m_nMaxMsgLine[6] = 20 ;
	m_nMaxMsgLine[7] = 16 ;
	m_nMaxMsgLine[8] = 20 ;
	m_nMaxMsgLine[9] = 16 ;
	m_nMaxMsgLine[10] = 1 ;
	m_nMaxMsgLine[11] = 16 ;
	m_nMaxMsgLine[12] = 16 ;
	m_nMaxMsgLine[13] = 20 ;
	m_nNorAreaAmount = 0 ;
	m_nSysAreaAmount = 0 ;
	m_nManAreaAmount = 0 ;
	m_nWarningTime = 0 ;
}
CGameMsg::~CGameMsg()
{
	this->ClearMsg();
	this->DestroyBlackName();
}
void CGameMsg::Show(BOOL bCapture)
{
#ifdef _ANALYSIS_ON
	DWORD dwTimeBegin = ::TimeGet();
#endif
	int nScrWidth, nScrHeight;
	CMyBitmap::GetScreenSize(nScrWidth, nScrHeight);
	int nFontHeight;
	nFontHeight	= CMyBitmap::GetFontSize();
	int nHalfFontHeight = (int)(nFontHeight / 2.0f + 0.5f);
	int i;
	CMyPos pos;
	pos.x = 2;
	pos.y = nFontHeight * (2 + _SYSAREA_MSG_LINE);
	try
	{
		for ( i = m_nSysAreaAmount - 1; i >= 0; i-- )
		{
			CGameMsgUnit* pMsg = m_dequeMsg[m_nSysAreaShowIndex[i * 2]][m_nSysAreaShowIndex[i * 2 + 1]] ;
			if( pMsg )
			{
				if ( ::TimeGet() > pMsg->m_dwBeginTime + 15 * 1000 )
				{
					m_dequeMsg[m_nSysAreaShowIndex[i * 2]].erase ( m_dequeMsg[m_nSysAreaShowIndex[i * 2]].begin() + i ) ;
					SAFE_DELETE(pMsg);
					m_nSysAreaAmount-- ;
					for ( int j = i;  j < m_nSysAreaAmount; j++ )
					{
						m_nSysAreaShowIndex[j * 2] = m_nSysAreaShowIndex[( j + 1 ) * 2]  ;
						m_nSysAreaShowIndex[j * 2 + 1] = m_nSysAreaShowIndex[( j + 1 ) * 2 + 1]  ;
						m_nSysAreaShowIndex[j * 2 + 1]-- ;
					}
					break;
				}
				int nLines = pMsg->GetLines();
				pos.y = pos.y - (nFontHeight+2) * nLines;
				pMsg->Show(pos.x, pos.y, false);
			}
		}
	}
	catch(...)
	{
		char szTemp[384];
		sprintf(szTemp, "Display system area message,Amount=%d,DequeIndex=%d,MsgIndex=%d", m_nSysAreaAmount, m_nSysAreaShowIndex[i * 2], m_nSysAreaShowIndex[i * 2 + 1]);
		::MessageBox(g_hGameWnd, szTemp, "Error", MB_OK);
		return;
	}
	pos.x = 100;
	pos.y = (nScrHeight - nFontHeight - 130) / 2;
	try
	{
		for ( i = m_nManAreaAmount - 1; i >= 0; i-- )
		{
			CGameMsgUnit* pMsg = m_dequeMsg[m_nManAreaShowIndex[i * 2]][m_nManAreaShowIndex[i * 2 + 1]] ;
			if ( pMsg )
			{
				if ( ::TimeGet() > pMsg->m_dwBeginTime + 15 * 1000 )
				{
					m_dequeMsg[m_nManAreaShowIndex[i * 2]].erase ( m_dequeMsg[m_nManAreaShowIndex[i * 2]].begin() + i ) ;
					SAFE_DELETE(pMsg);
					m_nManAreaAmount-- ;
					for ( int j = i;  j < m_nManAreaAmount; j++ )
					{
						m_nManAreaShowIndex[j * 2] = m_nManAreaShowIndex[( j + 1 ) * 2]  ;
						m_nManAreaShowIndex[j * 2 + 1] = m_nManAreaShowIndex[( j + 1 ) * 2 + 1]  ;
						m_nManAreaShowIndex[j * 2 + 1]-- ;
					}
					break;
				}
				int nLines = pMsg->GetLines();
				pos.y = pos.y - (nFontHeight+2) * nLines;
				pMsg->Show(pos.x, pos.y, false);
			}
		}
	}
	catch(...)
	{
		char szTemp[384];
		sprintf(szTemp, "Display manage area message,Amount=%d,DequeIndex=%d,MsgIndex=%d", m_nManAreaAmount, m_nManAreaShowIndex[i * 2], m_nManAreaShowIndex[i * 2 + 1]);
		::MessageBox(g_hGameWnd, szTemp, "Error", MB_OK);
		return;
	}
	int nTotalLines = 0;
	pos.x = 2;
	pos.y = nScrHeight - 130;
	BOOL bGetCapturename = false;
	try
	{
		for ( i = m_nNorAreaAmount - 1; i >= 0; i-- )
		{
			CGameMsgUnit* pMsg = m_dequeMsg[m_nNorAreaShowIndex[i * 2]][m_nNorAreaShowIndex[i * 2 + 1]] ;
			if ( pMsg )
			{
				if(pMsg->m_unTxtAttribute == _TXTATR_GHOST
					&& g_objHero.GetProfessionSort() != 13 && !g_objHero.IsGhost())
					continue;
				int nLines = pMsg->GetLines();
				pos.y = pos.y - (nFontHeight+2) * nLines;
				char* pszName = pMsg->Show(pos.x, pos.y, bCapture);
				if(pszName)
				{
					strcpy(m_szCaptureName, pszName);
					bGetCapturename = true;
				}
				nTotalLines += nLines;
				if (nTotalLines >= m_nMaxLine)
				{
					if(bCapture)
					{
						const OBJID idStr = 100130;
						CMyBitmap::ShowString(pos.x+6, pos.y-nFontHeight-1, 0xff000000, GameDataSetQuery()->GetStr(idStr));
						CMyBitmap::ShowString(pos.x+5, pos.y-nFontHeight-2, 0xffffff00, GameDataSetQuery()->GetStr(idStr));
					}
					break;
				}
			}
		}
	}
	catch(...)
	{
		char szTemp[384];
		sprintf(szTemp, "Display normal area message,Amount=%d,DequeIndex=%d,MsgIndex=%d", m_nNorAreaAmount, m_nNorAreaShowIndex[i * 2], m_nNorAreaShowIndex[i * 2 + 1]);
		::MessageBox(g_hGameWnd, szTemp, "Error", MB_OK);
		return;
	}
	if(!bGetCapturename)
		strcpy(m_szCaptureName, "");
	m_dwTime++;
#ifdef _ANALYSIS_ON
	g_dwTxtMsgTimeCost += ::TimeGet() - dwTimeBegin;
#endif
}
static BOOL IsEmotionCode(const char* pszEmotion)
{
	if (!pszEmotion)
		return false;
	if (pszEmotion[0] != '#')
		return false;
	for (const char* p = pszEmotion; *p; p++)
	{
		if (!isdigit(*p))
			return false;
	}
	return true;
}
BOOL CGameMsg::AddMsg ( const char *pszSender,
					    char *pszReceiver,
						char *pszEmotion,
						char *pszMsg,
						DWORD dwWordsColor,
						unsigned short unTxtStyle,
						unsigned short unTxtAttribute,
						DWORD dwTime)
{
	if ( ( strlen( pszSender ) == 0 ) ||
			( !pszMsg || _MAX_WORDSSIZE <= strlen( pszMsg ) ) ||
				( !pszSender || _MAX_NAMESIZE <= strlen( pszSender ) ) ||
					( !pszReceiver || _MAX_NAMESIZE <= strlen( pszReceiver ) ) ||
						( pszEmotion && 32 <= strlen( pszEmotion ) ) )
		return false;
	if ( unTxtAttribute == _TXTATR_SYSTEM )
	{
		for (int i = 0; i < m_nSysAreaAmount; i++ )
		{
			CGameMsgUnit* pMsg = m_dequeMsg[m_nSysAreaShowIndex[i * 2]][m_nSysAreaShowIndex[i * 2 + 1]] ;
			if( pMsg )
			{
				if ( 0 == strcmp(pMsg->m_szMsg, pszMsg))
				{
					pMsg->m_dwBeginTime = ::TimeGet();
					pMsg->m_unTxtStyle |= 0x0002;
					pMsg->m_dwTime = dwTime;
					return false;
				}
			}
		}
	}
	if (m_nWarningTime < 3 && 0 != strcmp(pszSender, g_objHero.GetName()))
	{
		if (strstr(pszMsg, "密码") || strstr(pszMsg, "密 码") || strstr(pszMsg, "密  码") || strstr(pszMsg, "密　码"))
		{
			m_nWarningTime++;
			this->AddMsg(GameDataSetQuery()->GetStr(100160), _TXTATR_GM, _COLOR_SYSTEM_MESSAGE, 0, false);
		}
	}
	CGameMsgUnit* pNewMsg = new CGameMsgUnit;
	if(!pNewMsg)
		return false;
	char szTemp[_MAX_WORDSSIZE * 2];
	strcpy(szTemp, pszMsg);
	if(_TXTATR_ACTION == unTxtAttribute)
		sprintf(szTemp, pszMsg, pszSender, pszReceiver);
	strcpy(pNewMsg->m_szMsg, szTemp);
	strcpy(pNewMsg->m_szSender, pszSender);
	if (strcmp(pszReceiver,STRING_ALL_USERS) == 0)
		strcpy(pNewMsg->m_szReceiver, GameDataSetQuery()->GetStr(STR_ALLUSERS));
	else
		strcpy(pNewMsg->m_szReceiver, pszReceiver);
	if (unTxtAttribute != _TXTATR_LEAVEWORD)
	{
		if (IsEmotionCode(pszEmotion))
		{
			strncpy(pNewMsg->m_szEmotion, pszEmotion, _MAX_WORDSSIZE);
			pNewMsg->m_szEmotion[_MAX_WORDSSIZE - 1] = '\0';
		}
		else
			pNewMsg->m_szEmotion[0] = '\0';
	}
	pNewMsg->m_dwWordsColor = dwWordsColor;
	g_objGameMsg.GetChannelColor(unTxtAttribute, pNewMsg->m_dwWordsColor);
	pNewMsg->m_unTxtStyle	= unTxtStyle;
	pNewMsg->m_unTxtAttribute = unTxtAttribute;
	pNewMsg->m_dwTime		= dwTime;
	pNewMsg->m_dwBeginTime	= ::TimeGet();
	pNewMsg->m_nStartX		= 0;
	pNewMsg->m_nDragStep	= 0;
	pNewMsg->m_nQuakeFlag	= 1;
	pNewMsg->m_bSuper		= false;
	if ( pNewMsg->m_unTxtAttribute == _TXTATR_LEAVEWORD )
	{
		if ( strcmp( pszSender, g_objHero.GetName() ) != 0 )
			m_objChatLog.LogLeaveWord ( pNewMsg ) ;
		SAFE_DELETE( pNewMsg ) ;
		return true;
	}
	m_objChatLog.Log ( pNewMsg ) ;
	int		nDequeIndex = pNewMsg->m_unTxtAttribute - _TXTATR_NORMAL - 1 ;
	if ( pNewMsg->m_unTxtAttribute == _TXTATR_NORMAL )
		nDequeIndex = 6 ;
	try
	{
		m_dequeMsg[nDequeIndex].push_back ( pNewMsg ) ;
	}
	catch(...)
	{
		char szErr[256];
		sprintf(szErr, "DequeMsg%d push_back error", nDequeIndex);
		::MessageBox(g_hGameWnd, szErr, "Error", MB_OK);
		return false;
	}
	int	nDequeSize = ( int ) m_dequeMsg[nDequeIndex].size() ;
	if ( nDequeSize > m_nMaxMsgLine[nDequeIndex] )
	{
		try
		{
			SAFE_DELETE ( m_dequeMsg[nDequeIndex][0] ) ;
			m_dequeMsg[nDequeIndex].pop_front();
		}
		catch(...)
		{
			char szErr[256];
			sprintf(szErr, "DequeMsg%d SAFE_DELETE or pop_front error", nDequeIndex);
			::MessageBox(g_hGameWnd, szErr, "Error", MB_OK);
			return false;
		}
		nDequeSize-- ;
		if ( pNewMsg->m_unTxtAttribute == _TXTATR_SYSTEM )
			ProcessArea ( m_nSysAreaAmount, _SYSAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nSysAreaShowIndex, true ) ;
		else if ( pNewMsg->m_unTxtAttribute == _TXTATR_GM )
			ProcessArea ( m_nManAreaAmount, _MANAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nManAreaShowIndex, true ) ;
		else
			ProcessArea ( m_nNorAreaAmount, _NORAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nNorAreaShowIndex, true ) ;
	}
	else
	{
		if ( pNewMsg->m_unTxtAttribute == _TXTATR_SYSTEM )
			ProcessArea ( m_nSysAreaAmount, _SYSAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nSysAreaShowIndex, false ) ;
		else if ( pNewMsg->m_unTxtAttribute == _TXTATR_GM )
			ProcessArea ( m_nManAreaAmount, _MANAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nManAreaShowIndex, false ) ;
		else
			ProcessArea ( m_nNorAreaAmount, _NORAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nNorAreaShowIndex, false ) ;
	}
	pNewMsg->GetText(szTemp);
	szTemp[_MAX_WORDSSIZE-1] = '\0';
	m_setMsg.AddMsg(szTemp);
	return true;
}
BOOL CGameMsg::AddMsg ( const char* pszMsg,
						DWORD dwChannel,
						DWORD dwWordsColor,
						DWORD dwStyle, BOOL bSuper)
{
	if ( !pszMsg || _MAX_WORDSSIZE <= strlen ( pszMsg ) )
		return false;
	if ( dwChannel == _TXTATR_SYSTEM )
	{
		for (int i = 0; i < m_nSysAreaAmount; i++ )
		{
			CGameMsgUnit* pMsg = m_dequeMsg[m_nSysAreaShowIndex[i * 2]][m_nSysAreaShowIndex[i * 2 + 1]] ;
			if( pMsg )
			{
				if ( 0 == strcmp(pMsg->m_szMsg, pszMsg))
				{
					pMsg->m_dwBeginTime = ::TimeGet();
					pMsg->m_dwTime = ::SysTimeGet();
					pMsg->m_unTxtStyle |= 0x0002;
					return false;
				}
			}
		}
	}
	CGameMsgUnit* pNewMsg = new CGameMsgUnit;
	if(!pNewMsg)
		return false;
	strcpy(pNewMsg->m_szMsg, pszMsg);
	strcpy(pNewMsg->m_szSender, STRING_SYSTEM);
	strcpy(pNewMsg->m_szReceiver, STRING_ALL_USERS);
	strcpy(pNewMsg->m_szEmotion, "");
	pNewMsg->m_dwWordsColor		= dwWordsColor;
	g_objGameMsg.GetChannelColor(dwChannel, pNewMsg->m_dwWordsColor);
	pNewMsg->m_unTxtStyle		= dwStyle;
	pNewMsg->m_unTxtAttribute	= dwChannel;
	pNewMsg->m_nStartX			= 0;
	pNewMsg->m_nDragStep		= 0;
	pNewMsg->m_nQuakeFlag		= 1;
	pNewMsg->m_bSuper			= bSuper;
	pNewMsg->m_dwTime		= ::SysTimeGet();
	pNewMsg->m_dwBeginTime	= ::TimeGet();
	if ( pNewMsg->m_unTxtAttribute == _TXTATR_LEAVEWORD )
	{
		if ( strcmp( pNewMsg->m_szSender, g_objHero.GetName() ) != 0 )
			m_objChatLog.LogLeaveWord ( pNewMsg ) ;
		SAFE_DELETE( pNewMsg ) ;
		return true;
	}
	m_objChatLog.Log ( pNewMsg ) ;
	int		nDequeIndex = pNewMsg->m_unTxtAttribute - _TXTATR_NORMAL - 1 ;
	if ( pNewMsg->m_unTxtAttribute == _TXTATR_NORMAL )
		nDequeIndex = 6 ;
	try
	{
		m_dequeMsg[nDequeIndex].push_back ( pNewMsg ) ;
	}
	catch(...)
	{
		char szErr[256];
		sprintf(szErr, "DequeMsg%d push_back error", nDequeIndex);
		::MessageBox(g_hGameWnd, szErr, "Error", MB_OK);
		return false;
	}
	int	nDequeSize = ( int ) m_dequeMsg[nDequeIndex].size() ;
	if ( nDequeSize > m_nMaxMsgLine[nDequeIndex] )
	{
		try
		{
			SAFE_DELETE ( m_dequeMsg[nDequeIndex][0] ) ;
			m_dequeMsg[nDequeIndex].pop_front();
		}
		catch(...)
		{
			char szErr[256];
			sprintf(szErr, "DequeMsg%d SAFE_DELETE or pop_front error", nDequeIndex);
			::MessageBox(g_hGameWnd, szErr, "Error", MB_OK);
			return false;
		}
		nDequeSize-- ;
		if ( pNewMsg->m_unTxtAttribute == _TXTATR_SYSTEM )
			ProcessArea ( m_nSysAreaAmount, _SYSAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nSysAreaShowIndex, true ) ;
		else if ( pNewMsg->m_unTxtAttribute == _TXTATR_GM )
			ProcessArea ( m_nManAreaAmount, _MANAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nManAreaShowIndex, true ) ;
		else
			ProcessArea ( m_nNorAreaAmount, _NORAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nNorAreaShowIndex, true ) ;
	}
	else
	{
		if ( pNewMsg->m_unTxtAttribute == _TXTATR_SYSTEM )
			ProcessArea ( m_nSysAreaAmount, _SYSAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nSysAreaShowIndex, false ) ;
		else if ( pNewMsg->m_unTxtAttribute == _TXTATR_GM )
			ProcessArea ( m_nManAreaAmount, _MANAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nManAreaShowIndex, false ) ;
		else
			ProcessArea ( m_nNorAreaAmount, _NORAREA_MSG_LINE, nDequeIndex, nDequeSize, m_nNorAreaShowIndex, false ) ;
	}
	char szTemp[_MAX_WORDSSIZE * 2];
	pNewMsg->GetText(szTemp);
	szTemp[_MAX_WORDSSIZE-1] = '\0';
	m_setMsg.AddMsg(szTemp);
	return true;
}
BOOL CGameMsg::AddMsg(DWORD dwChannel, const char* fmt, ...)
{
    char szMsg[1024];
    vsprintf( szMsg, fmt, (char*) ((&fmt)+1) );
	return this->AddMsg(szMsg, dwChannel);
}
void CGameMsg::ClearMsg(void)
{
	for ( int i = 0; i < 14; i++ )
	{
		int nCount = m_dequeMsg[i].size() ;
		for ( int j = 0; j < nCount; j++ )
		{
			SAFE_DELETE ( m_dequeMsg[i][j] ) ;
		}
		m_dequeMsg[i].clear() ;
	}
	m_nNorAreaAmount = 0 ;
	m_nSysAreaAmount = 0 ;
	m_nManAreaAmount = 0 ;
}
void CGameMsg::ProcessArea ( int& nCurAmount, int nMsgAmount, int nDequeIndex, int nDequeSize, int* pIndex, BOOL bFull )
{
	nDequeSize-- ;
	if ( nCurAmount == nMsgAmount )
	{
		for ( int i = 0; i < nMsgAmount - 1; i++ )
		{
			pIndex[i * 2] = pIndex[( i + 1 ) * 2] ;
			pIndex[i * 2 + 1] = pIndex[( i + 1 ) * 2 + 1] ;
			
			if ( bFull && pIndex[i * 2] == nDequeIndex )
				pIndex[i * 2 + 1]-- ;
		}
		pIndex[ ( nMsgAmount - 1 ) * 2] = nDequeIndex ;
		pIndex[ ( nMsgAmount - 1 ) * 2 + 1] = nDequeSize ;
	}
	else
	{
		for ( int i = 0; i < nCurAmount; i++ )
		{
			if ( bFull && pIndex[i * 2] == nDequeIndex )
				pIndex[i * 2 + 1]-- ;
		}
		pIndex[nCurAmount * 2] = nDequeIndex ;
		pIndex[nCurAmount * 2 + 1] = nDequeSize ;
		nCurAmount++ ;
	}
}

void	CGameMsg::DestroyBlackName()
{
	int nAmount = m_setBlackName.size();
	for(int i = 0; i < nAmount; i ++)
	{
		char* pszName = m_setBlackName[i];
		if(pszName)
			delete[] pszName;
	}
	m_setBlackName.clear();
}
void	CGameMsg::AddBlackName(char* pszName)
{
	if(pszName && strlen(pszName) > 0)
	{
		char* pszBlackName = new char[strlen(pszName)+1];
		strcpy(pszBlackName, pszName);
		m_setBlackName.push_back(pszBlackName);
	}
}
void	CGameMsg::DelBlackName(int nIndex)
{
	int nAmount = this->GetBlackNameAmount();
	if((nIndex < 0) && (nIndex >= nAmount))
		return;
	char* pszName = m_setBlackName[nIndex];
	delete[] pszName;
	m_setBlackName.erase(m_setBlackName.begin() + nIndex);
}
int		CGameMsg::GetBlackNameAmount()
{
	return m_setBlackName.size();
}
char*	CGameMsg::GetBlackNameByIndex(int nIndex)
{
	int nAmount = this->GetBlackNameAmount();
	if((nIndex < 0) && (nIndex >= nAmount))
		return NULL;
	return m_setBlackName[nIndex];
}
BOOL	CGameMsg::IsBlackName(char* pszName)
{
	if(!pszName)
		return true;
	int nAmount = this->GetBlackNameAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		char* pszMyName = m_setBlackName[i];
		if(pszMyName && strcmp(pszName, pszMyName)==0)
		{
			return true;
		}
	}
	return false;
}
void	CGameMsg::GetChannelColor(unsigned short usChannel, DWORD& dwColor)
{
	switch(usChannel)
	{
	case _TXTATR_NORMAL:
		dwColor = m_colorChannelTalk;
		break;
	case _TXTATR_TALK:
		dwColor = m_colorChannelTalk;
		break;
	case _TXTATR_FRIEND:
		dwColor = m_colorChannelFriend;
		break;
	case _TXTATR_SYNDICATE:
		dwColor = m_colorChannelSyndicate;
		break;
	case _TXTATR_TEAM:
		dwColor = m_colorChannelTeam;
		break;
	case _TXTATR_PRIVATE:
		dwColor = m_colorChannelPrivate;
		break;
	case _TXTATR_ACTION:
		dwColor = m_colorChannelAction;
		break ;
	case _TXTATR_SERVE:
		dwColor = m_colorChannelServe;
		break ;
	default:
		break;
	}
}
void	CGameMsg::SetChannelColor(unsigned short usChannel, DWORD dwColor)
{
	switch(usChannel)
	{
	case _TXTATR_NORMAL:
	case _TXTATR_TALK:
		m_colorChannelTalk = dwColor;
		break;
	case _TXTATR_FRIEND:
		m_colorChannelFriend = dwColor;
		break;
	case _TXTATR_SYNDICATE:
		m_colorChannelSyndicate = dwColor;
		break;
	case _TXTATR_TEAM:
		m_colorChannelTeam = dwColor;
		break;
	case _TXTATR_PRIVATE:
		m_colorChannelPrivate = dwColor;
		break;
	case _TXTATR_ACTION:
		m_colorChannelAction = dwColor;
		break ;
	case _TXTATR_SERVE:
		m_colorChannelServe = dwColor;
		break ;
	default:
		break;
	}
}
void	CGameMsg::SetChannelStatus(unsigned short usChannel, BOOL bOn)
{
	switch(usChannel)
	{
	case _TXTATR_TALK:
		m_bChannelTalk = bOn;
		break;
	case _TXTATR_FRIEND:
		m_bChannelFriend = bOn;
		break;
	case _TXTATR_SYNDICATE:
		m_bChannelSyndicate = bOn;
		break;
	case _TXTATR_TEAM:
		m_bChannelTeam = bOn;
		break;
	case _TXTATR_ACTION:
		m_bChannelAction = bOn;
		break ;
	case _TXTATR_SERVE:
		m_bChannelServe = bOn;
		break ;
	default:
		break;
	}
}
BOOL	CGameMsg::GetChannelStatus(unsigned short usChannel)
{
	switch(usChannel)
	{
	case _TXTATR_TALK:
		return m_bChannelTalk;
	case _TXTATR_FRIEND:
		return m_bChannelFriend;
	case _TXTATR_SYNDICATE:
		return m_bChannelSyndicate;
	case _TXTATR_TEAM:
		return m_bChannelTeam;
	case _TXTATR_ACTION:
		return m_bChannelAction;
	case _TXTATR_SERVE:
		return m_bChannelServe;
	default:
		return true;
	}
	return true;
}
void	CGameMsg::SetPrivateChannelStatusExceptFriend(BOOL bOn)
{
	m_bChannelPrivateExceptFriend = bOn;
}
void	CGameMsg::SetPrivateChannelStatusFriendOnly(BOOL bOn)
{
	m_bChannelPrivateFriend = bOn;
}
BOOL	CGameMsg::TestPriveteChannel(char* pszName)
{
	if(m_bChannelPrivateExceptFriend)
		return true;
	if(m_bChannelPrivateFriend)
	{
		if(!pszName)
			return false;
		if(g_objHero.GetFriendByName(pszName))
			return true;
	}
	return false;
}
