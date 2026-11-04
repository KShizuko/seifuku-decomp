#if !defined(AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_)
#define AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {
		actionNone				=0,
		actionEnterMap			=74,
		actionGetItemSet		=75,
		actionGetGoodFriend		=76,
		actionGetWeaponSkillSet	=77,
		actionGetMagicSet		=78,
		actionChgDir			=79,
		actionEmotion			=81,
		actionChgMap			=85,
		actionFlyMap			=86,
		actionQuitSyn			=90,
		actionUplev				=92,
		actionXpCLear			=93,
		actionReborn			=94,
		actionDelRole			=95,
		actionSetPkMode			=96,
		actionGetSynAttr		=97,
		actionMine				=99,
		actionTeamMemeberPos	=100,
		actionTeamLeaderPos		=101,
		actionQueryPlayer		=102,
		actionAbortMagic		=103,
		actionMapARGB			=104,
		actionQueryTeamMember	=106,
		actionKickBack			=108,
		actionDropMagic			=109,
		actionDropSkill			=110,
		actionCreateBooth		=111,
		actionSuspendBooth		=112,
		actionResumeBooth		=113,
		actionDestroyBooth		=114,
		actionPostCmd			=116,
		actionQueryEquipment	=117,
		actionAbortTransform	=118,
		actionSetCheckKey		=119,
		actionLanding			=120,
		actionGetMoney			=121,
		actionDischarge			=122,
		actionQueryEnemyInfo	=123,
		actionSoundEffect		=124,
		actionOpenDialog		=126,
		actionRequest			=127,
		actionCheckMotion		=128,
		actionAlignPos			=129,
		actionGetStatistic		=130,
		actionDisappear			=131,
		actionLeaveMap			=132,
		actionJump				=133,
		actionEquip				=135,
		actionUnequip			=136,
		actionGhost				=137,
		actionSynchro			=138,
		actionReportSuperState	=139,
		actionQueryFriendInfo	=140,
		actionQueryLeaveWord	=141,
		actionChangeFace		=142,
		actionTableRequest		=230,
		actionTableAct			=231,
		actionTableClear		=232,
		actionTaskKillCount		=1000,
		actionTaskKillCountAdd	=1001,
};

class CMsgAction : public CNetMsg
{
public:
	CMsgAction();
	virtual ~CMsgAction();

	BOOL	Create	(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, DWORD dwData=0);
	BOOL	Create	(OBJID idPlayer, int nPosX, int nPosY, USHORT usDir, USHORT usAction, USHORT usTargetPosX, USHORT usTargetPosY, DWORD dwTimeStamp=0);
	BOOL	Create	(DWORD dwFirst, OBJID idUser, DWORD dwData, USHORT usAction);

public:
	BOOL			Create		(char* pMsgBuf, DWORD dwSize);
	void			Process		(void* pInfo);

protected:
	typedef struct {
		USHORT		unMsgSize;
		USHORT		unMsgType;
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
