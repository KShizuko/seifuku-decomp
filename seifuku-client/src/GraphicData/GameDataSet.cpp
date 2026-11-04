#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <set>
#include <vector>
#include "GameDataSet.h"
#include "MyIni.h"
#include "StrRes.h"
#include "TqPackage.h"
#include "DataThread.h"

typedef std::map<unsigned __int64, std::string>	MAP_RES;

#define GRAPHIC_RELEASE(p) { if(p){ try{ (p)->Release(); } catch(...){} p=NULL; }}

const OBJID ID_NONE = 0;

typedef std::deque<IAni*>	DEQUE_ANI;
typedef std::map<OBJID, IAni*>	MAP_DYNAMICANI;

DWORD			g_dwDeadLine	=	10000;
static CRITICAL_SECTION	csReadThread;
static DEQUE_ANI		dequeDataToLoad;

static HANDLE	hExitEvent	=NULL;
static HANDLE	hReadEvent	=NULL;
static HANDLE	hReadThread	=NULL;

DWORD g_dwEgLoad = 0, g_dwImLoad = 0, g_dwNmLoad = 0;
DWORD g_dwMeshTime = 0;
DWORD g_dwMeshLoad = 0;

struct C3DMotionInfo
{
	unsigned __int64	id;
	I3DMotion*	p3DMotion;
	DWORD		timeBirth;
	DWORD		dwLife;

	~C3DMotionInfo();
};

struct C3DTextureInfo
{
	OBJID		id;
	I3DTexture*	p3DTexture;
	DWORD		timeBirth;
	DWORD		dwLife;

	~C3DTextureInfo();
};

struct C3DEffectInfo
{
	OBJID		id;
	I3DEffect*	p3DEffect;
	DWORD		timeBirth;
	DWORD		dwLife;

	~C3DEffectInfo();
};

const DWORD _3DOBJ_DEADLINE = 30*1000;

typedef std::map<OBJID, C3DObjInfo* >				MAP_3DOBJEX;
typedef std::map<unsigned __int64, C3DMotionInfo* >	MAP_3DMOTIONEX;
typedef std::map<OBJID, C3DTextureInfo* >			MAP_3DTEXTUREEX;
typedef std::map<OBJID, C3DEffectInfo* >			MAP_3DEFFECTEX;

class CGameDataSetX;
static CGameDataSetX* s_pGameDataSet = NULL;

enum { DATAITEM_TEXTURE = 0, DATAITEM_EFFECT, DATAITEM_OBJ, DATAITEM_MOTION };
class CDataItem : public IDataThreadItem
{
public:
	std::string			strKind;
	I3DTexture*			p3DTexture;
	I3DEffect*			p3DEffect;
	I3DMotion*			p3DMotion;
	I3DObj*				p3DObj;
	int					bImmediate;
	int					nKind;
	unsigned __int64	id;

	CDataItem()	{ p3DTexture = NULL; p3DEffect = NULL; p3DMotion = NULL; p3DObj = NULL; bImmediate = 0; nKind = 0; id = 0; }
	virtual ~CDataItem()	{}
	virtual unsigned __int64	GetID ()		{ return id; }
	virtual void	Process ();
	virtual int		IsImmediate ()	{ return bImmediate; }
	virtual void	Destroy ()		{ delete this; }
};
typedef std::vector<CDataItem*>	VEC_DATAITEM;

struct ResRef
{
	void*	pObj;
	int		nRef;
};
typedef std::map<std::string, ResRef>	MAP_RESCACHE;
typedef std::map<__int64, int>		MAP_ID64;

typedef struct
{
	OBJID	idLevel0;
	OBJID	idLevel1;
	OBJID	idLevel2;
}MeshLodInfo;

void AddReadDeque(IAni* pAni, int nExigence);
void UpPriority(IAni* pAni);
BOOL DataReadThreadInit(void);
void DataReadThreadDestroy(void);

class CGameDataSetX : public IGameDataSet, public IDataIcon
{
public:
	CGameDataSetX();
	~CGameDataSetX();

	virtual void		Init		( BOOL bBin );
	virtual void		InitStrRes	( void )	{ m_objStrRes.Init(); }
	virtual void		Destroy		( void );
	virtual void		Process		( void );
	virtual IAni*		GetDataAni	( const char* pszFileName, const char* pszTitle, int nExigence, DWORD dwLife );
	virtual I3DObj*		Get3DObj	( OBJID id, int bLoadAtOnce, DWORD dwLife );
	virtual I3DTexture*	Get3DTexture( OBJID id, int bLoadAtOnce, DWORD dwLife );
	virtual I3DEffect*	Get3DEffect	( OBJID id, int bLoadAtOnce, DWORD dwLife );
	virtual I3DMotion*	Get3DMotion	( const char* pszKind, unsigned __int64 id, int bLoadAtOnce, DWORD dwLife );
	virtual C3DObjInfo*	Get3DObjInfo( OBJID id );
	virtual void*		Slot10		( DWORD dw )	{ return NULL; }
	virtual const char*	GetSound	( OBJID idSound );
	virtual const char*	GetVersionInfo ( void );
	virtual const char*	GetMotionRes ( const char* pszKind, OBJID id );
	virtual const char*	GetStr		( OBJID idRes )	{ return m_objStrRes.GetStr(idRes); }
	virtual C3DSimpleObjInfo*	Get3DSimpleObjInfo ( OBJID id );
	virtual CMy3DEffectInfo*	GetMy3DEffectInfo ( const char* pszIndex );
	virtual CMy3DEffectInfo*	FindMy3DEffectInfo ( const char* pszIndex );
	virtual void		AddMy3DEffectInfo ( CMy3DEffectInfo* pInfo );
	virtual RolePartInfo*	GetRolePartInfo ( const char* pszKind, OBJID id );
	virtual BOOL		IsHiddenPhy	( const char* pszPhyName );
	virtual void*		QueryInterface ( int nInterface );
	virtual int			GetEmotionAmount ( void );
	virtual const char*	GetEmotionShutCut ( int nIndex );
	virtual void		BuildBin	( void );
	virtual DWORD		GetActionTransTime ( DWORD idAction, DWORD idTrans );
	virtual C3DSimpleObjExInfo*	Get3DSimpleObjExInfo ( OBJID id );
	virtual void		SetLoadAtOnce ( DWORD dwMilliseconds );
	virtual OBJID		GetMeshLod ( OBJID idMesh, int nLevel );
	virtual BOOL		Reload ( BOOL bBin, DWORD dwFlags );

	virtual IAni*		GetIcon ( int nIndex );

public:
	BOOL	CreateEmotionInfo ( void );
	I3DObj*		CreateNew3DObj ( OBJID id, I3DMotion** ppDefaultMotion );
	I3DMotion*	CreateNew3DMotion ( unsigned __int64 id, const char* pszKind );
	I3DMotion*	CreateNew3DMotion ( unsigned __int64 id, MAP_RES& mapRes );
	I3DMotion*	CreateNew3DMotion ( const char* pszFileName, BOOL bCache, BOOL bBuffer );
	I3DTexture*	CreateNew3DTexture ( OBJID id );
	virtual I3DEffect*	CreateNew3DEffect ( OBJID id );
	ResRef*	FindCacheByPointer ( MAP_RESCACHE& mapCache, void* pObj, MAP_RESCACHE::iterator& iterOut );
	bool	ReleaseCachedMotion ( I3DMotion* pMotion );
	bool	ReleaseCachedMesh ( I3DObj* pObj );
	bool	ReleaseCachedEffect ( I3DEffect* pEffect );
	bool	ReleaseCachedTexture ( I3DTexture* pTexture );
	void	UpdateLoadAtOnce ( void );
	void	ProcessFinished ( void );
	void	FileDataItem ( CDataItem* pItem );
	BOOL	Create3DSimpleObjExInfo ( void );
	BOOL	CreateBufferMotionInfo ( void );
	void	CreateImmediateInfo ( void );
	void	CreateMeshLodInfo ( void );
	bool	IsImmediate ( const char* pszKind, unsigned __int64 id );
	void	ProcessDataAni ( void );
	void	Process3DObjSet ( void );
	void	Process3DMotionSet ( MAP_3DMOTIONEX& mapMotion );
	void	Process3DMotionSet ( void );
	void	Process3DTextureSet ( void );
	void	Process3DEffectSet ( void );
	void	Destroy3DSimpleObjInfo ( void );
	void	DestroyMy3DEffectInfo ( void );
	void	DestroyImmediateInfo ( void );
	void	Destroy3DObj ( void );
	void	Destroy3DMotion ( MAP_3DMOTIONEX& mapMotion );
	void	Destroy3DMotion ( void );
	void	Destroy3DTexture ( void );
	void	Destroy3DEffect ( void );
	void	DestroyDataAni ( void );
	BOOL	Create3DSimpleObjInfo ( BOOL bBin );
	BOOL	Create3DSimpleObjInfoTxt ( void );
	BOOL	Create3DSimpleObjInfoBin ( void );
	BOOL	Build3DSimpleObjInfoBin ( void );
	BOOL	IsResDefined ( unsigned __int64 id, MAP_RES& mapRes );
	BOOL	CreateMy3DEffectInfo ( BOOL bBin );
	BOOL	CreateMy3DEffectInfoTxt ( void );
	BOOL	CreateMy3DEffectInfoBin ( void );
	BOOL	BuildMy3DEffectInfoBin ( void );
	BOOL	CreateRolePartInfo ( BOOL bBin );
	BOOL	CreateRolePart ( const char* pszMeshIni, BOOL bBin );
	BOOL	CreateRolePartTxt ( const char* pszMeshIni );
	BOOL	CreateRolePartBin ( const char* pszMeshIni );
	void	BuildRolePartInfoBin ( void );
	BOOL	BuildRolePartBin ( const char* pszMeshIni );
	void	BuildMotionResBin ( const char* pszMotionIni );
	typedef std::map<OBJID, RolePartInfo>	MAP_ROLEPART;
	RolePartInfo*	FindRolePart ( MAP_ROLEPART& mapPart, OBJID id );
	struct PartKind;
	PartKind*	FindPartKind ( const char* pszKind );
	BOOL	CreateMotionRes ( const char* pszMotionIni, BOOL bBin );
	void	CreateActionTransInfo ( void );
	BOOL	CreateMaterialInfo ( void );
	D3DMATERIAL8*	GetMaterial ( const char* pszName );
	void	Color2Value ( DWORD dwColor, D3DCOLORVALUE* pValue );
	void	OpenPackages ( void );
	void	InitSystemPolicy ( void );
	void	LoadRes ( const char* pszResFile, MAP_RES& mapRes, BOOL bBin );
	void	LoadResTxt ( const char* pszResFile, MAP_RES& mapRes );
	void	LoadResBin ( const char* pszResFile, MAP_RES& mapRes );
	void	BuildResBin ( const char* pszResFile, MAP_RES& mapRes );

	std::map<OBJID, C3DSimpleObjInfo*>	m_map3DSimpleObjInfo;
	std::map<OBJID, C3DSimpleObjExInfo>	m_map3DSimpleObjExInfo;
	char*						m_p3DSimpleObjBin;
	std::map<std::string, CMy3DEffectInfo*>	m_mapMy3DEffectInfo;
	char*						m_pMy3DEffectBin;
	std::map<std::string, D3DMATERIAL8>	m_mapMaterial;
	struct PartKind
	{
		std::string	strMeshIni;
		std::string	strMotionIni;
	};
	std::map<std::string, PartKind>	m_mapPartKind;
	std::map<std::string, MAP_ROLEPART>	m_mapRolePart;
	std::map<std::string, MAP_RES>	m_mapMotionRes;
	std::vector<std::string>	m_setEmotion;
	std::set<std::string>		m_setDumy;
	std::map<DWORD, std::map<DWORD, DWORD> >	m_mapActionTrans;
	MAP_3DOBJEX					m_set3DObj;
	std::map<std::string, MAP_3DMOTIONEX>	m_set3DMotion;
	MAP_3DTEXTUREEX				m_set3DTexture;
	MAP_3DEFFECTEX				m_set3DEffect;
	MAP_DYNAMICANI				m_mapDataAni;
	DWORD						m_timerCheck;
	MAP_RES						m_mapResMesh;
	MAP_RES						m_mapResTexture;
	MAP_RES						m_mapResEffectObj;
	MAP_RES						m_mapSound;
	CStrRes						m_objStrRes;
	DWORD						m_dw3DMotionLife;
	DWORD						m_dw3DTextureLife;
	DWORD						m_dw3DEffectLife;
	DWORD						m_dwPuzzleLife;
	DWORD						m_dwPuzzlePreloadSize;
	DWORD						m_dwReadThreadPriority;
	MAP_RESCACHE				m_mapTextureCache;
	MAP_RESCACHE				m_mapMeshCache;
	MAP_RESCACHE				m_mapMotionCache;
	MAP_RESCACHE				m_mapEffectCache;
	IDataThread*				m_pDataThread;
	VEC_DATAITEM				m_vecDataItem;
	DWORD						m_timeLoadAtOnce;
	BYTE						m_bLoadAtOnce;
	std::map<std::string, MAP_ID64*>	m_mapImmediate;
	std::set<std::string>		m_setBufferMotion;
	std::map<OBJID, MeshLodInfo>	m_mapMeshLod;
};

void AddReadDeque(IAni* pAni, int nExigence)
{
	if (!pAni)
		return;

	::EnterCriticalSection(&csReadThread);
		if(nExigence == EXIGENCE_NORMAL)
			dequeDataToLoad.push_back(pAni);
		else
			dequeDataToLoad.push_front(pAni);

	::LeaveCriticalSection(&csReadThread);

	::SetEvent(hReadEvent);
}

void UpPriority(IAni* pAni)
{
	if (!pAni)
		return;

	::EnterCriticalSection(&csReadThread);
		if (pAni->GetStatus() == ANISTATUS_INIT)
		{
			int nAmount = dequeDataToLoad.size();
			for (int i=nAmount-1; i>0; i--)
			{
				IAni* pTheAni = dequeDataToLoad[i];
				if (pTheAni == pAni)
				{
					dequeDataToLoad.erase(dequeDataToLoad.begin()+i);
					dequeDataToLoad.push_front(pAni);
					break;
				}
			}
		}
	::LeaveCriticalSection(&csReadThread);
}

DWORD WINAPI DataReadThread(LPVOID lpThreadParameter)
{
	HANDLE		eventHandles[2];
	eventHandles[0]	=hReadEvent;
	eventHandles[1] =hExitEvent;

	while (WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE)==WAIT_OBJECT_0)
	{
		while(!dequeDataToLoad.empty())
		{
			IAni* pAni = NULL;
			try {
				::EnterCriticalSection(&csReadThread);

					pAni = dequeDataToLoad.front();
					dequeDataToLoad.pop_front();

					pAni->SetStatus(ANISTATUS_LOADING);
				::LeaveCriticalSection(&csReadThread);

				pAni->Load();
			}
			catch(...)
			{
				::LogMsg("catch error in data load thread.");
			}
		}
	}

	return 0;
}

IGameDataSet* GameDataSetQuery ( void )
{
	if (!s_pGameDataSet)
		s_pGameDataSet = new CGameDataSetX;
	return s_pGameDataSet;
}

void GameDataSetDestroy ( void )
{
	if (s_pGameDataSet)
	{
		try { delete s_pGameDataSet; } catch(...) {}
		s_pGameDataSet = NULL;
	}
}

BOOL DataReadThreadInit(void)
{
	hReadEvent =CreateEvent	 (NULL,
							  FALSE,
							  FALSE,
							  NULL);

	if (!hReadEvent)
		return false;

	hExitEvent =CreateEvent  (NULL,
							  FALSE,
							  FALSE,
							  NULL);
	if (!hExitEvent)
		return false;

	dequeDataToLoad.clear();

	DWORD dwIdThread;
	hReadThread		=CreateThread(NULL,
								  0,
								  DataReadThread,
								  NULL,
								  0,
								  &dwIdThread);
	if(!hReadThread)
		return false;

	SetThreadPriority(hReadThread, s_pGameDataSet->m_dwReadThreadPriority);

	return true;
}

void DataReadThreadDestroy(void)
{
	while (true)
	{
		if (dequeDataToLoad.size() <= 0)
			break;

		Sleep(50);
	}

	if(hReadThread)
	{
		SetEvent(hExitEvent);

		Sleep(100);
		CloseHandle(hReadThread);
		hReadThread	=NULL;
	}

	if(hReadEvent)
	{
		CloseHandle(hReadEvent);
		hReadEvent	=NULL;
	}

	if(hExitEvent)
	{
		CloseHandle(hExitEvent);
		hExitEvent	=NULL;
	}
}

CGameDataSetX::CGameDataSetX()
{
	m_pDataThread = NULL;
	m_timeLoadAtOnce = 0;
	m_bLoadAtOnce = 0;
}

CGameDataSetX::~CGameDataSetX()
{
	this->Destroy();
	::DeleteCriticalSection(&csReadThread);
}

void CGameDataSetX::Destroy ( void )
{
	::DataReadThreadDestroy();

	this->DestroyDataAni();
	this->Destroy3DMotion();
	this->Destroy3DObj();
	this->Destroy3DTexture();
	this->Destroy3DEffect();
	this->Destroy3DSimpleObjInfo();
	this->DestroyMy3DEffectInfo();

	for (VEC_DATAITEM::iterator it = m_vecDataItem.begin(); it != m_vecDataItem.end(); )
	{
		if (*it)
			delete *it;
	}
	m_vecDataItem.clear();
	if (m_pDataThread)
	{
		try { m_pDataThread->Release(); } catch (...) {}
		m_pDataThread = NULL;
	}

	::TqPackagesClose(NULL);
	this->DestroyImmediateInfo();

	CMyBitmap::Quit3D();
}

void CGameDataSetX::Process ( void )
{
	this->UpdateLoadAtOnce();
	if (m_pDataThread->IsIdle())
	{
		for (VEC_DATAITEM::iterator it = m_vecDataItem.begin(); it != m_vecDataItem.end(); it++)
		{
			CDataItem* pDone = (CDataItem*)m_pDataThread->Add(*it);
			if (pDone)
				this->FileDataItem(pDone);
		}
		m_vecDataItem.clear();
	}
	this->ProcessFinished();

	static DWORD s_timeProcess = ::timeGetTime();
	if (m_pDataThread->IsIdle())
	{
		if (::timeGetTime() - s_timeProcess > 30000)
		{
			s_timeProcess = ::timeGetTime();
			::EnterCriticalSection(&csReadThread);
			this->ProcessDataAni();
			this->Process3DMotionSet();
			this->Process3DObjSet();
			this->Process3DTextureSet();
			this->Process3DEffectSet();
			::LeaveCriticalSection(&csReadThread);
		}
	}
}

IAni* CGameDataSetX::GetDataAni	(const char* pszFileName, const char* pszTitle, int nExigence, DWORD dwLife)
{
	if((!pszFileName || strlen(pszFileName) < 1) || (!pszTitle || strlen(pszTitle) < 1) )
		return NULL;

	static char szTemp[256];
	strcpy(szTemp, pszFileName);
	strcat((char*)szTemp, pszTitle);

	OBJID idAni = Str2ID(szTemp);

	MAP_DYNAMICANI::iterator iter = m_mapDataAni.find(idAni);
	if(iter != m_mapDataAni.end())
	{
		IAni* pDAni = iter->second;
		if (pDAni)
		{
			switch (pDAni->GetStatus())
			{
			case ANISTATUS_INIT:
			case ANISTATUS_LOADING:
				{
					if (nExigence == EXIGENCE_IMMEDIATE)
					{
						g_dwEgLoad ++;

						if (ANISTATUS_INIT == pDAni->GetStatus())
							::UpPriority(pDAni);

						DWORD timeBegin = ::TimeGet();
						while (ANISTATUS_INIT == pDAni->GetStatus()
								|| ANISTATUS_LOADING == pDAni->GetStatus())
						{
							::Sleep(30);
							if (::TimeGet() >= timeBegin+3000)
								break;
						}

						if (ANISTATUS_LOADED == pDAni->GetStatus())
						{
							pDAni->ResetLife();
							return pDAni;
						}
						else
						{
							m_mapDataAni[idAni] = pDAni;
							return NULL;
						}
					}
					else
						return NULL;
				}
				break;

			case ANISTATUS_LOADED:
				{
					pDAni->ResetLife();
					return pDAni;
				}
				break;

			case ANISTATUS_FAILED:
				{
					return NULL;
				}
				break;

			default:
				{
					m_mapDataAni.erase(iter);
					GRAPHIC_RELEASE (pDAni);
					return NULL;
				}
				break;
			}
		}
		else
		{
			::LogMsg("Error: found invalid data ani in CGameDataSetX::GetDataAni()");

			m_mapDataAni.erase(iter);
		}
	}

	IAni* pNewDAni = ::AniCreate(NULL, NULL, FALSE);
	if (!pNewDAni)
		return NULL;

	pNewDAni->SetLife(dwLife);
	pNewDAni->SetAniFilename(pszFileName, pszTitle);
	if (EXIGENCE_IMMEDIATE == nExigence)
	{
		g_dwImLoad ++;
		if (pNewDAni->Load() && ANISTATUS_LOADED == pNewDAni->GetStatus())
		{
			m_mapDataAni[idAni] = pNewDAni;
			return pNewDAni;
		}
		else
		{
			GRAPHIC_RELEASE (pNewDAni) ;
			return NULL;
		}
	}
	else
	{
		g_dwNmLoad ++;

		pNewDAni->SetStatus(ANISTATUS_INIT);

		m_mapDataAni[idAni] = pNewDAni;

		::AddReadDeque(pNewDAni, nExigence);

		return NULL;
	}
}

I3DObj* CGameDataSetX::Get3DObj	(OBJID id, int bLoadAtOnce, DWORD dwLife)
{
	if (m_bLoadAtOnce)
		bLoadAtOnce = 1;

	MAP_3DOBJEX::iterator iterF = m_set3DObj.find(id);
	if(iterF != m_set3DObj.end())
	{
		C3DObjInfo* pInfo = iterF->second;
		if(pInfo)
		{
			pInfo->timeBirth = ::timeGetTime();
			if (!pInfo->p3DObj && bLoadAtOnce == 1)
				return NULL;
			pInfo->dwLife = dwLife;
			return pInfo->p3DObj;
		}
	}

	if (this->IsImmediate("mesh", id))
		bLoadAtOnce = 1;

	C3DObjInfo* pInfo = new C3DObjInfo;
	if (!pInfo)
		return NULL;
	pInfo->id				= id;
	pInfo->p3DObj			= NULL;
	pInfo->pDefaultMotion	= NULL;
	pInfo->timeBirth		= ::timeGetTime();
	pInfo->dwLife			= 0;
	if (pInfo->dwLife < dwLife)
		pInfo->dwLife = dwLife;
	m_set3DObj[id] = pInfo;

	CDataItem* pItem = new CDataItem;
	if (!pItem)
		return NULL;
	pItem->nKind		= DATAITEM_OBJ;
	pItem->id			= id;
	pItem->bImmediate	= bLoadAtOnce;
	if (bLoadAtOnce == 1)
	{
		CDataItem* pDone = (CDataItem*)m_pDataThread->Add(pItem);
		if (pDone)
		{
			pInfo->p3DObj			= pItem->p3DObj;
			pInfo->pDefaultMotion	= pItem->p3DMotion;
			delete pDone;
		}
	}
	else
		m_vecDataItem.push_back(pItem);
	return pInfo->p3DObj;
}

C3DObjInfo* CGameDataSetX::Get3DObjInfo (OBJID id)
{
	MYASSERT (id != ID_NONE);

	MAP_3DOBJEX::iterator iter = m_set3DObj.find(id);
	if(iter != m_set3DObj.end())
	{
		C3DObjInfo* pInfo = iter->second;
		if(pInfo)
		{
			pInfo->timeBirth = ::TimeGet();
			return pInfo;
		}
	}

	return NULL;
}

I3DMotion* CGameDataSetX::Get3DMotion ( const char* pszKind, unsigned __int64 id, int bLoadAtOnce, DWORD dwLife )
{
	if (m_bLoadAtOnce)
		bLoadAtOnce = 1;
	if (id == 0)
		return NULL;
	if (!pszKind)
		return NULL;

	std::string strKind = pszKind;
	MAP_3DMOTIONEX& mapMotion = m_set3DMotion[strKind];
	MAP_3DMOTIONEX::iterator iter = mapMotion.find(id);
	if(mapMotion.end() != iter)
	{
		C3DMotionInfo* pInfo = iter->second;
		if(pInfo)
		{
			pInfo->timeBirth = ::timeGetTime();
			if (!pInfo->p3DMotion && bLoadAtOnce == 1)
				return NULL;
			if (pInfo->dwLife < dwLife)
				pInfo->dwLife = dwLife;
			return pInfo->p3DMotion;
		}
	}

	if (this->IsImmediate(pszKind, id))
		bLoadAtOnce = 1;

	C3DMotionInfo* pInfo = new C3DMotionInfo;
	if (!pInfo)
		return NULL;
	pInfo->id		 = id;
	pInfo->p3DMotion = NULL;
	pInfo->timeBirth = ::timeGetTime();
	pInfo->dwLife	 = 0;
	if (pInfo->dwLife < dwLife)
		pInfo->dwLife = dwLife;
	mapMotion[id]	=	pInfo;

	CDataItem* pItem = new CDataItem;
	if (!pItem)
		return NULL;
	pItem->nKind		= DATAITEM_MOTION;
	pItem->id			= id;
	pItem->bImmediate	= bLoadAtOnce;
	pItem->strKind		= pszKind;
	if (bLoadAtOnce != 1)
	{
		m_vecDataItem.push_back(pItem);
		return pInfo->p3DMotion;
	}
	CDataItem* pDone = (CDataItem*)m_pDataThread->Add(pItem);
	if (pDone)
	{
		pInfo->p3DMotion = pItem->p3DMotion;
		delete pDone;
	}
	return pInfo->p3DMotion;
}

I3DTexture* CGameDataSetX::Get3DTexture(OBJID id, int bLoadAtOnce, DWORD dwLife)
{
	if (m_bLoadAtOnce)
		bLoadAtOnce = 1;

	MAP_3DTEXTUREEX::iterator iter = m_set3DTexture.find(id);
	if(iter != m_set3DTexture.end())
	{
		C3DTextureInfo* pObj = iter->second;
		if(pObj)
		{
			pObj->timeBirth = ::timeGetTime();
			if (!pObj->p3DTexture && bLoadAtOnce == 1)
				return NULL;
			pObj->dwLife = dwLife;
			return pObj->p3DTexture;
		}
	}

	if (this->IsImmediate("texture", id))
		bLoadAtOnce = 1;

	C3DTextureInfo* pInfo = new C3DTextureInfo;
	if(!pInfo)
		return NULL;
	pInfo->id		  = id;
	pInfo->p3DTexture = NULL;
	pInfo->timeBirth  = ::timeGetTime();
	pInfo->dwLife	  = 0;
	if (pInfo->dwLife < dwLife)
		pInfo->dwLife = dwLife;
	m_set3DTexture[id]	=	pInfo;

	CDataItem* pItem = new CDataItem;
	if (!pItem)
		return NULL;
	pItem->nKind		= DATAITEM_TEXTURE;
	pItem->id			= id;
	pItem->bImmediate	= bLoadAtOnce;
	if (bLoadAtOnce == 1)
	{
		CDataItem* pDone = (CDataItem*)m_pDataThread->Add(pItem);
		if (pDone)
		{
			pInfo->p3DTexture = pItem->p3DTexture;
			delete pDone;
		}
	}
	else
		m_vecDataItem.push_back(pItem);
	return pInfo->p3DTexture;
}

I3DEffect* CGameDataSetX::Get3DEffect(OBJID id, int bLoadAtOnce, DWORD dwLife)
{
	if (m_bLoadAtOnce)
		bLoadAtOnce = 1;

	MAP_3DEFFECTEX::iterator iter = m_set3DEffect.find(id);
	if(iter != m_set3DEffect.end())
	{
		C3DEffectInfo* pObj = iter->second;
		if(pObj)
		{
			pObj->timeBirth = ::timeGetTime();
			if (!pObj->p3DEffect && bLoadAtOnce == 1)
				return NULL;
			pObj->dwLife = dwLife;
			return pObj->p3DEffect;
		}
	}

	if (this->IsImmediate("effect", id))
		bLoadAtOnce = 1;

	C3DEffectInfo* pInfo = new C3DEffectInfo;
	if(!pInfo)
		return NULL;
	pInfo->id		 = id;
	pInfo->p3DEffect = NULL;
	pInfo->timeBirth = ::timeGetTime();
	pInfo->dwLife	 = 0;
	if (pInfo->dwLife < dwLife)
		pInfo->dwLife = dwLife;
	m_set3DEffect[id]	=	pInfo;

	CDataItem* pItem = new CDataItem;
	if (!pItem)
		return NULL;
	pItem->nKind		= DATAITEM_EFFECT;
	pItem->id			= id;
	pItem->bImmediate	= bLoadAtOnce;
	if (bLoadAtOnce == 1)
	{
		CDataItem* pDone = (CDataItem*)m_pDataThread->Add(pItem);
		if (pDone)
		{
			pInfo->p3DEffect = pItem->p3DEffect;
			delete pDone;
		}
	}
	else
		m_vecDataItem.push_back(pItem);
	return pInfo->p3DEffect;
}

I3DObj* CGameDataSetX::CreateNew3DObj ( OBJID id, I3DMotion** ppDefaultMotion )
{
	if (id == ID_NONE)
		return NULL;

	MAP_RES::iterator iter = m_mapResMesh.find(id);
	if (iter == m_mapResMesh.end())
	{
		::LogMsg("Mesh [%u] not found!", id);
		return NULL;
	}

	const char* pszFileName = iter->second.c_str();
	MYASSERT (pszFileName);

	std::string strName = pszFileName;
	I3DObj* p3DObj = NULL;
	MAP_RESCACHE::iterator iterCache = m_mapMeshCache.find(strName);
	if (iterCache != m_mapMeshCache.end())
	{
		p3DObj = (I3DObj*)iterCache->second.pObj;
		iterCache->second.nRef++;
	}
	if (!p3DObj)
	{
		p3DObj = ::MeshCreate((char*)pszFileName);
		if (!p3DObj)
		{
			::LogMsg("create Mesh [%s] failed.", pszFileName);
			return NULL;
		}
		ResRef& ref = m_mapMeshCache[strName];
		ref.pObj = p3DObj;
		ref.nRef = 1;

		BOOL bBuffer = (m_setBufferMotion.find(strName) != m_setBufferMotion.end());
		I3DMotion* p3DMotion = this->CreateNew3DMotion(pszFileName, FALSE, bBuffer);
		if (!p3DMotion)
		{
			::LogMsg("3d motion [%s] create error.", pszFileName);
			return NULL;
		}
		if (ppDefaultMotion)
			*ppDefaultMotion = p3DMotion;
		p3DObj->SetMotion(p3DMotion);
	}

	DWORD dwPhyAmount = p3DObj->GetPhyAmount();
	for (DWORD i = 0; i < dwPhyAmount; i++)
	{
		if (this->IsHiddenPhy(p3DObj->GetPhyName(i)))
			p3DObj->SetPhyHidden(i, true);
	}
	return p3DObj;
}

I3DMotion* CGameDataSetX::CreateNew3DMotion ( unsigned __int64 id, const char* pszKind )
{
	if (id == 0)
		return NULL;
	if (!pszKind)
		return NULL;

	std::string strKind = pszKind;
	std::map<std::string, PartKind>::iterator iterKind = m_mapPartKind.find(strKind);
	if (iterKind == m_mapPartKind.end())
		return NULL;
	std::map<std::string, MAP_RES>::iterator iterRes = m_mapMotionRes.find(iterKind->second.strMotionIni);
	if (iterRes == m_mapMotionRes.end())
		return NULL;
	if (iterRes->second.find(id) == iterRes->second.end())
		return NULL;
	return this->CreateNew3DMotion(id, iterRes->second);
}

I3DMotion* CGameDataSetX::CreateNew3DMotion ( unsigned __int64 id, MAP_RES& mapRes )
{
	if (id == 0)
		return NULL;
	MAP_RES::iterator iter = mapRes.find(id);
	if (iter == mapRes.end())
		return NULL;
	BOOL bBuffer = (m_setBufferMotion.find(iter->second) != m_setBufferMotion.end());
	return this->CreateNew3DMotion(iter->second.c_str(), TRUE, bBuffer);
}

I3DMotion* CGameDataSetX::CreateNew3DMotion ( const char* pszFileName, BOOL bCache, BOOL bBuffer )
{
	if (!pszFileName)
		return NULL;

	I3DMotion* pObj = NULL;
	if (!bCache)
		pObj = ::MotionCreate((char*)pszFileName, bBuffer);
	else
	{
		std::string strName = pszFileName;
		MAP_RESCACHE::iterator iter = m_mapMotionCache.find(strName);
		if (iter != m_mapMotionCache.end())
		{
			iter->second.nRef++;
			if (iter->second.pObj)
				return (I3DMotion*)iter->second.pObj;
		}
		pObj = ::MotionCreate((char*)pszFileName, bBuffer);
		if (pObj)
		{
			ResRef& ref = m_mapMotionCache[strName];
			ref.pObj = pObj;
			ref.nRef = 1;
		}
	}
	if (pObj)
		return pObj;

	::LogMsg("3d motion [%s] create error.", pszFileName);
	return NULL;
}

I3DTexture* CGameDataSetX::CreateNew3DTexture(OBJID id)
{
	if (id == ID_NONE)
		return NULL;

	MAP_RES::iterator iter = m_mapResTexture.find(id);
	if (iter == m_mapResTexture.end())
	{
		::LogMsg("texture [%u] not found!", id);
		return NULL;
	}

	const char* pszFileName = iter->second.c_str();
	MYASSERT (pszFileName);

	std::string strName = pszFileName;
	MAP_RESCACHE::iterator iterCache = m_mapTextureCache.find(strName);
	if (iterCache != m_mapTextureCache.end())
	{
		iterCache->second.nRef++;
		if (iterCache->second.pObj)
			return (I3DTexture*)iterCache->second.pObj;
	}

	I3DTexture* pTexture = ::TextureCreate(pszFileName);
	if (!pTexture)
	{
		::LogMsg("create texture [%s] failed.", pszFileName);
		return NULL;
	}
	ResRef& ref = m_mapTextureCache[strName];
	ref.pObj = pTexture;
	ref.nRef = 1;
	return pTexture;
}

I3DEffect* CGameDataSetX::CreateNew3DEffect(OBJID id)
{
	if (id == ID_NONE)
		return NULL;

	MAP_RES::iterator iter = m_mapResEffectObj.find(id);
	if (iter == m_mapResEffectObj.end())
	{
		::LogMsg("3d effect [%u] not found!", id);
		return NULL;
	}

	char szFileName[_MAX_PATH] = "";
	strcpy(szFileName, iter->second.c_str());

	std::string strName = szFileName;
	MAP_RESCACHE::iterator iterCache = m_mapEffectCache.find(strName);
	if (iterCache != m_mapEffectCache.end())
	{
		iterCache->second.nRef++;
		if (iterCache->second.pObj)
			return (I3DEffect*)iterCache->second.pObj;
	}

	I3DEffect* pEffect = ::EffectCreate(szFileName);
	if (!pEffect)
	{
		::LogMsg("Error: CreateNew3DEffect(%s) failed.", szFileName);
		return NULL;
	}
	ResRef& ref = m_mapEffectCache[strName];
	ref.pObj = pEffect;
	ref.nRef = 1;
	return pEffect;
}

void CGameDataSetX::Process3DObjSet(void)
{
	MAP_3DOBJEX::iterator iter = m_set3DObj.begin();
	while(iter != m_set3DObj.end())
	{
		C3DObjInfo* pInfo = iter->second;
		bool bOut = false;
		if (!pInfo)
			bOut = true;
		else if (pInfo->dwLife == 0)
		{
			if (::TimeGet() > pInfo->timeBirth + _3DOBJ_DEADLINE)
				bOut = true;
		}
		else if (::TimeGet() > pInfo->timeBirth + pInfo->dwLife)
			bOut = true;
		if (pInfo->dwLife != (DWORD)-1 && bOut)
		{
			if(pInfo)
			{
				SAFE_DELETE(pInfo);
			}

			MAP_3DOBJEX::iterator iterCopy = iter;
			iterCopy ++;
			m_set3DObj.erase(iter);
			iter = iterCopy;
		}
		else
		{
			iter ++;
		}
	}
	static DWORD s_timeObjSet = 0;
	if (::TimeGet() - s_timeObjSet >= 10000)
		s_timeObjSet = ::TimeGet();
}

void CGameDataSetX::Process3DMotionSet(MAP_3DMOTIONEX& mapMotion)
{
	MAP_3DMOTIONEX::iterator iter = mapMotion.begin();
	while(iter != mapMotion.end())
	{
		C3DMotionInfo* pInfo = iter->second;
		bool bOut = false;
		if (!pInfo)
			bOut = true;
		else if (pInfo->dwLife == 0)
		{
			if (::TimeGet() > m_dw3DMotionLife + pInfo->timeBirth)
				bOut = true;
		}
		else if (::TimeGet() > pInfo->timeBirth + pInfo->dwLife)
			bOut = true;
		if (pInfo->dwLife != (DWORD)-1 && bOut)
		{
			if(pInfo)
			{
				SAFE_DELETE(pInfo);
			}

			MAP_3DMOTIONEX::iterator iterCopy = iter;
			iterCopy ++;
			mapMotion.erase(iter);
			iter = iterCopy;
		}
		else
		{
			iter ++;
		}
	}
}

void CGameDataSetX::Process3DMotionSet(void)
{
	std::map<std::string, MAP_3DMOTIONEX>::iterator iter = m_set3DMotion.begin();
	while(iter != m_set3DMotion.end())
	{
		this->Process3DMotionSet(iter->second);
		iter ++;
	}
	::TimeGet();
}

void CGameDataSetX::Process3DTextureSet(void)
{
	MAP_3DTEXTUREEX::iterator iter = m_set3DTexture.begin();
	while(iter != m_set3DTexture.end())
	{
		C3DTextureInfo* pInfo = iter->second;
		bool bOut = false;
		if (!pInfo)
			bOut = true;
		else if (pInfo->dwLife == 0)
		{
			if (::TimeGet() > pInfo->timeBirth + m_dw3DTextureLife)
				bOut = true;
		}
		else if (::TimeGet() > pInfo->timeBirth + pInfo->dwLife)
			bOut = true;
		if (pInfo->dwLife != (DWORD)-1 && bOut)
		{
			if(pInfo)
			{
				SAFE_DELETE(pInfo);
			}

			MAP_3DTEXTUREEX::iterator iterCopy = iter;
			iterCopy ++;
			m_set3DTexture.erase(iter);
			iter = iterCopy;
		}
		else
		{
			iter ++;
		}
	}
	::TimeGet();
}

void CGameDataSetX::Process3DEffectSet(void)
{
	MAP_3DEFFECTEX::iterator iter = m_set3DEffect.begin();
	while(iter != m_set3DEffect.end())
	{
		C3DEffectInfo* pInfo = iter->second;
		bool bOut = false;
		if (!pInfo)
			bOut = true;
		else if (pInfo->dwLife == 0)
		{
			if (::TimeGet() > pInfo->timeBirth + m_dw3DEffectLife)
				bOut = true;
		}
		else if (::TimeGet() > pInfo->timeBirth + pInfo->dwLife)
			bOut = true;
		if (pInfo->dwLife != (DWORD)-1 && bOut)
		{
			if(pInfo)
			{
				SAFE_DELETE(pInfo);
			}

			MAP_3DEFFECTEX::iterator iterCopy = iter;
			iterCopy ++;
			m_set3DEffect.erase(iter);
			iter = iterCopy;
		}
		else
		{
			iter ++;
		}
	}
	static DWORD s_timeEffectSet = 0;
	if (::TimeGet() - s_timeEffectSet >= 10000)
		s_timeEffectSet = ::TimeGet();
}

void CGameDataSetX::ProcessDataAni ()
{
	MAP_DYNAMICANI::iterator iter = m_mapDataAni.begin();
	while(iter != m_mapDataAni.end())
	{
		IAni* pAni = iter->second;
		if(pAni && ::TimeGet() - pAni->GetTimeBorn() >= pAni->GetLife() &&
			ANISTATUS_LOADED == pAni->GetStatus())
		{
			GRAPHIC_RELEASE(pAni);
			MAP_DYNAMICANI::iterator iterCopy = iter;
			iterCopy ++;
			m_mapDataAni.erase(iter);
			iter = iterCopy;
		}
		else
		{
			iter ++;
		}
	}
}

void CGameDataSetX::Destroy3DSimpleObjInfo()
{
	if (m_p3DSimpleObjBin)
	{
		::operator delete(m_p3DSimpleObjBin);
		m_p3DSimpleObjBin = NULL;
		m_map3DSimpleObjInfo.clear();
		return;
	}

	std::map<OBJID, C3DSimpleObjInfo*>::iterator iter = m_map3DSimpleObjInfo.begin();
	while(iter != m_map3DSimpleObjInfo.end())
	{
		if(iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		iter ++;
	}
	m_map3DSimpleObjInfo.clear();
}

void CGameDataSetX::DestroyMy3DEffectInfo()
{
	if (m_pMy3DEffectBin)
	{
		::operator delete(m_pMy3DEffectBin);
		m_pMy3DEffectBin = NULL;
		m_mapMy3DEffectInfo.clear();
		return;
	}

	std::map<std::string, CMy3DEffectInfo*>::iterator iter = m_mapMy3DEffectInfo.begin();
	while(iter != m_mapMy3DEffectInfo.end())
	{
		if(iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		iter ++;
	}
	m_mapMy3DEffectInfo.clear();
}

void CGameDataSetX::Destroy3DObj()
{
	MAP_3DOBJEX::iterator iter = m_set3DObj.begin();
	while(iter != m_set3DObj.end())
	{
		if(iter->second)
		{
			SAFE_DELETE(iter->second);
		}
		iter ++;
	}
	m_set3DObj.clear();
}

void CGameDataSetX::Destroy3DMotion(MAP_3DMOTIONEX& mapMotion)
{
	MAP_3DMOTIONEX::iterator iter = mapMotion.begin();
	while(iter != mapMotion.end())
	{
		if(iter->second)
		{
			SAFE_DELETE(iter->second);
		}
		iter ++;
	}
	mapMotion.clear();
}

void CGameDataSetX::Destroy3DMotion()
{
	std::map<std::string, MAP_3DMOTIONEX>::iterator iter = m_set3DMotion.begin();
	while(iter != m_set3DMotion.end())
	{
		this->Destroy3DMotion(iter->second);
		iter ++;
	}
}

void CGameDataSetX::Destroy3DTexture()
{
	MAP_3DTEXTUREEX::iterator iter = m_set3DTexture.begin();
	while(iter != m_set3DTexture.end())
	{
		if(iter->second)
		{
			SAFE_DELETE(iter->second);
		}
		iter ++;
	}
	m_set3DTexture.clear();
}
void CGameDataSetX::Destroy3DEffect()
{
	MAP_3DEFFECTEX::iterator iter = m_set3DEffect.begin();
	while(iter != m_set3DEffect.end())
	{
		if(iter->second)
		{
			SAFE_DELETE(iter->second);
		}
		iter ++;
	}
	m_set3DEffect.clear();
	int nAmount = m_set3DEffect.size();
}

void CGameDataSetX::DestroyDataAni()
{
	MAP_DYNAMICANI::iterator iter = m_mapDataAni.begin();
	while(iter != m_mapDataAni.end())
	{
		if(iter->second)
			GRAPHIC_RELEASE(iter->second);
		iter ++;
	}
	m_mapDataAni.clear();
}

void CGameDataSetX::Init ( BOOL bBin )
{
	DWORD m_dw3DMotionLife			=	30*1000;
	DWORD m_dw3DTextureLife			=	30*1000;
	DWORD m_dw3DEffectLife			=	30*1000;
	DWORD m_dwPuzzleLife			=	30*1000;
	DWORD m_dwPuzzlePreloadSize		=	1;
	DWORD m_dwReadThreadPriority	=	THREAD_PRIORITY_NORMAL;

	this->OpenPackages();
	this->InitSystemPolicy();

	this->LoadRes("ini/3dobj.ini", m_mapResMesh, bBin);
	this->LoadRes("ini/3dtexture.ini", m_mapResTexture, bBin);
	this->LoadRes("ini/3deffectobj.ini", m_mapResEffectObj, bBin);
	this->LoadRes("ini/sound.ini", m_mapSound, bBin);

	this->Create3DSimpleObjInfo(bBin);
	this->CreateMy3DEffectInfo(bBin);
	this->CreateMaterialInfo();
	this->CreateEmotionInfo();
	this->CreateRolePartInfo(bBin);
	this->CreateActionTransInfo();
	this->CreateImmediateInfo();
	this->CreateMeshLodInfo();
	this->CreateBufferMotionInfo();

	::InitializeCriticalSection(&csReadThread);
	if (!::DataReadThreadInit())
		::ErrorMsg("动态数据读入线程初始化错误！");

	m_timerCheck = ::TimeGet();
	g_dwDeadLine	= 10000;

	m_pDataThread = ::DataThreadCreateNew();
	m_pDataThread->SetPriority(0);
	m_timeLoadAtOnce = (DWORD)-1;
	m_bLoadAtOnce = 1;
	this->SetLoadAtOnce(1000);
}

BOOL CGameDataSetX::Create3DSimpleObjInfo ( BOOL bBin )
{
	m_p3DSimpleObjBin = NULL;
	if (bBin)
		this->Create3DSimpleObjInfoBin();
	else
		this->Create3DSimpleObjInfoTxt();
	this->Create3DSimpleObjExInfo();
	return true;
}

BOOL CGameDataSetX::Create3DSimpleObjExInfo ( void )
{
	const char* pszFile = "ini/3DSimpleObjEx.ini";
	FILE* fp = fopen(pszFile, "r");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", pszFile);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle) - 1] = '\0';

		C3DSimpleObjExInfo info;
		info.idType = atoi(szTitle);
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Part=%d\n", &info.part.nPart);
		if (!bSuc || info.part.nPart >= 5)
		{
			::ErrorMsg("error part amount data: %s of %s, must <= %d", szTitle, pszFile, _MAXPART_ROLEPART);
			break;
		}
		for (int i=0; i<info.part.nPart; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "Mesh%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.part.idMesh[i]);
			sprintf(szIndex, "Texture%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.part.idTexture[i]);
			sprintf(szIndex, "MixTex%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.part.idMixTex[i]);
			sprintf(szIndex, "MixOpt%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.part.nMixOpt[i]);
			sprintf(szIndex, "Asb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.part.nAsb[i]);
			sprintf(szIndex, "Adb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.part.nAdb[i]);
			char szMaterial[256] = "";
			sprintf(szIndex, "Material%d=%s\n", i, "%s");
			bSuc &= fscanf(fp, szIndex, szMaterial);
			info.part.pMaterial[i] = this->GetMaterial(szMaterial);
		}
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, pszFile);
			break;
		}
		m_map3DSimpleObjExInfo[info.idType] = info;
	}
	fclose(fp);
	return true;
}

C3DSimpleObjExInfo* CGameDataSetX::Get3DSimpleObjExInfo ( OBJID id )
{
	std::map<OBJID, C3DSimpleObjExInfo>::iterator it = m_map3DSimpleObjExInfo.find(id);
	if (it == m_map3DSimpleObjExInfo.end())
		return NULL;
	return &it->second;
}

BOOL CGameDataSetX::CreateBufferMotionInfo ( void )
{
	FILE* fp = fopen("ini/BufferMotion.ini", "r");
	if (!fp)
		return false;
	m_setBufferMotion.clear();
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		if (szLine[0] == '\0')
			continue;
		m_setBufferMotion.insert(std::string(szLine));
	}
	fclose(fp);
	return true;
}

void CGameDataSetX::CreateImmediateInfo ( void )
{
	FILE* fp = fopen("ini/immediate.ini", "r");
	if (!fp)
		return;
	MAP_ID64* pMap = NULL;
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		if (strlen(szLine) == 0 || szLine[0] == ';')
			continue;
		if (szLine[0] == '[')
		{
			MAP_ID64* pNew = new MAP_ID64;
			char szName[260] = "";
			szLine[strlen(szLine) - 1] = 0;
			if (1 == sscanf(szLine, "[%s", szName))
			{
				std::string strName(szName);
				m_mapImmediate[strName] = pNew;
				pMap = pNew;
			}
		}
		else
		{
			unsigned __int64 id = 0;
			if (1 == sscanf(szLine, "%I64u", &id))
				(*pMap)[id] = 0;
		}
	}
	fclose(fp);
}

void CGameDataSetX::DestroyImmediateInfo ( void )
{
	for (std::map<std::string, MAP_ID64*>::iterator it = m_mapImmediate.begin(); it != m_mapImmediate.end(); it++)
	{
		if (it->second)
			delete it->second;
	}
	m_mapImmediate.clear();
}

bool CGameDataSetX::IsImmediate ( const char* pszKind, unsigned __int64 id )
{
	if (!pszKind)
		return false;
	std::map<std::string, MAP_ID64*>::iterator it = m_mapImmediate.find(std::string(pszKind));
	if (it == m_mapImmediate.end())
		return false;
	return it->second->find(id) != it->second->end();
}

void CGameDataSetX::CreateMeshLodInfo ( void )
{
	FILE* fp = fopen("ini/MeshLod.ini", "r");
	if (!fp)
		return;
	m_mapMeshLod.clear();
	while(true)
	{
		char szLine[1024] = "";
		if (!fgets(szLine, 0x3FF, fp))
			break;
		OBJID id = 0;
		MeshLodInfo info = { 0, 0, 0 };
		if (4 == sscanf(szLine, "%u %u %u %u\n", &id, &info.idLevel0, &info.idLevel1, &info.idLevel2))
			m_mapMeshLod[id] = info;
	}
	fclose(fp);
}

OBJID CGameDataSetX::GetMeshLod ( OBJID idMesh, int nLevel )
{
	std::map<OBJID, MeshLodInfo>::iterator it = m_mapMeshLod.find(idMesh);
	if (it == m_mapMeshLod.end())
		return idMesh;
	if (nLevel == 1)
		return it->second.idLevel1;
	if (nLevel == 2)
		return it->second.idLevel2;
	return it->second.idLevel0;
}

void CGameDataSetX::SetLoadAtOnce ( DWORD dwMilliseconds )
{
	m_bLoadAtOnce = 1;
	m_timeLoadAtOnce = ::timeGetTime() + dwMilliseconds;
}

BOOL CGameDataSetX::Reload ( BOOL bBin, DWORD dwFlags )
{
	for (VEC_DATAITEM::iterator it = m_vecDataItem.begin(); it != m_vecDataItem.end(); )
	{
		if (*it)
			delete *it;
	}
	m_vecDataItem.clear();

	while (!m_pDataThread->IsIdle())
	{
		::Sleep(100);
		this->ProcessFinished();
	}

	if (dwFlags & 0x01)
		this->Destroy3DTexture();
	if (dwFlags & 0x10)
		this->DestroyDataAni();
	if (dwFlags & 0x04)
		this->Destroy3DEffect();
	if (dwFlags & 0x02)
		this->Destroy3DObj();
	if (dwFlags & 0x08)
		this->Destroy3DMotion();
	if (dwFlags & 0x200)
	{
		this->Destroy3DSimpleObjInfo();
		this->Create3DSimpleObjInfo(bBin);
	}
	if (dwFlags & 0x40)
	{
		m_mapResEffectObj.clear();
		this->LoadRes("ini/3deffectobj.ini", m_mapResEffectObj, bBin);
		this->DestroyMy3DEffectInfo();
		this->CreateMy3DEffectInfo(bBin);
	}
	if (dwFlags & 0x80)
	{
		m_mapResMesh.clear();
		this->LoadRes("ini/3dobj.ini", m_mapResMesh, bBin);
	}
	if (dwFlags & 0x20)
	{
		m_mapResTexture.clear();
		this->LoadRes("ini/3dtexture.ini", m_mapResTexture, bBin);
	}
	if (dwFlags & 0x100)
	{
		m_mapPartKind.clear();
		m_mapRolePart.clear();
		m_mapMotionRes.clear();
		this->CreateRolePartInfo(bBin);
	}
	return TRUE;
}

void CDataItem::Process ()
{
	::EnterCriticalSection(&csReadThread);
	CGameDataSetX* pDataSet = (CGameDataSetX*)::GameDataSetQuery();
	switch (nKind)
	{
	case DATAITEM_TEXTURE:
		p3DTexture = pDataSet->CreateNew3DTexture((OBJID)id);
		break;
	case DATAITEM_EFFECT:
		p3DEffect = pDataSet->CreateNew3DEffect((OBJID)id);
		break;
	case DATAITEM_OBJ:
		p3DObj = pDataSet->CreateNew3DObj((OBJID)id, &p3DMotion);
		break;
	case DATAITEM_MOTION:
		p3DMotion = pDataSet->CreateNew3DMotion(id, strKind.c_str());
		break;
	}
	::LeaveCriticalSection(&csReadThread);
}

C3DObjInfo::~C3DObjInfo()
{
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = s_pGameDataSet->FindCacheByPointer(s_pGameDataSet->m_mapMeshCache, p3DObj, iter);
	if (pRef && --pRef->nRef == 0)
	{
		s_pGameDataSet->m_mapMeshCache.erase(iter);
		if (pDefaultMotion)
			GRAPHIC_RELEASE(pDefaultMotion);
		if (p3DObj)
			GRAPHIC_RELEASE(p3DObj);
	}
}

C3DMotionInfo::~C3DMotionInfo()
{
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = s_pGameDataSet->FindCacheByPointer(s_pGameDataSet->m_mapMotionCache, p3DMotion, iter);
	if (pRef && --pRef->nRef == 0)
	{
		s_pGameDataSet->m_mapMotionCache.erase(iter);
		if (p3DMotion)
			GRAPHIC_RELEASE(p3DMotion);
	}
}

C3DTextureInfo::~C3DTextureInfo()
{
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = s_pGameDataSet->FindCacheByPointer(s_pGameDataSet->m_mapTextureCache, p3DTexture, iter);
	if (pRef && --pRef->nRef == 0)
	{
		s_pGameDataSet->m_mapTextureCache.erase(iter);
		if (p3DTexture)
			GRAPHIC_RELEASE(p3DTexture);
	}
}

C3DEffectInfo::~C3DEffectInfo()
{
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = s_pGameDataSet->FindCacheByPointer(s_pGameDataSet->m_mapEffectCache, p3DEffect, iter);
	if (pRef && --pRef->nRef == 0)
	{
		s_pGameDataSet->m_mapEffectCache.erase(iter);
		if (p3DEffect)
			GRAPHIC_RELEASE(p3DEffect);
	}
}

ResRef* CGameDataSetX::FindCacheByPointer ( MAP_RESCACHE& mapCache, void* pObj, MAP_RESCACHE::iterator& iterOut )
{
	for (MAP_RESCACHE::iterator it = mapCache.begin(); it != mapCache.end(); it++)
	{
		if (it->second.pObj == pObj)
		{
			iterOut = it;
			return &it->second;
		}
	}
	return NULL;
}

bool CGameDataSetX::ReleaseCachedMotion ( I3DMotion* pMotion )
{
	if (!pMotion)
		return false;
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = this->FindCacheByPointer(m_mapMotionCache, pMotion, iter);
	if (pRef && --pRef->nRef == 0)
	{
		m_mapMotionCache.erase(iter);
		GRAPHIC_RELEASE(pMotion);
		return true;
	}
	return false;
}

bool CGameDataSetX::ReleaseCachedMesh ( I3DObj* pObj )
{
	if (!pObj)
		return false;
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = this->FindCacheByPointer(m_mapMeshCache, pObj, iter);
	if (pRef && --pRef->nRef == 0)
	{
		m_mapMeshCache.erase(iter);
		GRAPHIC_RELEASE(pObj);
		return true;
	}
	return false;
}

bool CGameDataSetX::ReleaseCachedEffect ( I3DEffect* pEffect )
{
	if (!pEffect)
		return false;
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = this->FindCacheByPointer(m_mapEffectCache, pEffect, iter);
	if (pRef && --pRef->nRef == 0)
	{
		m_mapEffectCache.erase(iter);
		GRAPHIC_RELEASE(pEffect);
		return true;
	}
	return false;
}

bool CGameDataSetX::ReleaseCachedTexture ( I3DTexture* pTexture )
{
	if (!pTexture)
		return false;
	MAP_RESCACHE::iterator iter;
	ResRef* pRef = this->FindCacheByPointer(m_mapTextureCache, pTexture, iter);
	if (pRef && --pRef->nRef == 0)
	{
		m_mapTextureCache.erase(iter);
		GRAPHIC_RELEASE(pTexture);
		return true;
	}
	return false;
}

void CGameDataSetX::UpdateLoadAtOnce ( void )
{
	if (m_timeLoadAtOnce > ::timeGetTime())
	{
		if (!m_bLoadAtOnce)
			::OutputDebugString("New Load EveryThing ImmediateExigence");
		m_bLoadAtOnce = 1;
	}
	else
	{
		if (m_bLoadAtOnce)
			::OutputDebugString("New Load EveryThing NormalExigence");
		m_bLoadAtOnce = 0;
	}
}

void CGameDataSetX::ProcessFinished ( void )
{
	if (!m_pDataThread->Commit())
		return;
	while (true)
	{
		CDataItem* pItem = (CDataItem*)m_pDataThread->PopFinished();
		if (!pItem)
			return;
		this->FileDataItem(pItem);
	}
}

void CGameDataSetX::FileDataItem ( CDataItem* pItem )
{
	switch (pItem->nKind)
	{
	case DATAITEM_TEXTURE:
		{
			MAP_3DTEXTUREEX::iterator iter = m_set3DTexture.find((OBJID)pItem->GetID());
			if (iter == m_set3DTexture.end())
				this->ReleaseCachedTexture(pItem->p3DTexture);
			else
			{
				C3DTextureInfo* pInfo = iter->second;
				if (pInfo)
				{
					pInfo->timeBirth = ::timeGetTime();
					if (!pInfo->p3DTexture)
						pInfo->p3DTexture = pItem->p3DTexture;
					else
						this->ReleaseCachedTexture(pItem->p3DTexture);
				}
			}
			delete pItem;
		}
		break;
	case DATAITEM_EFFECT:
		{
			MAP_3DEFFECTEX::iterator iter = m_set3DEffect.find((OBJID)pItem->GetID());
			if (iter == m_set3DEffect.end())
				this->ReleaseCachedEffect(pItem->p3DEffect);
			else
			{
				C3DEffectInfo* pInfo = iter->second;
				if (pInfo)
				{
					pInfo->timeBirth = ::timeGetTime();
					if (!pInfo->p3DEffect)
						pInfo->p3DEffect = pItem->p3DEffect;
					else
						this->ReleaseCachedEffect(pItem->p3DEffect);
				}
			}
			delete pItem;
		}
		break;
	case DATAITEM_OBJ:
		{
			MAP_3DOBJEX::iterator iter = m_set3DObj.find((OBJID)pItem->GetID());
			if (iter == m_set3DObj.end())
			{
				if (this->ReleaseCachedMesh(pItem->p3DObj) && pItem->p3DMotion)
					pItem->p3DMotion->Release();
			}
			else
			{
				C3DObjInfo* pInfo = iter->second;
				if (pInfo)
				{
					pInfo->timeBirth = ::timeGetTime();
					if (!pInfo->p3DObj)
					{
						pInfo->p3DObj			= pItem->p3DObj;
						pInfo->pDefaultMotion	= pItem->p3DMotion;
					}
					else if (this->ReleaseCachedMesh(pItem->p3DObj) && pItem->p3DMotion)
						pItem->p3DMotion->Release();
				}
			}
			delete pItem;
		}
		break;
	case DATAITEM_MOTION:
		{
			MAP_3DMOTIONEX& mapMotion = m_set3DMotion[pItem->strKind];
			MAP_3DMOTIONEX::iterator iter = mapMotion.find(pItem->GetID());
			if (iter == mapMotion.end())
				this->ReleaseCachedMotion(pItem->p3DMotion);
			else
			{
				C3DMotionInfo* pInfo = iter->second;
				if (!pInfo)
				{
					if (pItem->p3DMotion)
						pItem->p3DMotion->Release();
				}
				else
				{
					pInfo->timeBirth = ::timeGetTime();
					if (!pInfo->p3DMotion)
						pInfo->p3DMotion = pItem->p3DMotion;
					else
						this->ReleaseCachedMotion(pItem->p3DMotion);
				}
			}
			delete pItem;
		}
		break;
	}
}

BOOL CGameDataSetX::Create3DSimpleObjInfoTxt ( void )
{
	CONST CHAR INI_FILE[] = "ini/3DSimpleObj.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		C3DSimpleObjInfo* pInfo = new C3DSimpleObjInfo;
		if (!pInfo)
			break;
		pInfo->idType = atoi(szTitle+strlen("ObjIDType"));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "PartAmount=%d\n", &(pInfo->nParts));
		if (!bSuc || pInfo->nParts > _MAXPART_SIMPLEOBJ)
		{
			::ErrorMsg("error part amount data: %s of %s, must <= %d", szTitle, INI_FILE, _MAXPART_SIMPLEOBJ);
			delete pInfo;
			break;
		}
		for (int i=0; i<pInfo->nParts; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "Part%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &(pInfo->idMesh[i]));
			sprintf(szIndex, "Texture%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &(pInfo->idTexture[i]));
		}
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			delete pInfo;
			break;
		}
		m_map3DSimpleObjInfo[pInfo->idType] = pInfo;
	}
	fclose(fp);
	return true;
}

BOOL CGameDataSetX::Create3DSimpleObjInfoBin ( void )
{
	CONST CHAR BIN_FILE[] = "ini/3DSimpleObj.bin";
	FILE* fp = fopen(BIN_FILE, "rb");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", BIN_FILE);
		return false;
	}

	char szHeader[32] = "";
	fread(szHeader, 32, 1, fp);
	DWORD dwVersion = 0x10000;
	fread(&dwVersion, 4, 1, fp);

	int nAmount = _getw(fp);
	m_p3DSimpleObjBin = (char*) ::operator new(nAmount * sizeof(C3DSimpleObjInfo));
	if (m_p3DSimpleObjBin)
	{
		fread(m_p3DSimpleObjBin, sizeof(C3DSimpleObjInfo), nAmount, fp);
		for (int i = 0; i < nAmount; i++)
		{
			C3DSimpleObjInfo* pInfo = (C3DSimpleObjInfo*) m_p3DSimpleObjBin + i;
			m_map3DSimpleObjInfo[pInfo->idType] = pInfo;
		}
	}

	fclose(fp);
	return true;
}

BOOL CGameDataSetX::IsResDefined ( unsigned __int64 id, MAP_RES& mapRes )
{
	return mapRes.find(id) != mapRes.end();
}

BOOL CGameDataSetX::Build3DSimpleObjInfoBin ( void )
{
	CONST CHAR INI_FILE[] = "ini/3DSimpleObj.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", INI_FILE);
		return false;
	}

	CONST CHAR BIN_FILE[] = "ini/3DSimpleObj.bin";
	FILE* fpBin = fopen(BIN_FILE, "wb");
	if (!fpBin)
	{
		::LogMsg("open file %s failed.", BIN_FILE);
		fclose(fp);
		return false;
	}

	char szHeader[32] = "TqDigital3DSimpleObj";
	fwrite(szHeader, 32, 1, fpBin);
	DWORD dwVersion = 0x10000;
	fwrite(&dwVersion, 4, 1, fpBin);

	int nAmount = 0;
	_putw(0, fpBin);

	C3DSimpleObjInfo info;
	memset(&info, 0L, sizeof(info));
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		info.idType = atoi(szTitle+strlen("ObjIDType"));
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "PartAmount=%d\n", &(info.nParts));
		if (!bSuc || info.nParts > _MAXPART_SIMPLEOBJ)
		{
			::LogMsg("error part amount data: %s of %s, must <= %d", szTitle, INI_FILE, _MAXPART_SIMPLEOBJ);
			break;
		}
		for (int i=0; i<info.nParts; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "Part%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &(info.idMesh[i]));
			sprintf(szIndex, "Texture%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &(info.idTexture[i]));

			if (!this->IsResDefined(info.idMesh[i], m_mapResMesh))
				::LogMsg("%u is undefined: [ObjIDType%u]Part%d=%u, %s.", info.idMesh[i], info.idType, i, info.idMesh[i], INI_FILE);
			if (!this->IsResDefined(info.idTexture[i], m_mapResTexture))
				::LogMsg("%u is undefined: [ObjIDType%u]Texture%d=%u, %s.", info.idTexture[i], info.idType, i, info.idTexture[i], INI_FILE);
		}
		if (!bSuc)
		{
			::LogMsg("error data: %s of %s", szTitle, INI_FILE);
			break;
		}
		fwrite(&info, sizeof(info), 1, fpBin);
		nAmount++;
	}

	fseek(fpBin, 36, SEEK_SET);
	_putw(nAmount, fpBin);
	fclose(fpBin);
	fclose(fp);
	return true;
}

C3DSimpleObjInfo* CGameDataSetX::Get3DSimpleObjInfo ( OBJID id )
{
	std::map<OBJID, C3DSimpleObjInfo*>::iterator iter = m_map3DSimpleObjInfo.find(id);
	if (iter != m_map3DSimpleObjInfo.end())
		return iter->second;
	return NULL;
}

BOOL CGameDataSetX::CreateMy3DEffectInfo ( BOOL bBin )
{
	m_pMy3DEffectBin = NULL;
	if (bBin)
		this->CreateMy3DEffectInfoBin();
	else
		this->CreateMy3DEffectInfoTxt();
	return true;
}

BOOL CGameDataSetX::CreateMy3DEffectInfoTxt ( void )
{
	CONST CHAR INI_FILE[] = "ini/3DEffect.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", INI_FILE);
		return false;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		CMy3DEffectInfo* pInfo = new CMy3DEffectInfo;
		if (!pInfo)
			break;
		strcpy(pInfo->szIndex, szTitle);
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Amount=%d\n", &pInfo->nAmount);
		if (!bSuc || pInfo->nAmount > _MAXPART_MY3DEFFECT)
		{
			::ErrorMsg("error part amount data: %s of %s, must <= %d", szTitle, INI_FILE, _MAXPART_MY3DEFFECT);
			delete pInfo;
			break;
		}
		for (int i=0; i<pInfo->nAmount; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "EffectId%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &pInfo->idEffect[i]);
			sprintf(szIndex, "TextureId%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &pInfo->idTexture[i]);
			sprintf(szIndex, "Scale%d=%s\n", i, "%d");
			if (1 != fscanf(fp, szIndex, &pInfo->nScale[i]))
				pInfo->nScale[i] = 100;
			sprintf(szIndex, "ASB%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &pInfo->nASB[i]);
			sprintf(szIndex, "ADB%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &pInfo->nADB[i]);
		}
		bSuc &= fscanf(fp, "Delay=%d\n", &pInfo->nDelay);
		bSuc &= fscanf(fp, "LoopTime=%d\n", &pInfo->nLoopTime);
		bSuc &= fscanf(fp, "FrameInterval=%d\n", &pInfo->nFrameInterval);
		bSuc &= fscanf(fp, "LoopInterval=%d\n", &pInfo->nLoopInterval);
		bSuc &= fscanf(fp, "OffsetX=%d\n", &pInfo->nOffsetX);
		bSuc &= fscanf(fp, "OffsetY=%d\n", &pInfo->nOffsetY);
		bSuc &= fscanf(fp, "OffsetZ=%d\n", &pInfo->nOffsetZ);
		if (!bSuc)
		{
			::ErrorMsg("error data: [%s] of %s", szTitle, INI_FILE);
			delete pInfo;
			break;
		}
		std::string strIndex = pInfo->szIndex;
		std::map<std::string, CMy3DEffectInfo*>::iterator iterOld = m_mapMy3DEffectInfo.find(strIndex);
		if (iterOld != m_mapMy3DEffectInfo.end())
		{
			if (iterOld->second)
				delete iterOld->second;
			m_mapMy3DEffectInfo.erase(iterOld);
		}
		m_mapMy3DEffectInfo[strIndex] = pInfo;
	}
	fclose(fp);
	return true;
}

BOOL CGameDataSetX::CreateMy3DEffectInfoBin ( void )
{
	CONST CHAR BIN_FILE[] = "ini/3DEffect.bin";
	FILE* fp = fopen(BIN_FILE, "rb");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", BIN_FILE);
		return false;
	}

	char szHeader[32] = "";
	fread(szHeader, 32, 1, fp);
	DWORD dwVersion = 0;
	fread(&dwVersion, 4, 1, fp);

	int nAmount = _getw(fp);
	m_pMy3DEffectBin = (char*) ::operator new(nAmount * sizeof(CMy3DEffectInfo));
	if (m_pMy3DEffectBin)
	{
		fread(m_pMy3DEffectBin, sizeof(CMy3DEffectInfo), nAmount, fp);
		for (int i = 0; i < nAmount; i++)
		{
			CMy3DEffectInfo* pInfo = (CMy3DEffectInfo*) m_pMy3DEffectBin + i;
			std::string strIndex = pInfo->szIndex;
			m_mapMy3DEffectInfo[strIndex] = pInfo;
		}
	}

	fclose(fp);
	return true;
}

BOOL CGameDataSetX::BuildMy3DEffectInfoBin ( void )
{
	CONST CHAR INI_FILE[] = "ini/3DEffect.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::LogMsg("open file %s failed.", INI_FILE);
		return false;
	}

	CONST CHAR BIN_FILE[] = "ini/3DEffect.bin";
	FILE* fpBin = fopen(BIN_FILE, "wb");
	if (!fpBin)
	{
		::LogMsg("open file %s failed.", BIN_FILE);
		fclose(fp);
		return false;
	}

	char szHeader[32] = "TqDigital3DEffect";
	fwrite(szHeader, 32, 1, fpBin);
	DWORD dwVersion = 0x10000;
	fwrite(&dwVersion, 4, 1, fpBin);

	int nAmount = 0;
	_putw(0, fpBin);

	CMy3DEffectInfo info;
	memset(&info, 0L, sizeof(info));
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256] = "";
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		strcpy(info.szIndex, szTitle);
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Amount=%d\n", &info.nAmount);
		if (!bSuc || info.nAmount > _MAXPART_MY3DEFFECT)
		{
			::LogMsg("error part amount data: %s of %s, must <= %d", szTitle, INI_FILE, _MAXPART_MY3DEFFECT);
			break;
		}
		for (int i=0; i<info.nAmount; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "EffectId%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.idEffect[i]);
			sprintf(szIndex, "TextureId%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.idTexture[i]);

			if (!this->IsResDefined(info.idEffect[i], m_mapResEffectObj))
				::LogMsg("%u is undefined: [%s]EffectId%d=%u, %s.", info.idEffect[i], info.szIndex, i, info.idEffect[i], INI_FILE);
			if (!this->IsResDefined(info.idTexture[i], m_mapResTexture))
				::LogMsg("%u is undefined: [%s]TextureId%d=%u, %s.", info.idTexture[i], info.szIndex, i, info.idTexture[i], INI_FILE);

			sprintf(szIndex, "ASB%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.nASB[i]);
			sprintf(szIndex, "ADB%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.nADB[i]);
		}
		bSuc &= fscanf(fp, "Delay=%d\n", &info.nDelay);
		bSuc &= fscanf(fp, "LoopTime=%d\n", &info.nLoopTime);
		bSuc &= fscanf(fp, "FrameInterval=%d\n", &info.nFrameInterval);
		bSuc &= fscanf(fp, "LoopInterval=%d\n", &info.nLoopInterval);
		bSuc &= fscanf(fp, "OffsetX=%d\n", &info.nOffsetX);
		bSuc &= fscanf(fp, "OffsetY=%d\n", &info.nOffsetY);
		bSuc &= fscanf(fp, "OffsetZ=%d\n", &info.nOffsetZ);
		if (!bSuc)
		{
			::LogMsg("error data: [%s] of %s", szTitle, INI_FILE);
			break;
		}
		fwrite(&info, sizeof(info), 1, fpBin);
		nAmount++;
	}

	fseek(fpBin, 36, SEEK_SET);
	_putw(nAmount, fpBin);
	fclose(fpBin);
	fclose(fp);
	return true;
}

CMy3DEffectInfo* CGameDataSetX::GetMy3DEffectInfo ( const char* pszIndex )
{
	if (!pszIndex)
		return NULL;
	std::string strIndex = pszIndex;
	std::map<std::string, CMy3DEffectInfo*>::iterator iter = m_mapMy3DEffectInfo.find(strIndex);
	if (iter == m_mapMy3DEffectInfo.end())
		return NULL;
	return iter->second;
}

CMy3DEffectInfo* CGameDataSetX::FindMy3DEffectInfo ( const char* pszIndex )
{
	if (!pszIndex)
		return NULL;
	std::string strIndex(pszIndex);
	std::map<std::string, CMy3DEffectInfo*>::iterator iter = m_mapMy3DEffectInfo.find(strIndex);
	if (iter == m_mapMy3DEffectInfo.end())
		return NULL;
	return iter->second;
}

void CGameDataSetX::AddMy3DEffectInfo ( CMy3DEffectInfo* pInfo )
{
	if (!pInfo)
		return;
	CMy3DEffectInfo* pNew = new CMy3DEffectInfo;
	memcpy(pNew, pInfo, sizeof(CMy3DEffectInfo));
	std::string strIndex = pInfo->szIndex;
	m_mapMy3DEffectInfo[strIndex] = pNew;
}

BOOL CGameDataSetX::CreateRolePartInfo ( BOOL bBin )
{
	BOOL bSuc = true;
	CMyIni* pIni = CMyIni::CreateNew("ini/RolePart.ini", true);
	if (!pIni)
	{
		::ErrorMsg("open file %s failed.", "ini/RolePart.ini");
		return bSuc;
	}

	char szConfig[] = "Config";
	int nCount = pIni->GetInt(szConfig, "Count");
	int i;
	for (i = 0; i < nCount; i++)
	{
		const char* pszPart			= pIni->GetString(szConfig, CMyString("Part%d", i).c_str());
		const char* pszMeshIni		= pIni->GetString(szConfig, CMyString("MeshIni%d", i).c_str());
		const char* pszMotionIni	= pIni->GetString(szConfig, CMyString("MotionIni%d", i).c_str());
		std::string strMeshIni		= pszMeshIni;
		std::string strMotionIni	= pszMotionIni;
		std::string strPart			= pszPart;
		PartKind& kind = m_mapPartKind[strPart];
		kind.strMeshIni		= strMeshIni;
		kind.strMotionIni	= strMotionIni;
		bSuc &= this->CreateRolePart(pszMeshIni, bBin);
		bSuc &= this->CreateMotionRes(pszMotionIni, bBin);
	}

	char szDumy[] = "Dumy";
	int nDumy = pIni->GetInt(szDumy, "Count");
	for (i = 0; i < nDumy; i++)
	{
		const char* pszDumy = pIni->GetString(szDumy, CMyString("Dumy%d", i).c_str());
		std::string strDumy = pszDumy;
		m_setDumy.insert(strDumy);
	}

	delete pIni;
	return bSuc;
}

BOOL CGameDataSetX::CreateRolePart ( const char* pszMeshIni, BOOL bBin )
{
	if (bBin)
		this->CreateRolePartBin(pszMeshIni);
	else
		this->CreateRolePartTxt(pszMeshIni);
	return true;
}

BOOL CGameDataSetX::CreateRolePartTxt ( const char* pszMeshIni )
{
	if (!pszMeshIni)
		return false;
	std::string strFile = pszMeshIni;
	if (m_mapRolePart.find(strFile) != m_mapRolePart.end())
		return true;

	FILE* fp = fopen(pszMeshIni, "r");
	if (!fp)
		return false;
	MAP_ROLEPART& mapPart = m_mapRolePart[strFile];

	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		OBJID id = 0;
		if (1 != sscanf(szLine, "[%u]", &id))
			continue;

		RolePartInfo info;
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Part=%d\n", &info.nPart);
		if (!bSuc || info.nPart > _MAXPART_ROLEPART)
		{
			::ErrorMsg("error part amount data : [%u] of %s, must <= %d", id, pszMeshIni, _MAXPART_ROLEPART);
			return false;
		}
		for (int i=0; i<info.nPart; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "Mesh%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.idMesh[i]);
			sprintf(szIndex, "Texture%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.idTexture[i]);
			sprintf(szIndex, "MixTex%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &info.idMixTex[i]);
			sprintf(szIndex, "MixOpt%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.nMixOpt[i]);
			sprintf(szIndex, "Asb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.nAsb[i]);
			sprintf(szIndex, "Adb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &info.nAdb[i]);
			char szMaterial[256] = "";
			sprintf(szIndex, "Material%d=%s\n", i, "%s");
			bSuc &= fscanf(fp, szIndex, szMaterial);
			info.pMaterial[i] = this->GetMaterial(szMaterial);
		}
		if (!bSuc)
		{
			::ErrorMsg("error data : [%u] of %s", id, pszMeshIni);
			break;
		}
		mapPart[id] = info;
	}
	fclose(fp);
	return true;
}

struct RolePartBinRecord
{
	OBJID			id;
	RolePartInfo	info;
	char			szMaterial[_MAXPART_ROLEPART][32];
};

BOOL CGameDataSetX::CreateRolePartBin ( const char* pszMeshIni )
{
	if (!pszMeshIni)
		return false;
	std::string strFile = pszMeshIni;
	if (m_mapRolePart.find(strFile) != m_mapRolePart.end())
		return true;

	char szBin[256] = "";
	strcpy(szBin, pszMeshIni);
	int nExt = strstr(szBin, ".") - szBin + 1;
	strncpy(szBin + nExt, "bin", 3);
	szBin[nExt + 3] = '\0';

	FILE* fp = fopen(szBin, "rb");
	if (!fp)
	{
		::ErrorMsg("open file %s failed. ", szBin);
		return false;
	}

	char szHeader[32] = "";
	fread(szHeader, 32, 1, fp);
	DWORD dwVersion = 0x10000;
	fread(&dwVersion, 4, 1, fp);

	int nAmount = _getw(fp);
	RolePartBinRecord* pInfo = (RolePartBinRecord*) ::operator new(nAmount * sizeof(RolePartBinRecord));
	fread(pInfo, sizeof(RolePartBinRecord), nAmount, fp);

	MAP_ROLEPART& mapPart = m_mapRolePart[strFile];
	for (int i = 0; i < nAmount; i++)
	{
		for (int j = 0; j < pInfo[i].info.nPart && j < _MAXPART_ROLEPART; j++)
			pInfo[i].info.pMaterial[j] = this->GetMaterial(pInfo[i].szMaterial[j]);

		mapPart[pInfo[i].id] = pInfo[i].info;
	}

	if (pInfo)
		::operator delete(pInfo);
	fclose(fp);
	return true;
}

BOOL CGameDataSetX::BuildRolePartBin ( const char* pszMeshIni )
{
	if (!pszMeshIni)
		return false;
	std::string strFile = pszMeshIni;
	if (m_mapRolePart.find(strFile) != m_mapRolePart.end())
		return true;

	FILE* fp = fopen(pszMeshIni, "r");
	if (!fp)
	{
		::LogMsg("open file %s failed.", pszMeshIni);
		return false;
	}

	char szBin[256] = "";
	strcpy(szBin, pszMeshIni);
	int nExt = strstr(szBin, ".") - szBin + 1;
	strncpy(szBin + nExt, "bin", 3);
	szBin[nExt + 3] = '\0';

	FILE* fpBin = fopen(szBin, "wb");
	if (!fpBin)
	{
		::LogMsg("open file %s failed.", szBin);
		fclose(fp);
		return false;
	}

	int nName = strrchr(szBin, '/') - szBin + 1;
	char szName[32] = "";
	strncpy(szName, szBin + nName, nExt - nName - 1);
	char szHeader[32] = "";
	sprintf(szHeader, "%s%s", "TqDigital", szName);
	fwrite(szHeader, 32, 1, fpBin);
	DWORD dwVersion = 0x10000;
	fwrite(&dwVersion, 4, 1, fpBin);

	int nAmount = 0;
	_putw(0, fpBin);

	MAP_ROLEPART mapPart;
	m_mapRolePart.insert(std::map<std::string, MAP_ROLEPART>::value_type(std::string(pszMeshIni), mapPart));

	RolePartBinRecord rec;
	memset(&rec, 0L, sizeof(rec));
	char szLine[1024] = "";
	while(true)
	{
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		OBJID id = 0;
		if (1 != sscanf(szLine, "[%u]", &id))
			continue;

		rec.id = id;
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Part=%d\n", &rec.info.nPart);
		if (!bSuc || rec.info.nPart > _MAXPART_ROLEPART)
		{
			::LogMsg("error part amount data : [%u] of %s, must <= %d ", id, pszMeshIni, _MAXPART_ROLEPART);
			break;
		}
		for (int i=0; i<rec.info.nPart; i++)
		{
			char szIndex[256] = "";
			sprintf(szIndex, "Mesh%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &rec.info.idMesh[i]);
			sprintf(szIndex, "Texture%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &rec.info.idTexture[i]);

			if (!this->IsResDefined(rec.info.idMesh[i], m_mapResMesh))
				::LogMsg("%u is undefined: [%u]Mesh%d=%u, %s.", rec.info.idMesh[i], id, i, rec.info.idMesh[i], pszMeshIni);
			if (!this->IsResDefined(rec.info.idTexture[i], m_mapResTexture))
				::LogMsg("%u is undefined: [%u]Texture%d=%u, %s.", rec.info.idTexture[i], id, i, rec.info.idTexture[i], pszMeshIni);

			sprintf(szIndex, "MixTex%d=%s\n", i, "%u");
			bSuc &= fscanf(fp, szIndex, &rec.info.idMixTex[i]);
			sprintf(szIndex, "MixOpt%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &rec.info.nMixOpt[i]);
			sprintf(szIndex, "Asb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &rec.info.nAsb[i]);
			sprintf(szIndex, "Adb%d=%s\n", i, "%d");
			bSuc &= fscanf(fp, szIndex, &rec.info.nAdb[i]);
			char szMaterial[256] = "";
			rec.info.pMaterial[i] = NULL;
			sprintf(szIndex, "Material%d=%s\n", i, "%s");
			bSuc &= fscanf(fp, szIndex, szMaterial);
			strcpy(rec.szMaterial[i], szMaterial);
		}
		if (!bSuc)
		{
			::LogMsg("error data : [%u] of %s", id, pszMeshIni);
			break;
		}
		fwrite(&rec, sizeof(rec), 1, fpBin);
		nAmount++;
	}

	fseek(fpBin, 36, SEEK_SET);
	_putw(nAmount, fpBin);
	fclose(fpBin);
	fclose(fp);
	return true;
}

void CGameDataSetX::BuildMotionResBin ( const char* pszMotionIni )
{
	if (!pszMotionIni)
		return;
	std::string strFile = pszMotionIni;
	if (m_mapMotionRes.find(strFile) != m_mapMotionRes.end())
		return;
	MAP_RES& mapRes = m_mapMotionRes[strFile];
	this->BuildResBin(pszMotionIni, mapRes);
}

void CGameDataSetX::BuildRolePartInfoBin ( void )
{
	CMyIni* pIni = CMyIni::CreateNew("ini/RolePart.ini", true);
	if (!pIni)
	{
		::LogMsg("open file %s failed.", "ini/RolePart.ini");
		return;
	}

	char szConfig[] = "Config";
	int nCount = pIni->GetInt(szConfig, "Count");
	for (int i = 0; i < nCount; i++)
	{
		const char* pszMeshIni		= pIni->GetString(szConfig, CMyString("MeshIni%d", i).c_str());
		const char* pszMotionIni	= pIni->GetString(szConfig, CMyString("MotionIni%d", i).c_str());
		this->BuildRolePartBin(pszMeshIni);
		this->BuildMotionResBin(pszMotionIni);
	}

	SAFE_DELETE(pIni);
}

RolePartInfo* CGameDataSetX::FindRolePart ( MAP_ROLEPART& mapPart, OBJID id )
{
	MAP_ROLEPART::iterator iter = mapPart.find(id);
	if (iter == mapPart.end())
		return NULL;
	return &iter->second;
}

RolePartInfo* CGameDataSetX::GetRolePartInfo ( const char* pszKind, OBJID id )
{
	if (!pszKind)
		return NULL;
	std::string strKind = pszKind;
	std::map<std::string, PartKind>::iterator iterKind = m_mapPartKind.find(strKind);
	if (iterKind == m_mapPartKind.end())
		return NULL;
	std::map<std::string, MAP_ROLEPART>::iterator iterFile = m_mapRolePart.find(iterKind->second.strMeshIni);
	if (iterFile == m_mapRolePart.end())
		return NULL;
	return this->FindRolePart(iterFile->second, id);
}

BOOL CGameDataSetX::CreateMotionRes ( const char* pszMotionIni, BOOL bBin )
{
	if (!pszMotionIni)
		return false;
	std::string strFile = pszMotionIni;
	if (m_mapMotionRes.find(strFile) != m_mapMotionRes.end())
		return true;
	MAP_RES& mapRes = m_mapMotionRes[strFile];
	this->LoadRes(pszMotionIni, mapRes, bBin);
	return true;
}

CGameDataSetX::PartKind* CGameDataSetX::FindPartKind ( const char* pszKind )
{
	if (!pszKind)
		return NULL;
	std::string strKind = pszKind;
	std::map<std::string, PartKind>::iterator iter = m_mapPartKind.find(strKind);
	if (iter == m_mapPartKind.end())
		return NULL;
	return &iter->second;
}

const char* CGameDataSetX::GetMotionRes ( const char* pszKind, OBJID id )
{
	if (!pszKind)
		return NULL;
	PartKind* pKind = this->FindPartKind(pszKind);
	if (!pKind)
		return NULL;
	std::map<std::string, MAP_RES>::iterator iterRes = m_mapMotionRes.find(pKind->strMotionIni);
	if (iterRes == m_mapMotionRes.end())
		return NULL;
	MAP_RES::iterator iter = iterRes->second.find(id);
	if (iter == iterRes->second.end())
		return NULL;
	return iter->second.c_str();
}

void CGameDataSetX::CreateActionTransInfo ( void )
{
	CONST CHAR INI_FILE[] = "ini/ActionTransition.ini";
	FILE* fp = fopen(INI_FILE, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", INI_FILE);
		return;
	}
	while(true)
	{
		char szLine[1024];
		szLine[0] = '\0';
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		char szTitle[256];
		szTitle[0] = '\0';
		if (1 != sscanf(szLine, "[%s]", szTitle))
			continue;
		szTitle[strlen(szTitle)-1] = '\0';
		DWORD idAction = atoi(szTitle);

		std::map<DWORD, DWORD> mapTrans;
		int nCount = 0;
		BOOL bSuc = true;
		bSuc &= fscanf(fp, "Count=%d\n", &nCount);
		for (int i=0; i<nCount; i++)
		{
			DWORD dwIndex = 0, dwTrans = 0, dwTime = 0;
			bSuc &= (2 == fscanf(fp, "Trans%u=%u\n", &dwIndex, &dwTrans));
			bSuc &= (2 == fscanf(fp, "Time%u=%u\n", &dwIndex, &dwTime));
			if (dwTrans != 0 && dwTime != 0)
				mapTrans[dwTrans] = dwTime;
		}
		if (!bSuc)
		{
			::ErrorMsg("error data: %s of %s", szTitle, INI_FILE);
			return;
		}
		m_mapActionTrans[idAction] = mapTrans;
	}
	fclose(fp);
}

DWORD CGameDataSetX::GetActionTransTime ( DWORD idAction, DWORD idTrans )
{
	std::map<DWORD, std::map<DWORD, DWORD> >::iterator iterAction = m_mapActionTrans.find(idAction);
	if (iterAction == m_mapActionTrans.end())
		return 0;
	std::map<DWORD, DWORD>::iterator iter = iterAction->second.find(idTrans);
	if (iter == iterAction->second.end())
		return 0;
	return iter->second;
}

BOOL CGameDataSetX::IsHiddenPhy ( const char* pszPhyName )
{
	std::string strName = pszPhyName;
	return m_setDumy.find(strName) != m_setDumy.end();
}

void CGameDataSetX::Color2Value ( DWORD dwColor, D3DCOLORVALUE* pValue )
{
	pValue->a = (float)(dwColor >> 24) * (1.0f / 255);
	pValue->r = (float)((dwColor >> 16) & 0xff) * (1.0f / 255);
	pValue->g = (float)((dwColor >> 8) & 0xff) * (1.0f / 255);
	pValue->b = (float)(dwColor & 0xff) * (1.0f / 255);
}

BOOL CGameDataSetX::CreateMaterialInfo ( void )
{
	FILE* fp = fopen("ini/Material.ini", "r");
	if (!fp)
		return false;
	int nAmount = 0;
	fscanf(fp, "material=%d\n", &nAmount);
	char szName[256] = "";
	DWORD dwAmbient = 0, dwDiffuse = 0, dwSpecular = 0, dwEmissive = 0, dwPower = 0;
	for (int i = 0; i < nAmount; i++)
	{
		fscanf(fp, "%s %x %x %x %x %u\n", szName, &dwAmbient, &dwDiffuse, &dwSpecular, &dwEmissive, &dwPower);
		D3DMATERIAL8 material;
		ZeroMemory(&material, sizeof(material));
		this->Color2Value(dwAmbient, &material.Ambient);
		this->Color2Value(dwDiffuse, &material.Diffuse);
		this->Color2Value(dwSpecular, &material.Specular);
		this->Color2Value(dwEmissive, &material.Emissive);
		material.Power = (float)dwPower;
		std::string strName = szName;
		m_mapMaterial[strName] = material;
	}
	fclose(fp);
	return true;
}

D3DMATERIAL8* CGameDataSetX::GetMaterial ( const char* pszName )
{
	std::map<std::string, D3DMATERIAL8>::iterator iter;
	if (pszName)
	{
		std::string strName = pszName;
		iter = m_mapMaterial.find(strName);
	}
	else
		iter = m_mapMaterial.begin();
	if (iter == m_mapMaterial.end())
	{
		if (m_mapMaterial.empty())
		{
			static D3DMATERIAL8 s_material;
			return &s_material;
		}
		iter = m_mapMaterial.begin();
	}
	return &iter->second;
}
void CGameDataSetX::OpenPackages ( void )
{
	FILE* fp = fopen("ini/package.ini", "r");
	if (!fp)
	{
		::ErrorMsg("open file ini/package.ini failed.");
		return;
	}
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;
		::TqPackagesOpen(szLine);
	}
	fclose(fp);
}

void CGameDataSetX::InitSystemPolicy(void)
{
	MEMORYSTATUS stat;
	::GlobalMemoryStatus(&stat);
	char szTitle[64];
	if(stat.dwTotalPhys >= 400*1000*1000)
		strcpy(szTitle, "MEM512");
	else if(stat.dwTotalPhys >= 192*1000*1000)
		strcpy(szTitle, "MEM256");
	else
		strcpy(szTitle, "MEM128");

	const char szCommon[] = "ini/common.ini";
	char szSubtitle[128];
	int nData;
	strcpy(szSubtitle, "3DMotionLife");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
		m_dw3DMotionLife = nData;

	strcpy(szSubtitle, "3DTextureLife");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
		m_dw3DTextureLife = nData;

	strcpy(szSubtitle, "3DEffectLife");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
		m_dw3DEffectLife = nData;

	strcpy(szSubtitle, "PuzzleLife");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
		m_dwPuzzleLife = nData;

	strcpy(szSubtitle, "PuzzlePreloadSize");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
		m_dwPuzzlePreloadSize = nData;

	strcpy(szSubtitle, "ReadThreadPriority");
	if(::IniDataGet(szCommon, szTitle, szSubtitle, nData))
	{
		switch(nData)
		{
		case -1:
			m_dwReadThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case 0:
			m_dwReadThreadPriority = THREAD_PRIORITY_NORMAL;
			break;
		case 1:
			m_dwReadThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		default:
			m_dwReadThreadPriority = THREAD_PRIORITY_NORMAL;
			break;
		}
	}
}

void CGameDataSetX::LoadRes(const char* pszResFile, MAP_RES& mapRes, BOOL bBin)
{
	if (!pszResFile)
		return;
	if (bBin)
		this->LoadResBin(pszResFile, mapRes);
	else
		this->LoadResTxt(pszResFile, mapRes);
}

void CGameDataSetX::LoadResTxt(const char* pszResFile, MAP_RES& mapRes)
{
	if (!pszResFile)
		return;
	FILE* fp = fopen(pszResFile, "r");
	if (!fp)
		return;

	mapRes.clear();
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;

		unsigned __int64 id = 0;
		char szFile[256]	= "";

		if (2 == sscanf(szLine, "%I64u=%s", &id, szFile))
			mapRes[id] = szFile;
	}

	fclose(fp);
}

struct ResBinRecord
{
	unsigned __int64	id;
	BYTE				ucLen;
};

void CGameDataSetX::LoadResBin(const char* pszResFile, MAP_RES& mapRes)
{
	if (!pszResFile)
		return;

	char szBin[256] = "";
	strcpy(szBin, pszResFile);
	int nExt = strstr(szBin, ".") - szBin + 1;
	strncpy(szBin + nExt, "bin", 3);
	szBin[nExt + 3] = '\0';

	FILE* fp = fopen(szBin, "rb");
	if (!fp)
	{
		::ErrorMsg("open file %s failed.", szBin);
		return;
	}

	char szHeader[32] = "";
	fread(szHeader, 32, 1, fp);
	DWORD dwVersion = 0;
	fread(&dwVersion, 4, 1, fp);

	mapRes.clear();
	int nAmount = _getw(fp);
	for (int i = 0; i < nAmount; i++)
	{
		ResBinRecord rec;
		rec.id = 0;
		rec.ucLen = 0;
		char szFile[256] = "";
		fread(&rec, sizeof(rec), 1, fp);
		fread(szFile, rec.ucLen, 1, fp);
		mapRes[rec.id] = szFile;
	}

	fclose(fp);
}

void CGameDataSetX::BuildResBin(const char* pszResFile, MAP_RES& mapRes)
{
	if (!pszResFile)
		return;

	FILE* fp = fopen(pszResFile, "r");
	if (!fp)
	{
		::LogMsg("open file %s failed.", pszResFile);
		return;
	}

	char szBin[256] = "";
	strcpy(szBin, pszResFile);
	int nExt = strstr(szBin, ".") - szBin + 1;
	strncpy(szBin + nExt, "bin", 3);
	szBin[nExt + 3] = '\0';

	FILE* fpBin = fopen(szBin, "wb");
	if (!fpBin)
	{
		::LogMsg("open file %s failed.", szBin);
		fclose(fp);
		return;
	}

	char szHeader[32] = "TqDigitalRes";
	fwrite(szHeader, 32, 1, fpBin);
	DWORD dwVersion = 0x10000;
	fwrite(&dwVersion, 4, 1, fpBin);

	mapRes.clear();
	int nAmount = 0;
	_putw(0, fpBin);
	while(true)
	{
		char szLine[1024] = "";
		if (EOF == fscanf(fp, "%s\n", szLine))
			break;

		ResBinRecord rec;
		memset(&rec, 0, sizeof(rec));
		char szFile[256]	= "";

		if (2 == sscanf(szLine, "%I64u=%s", &rec.id, szFile))
		{
			if (!::TqFCheck(szFile, FILETYPE_ANY))
				::LogMsg("%s is missing: %I64u=%s, %s.", szFile, rec.id, szFile, pszResFile);

			rec.ucLen = (BYTE) strlen(szFile);
			fwrite(&rec, sizeof(rec), 1, fpBin);
			fwrite(szFile, rec.ucLen, 1, fpBin);
			mapRes[rec.id] = szFile;
			nAmount++;
		}
	}

	fseek(fpBin, 36, SEEK_SET);
	_putw(nAmount, fpBin);
	fclose(fpBin);
	fclose(fp);
}

const char* CGameDataSetX::GetSound ( OBJID idSound )
{
	MAP_RES::iterator iter = m_mapSound.find(idSound);
	if (iter == m_mapSound.end())
		return NULL;
	return iter->second.c_str();
}

const char* CGameDataSetX::GetVersionInfo ( void )
{
	return this->GetStr(10426);
}

void* CGameDataSetX::QueryInterface ( int nInterface )
{
	if (nInterface == 0)
		return static_cast<IDataIcon*>(this);
	return NULL;
}

IAni* CGameDataSetX::GetIcon ( int nIndex )
{
	int nAmount = this->GetEmotionAmount();
	if (nAmount <= 0)
		return NULL;
	char szTitle[32] = "";
	sprintf(szTitle, "ICON%d", nIndex % nAmount);
	return ::GameDataSetQuery()->GetDataAni("ani/EmotionIco.ani", szTitle, EXIGENCE_IMMEDIATE, 30000);
}

BOOL CGameDataSetX::CreateEmotionInfo ( void )
{
	char szIniFile[] = "ini/EmotionIco.ini";
	FILE* fp = fopen(szIniFile, "r");
	if (!fp)
	{
		::ErrorMsg("error on open %s.", szIniFile);
		return false;
	}
	while(true)
	{
		char szShutCut[1024] = "";
		int nIndex;
		if (EOF == fscanf(fp, "%d %s\n", &nIndex, szShutCut))
			break;
		std::string str = szShutCut;
		m_setEmotion.push_back (str);
	}
	fclose(fp);
	return true;
}

int CGameDataSetX::GetEmotionAmount ( void )
{
	return m_setEmotion.size ();
}

const char* CGameDataSetX::GetEmotionShutCut ( int nIndex )
{
	if (nIndex < 0 || nIndex >= (int)m_setEmotion.size())
		return NULL;
	return m_setEmotion[nIndex].c_str();
}

void CGameDataSetX::BuildBin ( void )
{
	fclose(fopen("debug.log", "w"));

	this->OpenPackages();

	this->BuildResBin("ini/3dobj.ini", m_mapResMesh);
	this->BuildResBin("ini/3dtexture.ini", m_mapResTexture);
	this->BuildResBin("ini/3deffectobj.ini", m_mapResEffectObj);
	this->BuildResBin("ini/sound.ini", m_mapSound);

	this->Build3DSimpleObjInfoBin();
	this->BuildMy3DEffectInfoBin();
	this->BuildRolePartInfoBin();
}
