#ifndef _SAYHELLO_H
#define _SAYHELLO_H

#include "windows.h"
#include "BaseFunc.h"

class CSayHello
{
public:
	CSayHello();
	~CSayHello();

private:
	char	m_szWords[1024];
	DWORD	m_dwTimeStart;
	int GetLine(const char* pszWords, int* pnOffset, char* pszLine, int nLineSize);

public:
	void ShowWords(int iScrX, int iScrY, int iDir);
	void SetWords(const char* pszWords);
	void Clear();

	friend struct SayHelloProbe;
};
#endif
