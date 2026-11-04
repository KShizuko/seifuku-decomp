#ifndef		_MSGMAGICEFFECT_H_
#define		_MSGMAGICEFFECT_H_

#include "NetMsg.h"

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
	BOOL	Create(OBJID idUser, int nType, int nLevel, OBJID idTarget);
	BOOL	Create(OBJID idUser, int nType, int nLevel, OBJID idTarget, DWORD dwData, int nDeltaBattleEffect=0);
	BOOL	CreateByPos(OBJID idUser, int nType, int nLevel, int x, int y, OBJID idTarget=ID_NONE, DWORD dwPower=0, int nDeltaBattleEffect=0);
	BOOL	AppendRole(OBJID idRole, DWORD dwData, int nDeltaBattleEffect);

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
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
