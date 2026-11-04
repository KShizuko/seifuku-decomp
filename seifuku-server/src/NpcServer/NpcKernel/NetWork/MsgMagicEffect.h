#ifndef		_MSGMAGICEFFECT_H_
#define		_MSGMAGICEFFECT_H_

#include "NetMsg.h"

enum {
		MAGICSORT_ATTACK			= 1,
		MAGICSORT_RECRUIT			= 2,
		MAGICSORT_CROSS				= 3,
		MAGICSORT_FAN				= 4,
		MAGICSORT_BOMB				= 5,
		MAGICSORT_ATTACHSTATUS		= 6,
		MAGICSORT_DETACHSTATUS		= 7,
		MAGICSORT_SQUARE			= 8,
		MAGICSORT_JUMPATTACK		= 9,
		MAGICSORT_RANDOMTRANS		= 10,
		MAGICSORT_DISPATCHXP		= 11,
		MAGICSORT_COLLIDE			= 12,
		MAGICSORT_SERIALCUT			= 13,
		MAGICSORT_LINE				= 14,
		MAGICSORT_ATKRANGE			= 15,
		MAGICSORT_ATKSTATUS			= 16,
		MAGICSORT_CALLTEAMMEMBER	= 17,
		MAGICSORT_RECORDTRANSSPELL	= 18,
		MAGICSORT_TRANSFORM			= 19,
		MAGICSORT_ADDMANA			= 20,
		MAGICSORT_LAYTRAP			= 21,
		MAGICSORT_DANCE				= 22,
};

typedef struct 
{
	DWORD	idRole;
	DWORD	dwData;
}EffectRoleStruct;

class CMsgMagicEffect : public CNetMsg
{
public:
	CMsgMagicEffect();
	virtual ~CMsgMagicEffect();

public:

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

private:
	typedef struct
	{
		MSGHEAD_DEFINE

		OBJID				idUser;
		union{
			OBJID				idTarget;
			struct{
				USHORT				usPosX;
				USHORT				usPosY;
			};
			UCHAR				ucCollideDir;
		};
		USHORT				usType;
		USHORT				usLevel;
		UCHAR				ucEffectNum;
		EffectRoleStruct	setEffectRole[1];
	}MSG_Info;

	MSG_Info*	m_pInfo;
};

#endif