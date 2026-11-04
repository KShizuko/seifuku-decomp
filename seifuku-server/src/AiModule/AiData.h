#include <windows.h>

const int	FACT_ID_FIRST				= 1000000;
inline char*	SystemFunction(int idx)
{
	static char*	g_setSysFunc[]={
		"dump",
		"keep",
		"now",
		"less",
		"less_equ",
		"strstr",
		"percent",

		NULL
	};

	CHECKF(idx < sizeof(g_setSysFunc));
	return g_setSysFunc[idx];
}

enum {
		SYS_FUNC_DUMP,
		SYS_FUNC_KEEP,

		SYS_FUNC_NOW,
		SYS_FUNC_LESS,
		SYS_FUNC_LESS_EQU,
		SYS_FUNC_STRSTR,
		SYS_FUNC_PERCENT,
};