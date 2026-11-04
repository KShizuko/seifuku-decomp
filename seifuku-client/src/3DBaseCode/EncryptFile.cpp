#include "EncryptFile.h"
#include <string.h>

CEncryptFile::CEncryptFile(unsigned int uSeed)
{
	m_pBuf = NULL;
	m_nSize = 0;
	m_nPos = 0;
	::srand(uSeed);
	for (int i = 0; i < 128; i++)
		m_szKey[i] = (char)(::rand() % 256);
}
CEncryptFile::~CEncryptFile()
{
	this->Close();
}
void CEncryptFile::Close()
{
	if (m_pBuf)
		free(m_pBuf);
	m_pBuf = NULL;
	m_nSize = 0;
	m_nPos = 0;
}
bool CEncryptFile::Open(const char* pszFile)
{
	if (!pszFile)
		return false;
	this->Close();
	FILE* fp = fopen(pszFile, "rb");
	if (!fp)
		return false;
	m_nSize = _filelength(_fileno(fp));
	m_pBuf = (char*)malloc(m_nSize);
	if (!m_pBuf)
	{
		fclose(fp);
		return false;
	}
	if (1 != fread(m_pBuf, m_nSize, 1, fp))
	{
		fclose(fp);
		return false;
	}
	fclose(fp);
	this->Decode(m_pBuf, m_nSize);
	return true;
}
void CEncryptFile::Decode(char* pBuf, int nSize)
{
	if (!pBuf || nSize <= 0)
		return;
	for (int i = 0; i < nSize; i++)
	{
		unsigned char c = (unsigned char)(m_szKey[i % 128] ^ pBuf[i]);
		int d = i % 8;
		pBuf[i] = (char)((unsigned char)(c << (8 - d)) + (unsigned char)(c >> d));
	}
}
int CEncryptFile::ReadLine(char* pszLine, int nMaxSize)
{
	if (!m_pBuf)
		return 0;
	int nMax = nMaxSize < 0 ? 0 : nMaxSize;
	memset(pszLine, 0, nMax);
	int n = 0;
	if (nMax <= 0)
		return 0;
	while (true)
	{
		pszLine[n] = m_pBuf[m_nPos];
		m_nPos++;
		if (m_nPos >= m_nSize)
			return n;
		if (m_pBuf[m_nPos] == '\r')
		{
			m_nPos++;
			if (m_nPos >= m_nSize)
				return n;
			if (m_pBuf[m_nPos] == '\n')
				m_nPos++;
			return n;
		}
		n++;
		if (n >= nMax)
			return n;
	}
}
