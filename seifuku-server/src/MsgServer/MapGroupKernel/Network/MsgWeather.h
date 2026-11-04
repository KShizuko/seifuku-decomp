#ifndef	MSGWEATHER_H
#define MSGWEATHER_H

#include "NetMsg.h"

class CMsgWeather : public CNetMsg
{
public:
	CMsgWeather();
	virtual ~CMsgWeather();

	BOOL	Create (int nType, int nIntensity, int nDir, DWORD nColor);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		int 			nType;
		int				nIntensity;
		int				nDir;
		DWORD			dwColor;

	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif
