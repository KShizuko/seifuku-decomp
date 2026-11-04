#if !defined(AFX_ENCRYPTFILE_H__SFK_4343_INCLUDED_)
#define AFX_ENCRYPTFILE_H__SFK_4343_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

class CEncryptFile
{
public:
	CEncryptFile(unsigned int uSeed);
	virtual ~CEncryptFile();
	bool Open(const char* pszFile);
	void Close();
	int  ReadLine(char* pszLine, int nMaxSize);
	const char* GetBuffer() const { return m_pBuf; }
	int GetSize() const { return m_nSize; }
	int GetPos() const { return m_nPos; }
	bool IsEnd() const { return m_nPos >= m_nSize; }
private:
	void Decode(char* pBuf, int nSize);
	char* m_pBuf;
	int   m_nSize;
	int   m_nPos;
	char  m_szKey[128];
};

#endif
