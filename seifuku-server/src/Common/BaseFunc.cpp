#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "common.h"
#include "BaseFunc.h"

String	DumpBuffer(const char* buf, int nSize)
{
	IF_NOT(buf && nSize > 0 && nSize <= 256)
		return String();

	String str;
	for(int i = 0; i < nSize; i++)
	{
		char bufChar[255];
		sprintf(bufChar, "%02x", (*(buf++) & 0xFF));
		str	+= bufChar;
		if((i % 16) == 15 && i != nSize-1)
			str += ' ';
		else if((i % 4) == 3 && i != nSize-1)
			str	+= '-';
	}
	return str;
}

void	DateTime(char * buf20, time_t tInput )
{
	if(!buf20)
		return;

	if(tInput == 0)
		tInput = time(NULL);

	tm * pTm = localtime(&tInput);
	if(buf20)
		sprintf(buf20, "%04d-%02d-%02d %02d:%02d:%02d", 
					pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
}

void DateTimeStamp(char * buf15, time_t tInput )
{
	if(!buf15)
		return;

	if(tInput == 0)
		tInput = time(NULL);

	tm * pTm = localtime(&tInput);
	if(buf15)
		sprintf(buf15, "%04d%02d%02d%02d%02d%02d", 
					pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
}

int DateStamp(time_t tTime )
{
	time_t	tInput;
	if(tTime)
		tInput	= tTime;
	else
		tInput = time(NULL);

	tm * pTm = localtime(&tInput);
	return (pTm->tm_year+1900)	* 10000
			+ (pTm->tm_mon+1) * 100
			+ pTm->tm_mday;
}

int DateStampPass(int nDate, int nDays)
{
	time_t	tCurr = time(NULL);
	tm	tmComp;
	memset(&tmComp, 0, sizeof(tm));
	tmComp.tm_year	= nDate/10000 - 1900;
	tmComp.tm_mon	= (nDate/100)%100 - 1;
	tmComp.tm_mday	= nDate%100 + nDays;

	time_t	tComp	= mktime(&tmComp);
	if(tComp == -1)
		return -1;

	return DateStamp(tComp);
}

bool IsActiveTime(time_t tCurr, unsigned long nFormat)
{
	tm* pTm = localtime(&tCurr);
	CHECKF(pTm);

	if(nFormat / 100000000)
	{
		if(pTm->tm_mday == int(nFormat/100000000)
			&& pTm->tm_hour == int(nFormat/10000) % 100
			&& pTm->tm_min	== int(nFormat/100) % 100
			&& pTm->tm_sec	== int(nFormat % 100)
			)
			return true;
	}
	else if(nFormat / 1000000)
	{
		if(((pTm->tm_wday+6)%7)+1 == int(nFormat/1000000)
			&& pTm->tm_hour == int(nFormat/10000) % 100
			&& pTm->tm_min	== int(nFormat/100) % 100
			&& pTm->tm_sec	== int(nFormat % 100)
			)
			return true;
	}
	else
	{
		if(pTm->tm_hour == int(nFormat/10000) % 100
			&& pTm->tm_min	== int(nFormat/100) % 100
			&& pTm->tm_sec	== int(nFormat % 100)
			)
			return true;
	}
	return false;
}

bool IsBetween(int nCurr, int nBegin, int nEnd)
{
	if(nBegin <= nEnd)
	{
		if(nBegin <= nCurr && nCurr < nEnd)
			return true;
	}
	else
	{
		if(nCurr < nEnd || nBegin <= nCurr)
			return true;
	}
	return false;
}

bool IsTimeRange(unsigned long nBegin, unsigned long nEnd, time_t tCurr )
{
	if(tCurr == 0)
		tCurr = time(NULL);

	tm* pTm = localtime(&tCurr);
	CHECKF(pTm);

	if(nBegin / 100000000)
	{
		if(IsBetween(pTm->tm_mday*100*1000000 + pTm->tm_hour*10000 + pTm->tm_min*100 + pTm->tm_sec, nBegin, nEnd))
			return true;
	}
	else
	{
		if(IsBetween(pTm->tm_wday*1000000 + pTm->tm_hour*10000 + pTm->tm_min*100 + pTm->tm_sec, nBegin, nEnd))
			return true;
	}
	return false;
}

char*	StrStrCh(const char* string, const char* strCharSet)
{
	CHECKF(string && strCharSet);
	unsigned int	nLen	= strlen(strCharSet);
	if(nLen == 0 || strlen(string) < nLen)
		return NULL;

	char	head	= *strCharSet;
	const char* ptr = string;
	while(*ptr)
	{
		while(*ptr && *ptr != head)
		{
			unsigned char uch = static_cast<unsigned char>(*ptr);
			if(uch >= 0x81 && uch <= 0xFE && *(ptr+1))
				ptr++;
			ptr++;
		}
		if(*ptr && strncmp(ptr, strCharSet, nLen) == 0)
			return const_cast<char*>(ptr);
		ptr++;
	}

	return NULL;
}

DWORD SysTimeGet(void)
{
	time_t long_time;
	time( &long_time );

	struct tm *pTime;
	pTime = localtime( &long_time );

	DWORD dwTime	=pTime->tm_hour*100 + pTime->tm_min;
	return dwTime;
}

DWORD SysTimeGetEx(void)
{
	time_t long_time;
	time( &long_time );

	struct tm *pTime;
	pTime = localtime( &long_time );

	DWORD dwTime	=	pTime->tm_mon *100000000 +
						pTime->tm_mday*1000000 +
						pTime->tm_hour*10000 + 
						pTime->tm_min *100 +
						pTime->tm_sec;
	return dwTime;
}

DWORD TimeGetMinute(void)
{
	time_t long_time;
	time( &long_time );

	struct tm *pTime;
	pTime = localtime( &long_time );

	DWORD dwTime	=	pTime->tm_year%100*100000000 +
						(pTime->tm_mon+1)*1000000 +
						pTime->tm_mday*10000 +
						pTime->tm_hour*100 + 
						pTime->tm_min;
	return dwTime;
}

DWORD TimeGet(void)
{
	return timeGetTime();
}

int RandGet(int nMax, BOOL bReset)
{
	if(nMax <= 0)
		return 0;

	int nHigh = ::rand() << 15;
	return (nHigh + ::rand()) % nMax;
}

double RandomRateGet(double dRange)
{
	double pi=3.1415926;

	int nRandom	=RandGet(999, false)+1;
	double a	=sin(nRandom*(pi/1000));
	double b;
	if (nRandom >= 90)
		b	=(1.0+dRange)-sqrt(sqrt(a))*dRange;
	else
		b	=(1.0-dRange)+sqrt(sqrt(a))*dRange;

	return b;
}

void DDALineEx(int x0, int y0, int x1, int y1, vector<POINT>& vctPoint)
{
	vctPoint.clear();
	if (x0 == x1 && y0 == y1)
		return;

	int dx = x1 - x0;
	int dy = y1 - y0;
	int abs_dx = abs(dx);
	int abs_dy = abs(dy);
	if(abs_dx > abs_dy)
	{
		int _0_5		= abs_dx * (dy>0 ? 1:-1);
		int numerator	= dy * 2;
		int denominator	= abs_dx * 2;
		if(dx > 0)
		{
			for(int i = 1; i <= abs_dx; i ++)
			{
				POINT point;
				point.x = x0 + i;
				point.y = y0 + ((numerator*i + _0_5) / denominator);
				vctPoint.push_back(point);
			}
		}
		else if(dx < 0)
		{
			for(int i = 1; i <= abs_dx; i ++)
			{
				POINT point;
				point.x = x0 - i;
				point.y = y0 + ((numerator*i + _0_5) / denominator);
				vctPoint.push_back(point);
			}
		}
	}
	else
	{
		int _0_5		= abs_dy * (dx>0 ? 1:-1);
		int numerator	= dx * 2;
		int denominator	= abs_dy * 2;
		if(dy > 0)
		{
			for(int i = 1; i <= abs_dy; i ++)
			{
				POINT point;
				point.y = y0 + i;
				point.x = x0 + ((numerator*i + _0_5) / denominator);
				int nAmount = vctPoint.size();
				vctPoint.push_back(point);
			}
		}
		else if(dy < 0)
		{
			for(int i = 1; i <= abs_dy; i ++)
			{
				POINT point;
				point.y = y0 - i;
				point.x = x0 + ((numerator*i + _0_5) / denominator);
				vctPoint.push_back(point);
			}
		}
	}
}

BOOL StringCheck(char* pszString, BOOL bNewLineEnable)
{
	if (!pszString)
		return false;

	int nLen	=strlen(pszString);
	for (int i=0; i < nLen; i++)
	{
		unsigned char c	=(unsigned char)pszString[i];
		if (c >= 0x81 && c <= 0xfe)	
		{
			if (i+1 >= nLen)
				return false;

			unsigned char c2	=(unsigned char)pszString[i+1];
			if (c2 < 0x40 && c2 > 0x7e && c2 < 0x80 && c2 > 0xfe)
				return false;
			else
				i++;
		}
		else
		{
			if (c==0x80)
				return false;
			else
			{
				if(bNewLineEnable)
				{
					if (c < ' ' && c != 0x0A && c != 0x0D)
						return false;
				}
				else
				{
					if (c < ' ')
						return false;
				}
			}
		}
	}

	return true;
}

BOOL NameStrCheck(char* pszString)
{
	if (!pszString)
		return false;

	int nLen	=strlen(pszString);
	for (int i=0; i < nLen; i++)
	{
		unsigned char c	=pszString[i];
		if(c < ' ')
			return false;
		switch(c)
		{
		case ' ':
		case ';':
		case ',':
		case '/':
		case '\\':
		case '=':
		case '%':
		case '@':
		case '\'':
		case '"':
		case '[':
		case ']':
			return false;
		}
	}

	if (strcmp(pszString, "无") == 0)
		return false;

	if (strstr(pszString, "※"))
		return false;

	if (strstr(pszString, ""))
		return false;

	if (strstr(pszString, "　"))
		return false;

	if (strstr(pszString, "【"))
		return false;

	if (strstr(pszString, "】"))
		return false;

	if (strstr(pszString, "ｐｍ"))
		return false;

	if (strstr(pszString, "ＰＭ"))
		return false;

	if (strstr(pszString, "Ｐｍ"))
		return false;

	if (strstr(pszString, "ｐＭ"))
		return false;

	if (strstr(pszString, "ｇｍ"))
		return false;

	if (strstr(pszString, "ＧＭ"))
		return false;

	if (strstr(pszString, "ｇＭ"))
		return false;

	if (strstr(pszString, "Ｇｍ"))
		return false;

	if (strstr(pszString, "Ｇm"))
		return false;

	if (strstr(pszString, "ＧM"))
		return false;

	if (strstr(pszString, "ｇm"))
		return false;

	if (strstr(pszString, "ｇM"))
		return false;

	if (strstr(pszString, "GＭ"))
		return false;

	if (strstr(pszString, "gＭ"))
		return false;

	if (strstr(pszString, "Gｍ"))
		return false;

	if (strstr(pszString, "gｍ"))
		return false;

	if (strstr(pszString, "天晴"))
		return false;

	if (strstr(pszString, "系统"))
		return false;

	if (strstr(pszString, "策划"))
		return false;

	if (strstr(pszString, "服务"))
		return false;

	if (strstr(pszString, "管理"))
		return false;

	if (strstr(pszString, "客服"))
		return false;

	if (strstr(pszString, "公告"))
		return false;

	if (strstr(pszString, "领奖"))
		return false;

	if (strstr(pszString, "帮派"))
		return false;
	if (strstr(pszString, "卫兵"))
		return false;
	if (strstr(pszString, "守卫"))
		return false;
	if (strstr(pszString, "队伍"))
		return false;
	if (strstr(pszString, "好友"))
		return false;
	if (strstr(pszString, "大家"))
		return false;
	if (strstr(pszString, "张三丰"))
		return false;

	char szName[256] = "";
	strcpy(szName, pszString);
	_strupr(szName);
	if (strstr(szName, "GM"))
		return false;
	if (strstr(szName, "PM"))
		return false;
	if (strstr(szName, "SYSTEM"))
		return false;
	if (strstr(szName, "NPC"))
		return false;
	if (strstr(szName, "GUARD"))
		return false;
	if (strstr(szName, "PATROL"))
		return false;
	if (strstr(szName, "NONE"))
		return false;

	return ::DisableWordCheck(pszString);
}

BOOL TalkStrCheck(char* pszString, BOOL bNewLineEnable)
{
	if (!pszString)
		return false;

	int nLen	=strlen(pszString);
	for (int i=0; i < nLen; i++)
	{
		unsigned char c	=pszString[i];
		if(c < ' ' && c != 0x0d && c != 0x0a)
			return false;
		switch(c)
		{
		case '\\':
		case '\'':
		case '"':
			return false;
		}
	}

	return ::DisableWordCheck(pszString, bNewLineEnable);
}

BOOL DisableWordCheck(char* pszString, BOOL bNewLineEnable)
{
	if (strstr(pszString, "法轮"))
		return false;

	if (strstr(pszString, "台独"))
		return false;

	if (strstr(pszString, "独立"))
		return false;

	if (strstr(pszString, "民进"))
		return false;

	if (strstr(pszString, "宏志"))
		return false;

	if (strstr(pszString, "泽民"))
		return false;

	if (strstr(pszString, "锦涛"))
		return false;

	return ::StringCheck(pszString, bNewLineEnable);
}

DWORD TimeGet(TIME_TYPE type)
{
	DWORD dwTime = 0;
	switch(type)
	{
	case TIME_SECOND:
		dwTime = ::time(NULL);
		break;

	case TIME_MINUTE:
		{
			time_t long_time;
			time( &long_time );

			struct tm *pTime;
			pTime = localtime( &long_time );

			dwTime	=	pTime->tm_year%100*100000000 +
						(pTime->tm_mon+1)*1000000 +
						pTime->tm_mday*10000 +
						pTime->tm_hour*100 + 
						pTime->tm_min;
		}
		break;

	case TIME_DAY:
		{
			time_t long_time;
			time( &long_time );

			struct tm *pTime;
			pTime = localtime( &long_time );

			dwTime	=	pTime->tm_year*10000 +
						(pTime->tm_mon+1)*100 +
						pTime->tm_mday;
		}
		break;

	case TIME_DAYTIME: 
		{
			time_t long_time;
			time( &long_time );

			struct tm *pTime;
			pTime = localtime( &long_time );

			dwTime	=	pTime->tm_hour*10000 + 
						pTime->tm_min *100 +
						pTime->tm_sec;
		}
		break;

	case TIME_STAMP: 
		{
			time_t long_time;
			time( &long_time );

			struct tm *pTime;
			pTime = localtime( &long_time );

			dwTime	=	(pTime->tm_mon+1)*100000000 +
						pTime->tm_mday*1000000 +
						pTime->tm_hour*10000 + 
						pTime->tm_min *100 +
						pTime->tm_sec;
		}
		break;

	default:
		dwTime = ::timeGetTime();
		break;
	}

	return dwTime;
}
