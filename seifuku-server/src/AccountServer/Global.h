#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
using namespace std;

extern HANDLE	g_xDatabase;

extern long	s_nSocketCount;
extern long	s_nLoginCount;
extern long	s_nMaxServerTime;
extern long	s_nAvgServerTime;
extern long	s_nPointCount;
extern long s_nLoginAccept;
extern long s_nPointFee;
extern long s_nPointSum;
extern long s_nAllTimeSum;
extern long s_nDatabaseTimeSum;

void	PrintText(const char * szFormat, ...);
bool	LockedGetText(char * buf);

enum	{ c_flagNone='.', c_flagOffline=',', c_flagSocket=':', c_flagAccount='-', c_flagStop='#', c_flagNormal='=', c_flagNormal91U='U', 
						c_flagBaseMax='@',
						c_flagBegin='S', c_flagPoint='Y', c_flagEnd='X', c_flagHeartbeat='?', 
						c_flagBegin2='s', c_flagPoint2='y', c_flagEnd2='x' };
void	SetServerState(int nServer, int nState);
void	GetServerState(char * buf);

extern	time_t	g_tStartServerTime;

typedef	char	NAMESTR[_MAX_NAMESIZE];
struct	ISP_ST{
	NAMESTR		szName;
	NAMESTR		szFirstIP;
	NAMESTR		szLastIP;
	NAMESTR		szAccount;
	NAMESTR		szPassword;
};
typedef vector<ISP_ST>		ISP_SET;

extern COnlineTable *	g_pOnlineTable;
extern CPointThread *	g_pPointThread;
extern CLoginThread *	g_pLoginThread;
extern CAccount			g_cDatabase;
extern bool				g_bEnableLogin;

extern CServerAccount	g_aServerAccount[];
extern int				g_nServerAccount;
extern char				g_szCountHeadLine[];
extern ISP_SET			g_setISPList;

DWORD	NewAuthenID(unsigned long nClientIP);
bool	SafeCopy(char * pTarget, const char * pSource, int nBufLen = 0);
bool	FormatStr(char* pszBuf, int nBufLen = 0);

enum	{	c_errPassword		= 1,
			c_errNotPoint		= 6,
			c_errNotTime		= 7,
			c_errUnknowServer	= 10,
			c_errOnline			= 11,
			c_errBan			= 12,
			c_errBarPassword	= 13,
			c_errBarOverflow	= 14,
			c_errServerBusy		= 20,
			c_errServerFull		= 21,
			c_errUnknown		= 999
};

bool	GetAccount(LPCTSTR szAccount, LPCTSTR szPassword, 
					OBJID& idAccount, int& nType, int& nPoint, DWORD& nPointTime, int& nBlock);
bool	IsFreeServer(LPCTSTR szServer);
bool	CheckISP(OBJID idAccount, unsigned long nClientIP, LPCTSTR pClientIP, 
				 OBJID& idFeeAccount, char* szFeeAccount, int& nPoint, DWORD& nPointTime, char* szNotifyMsg);
bool	Check7DaysNodify(DWORD nPointTime, char* szNotifyMsg);

#endif