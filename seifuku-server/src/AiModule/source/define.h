#ifndef	DEFINE_H
#define	DEFINE_H

#include "common.h"
#include "T_SafePtr.h"

const int	IPSTR_SIZE			= 16;
const int	_MAX_NAMESIZE		= 16;
const int	MAX_TALKSIZE		= 64;
const int	MAX_FIELDSIZE		= 256;

typedef	unsigned long	OBJID;

typedef	char			IPSTR[IPSTR_SIZE];
typedef	char			NAMESTR[_MAX_NAMESIZE];
typedef	char			SQLBUF[1024];
typedef	char			MSGBUF[1024];
typedef	char			TALKSTR[MAX_TALKSIZE];
typedef	char			FIELDBUF[MAX_FIELDSIZE];

const int	ID_NONE			= 0;

const bool	UPDATE_TRUE				= true;
const bool	UPDATE_FALSE			= false;
const bool	DELRECORD_TRUE			= true;
const bool	DELRECORD_FALSE			= false;
const bool	SYNCHRO_TRUE			= true;
const bool	SYNCHRO_FALSE			= false;

const int _DELTA_X[9]={ 0,-1,-1,-1, 0, 1, 1, 1, 0 };
const int _DELTA_Y[9]={ 1, 1, 0,-1,-1,-1, 0, 1, 0 };
const int MAX_DIRSIZE = 8;
inline int GetDirByPos(int nFromX, int nFromY, int nToX, int nToY)
{
	if(nFromX < nToX)
	{
		if(nFromY < nToY)
			return 7;
		else if(nFromY > nToY)
			return 5;
		else
			return 6;
	}
	else if(nFromX > nToX)
	{
		if(nFromY < nToY)
			return 2;
		else if(nFromY > nToY)
			return 4;
		else
			return 3;
	}
	else
	{
		if(nFromY < nToY)
			return 0;
		else if(nFromY > nToY)
			return 4;
	}
	return MAX_DIRSIZE;
}

#endif