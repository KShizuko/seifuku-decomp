#if !defined(AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_)
#define AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {
		actionEnterMap			=74,
		actionGetItemSet		= 75,
		actionGetGoodFriend		=76,
		actionGetWeaponSkillSet	=77,
		actionGetMagicSet		=78,
		actionChgDir			=79, 
		actionEmotion			=81,
		actionDivorce			= 83,
		actionSelfUnfreeze		=84,
		actionChgMap			=85,
		actionFlyMap			=86,
		actionDie				=89,
		actionQuitSyn			=90,
		actionReborn			= 94,
		actionGetSynAttr		= 97,
		actionMapStatus			=105,
		actionMoveStop			=107,
		actionKickBack			=108,
		actionLeaveMap			=132,
		actionJump				=133,
		actionSynchro			=138,

		actionSuperChgMap		= 200,
		actionFullItem			= 201,
		actionChangeMapDoc		= 202,
		actionAddTerrainObj		= 203,
		actionDelTerrainObj		= 204,

};

class CMsgAction : public CNetMsg  
{
public:
	CMsgAction();
	virtual ~CMsgAction();

	BOOL	Create	(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, DWORD dwData=0);
	BOOL	Create	(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, USHORT usTargetPosX, USHORT usTargetPosY);

public:	
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void *pInfo);

protected:
	typedef struct {
		MSGHEAD_DEFINE

		DWORD		dwTimeStamp;

		OBJID		idUser;
		union {
			OBJID		idTarget;
			DWORD		dwData;
			INT			iData;
			BOOL		bSucDone;
			struct {
				USHORT usTargetPosX;
				USHORT usTargetPosY;
			};
		};

		USHORT		unPosX, unPosY;
		USHORT		unDir;

		USHORT		usAction;
	}MSG_Info;

	MSG_Info*	m_pInfo;

};

#endif
