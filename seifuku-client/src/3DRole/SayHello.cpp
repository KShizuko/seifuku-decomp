#include "SayHello.h"
#include "Gamedataset.h"
#include "nd_bitmap.h"
#include <list>
#include <string>
const int _SAY_HELLO_SHOW_TIME = 6000;

CSayHello::CSayHello()
{
}

CSayHello::~CSayHello()
{
}

void CSayHello::SetWords(const char* pszWords)
{
	if(!pszWords)
		return;

	if(strlen(pszWords) >= 1024)
		return;
	m_dwTimeStart	=::TimeGet();
	strncpy(m_szWords, pszWords, 64);
}

int CSayHello::GetLine(const char* pszWords, int* pnOffset, char* pszLine, int nLineSize)
{
	if (!pszWords || !pszLine)
		return 0;
	int nLen = strlen(pszWords);
	if (*pnOffset >= nLen)
		return 0;
	memset(pszLine, 0, nLineSize);
	strncpy(pszLine, pszWords + *pnOffset, nLineSize - 1);
	int n = strlen(pszLine);
	if (n < nLineSize - 1)
	{
		*pnOffset += n;
		return n;
	}
	if (*pnOffset + n >= nLen)
	{
		*pnOffset += n;
		return n;
	}
	switch (pszLine[n - 1])
	{
	case ' ': case '!': case '"': case '$': case '&': case '\'': case '(': case ')': case '*': case '+': case ',': case '-': case '.': case '/':
	case ';': case '<': case '>': case '?': case '@': case '[': case '\\': case ']': case '^': case '_': case '{': case '|': case '}': case '~':
		*pnOffset += n;
		return n;
	}
	if (::IsDBCSLeadByte(pszLine, n - 1))
	{
		pszLine[n - 1] = '\0';
		n--;
		*pnOffset += n;
		return n;
	}
	if (::IsDBCSLeadByte(pszLine, n - 2))
	{
		*pnOffset += n;
		return n;
	}
	if (pszLine[n - 1] == '#')
	{
		pszLine[n - 1] = '\0';
		n--;
		*pnOffset += n;
		return n;
	}
	if (pszLine[n - 1] >= '0' && pszLine[n - 1] <= '9')
	{
		if (n >= 2 && pszLine[n - 2] == '#')
		{
			pszLine[n - 2] = '\0';
			n -= 2;
			*pnOffset += n;
			return n;
		}
		if (n >= 3 && pszLine[n - 2] >= '0' && pszLine[n - 2] <= '9' && pszLine[n - 3] == '#')
		{
			*pnOffset += n;
			return n;
		}
	}
	char cNext = pszWords[*pnOffset + n];
	if (cNext != ' ' && cNext != ',' && cNext != ';' && cNext != '.' && cNext != '!' && cNext != '?' && cNext != ':')
	{
		char* p = strrchr(pszLine, ' ');
		if (p)
		{
			p++;
			*p = '\0';
			n = strlen(pszLine);
			*pnOffset += n;
			return n;
		}
	}
	*pnOffset += n;
	return n;
}

void CSayHello::ShowWords(int iScrX, int iScrY, int iDir)
{
	if(strlen(m_szWords) <= 0)
		return;
	if( ::TimeGet() - m_dwTimeStart >= _SAY_HELLO_SHOW_TIME )
		return;

	iDir = (iDir + 7) % 8;
	if(iDir < 4)
		iDir = 0;
	else
		iDir = 1;

	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "Dialog", EXIGENCE_IMMEDIATE);
	if(!pAni)
		return;
	CMyPos posPic;
	if(iDir == 0)
	{
		pAni->Show(0, iScrX, iScrY-200);
		posPic.x = iScrX + 12;
		posPic.y = iScrY - 192;
	}
	else
	{
		pAni->Show(1, iScrX-170, iScrY-200);
		posPic.x = iScrX - 158;
		posPic.y = iScrY - 192;
	}

	const CMySize sizeBmp = {160, 102};
	int nMaxLine = (sizeBmp.iHeight - 48) / CMyBitmap::GetFontSize();
	int nMaxWord = (sizeBmp.iWidth - 24) / (CMyBitmap::GetFontSize() / 2);
	std::list<std::string> lstLine;
	int nOffset = 0;
	while(true)
	{
		char szLine[256] = "";
		int n = GetLine(m_szWords, &nOffset, szLine, nMaxWord);
		if(n == 0)
			break;
		lstLine.push_back(std::string(szLine));
	}
	int i = 0;
	for(std::list<std::string>::iterator it = lstLine.begin(); it != lstLine.end(); it++, i++)
	{
		CMyBitmap::ShowEmotionString(posPic.x, posPic.y + i * CMyBitmap::GetFontSize(), _COLOR_YELLOW, it->c_str(), (IDataIcon*) GameDataSetQuery()->QueryInterface(0), NULL, 14, 0, 0, 0);
	}
}
void CSayHello::Clear()
{
	strcpy(m_szWords, "");
}
