#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "define.h"
#include "I_Shell.h"

int					LOGIN_FREE_LEVEL	= 16;
struct STAT_STRUCT	g_stat;
long				g_sKickoutCheat		= 0;
long				g_nServerClosed		= 0;
long				g_nRestart			= 0;
unsigned long		g_dwMaxPlayer		= 800;
char				szSynTable[]		= "cq_syndicate";

class IDatabase;
IDatabase* CreateDatabase(const char*, const char*, const char*, const char*, int, bool)	{ return 0; }
