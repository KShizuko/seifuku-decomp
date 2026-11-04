#if !defined(AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_)
#define AFX_MSGDIR_H__950C0770_DB2D_4D79_970C_E2863ECE9614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetMsg.h"

enum {	actionEnterMap			=74,
		actionGetItemSet		= 75,
		actionGetGoodFriend		=76,
		actionGetWeaponSkillSet	=77,
		actionGetMagicSet		=78,
		actionChgDir			=79,
		actionPosition			= 80,
		actionEmotion			=81,
		actionBroadcastPos		=82,
		actionDivorce			=83,
		actionSelfUnfreeze		=84,
		actionChgMap			= 85,
		actionFlyMap			=86,
		actionChgWeather		=87,
		actionFireworks			=88,
		actionDie				=89,
		actionQuitSyn			= 90,
		actionWalk				=91,
		actionUplev				=92,
		actionXpCLear			=93,
		actionReborn			=94,
		actionDelRole			= 95,
		actionSetPkMode			=96,
		actionGetSynAttr		=97,
		actionRun				=98,
		actionMine				=99,
		actionVerifyCode		= 100,
		actionTeamMemeberPos	=101,
		actionQueryPlayer		=102,
		actionAbordMagic		=103,
		actionMapARGB			=104,
		actionMapStatus			= 105,
		actionQueryTeamMember	=106,
		actionMoveStop			=107,
		actionKickBack			=108,
		actionDropMagic			=109,
		actionDropSkill			= 110,
		actionCreateBooth		=111,
		actionSuspendBooth		=112,
		actionResumeBooth		=113,
		actionDestroyBooth		=114,
		actionQueryCryOut		= 115,
		actionPostCmd			=116,
		actionQueryEquipment	=117,
		actionAbortTransform	=118,
		actionCombineSubSyn		=119,
		actionTakeOff			= 120,
		actionGetMoney			=121,
		actionCancelKeepBow		=122,
		actionQueryEnemyInfo	=123,
		actionSoundEffect		=124,
		actionResetMagic		= 125,
		actionOpenDialog		=126,
		actionCheatFound		=127,
		actionDetectMsg			=128,
		actionPetJump			=129,
		actionLoginComplete		= 130,
		actionMonsterBorn		=131,
		actionLeaveMap			=132,
		actionJump				=133,
		actionUnk134			=134,
		actionGhost				= 137,
		actionSynchro			=138,
		actionCheckAccount		=139,
		actionQueryFriendInfo	= 140,
		actionQueryLeaveWord	=141,
		actionChangeFace		=142,
		actionQueryTradeBuddyInfo	=143,
		actionUnk160			=160,
		actionUnk171			=171,
		actionUnk174			=174,

		actionSuperChgMap		= 200,
		actionFullItem			= 201,
		actionChangeMapDoc		= 202,
		actionAddTerrainObj		= 203,
		actionDelTerrainObj		= 204,

		actionUnk230			= 230,
		actionUnk231			= 231,
		actionSetWatchUser		= 305,
		actionClearWatchUser	= 306,
		actionUnk307			= 307,
		actionQueryPlayerInfo	= 310,
};

enum { PKMODE_FREE=0, PKMODE_SAFE, PKMODE_TEAM, PKMODE_ARRESTMENT, PKMODE_SYNDICATE };
const int _ACTION_DANCE1		=001;
const int _ACTION_STANDBY		=100;
const int _ACTION_LAUGH			=150;
const int _ACTION_GUFFAW		=151;
const int _ACTION_FURY          =160;
const int _ACTION_SAD	        =170;
const int _ACTION_EXCITEMENT	=180;
const int _ACTION_SAYHELLO		=190;
const int _ACTION_SALUTE		=200;
const int _ACTION_GENUFLECT     =210;
const int _ACTION_KNEEL			=220;
const int _ACTION_COOL			=230;
const int _ACTION_SWIM			=240;
const int _ACTION_SITDOWN       =250;
const int _ACTION_ZAZEN			=260;
const int _ACTION_FAINT	        =270;
const int _ACTION_LIE	        =271;

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
