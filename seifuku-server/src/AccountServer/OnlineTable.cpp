#include "AllHeads.h"
#include "OnlineTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const DWORD	INVALID_SERIAL			= 1234567890;

COnlineAccount::COnlineAccount()
{
	Init();
}

COnlineAccount::~COnlineAccount()
{
}

void COnlineAccount::Init()
{
	m_nState			= c_stateOnlineNone;
	m_idAccount			= 0;
	m_szLoginName[0]	= 0;
	m_szPassword[0]		= 0;
	m_nAuthenID			= 0;
	m_szClientIP[0]		= 0;
	m_szServerName[0]	= 0;
	m_nFeeType			= 0;
	m_idFeeAccount		= ID_NONE;
	m_szFeeAccount[0]	= 0;
	m_tStartPoint		= 0;
	m_tLastPoint		= 0;
	m_tHeartbeat		= 0;
	m_tTimeWait			= 0;
	m_tEndPoint			= 0;
	m_nEndType			= c_endNone;
	m_nPointCount		= 0;
	m_dwSerial			= INVALID_SERIAL;
}

void COnlineAccount::Init(int nState, 
						  OBJID		idAccount, 
						  DWORD		nAuthenID, 
						  LPCTSTR	pszLoginName, 
						  LPCTSTR	pszPassword, 
						  LPCTSTR	pszClientIP, 
						  LPCTSTR	pszServerName, 
						  int		nFeeType,
						  OBJID		idFeeAccount, 
						  LPCTSTR	szFeeAccount)
{
	ASSERT(nState);
	ASSERT(idAccount);

	m_nState			= nState;

	m_idAccount			= idAccount;
	m_nAuthenID			= nAuthenID;
	SafeCopy(m_szLoginName, pszLoginName, _MAX_NAMESIZE);
	SafeCopy(m_szPassword, pszPassword, _MAX_NAMESIZE);

	SafeCopy(m_szClientIP, pszClientIP, IPSTRSIZE);
	SafeCopy(m_szServerName, pszServerName, _MAX_NAMESIZE);

	m_nFeeType			= nFeeType;
	m_idFeeAccount		= idFeeAccount;
	SafeCopy(m_szFeeAccount, szFeeAccount, _MAX_NAMESIZE);
	m_tStartPoint		= 0;
	m_tLastPoint		= 0;
	m_tEndPoint			= 0;

	m_nEndType			= c_endNone;
	m_nPointCount		= 0;
	m_tHeartbeat		= clock();
	m_tTimeWait			= 0;

	m_dwSerial			= INVALID_SERIAL;
}

COnlineTable::COnlineTable(int nSize)
{ 
	LOCKTHREAD;

	m_pAccount = new COnlineAccount[nSize]; 

	m_nCount = 0; 

}

COnlineTable::~COnlineTable()
{
	LOCKTHREAD;

	LOGMSG("[%d] entries left when the online table was destroyed", m_nCount);
#ifdef	TEST_INDEX
	ASSERT(m_idxID.Size() == m_nCount);
	ASSERT(m_idxName.Size() == m_nCount);
#endif
	for(int nIndex = 1; nIndex < ONLINETABLESIZE; nIndex++)
	{
		if(m_pAccount[nIndex].m_nState)
		{
			m_pAccount[nIndex].m_tEndPoint	= time(NULL);
			LogFee(nIndex, COnlineAccount::c_endAccountCloseKick);
			Remove_0(nIndex);
		}
	}
#ifdef	TEST_INDEX
	ASSERT(m_idxID.Size() == m_nCount);
	ASSERT(m_idxName.Size() == m_nCount);
#endif

	LOGMSG("[%d] entries left after the online table was cleared", m_nCount);

	if(m_nCount)
		LOGWARNING("~COnlineTable() exception, m_nCount not 0 [%d]", m_nCount);
#ifdef	TEST_INDEX
	if(m_idxName.Size())
	{
		LOGWARNING("idxName.Size() : %d", m_idxName.Size());
		for(INDEXNAME::iterator j = m_idxName.Begin(); j != m_idxName.End(); j++)
		{
			LOGWARNING("---> name[%s], index[%d]", m_idxName.Key(j), m_idxName.Data(j));
		}
	}
	if(m_idxID.Size())
	{
		LOGWARNING("idxID.Size() : %d", m_idxID.Size());
		for(INDEXID::iterator i = m_idxID.Begin(); i != m_idxID.End(); i++)
		{
			LOGWARNING("---> id[%d], index[%d]", m_idxID.Key(i), m_idxID.Data(i));
		}
	}
#endif

	m_aPlayerCount.ClearAll();
	if(STAT_SERIAL)
		m_aSerialCount.ClearAll();
	delete [] m_pAccount; 
}
bool	COnlineTable::GetAttr(const char * szAccount, OBJID &idAccount, char * szPassword, 
					  DWORD &nAuthenID, char * szServerName, char* szFeeAccount, OBJID &idFeeAccount, char* szOldIP)
{
	LOCKTHREAD;

	int nIndex = FindIndex(szAccount);
	if(nIndex && m_pAccount[nIndex].m_nState)
	{
		idAccount		= m_pAccount[nIndex].m_idAccount;
		SafeCopy(szPassword, m_pAccount[nIndex].m_szPassword, _MAX_NAMESIZE);
		nAuthenID		= m_pAccount[nIndex].m_nAuthenID;
		SafeCopy(szServerName, m_pAccount[nIndex].m_szServerName, _MAX_NAMESIZE);
		SafeCopy(szFeeAccount, m_pAccount[nIndex].m_szFeeAccount, _MAX_NAMESIZE);
		idFeeAccount	= m_pAccount[nIndex].m_idFeeAccount;
		SafeCopy(szOldIP, m_pAccount[nIndex].m_szClientIP, _MAX_NAMESIZE);
		return true;
	}
	return false;
}

bool	COnlineTable::GetAttr2(OBJID idAccount, int &nFeeType, char * szServerName, 
												OBJID &idFeeAccount, char* szFeeAccount)
{
	LOCKTHREAD;

	int nIndex = FindIndex(idAccount);
	if(nIndex && m_pAccount[nIndex].m_nState)
	{
		nFeeType		= m_pAccount[nIndex].m_nFeeType;
		SafeCopy(szServerName, m_pAccount[nIndex].m_szServerName, _MAX_NAMESIZE);
		idFeeAccount	= m_pAccount[nIndex].m_idFeeAccount;
		SafeCopy(szFeeAccount, m_pAccount[nIndex].m_szFeeAccount, _MAX_NAMESIZE);
		return true;
	}
	return false;
}

#define		IDXASSERT(x,y,z)	{if(!(x)) LOGERROR("ASSERT(" #x ") -> [%s][%d] in %d line of %s", y, z, __LINE__, __FILE__); }
bool	COnlineTable::AddNew(OBJID idAccount, DWORD nAuthenID, const char * pClientIP, const char * szLoginName
			, const char * szPassword, int nPointType, const char * szServerName, OBJID	idFeeAccount, LPCTSTR szFeeAccount)
{
	LOCKTHREAD;

	int	nFreeIndex = FindFree(idAccount);
	if(nFreeIndex)
	{
		m_nCount++;
		m_aPlayerCount.Add(szServerName, m_aPlayerCount[szServerName] + 1);
		m_pAccount[nFreeIndex].Init(COnlineAccount::c_stateOnlineNormal,
									idAccount,
									nAuthenID,
									szLoginName,
									szPassword,
									pClientIP,
									szServerName,
									nPointType, 
									idFeeAccount,
									szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(!m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(!m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
		m_idxID.Add(idAccount, nFreeIndex);
		m_idxName.Add(szLoginName, nFreeIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

		return true;
	}
	else
	{
		LOGWARNING("Sharing conflict in ADDNEW(), account [%s] may log in twice. Using REJOIN", szLoginName);

		int		nIndex = FindIndex(szLoginName);
		ASSERT(nIndex == FindIndex(idAccount));
		ASSERT(nIndex);

		if(strcmp(szServerName, m_pAccount[nIndex].m_szServerName) != 0 
				|| strcmp(pClientIP, m_pAccount[nIndex].m_szClientIP) != 0
				|| strcmp(szFeeAccount, m_pAccount[nIndex].m_szFeeAccount) != 0)
			return false;

		if(m_pAccount[nIndex].m_tStartPoint)
		{
			m_pAccount[nIndex].m_tHeartbeat = clock();
			m_pAccount[nIndex].m_tEndPoint	= time(NULL);
			LogFee(nIndex, COnlineAccount::c_endAccountRejoinKick);
		}

		ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
		ASSERT(strcmp(m_pAccount[nIndex].m_szLoginName, szLoginName) == 0);
		m_pAccount[nIndex].Init(COnlineAccount::c_stateOnlineRejoin,
									idAccount,
									nAuthenID,
									szLoginName,
									szPassword,
									pClientIP,
									szServerName,
									nPointType, 
									idFeeAccount,
									szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
		ASSERT(m_idxID.IsIn(idAccount));
		ASSERT(m_idxName.IsIn(szLoginName));
		m_idxID.Add(idAccount, nIndex);
		m_idxName.Add(szLoginName, nIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

		return true;
	}
}

bool	COnlineTable::AppendNew(OBJID idAccount, DWORD nAuthenID, const char * pClientIP, const char * szLoginName
			, const char * szPassword, int nPointType, const char * szServerName, OBJID	idFeeAccount, LPCTSTR szFeeAccount)
{
	LOCKTHREAD;

	int	nFreeIndex = FindFree(idAccount);
	if(nFreeIndex)
	{
		m_nCount++;
		m_aPlayerCount.Add(szServerName, m_aPlayerCount[szServerName] + 1);
		m_pAccount[nFreeIndex].Init(COnlineAccount::c_stateOnlineRejoin,
									idAccount,
									nAuthenID,
									szLoginName,
									szPassword,
									pClientIP,
									szServerName,
									nPointType, 
									idFeeAccount,
									szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(!m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(!m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
		m_idxID.Add(idAccount, nFreeIndex);
		m_idxName.Add(szLoginName, nFreeIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

		return true;
	}
	else
	{
		LOGWARNING("Sharing conflict in APPENDNEW(). Account [%s] uses REJOIN", szLoginName);

		int		nIndex = FindIndex(szLoginName);
		ASSERT(nIndex == FindIndex(idAccount));
		ASSERT(nIndex);
		if(strcmp(szServerName, m_pAccount[nIndex].m_szServerName) != 0 
				|| strcmp(pClientIP, m_pAccount[nIndex].m_szClientIP) != 0
				|| strcmp(szFeeAccount, m_pAccount[nIndex].m_szFeeAccount) != 0)
			return false;

		if(m_pAccount[nIndex].m_tStartPoint)
		{
			m_pAccount[nIndex].m_tHeartbeat = clock();
			m_pAccount[nIndex].m_tEndPoint	= time(NULL);
			LogFee(nIndex, COnlineAccount::c_endAccountRejoinKick);
		}

		ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
		ASSERT(strcmp(m_pAccount[nIndex].m_szLoginName, szLoginName) == 0);
		m_pAccount[nIndex].Init(COnlineAccount::c_stateOnlineRejoin,
									idAccount,
									m_pAccount[nIndex].m_nAuthenID,
									szLoginName,
									m_pAccount[nIndex].m_szPassword,
									m_pAccount[nIndex].m_szClientIP,
									szServerName,
									nPointType, 
									idFeeAccount,
									szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
		m_idxID.Add(idAccount, nIndex);
		m_idxName.Add(szLoginName, nIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

		return true;
	}
}

bool	COnlineTable::Rejoin(OBJID idAccount, DWORD nAuthenID, const char * pClientIP, const char * szLoginName
				, const char * szPassword, int nPointType, const char * szServerName, OBJID	idFeeAccount, LPCTSTR szFeeAccount)
{
	LOCKTHREAD;

	int		nIndex = FindIndex(szLoginName);
	ASSERT(nIndex == FindIndex(idAccount));
	if(nIndex)
	{
		if(strcmp(szServerName, m_pAccount[nIndex].m_szServerName) != 0 
				|| strcmp(pClientIP, m_pAccount[nIndex].m_szClientIP) != 0
				|| strcmp(szFeeAccount, m_pAccount[nIndex].m_szFeeAccount) != 0)
			return false;

		if(m_pAccount[nIndex].m_tStartPoint)
		{
			m_pAccount[nIndex].m_tHeartbeat = clock();
			m_pAccount[nIndex].m_tEndPoint	= time(NULL);
			LogFee(nIndex, COnlineAccount::c_endAccountRejoinKick);
		}

		ASSERT(strcmp(m_pAccount[nIndex].m_szPassword, szPassword) == 0);

		ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
		ASSERT(strcmp(m_pAccount[nIndex].m_szLoginName, szLoginName) == 0);
		m_pAccount[nIndex].Init(COnlineAccount::c_stateOnlineRejoin,
									idAccount,
									nAuthenID,
									szLoginName,
									szPassword,
									pClientIP,
									szServerName,
									nPointType, 
									idFeeAccount,
									szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
		m_idxID.Add(idAccount, nIndex);
		m_idxName.Add(szLoginName, nIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

		return true;
	}
	else
	{
		LOGWARNING("Sharing conflict in REJOIN(). Player [%s] uses ADDNEW", szLoginName);

		int	nFreeIndex = FindFree(idAccount);
		if(nFreeIndex)
		{
			m_nCount++;
			m_aPlayerCount.Add(szServerName, m_aPlayerCount[szServerName] + 1);
			m_pAccount[nFreeIndex].Init(COnlineAccount::c_stateOnlineNormal,
										idAccount,
										nAuthenID,
										szLoginName,
										szPassword,
										pClientIP,
										szServerName,
										nPointType, 
										idFeeAccount,
										szFeeAccount);
#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(!m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(!m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif
			m_idxID.Add(idAccount, nFreeIndex);
			m_idxName.Add(szLoginName, nFreeIndex);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount+1, szLoginName, idAccount);
		IDXASSERT(m_idxID.IsIn(idAccount), szLoginName, idAccount);
		IDXASSERT(m_idxName.IsIn(szLoginName), szLoginName, idAccount);
#endif

			return true;
		}
		else
		{
			LOGERROR("Rejoin() internal exception. Online table count [%d/%d]", GetPlayerCount(), ONLINETABLESIZE);
			return false;
		}
	}
}

int  COnlineTable::StartFee(OBJID idAccount, const char* pszServerName)
{
	LOCKTHREAD;

	int nIndex = FindIndex(idAccount);
	ASSERT(nIndex);
	if(!nIndex)
		return -1;

	ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
	if(strcmp(m_pAccount[nIndex].m_szServerName, pszServerName))
	{
		LOGERROR("Bad STARTFEE. Player [%s][%d] logged in to [%s], but STARTFEE came from [%s]",
					m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount, 
					m_pAccount[nIndex].m_szServerName, pszServerName);
		return -1;
	}

	ASSERT(m_pAccount[nIndex].m_nState != COnlineAccount::c_stateOnlineNone);
	m_pAccount[nIndex].m_tHeartbeat = clock();
	if(m_pAccount[nIndex].m_tStartPoint)
	{
#ifdef	DUMP
		LOGERROR("MSG_STARTFEE received twice. Game server [%s], player account [%s]",
				m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName);
#endif
	}
	else
	{
		if(m_pAccount[nIndex].m_nPointCount)
		{
			LOGERROR("Program error. Player [%s] got MSG_STARTFEE with points not cleared in the online table [%d].",
				m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_nPointCount);
			m_pAccount[nIndex].m_nPointCount = 0;
		}
		m_pAccount[nIndex].m_tStartPoint	= time(NULL);
	}

	return 0;
}
		
int  COnlineTable::EndFee(OBJID idAccount, const char* pszServerName, bool bOffline)
{
	LOCKTHREAD;

	int nIndex = FindIndex(idAccount);
	if(!nIndex)
	{
		if(g_bEnableLogin && !bOffline)
			LOGWARNING("Extra ENDFEE, game server [%s] account ID [%d]", pszServerName, idAccount);
		return -1;
	}

	ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
	if(strcmp(pszServerName, m_pAccount[nIndex].m_szServerName))
	{
		if(!bOffline)
			LOGERROR("Bad END_FEE, ignored. Player [%s][%d] is on [%s], but the message came from [%s].",
						m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount, 
						m_pAccount[nIndex].m_szServerName, pszServerName);
		return -1;
	}

	m_pAccount[nIndex].m_tHeartbeat = clock();
	m_pAccount[nIndex].m_tEndPoint		= time(NULL);

	if(m_pAccount[nIndex].m_tStartPoint)
	{
		if(bOffline)
			LogFee(nIndex, COnlineAccount::c_endGameServerLose);
		else if(m_pAccount[nIndex].m_nState == COnlineAccount::c_stateOnlineNormal)
			LogFee(nIndex, COnlineAccount::c_endNormal);
		else if(m_pAccount[nIndex].m_nState == COnlineAccount::c_stateOnlineRejoin)
			LogFee(nIndex, COnlineAccount::c_endRejoin);
		else
			LOGERROR("COnlineTable::EndFee(OBJID idAccount) internal exception");
	}
	else
	{
		LogFee(nIndex, COnlineAccount::c_endGameServerKick);
	}

	int nCount = (m_pAccount[nIndex].m_tEndPoint - m_pAccount[nIndex].m_tStartPoint) / (POINTFEEINTERVALSECS*1000);
	if(m_pAccount[nIndex].m_tStartPoint && nCount - m_pAccount[nIndex].m_nPointCount > 0)
	{
		char szStartTime[20];
		char szPointTime[20];
		char szEndTime[20];
		char szStamp[15];
		DateTime(szStartTime, m_pAccount[nIndex].m_tStartPoint);
		DateTime(szPointTime, m_pAccount[nIndex].m_tLastPoint);
		DateTime(szEndTime, m_pAccount[nIndex].m_tEndPoint);
		time_t	tCurr = time(NULL);
		tm*	pTm = localtime(&tCurr);
		sprintf(szStamp, "%04d%02d%02d%02d%02d%02d", pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
		ASSERT(c_typeAll == 7);
		ASSERT(COnlineAccount::c_endAll == 8);
		const char *	aFeeType[] = { "Unknown", "Point", "Monthly", "NetBar point", "NetBar monthly", "ISP", "Free", "Error", "Error"  };
		const char *	aEndType[] = { "Unknown", "Normal logout", "Delayed logout", "Kicked by game server", "Duplicate login",
								"Timeout logout", "Account server closed", "Login info lost", "Error", "Error" };
		const char *	pFeeType = aFeeType[0];
		const char *	pEndType = aEndType[0];
		if(m_pAccount[nIndex].m_nFeeType >= c_typeNone && m_pAccount[nIndex].m_nFeeType < c_typeAll)
			pFeeType = aFeeType[m_pAccount[nIndex].m_nFeeType];
		if(m_pAccount[nIndex].m_nEndType > COnlineAccount::c_endNone && m_pAccount[nIndex].m_nEndType < COnlineAccount::c_endAll)
			pEndType = aEndType[m_pAccount[nIndex].m_nEndType];
		LOGERROR("Fee error too large: [%s] sent the end-fee message of [%d]\n"
					"Start [%s], last point [%s], end [%s], \n"
					"Fee type [%s], points total [%d], end type [%s]",
					m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_idAccount, 
					szStartTime, szPointTime, szEndTime, 
					pFeeType, m_pAccount[nIndex].m_nPointCount, pEndType);
	}

	ASSERT(m_pAccount[nIndex].m_nState);
	if(m_pAccount[nIndex].m_nState == COnlineAccount::c_stateOnlineNormal || bOffline || REJOINTIMEWAITSECS == 0)
		Remove_0(nIndex);
	else
		m_pAccount[nIndex].m_tTimeWait = clock() + REJOINTIMEWAITSECS*CLOCKS_PER_SEC;

	return 0;
}

int COnlineTable::PointFee(OBJID idAccount, const char* pszServerName)
{
	LOCKTHREAD;

	int nIndex = FindIndex(idAccount);
	if(!nIndex)
	{
		LOGERROR("COnlineTable::PointFee(OBJID idAccount) internal exception");
		return	-1;
	}

	ASSERT(m_pAccount[nIndex].m_idAccount == idAccount);
	if(strcmp(m_pAccount[nIndex].m_szServerName, pszServerName))
	{
		LOGERROR("Bad POINT_FEE. Player [%s][%d] logged in to [%s], but the message came from [%s]. IP [%s]",
					m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount, 
					m_pAccount[nIndex].m_szServerName, pszServerName, 
					m_pAccount[nIndex].m_szClientIP);
		return -1;
	}

	ASSERT(m_pAccount[nIndex].m_nState != COnlineAccount::c_stateOnlineNone);
	m_pAccount[nIndex].m_tHeartbeat = clock();
	if(!m_pAccount[nIndex].m_tStartPoint)
	{
#ifdef	DUMP
		LOGWARNING("POINTFEE received before STARTFEE, account ID [%d]. Maybe a backfilled account", m_pAccount[nIndex].m_idAccount);
#endif
		m_pAccount[nIndex].m_tStartPoint	= time(NULL);
	}
	m_pAccount[nIndex].m_tLastPoint	= time(NULL);
	m_pAccount[nIndex].m_nPointCount++;

	int nRet = m_pAccount[nIndex].m_nPointCount;
	if(nRet > 0)
		return nRet;
	else
		return 0;
}

bool	COnlineTable::RemoveOvertime()
{
	LOCKTHREAD;

	bool	ret = false;
	for(int nIndex = 1; nIndex < ONLINETABLESIZE; nIndex++)
	{
		if(m_pAccount[nIndex].m_nState && m_pAccount[nIndex].m_tTimeWait && clock() > m_pAccount[nIndex].m_tTimeWait)
		{
			Remove_0(nIndex);
			ret = true;
		}

		if(m_pAccount[nIndex].m_nState 
					&& clock() > m_pAccount[nIndex].m_tHeartbeat+ONLINEHEARTBEATSECS*CLOCKS_PER_SEC)
		{
			m_pAccount[nIndex].m_tEndPoint	= time(NULL);
			LogFee(nIndex, COnlineAccount::c_endAccountOvertimeKick);
			Remove_0(nIndex);
			ret = true;
		}
	}
	return ret;
}

bool	COnlineTable::GetServerName(OBJID idAccount, char* bufServer) 
{ 
	LOCKTHREAD; 

	int		nIndex = FindIndex(idAccount);
	if(nIndex)
	{
		SafeCopy(bufServer, m_pAccount[nIndex].m_szServerName, _MAX_NAMESIZE);
		return true;
	}
	else
		return false;
}

int	COnlineTable::GetFeeCount(OBJID idFeeAccount)
{
	LOCKTHREAD;

	int 	nCount = 0;
	for(int nIndex = 1; nIndex < ONLINETABLESIZE; nIndex++)
	{
		if(m_pAccount[nIndex].m_nState && m_pAccount[nIndex].m_idFeeAccount == idFeeAccount)
			nCount++;
	}
	return nCount;
}

bool COnlineTable::CheckUniqueIP(LPCTSTR pClientIP, OBJID idAccount)
{
	LOCKTHREAD;

	ASSERT(!( pClientIP == 0 || *pClientIP == 0 ));
	if(pClientIP == 0 || *pClientIP == 0)
		return false;

	for(int nIndex = 1; nIndex < ONLINETABLESIZE; nIndex++)
	{
		if(m_pAccount[nIndex].m_nFeeType == c_typeISP 
				&& strcmp(m_pAccount[nIndex].m_szClientIP, pClientIP) == 0 
				&& m_pAccount[nIndex].m_idAccount != idAccount)
			return false;
	}
	return true;
}

int		COnlineTable::FindIndex(const char * szLoginName)
{

#ifdef	INDEXOPTIMIZE
	int	nIndex = m_idxName[szLoginName];
	if(!nIndex || strcmp(m_pAccount[nIndex].m_szLoginName, szLoginName) == 0)
		return nIndex;
	LOGERROR("Online table index out of sync: account name [%s] has index [%d], but index [%d] has account name [%s]. Index removed.★",
					szLoginName, nIndex, nIndex, m_pAccount[nIndex].m_szLoginName);
	m_idxName.Del(szLoginName);
	return 0;
#endif

	for(int i = 1; i < ONLINETABLESIZE; i++)
	{
		if(m_pAccount[i].m_nState && strcmp(m_pAccount[i].m_szLoginName, szLoginName) == 0)
		{
			if(i != m_idxName[szLoginName])
				LOGERROR("TEST: login name [%s] index out of sync [%d][%d], internal error", szLoginName, i, m_idxName[szLoginName]);

			return i;
		}
	}

	if(m_idxName[szLoginName])
		LOGERROR("TEST: login name [%s] index out of sync [%d], internal error.", szLoginName, m_idxName[szLoginName]);

	return 0;
}

int		COnlineTable::FindIndex(OBJID idAccount)
{

#ifdef	INDEXOPTIMIZE
	int	nIndex = m_idxID[idAccount];
	if(!nIndex || m_pAccount[nIndex].m_idAccount == idAccount)
		return nIndex;
	LOGERROR("Online table index out of sync: account ID [%d] has index [%d], but index [%d] has account ID [%d]. Index removed.★",
					idAccount, nIndex, nIndex, m_pAccount[nIndex].m_idAccount);
	m_idxID.Del(idAccount);
	return 0;
#endif

	for(int i = 1; i < ONLINETABLESIZE; i++)
	{
		if(m_pAccount[i].m_nState && m_pAccount[i].m_idAccount == idAccount)
		{
			if(i != m_idxID[idAccount])
				LOGERROR("TEST: account ID [%d] index out of sync [%d][%d], internal error", idAccount, i, m_idxID[idAccount]);

			return i;
		}
	}

	if(m_idxID[idAccount])
		LOGERROR("TEST: account ID [%d] index out of sync [%d], internal error.", idAccount, m_idxID[idAccount]);

	return 0;
}

int		COnlineTable::FindFree(OBJID idAccount)
{

	int	nFreeIndex = 0;
	for(int i = 1; i < ONLINETABLESIZE; i++)
	{
		if(m_pAccount[i].m_nState && m_pAccount[i].m_idAccount == idAccount)
		{
			LOGERROR("Tried to add online player [%d] twice, sharing conflict.", idAccount);
			return 0;
		}
		else if(!m_pAccount[i].m_nState && !nFreeIndex)
		{
			nFreeIndex = i;
		}
	}

	if(nFreeIndex)
	{
		return nFreeIndex;
	}
	else
	{
		LOGERROR("Online player table overflow [%d]; edit config.ini and restart.", m_nCount);
		return 0;
	}
}

void	COnlineTable::Remove_0(int nIndex)
{
	if(nIndex && m_pAccount[nIndex].m_nState)
	{ 
		LPCTSTR	szServerName = m_pAccount[nIndex].m_szServerName;
		ASSERT(m_aPlayerCount.IsIn(szServerName));
		if(m_aPlayerCount[szServerName] > 0)
			m_aPlayerCount.Add(szServerName, m_aPlayerCount[szServerName] - 1);
		if(m_nCount > 0)
			m_nCount--;
		if(STAT_SERIAL)
		{
			DWORD dwSerial = m_pAccount[nIndex].m_dwSerial;
			if(dwSerial != INVALID_SERIAL && m_aSerialCount[dwSerial] > 0)
				m_aSerialCount.Add(dwSerial, m_aSerialCount[dwSerial] - 1);
		}

#ifdef	TEST_INDEX
		int	nOldIDCount = m_idxID.Size();
		int	nOldNameCount = m_idxName.Size();
		IDXASSERT(m_idxID.IsIn(m_pAccount[nIndex].m_idAccount), m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
		IDXASSERT(m_idxName.IsIn(m_pAccount[nIndex].m_szLoginName), m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
#endif
		m_idxID.Del(m_pAccount[nIndex].m_idAccount);
		m_idxName.Del(m_pAccount[nIndex].m_szLoginName);
#ifdef	TEST_INDEX
		IDXASSERT(m_idxID.Size() == nOldIDCount-1, m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
		IDXASSERT(m_idxName.Size() == nOldNameCount-1, m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
		IDXASSERT(!m_idxID.IsIn(m_pAccount[nIndex].m_idAccount), m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
		IDXASSERT(!m_idxName.IsIn(m_pAccount[nIndex].m_szLoginName), m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount);
#endif
		m_pAccount[nIndex].Init();
	}
	else
	{
		LOGERROR("Remove_0() internal exception, Index[%d], State[%d]", nIndex, m_pAccount[nIndex].m_nState);
	}
}

void	COnlineTable::LogFee(int nIndex, enum COnlineAccount::EEndType nEndType)
{
	m_pAccount[nIndex].m_nEndType	= nEndType;
	char szStartTime[20];
	char szPointTime[20];
	char szEndTime[20];
	char szStamp[255];
	DateTime(szStartTime, m_pAccount[nIndex].m_tStartPoint);
	DateTime(szPointTime, m_pAccount[nIndex].m_tLastPoint);
	DateTime(szEndTime, m_pAccount[nIndex].m_tEndPoint);
	time_t	tCurr = time(NULL);
	tm*	pTm = localtime(&tCurr);
	sprintf(szStamp, "%04d%02d%02d%02d%02d%02d", pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	ASSERT(c_typeAll == 7);
	ASSERT(COnlineAccount::c_endAll == 8);
	const char *	aFeeType[] = { "Unknown", "Point", "Monthly", "NetBar point", "NetBar monthly", "ISP", "Free", "Error", "Error" };
	const char *	aEndType[] = { "Unknown", "Normal logout", "Delayed logout", "Kicked by game server", "Duplicate login",
							"Timeout logout", "Account server closed", "Login info lost", "Error", "Error" };
	const char *	pFeeType = aFeeType[0];
	const char *	pEndType = aEndType[0];
	if(m_pAccount[nIndex].m_nFeeType >= c_typeNone && m_pAccount[nIndex].m_nFeeType < c_typeAll)
		pFeeType = aFeeType[m_pAccount[nIndex].m_nFeeType];
	if(m_pAccount[nIndex].m_nEndType > COnlineAccount::c_endNone && m_pAccount[nIndex].m_nEndType < COnlineAccount::c_endAll)
		pEndType = aEndType[m_pAccount[nIndex].m_nEndType];
	if(m_pAccount[nIndex].m_nPointCount)
	{
		LOGPOINT("[%s]: [%s][%d] logged out. Start [%s], last point [%s], end [%s],"
					"fee account [%s][%d], fee type [%s], points total [%d], end type [%s], IP [%s]\n"
			"INSERT DELAYED fee VALUES ('%s','%s','%s',   %d,'%s','%s','%s',   '%s',%d,%d,   %d,%d,'%s');"
					, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount
					, szStartTime, szPointTime, szEndTime
					, m_pAccount[nIndex].m_szFeeAccount, m_pAccount[nIndex].m_idFeeAccount
					, pFeeType, m_pAccount[nIndex].m_nPointCount, pEndType, m_pAccount[nIndex].m_szClientIP
			, szStamp, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName
			, m_pAccount[nIndex].m_idAccount, szStartTime, szPointTime, szEndTime
			, m_pAccount[nIndex].m_szFeeAccount, m_pAccount[nIndex].m_idFeeAccount, m_pAccount[nIndex].m_nFeeType
			, m_pAccount[nIndex].m_nPointCount, m_pAccount[nIndex].m_nEndType, m_pAccount[nIndex].m_szClientIP
			);
		if(m_pAccount[nIndex].m_nFeeType == c_typeISP)
		{
			LOGISPPOINT(m_pAccount[nIndex].m_szFeeAccount, "[%s]: [%s][%d] logged out. Start [%s], last point [%s], end [%s],"
						"fee account [%s][%d], fee type [%s], points total [%d], end type [%s], IP [%s]\n"
				"INSERT DELAYED fee VALUES ('%s','%s','%s',   %d,'%s','%s','%s',   '%s',%d,%d,   %d,%d,'%s');"
						, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount
						, szStartTime, szPointTime, szEndTime
						, m_pAccount[nIndex].m_szFeeAccount, m_pAccount[nIndex].m_idFeeAccount
						, pFeeType, m_pAccount[nIndex].m_nPointCount, pEndType, m_pAccount[nIndex].m_szClientIP
				, szStamp, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName
				, m_pAccount[nIndex].m_idAccount, szStartTime, szPointTime, szEndTime
				, m_pAccount[nIndex].m_szFeeAccount, m_pAccount[nIndex].m_idFeeAccount, m_pAccount[nIndex].m_nFeeType
				, m_pAccount[nIndex].m_nPointCount, m_pAccount[nIndex].m_nEndType, m_pAccount[nIndex].m_szClientIP
				);
		}
	}
	else
	{
		LOGPOINT("[%s]: [%s] logged out. Account ID [%d], start [%s], last point [%s], end [%s],"
					"fee account [%s], fee type [%s], points total [%d], end type [%s], IP [%s]"
					, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName, m_pAccount[nIndex].m_idAccount
					, szStartTime, szPointTime, szEndTime
					, m_pAccount[nIndex].m_szFeeAccount, pFeeType, m_pAccount[nIndex].m_nPointCount
					, pEndType, m_pAccount[nIndex].m_szClientIP
			);
	}

	char szSQL[1024];
	sprintf(szSQL, "INSERT DELAYED fee VALUES ('%s','%s','%s',   %d,'%s','%s','%s',   '%s',%d,%d,   %d,%d,'%s');"
			, szStamp, m_pAccount[nIndex].m_szServerName, m_pAccount[nIndex].m_szLoginName
			, m_pAccount[nIndex].m_idAccount, szStartTime, szPointTime, szEndTime
			, m_pAccount[nIndex].m_szFeeAccount, m_pAccount[nIndex].m_idFeeAccount, m_pAccount[nIndex].m_nFeeType
			, m_pAccount[nIndex].m_nPointCount, m_pAccount[nIndex].m_nEndType, m_pAccount[nIndex].m_szClientIP
			);
	g_db.ExecuteSQL(szSQL);
}
