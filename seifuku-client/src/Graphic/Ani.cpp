#include "BaseFunc.h"
#include "ND_Bitmap.h"
#include "Ani.h"
#include "MyBitmapX.h"
#include "MyHeap.h"
#include <mmsystem.h>

#include <map>
#include <deque>
#include <vector>
#include <string>
using namespace std;

typedef vector<string> VECTOR_ANILINE;

typedef struct
{
	VECTOR_ANILINE	setAniLine;
}AniIndexInfo;

typedef map<OBJID, AniIndexInfo>MAP_ANIINDEXINFO;

class CAniFileSet;

class CAniX : public IAni
{
public:
	CAniX	(void)	{Init();}
	virtual ~CAniX	(void)	{Destroy();}

	void	Destroy		(void);

	virtual DWORD	Release		(void);
	virtual BOOL	Create		(const char* pszDataFile, const char* pszIndex, BOOL bHalf);
	BOOL	Create		(AniIndexInfo* pInfo, BOOL bHalf);
	virtual BOOL	Show		(UINT uFrame, int x, int y, int alpha, DWORD dwShowWay);
	virtual BOOL	ShowEx		(UINT uFrame, int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, DWORD dwShowWay);

	virtual CMyBitmap*	GetFrame		(UINT uFrameIndex);
	virtual int			GetFrameAmount	(void)	{return m_iFrameAmount;}

	virtual int			GetStatus		(void) {return m_nStatus;}
	virtual void		SetStatus		(int nStatus){m_nStatus = nStatus;}
	virtual DWORD		GetSize			(void);
	virtual CMySize		GetSize			(UINT uFrame);

	virtual	BOOL	Load		(void);
	virtual void	Free		(void)	{Destroy();}

	virtual DWORD   GetLife		(void) {return m_dwLife;}
	virtual void	SetLife		(DWORD dwLife){m_dwLife = dwLife; this->ResetLife();}
	virtual void	ResetLife	(void){m_dwTimeBorn = ::timeGetTime();}
	virtual DWORD   GetTimeBorn	(void) {return m_dwTimeBorn;}

	virtual BOOL	SetAniFilename(const char *pszDataFile, const char *pszIndex);

private:
	volatile int m_nStatus;
	BOOL		m_bHalf;
	int			m_iFrameAmount;
	CMyBitmapX*	m_pbmpFrame[_MAX_FRAME];
	DWORD m_dwLife;
	DWORD m_dwTimeBorn;
	char m_szIniFile[_MAX_PATH];
	char m_szIndex[32];

	void	Init (void);

public:
	static void* operator new(size_t uSize) throw()	{ return s_heap.Alloc(uSize); }
	static void	 operator delete(void* p)	{ s_heap.Free(p); }
	static CMyHeap	s_heap;
};

class CAniFile
{
public:
	CAniFile();
	virtual ~CAniFile();

private:
	MAP_ANIINDEXINFO m_setAniIndexInfo;
public:
	char m_szFileName[_MAX_PATH];

public:
	BOOL Create(char* pszAniFile);
	AniIndexInfo* GetAniIndexInfo(char* pszIndex);

public:
	static CAniFile* CreateNew(char* pszAniFile);
};

typedef vector<CAniFile*> VECTOR_ANIFILE;
class CAniFileSet
{
public:
	CAniFileSet();
	virtual ~CAniFileSet();

private:
	VECTOR_ANIFILE m_setAniFile;

public:
	void Destroy();

public:
	CAniFile* AddAniFile(char* pszAniFile);
	CAniFile* GetAniFile(char* pszAniFile);
	AniIndexInfo* GetAniIndexInfo(char* pszAniFile, char* pszAniIndex);
};

CMyHeap CAniX::s_heap("CAniX");
static CAniFileSet s_setAniFile;

void AniFileSetDestroy ( void )
{
	s_setAniFile.Destroy();
}

void CAniX::Init(void)
{
	m_nStatus	=ANISTATUS_INIT;
	m_bHalf		=false;
	m_iFrameAmount	=0;
	for(int i=0;i<_MAX_FRAME;i++)
	{
		m_pbmpFrame[i]	=NULL;
	}

	return;
}

DWORD CAniX::Release (void)
{
	delete this;
	return 0;
}

BOOL CAniX::Create(const char* pszDataFile, const char* pszIndex, BOOL bHalf)
{
	IF_NOT(pszDataFile && pszIndex)
		return false;

	if (m_nStatus == ANISTATUS_LOADED)
		Destroy();

	AniIndexInfo* pInfo = s_setAniFile.GetAniIndexInfo((char*)pszDataFile, (char*)pszIndex);
	if (!pInfo)
	{
		::ErrorMsg("AniIndexInfo Not Found %s", pszIndex);
		this->SetStatus(ANISTATUS_FAILED);
		return false;
	}
	return this->Create(pInfo, bHalf);
}

BOOL CAniX::Create(AniIndexInfo* pInfo, BOOL bHalf)
{
	IF_NOT(pInfo)
	{
		m_nStatus	=ANISTATUS_FAILED;
		return false;
	}

	int nAmount = pInfo->setAniLine.size();

	for(int i=0; i<nAmount; i ++)
	{
		if(bHalf && i != 0 && (i%2 == 0))
		{
			m_pbmpFrame[i] = m_pbmpFrame[i-1];
			continue;
		}
		m_pbmpFrame[i]	=new CMyBitmapX;
		if (!m_pbmpFrame[i] || !m_pbmpFrame[i]->Load((char*)pInfo->setAniLine[i].c_str()))
		{
			for(int j=0; j<i+1; j++)
			{
				if (!(bHalf && j != 0 && (j%2 == 0)) && m_pbmpFrame[j])
					delete m_pbmpFrame[j];

				m_pbmpFrame[j]	=NULL;
			}

			::ErrorMsg("Error: %s load failed in CAniX::Create().", pInfo->setAniLine[i].c_str());
			m_nStatus	=ANISTATUS_FAILED;
			return false;
		}
	}
	m_iFrameAmount	=nAmount;
	m_nStatus	=ANISTATUS_LOADED;
	m_bHalf		=bHalf;
	return true;
}

BOOL CAniX::Show(UINT uFrame, int x, int y, int alpha,DWORD dwShowWay)
{
	if(m_nStatus != ANISTATUS_LOADED || m_iFrameAmount <= 0)
		return false;

	uFrame	%=m_iFrameAmount;

	m_pbmpFrame[uFrame]->Show(x, y, alpha,dwShowWay);

	if(uFrame+1 >= m_iFrameAmount)
		return true;
	else
		return false;
}

BOOL CAniX::ShowEx(UINT uFrame, int x, int y, RECT *lpSrc, DWORD dwWidth, DWORD dwHeight, int alpha, DWORD dwShowWay)
{
	if(m_nStatus != ANISTATUS_LOADED || m_iFrameAmount <= 0)
		return false;

	uFrame	%=m_iFrameAmount;

	m_pbmpFrame[uFrame]->ShowEx(x, y, lpSrc, dwWidth, dwHeight, alpha, dwShowWay, 0);

	if(uFrame+1 >= m_iFrameAmount)
		return true;
	else
		return false;
}

CMyBitmap* CAniX::GetFrame(UINT uFrameIndex)
{
	if(m_nStatus != ANISTATUS_LOADED || m_iFrameAmount <= 0)
		return NULL;

	return m_pbmpFrame[uFrameIndex%m_iFrameAmount];
}

void CAniX::Destroy(void)
{
	if(m_nStatus != ANISTATUS_LOADED)
		return;

	for(int i=0;i<m_iFrameAmount;i++)
	{
		if(m_bHalf)
		{
			if((i == 0)||(i%2 != 0))
	            SAFE_DELETE(m_pbmpFrame[i]);
		}
		else
		{
            SAFE_DELETE(m_pbmpFrame[i]);
		}

		m_pbmpFrame[i]	=NULL;
	}

	m_iFrameAmount	=0;
	m_nStatus	=ANISTATUS_INIT;
	m_bHalf		=false;

	return;
}

DWORD CAniX::GetSize(void)
{
	DWORD dwSize = 0;
	if(m_nStatus != ANISTATUS_LOADED)
		return dwSize;

	int nAmount = this->GetFrameAmount();
	for(int i=0; i<nAmount; i++)
	{
		if(m_bHalf && i != 0 && (i%2 == 0))
			continue;
		CMyBitmap* pBmp = this->GetFrame(i);
		if(pBmp)
			dwSize += pBmp->GetSize();
	}
	return dwSize;
}

CMySize CAniX::GetSize(UINT uFrame)
{
	CMySize sizeAni = {0, 0};
	CMyBitmap* pBmp = this->GetFrame(uFrame);
	if(pBmp)
	{
		pBmp->GetSize(sizeAni.iWidth, sizeAni.iHeight);
	}
	return sizeAni;
}

BOOL CAniX::Load(void)
{
	try {
		this->ResetLife();
		return this->Create(m_szIniFile, m_szIndex, m_bHalf);
	}
	catch(...)
	{
		::LogMsg("Error: exception catch when loading %s :%s", m_szIniFile, m_szIndex);
		return false;
	}
}

BOOL CAniX::SetAniFilename(const char *pszDataFile, const char *pszIndex)
{
	IF_NOT(pszDataFile && pszIndex)
		return false;
	strcpy(m_szIniFile, pszDataFile);
	strcpy(m_szIndex, pszIndex);
	return true;
}

IAni* AniCreate ( const char* pszDataFile, const char* pszIndex, BOOL bHalf )
{
	CAniX* pAni = new CAniX;
	if (!pAni)
		return NULL;
	if (pszDataFile && pszIndex)
	{
		if (!pAni->Create(pszDataFile, pszIndex, bHalf))
		{
			SAFE_DELETE(pAni);
			return NULL;
		}
	}
	return pAni;
}

CAniFile::CAniFile()
{
	memset ( m_szFileName, 0, sizeof ( m_szFileName ) );
}
CAniFile::~CAniFile()
{
	m_setAniIndexInfo.clear();
}
CAniFile* CAniFile::CreateNew(char* pszAniFile)
{
	IF_NOT(pszAniFile)
		return NULL;
	CAniFile* pAniFile = new CAniFile;
	IF_NOT(pAniFile)
		return NULL;

	if(!pAniFile->Create(pszAniFile))
	{
		SAFE_DELETE(pAniFile);
		return NULL;
	}
	return pAniFile;
}
BOOL CAniFile::Create(char* pszAniFile)
{
	IF_NOT(pszAniFile)
		return false;

	FILE* fp = fopen(pszAniFile, "r");
	IF_NOT(fp)
		return false;

	while(true)
	{
		char szLine[1024] = "";
		if(!fgets(szLine, 1024, fp))
		{
			fclose(fp);
			::DebugMsg("total %d ani info found in %s", m_setAniIndexInfo.size(), pszAniFile);
			strcpy(m_szFileName, pszAniFile);
			return true;
		}

		int nLen = strlen(szLine);
		if(nLen <= 0)
			continue;
		if(szLine[0] != '[')
			continue;
		if(szLine[nLen-1] == '\n')
			szLine[nLen-1] = '\0';

		char szTitle[MAX_PATH] = "";
		if (1 != sscanf(szLine, "[%s", szTitle))
			continue;

		char* p =strchr(szTitle, ']');
		if (p)
			*p = '\0';
		else
		{
			fclose(fp);

			::LogMsg("Error Title line %s in %s", szLine, pszAniFile);
			return false;
		}

		AniIndexInfo infoAni;

		int nFrameAmount = 0;
		if (1 != fscanf(fp, "FrameAmount=%d\n", &nFrameAmount))
		{
			fclose(fp);

			::LogMsg("FrameAmount error in [%s] of %s", szTitle, pszAniFile);
			return false;
		}

		nFrameAmount %= _MAX_FRAME;

		for (int i=0; i<nFrameAmount; i++)
		{
			char szFormat[MAX_PATH] = "";
			sprintf(szFormat, "Frame%d=%s\n", i, "%s");

			char szFile[MAX_PATH] = "";
			if (1 != fscanf(fp, szFormat, szFile))
			{
				fclose(fp);

				::LogMsg("%s error in [%s] of %s", szFormat, szTitle, pszAniFile);
				return false;
			}

			infoAni.setAniLine.push_back(string(szFile));
		}
		OBJID id  = Str2ID(szTitle);
		m_setAniIndexInfo[id] = infoAni;
	}
	return true;
}
AniIndexInfo* CAniFile::GetAniIndexInfo(char* pszIndex)
{
	if(!pszIndex)
		return NULL;
	OBJID id = Str2ID(pszIndex);
	map<OBJID, AniIndexInfo>::iterator iter = m_setAniIndexInfo.find(id);
	if (iter == m_setAniIndexInfo.end())
	{
		return NULL;
	}
	return &iter->second;
}

CAniFileSet::CAniFileSet()
{
}
CAniFileSet::~CAniFileSet()
{
	this->Destroy();
}
void CAniFileSet::Destroy()
{
	int nAmount = m_setAniFile.size();
	for(int i = 0; i < nAmount; i++)
	{
		SAFE_DELETE(m_setAniFile[i]);
	}
	m_setAniFile.clear();
}
CAniFile* CAniFileSet::AddAniFile(char* pszAniFile)
{
	IF_NOT(pszAniFile)
		return NULL;
	CAniFile* pFile = CAniFile::CreateNew(pszAniFile);
	if(!pFile)
		return NULL;
	m_setAniFile.push_back(pFile);
	return pFile;
}
CAniFile* CAniFileSet::GetAniFile(char* pszAniFile)
{
	IF_NOT(pszAniFile)
		return NULL;
	int nAmount = m_setAniFile.size();
	for(int i = 0; i < nAmount; i++)
	{
		CAniFile* pFile = m_setAniFile[i];
		if(pFile && (strcmp(pFile->m_szFileName, pszAniFile) == 0))
			return pFile;
	}
	return NULL;
}
AniIndexInfo* CAniFileSet::GetAniIndexInfo(char* pszAniFile, char* pszAniIndex)
{
	IF_NOT(pszAniFile && pszAniIndex)
		return NULL;
	CAniFile* pFile = this->GetAniFile(pszAniFile);
	if(!pFile)
		pFile = this->AddAniFile(pszAniFile);
	if(!pFile)
	{
		::LogMsg("Ani File [%s] Not Found!", pszAniFile);
		return NULL;
	}
	AniIndexInfo* pInfo = pFile->GetAniIndexInfo(pszAniIndex);
	if(!pInfo)
		::LogMsg("Ani Index [%s] Not Found In [%s]", pszAniIndex, pszAniFile);
	return pInfo;
}
