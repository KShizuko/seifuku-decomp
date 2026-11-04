#ifndef	AGENT_DATA_H
#define	AGENT_DATA_H

#include "common.h"
#include "BaseFunc.h"

const int		TICKS_PER_SEC			= 2;

inline const char* const * GetActionStringSet()
{
	static char* setAction[]={
			"ChangeMode",
			"Reborn",
			"TalkToName",
			"TalkToID",

			"MoveCloseTarget",
			"JumpClose",
			"JumpToSafePos",
			"ChangeMapTo",
			"RandomWalk",
			"KeepOutEnemy",

			"AttackRole",

			"UseItem",
			"FullMedicine",

			"AcceptTeamInvite",
			"SendTeamApply",
			"LeaveTeam",

			NULL
	};

	return setAction;
}

enum {
		ACTION_CHANGE_MODE,
		ACTION_REBORN,
		ACTION_TALK_TO_NAME,
		ACTION_TALK_TO_ID,
		ACTION_MOVE_CLOSE,
		ACTION_JUMP_CLOSE,
		ACTION_JUMP_TO_SAFE,
		ACTION_CHGMAP,
		ACTION_RANDOM_WALK,
		ACTION_LEAVE_EVIL,
		ACTION_ATTACK,
		ACTION_USE_ITEM,
		ACTION_FULL_MEDICINE,
		ACTION_ACCEPT_INVITE,
		ACTION_SEND_APPLY,
		ACTION_LEAVE_TEAM,
};

inline const char* const * GetFactFuncStringSet()
{
	static char*	setFactFunc[]={
			"IsAlive",
			"IsWing",
			"IsUser",
			"IsMonster",
			"EvilValue",
			"CoolValue",
			"Level",
			"MyLife",

			"RolePos",
			"MyDistance",

			"AttackDistance",
			"DangerValue",

			"ItemCount",

			"MyLeader",
			"IsLeader",

			NULL
	};

	return setFactFunc;
}

enum{
		FACTFUNC_IS_ALIVE,
		FACTFUNC_IS_WING,
		FACTFUNC_IS_USER,
		FACTFUNC_IS_MONSTER,
		FACTFUNC_EVIL_VALUE,
		FACTFUNC_COOL_VALUE,
		FACTFUNC_LEVEL,
		FACTFUNC_MY_LIFE,

		FACTFUNC_ROLE_POS,
		FACTFUNC_MYDISTANCE,

		FACTFUNC_ATTACK_DISTNACE,
		FACTFUNC_DANGER_VALUE,

		FACTFUNC_ITEM_COUNT,

		FACTFUNC_MY_LEADER,
		FACTFUNC_IS_LEADER,
};

#define FACT_LOOK_TARGET		"LookTarget"
#define FACT_TALK_TO_ME			"TalkToMe"
#define FACT_TEAM_INVITE		"TeamInvite"

inline int	 Priority2Durable(int nPriority)
{
	static int	setPriority2Durable[]={ INT_MAX/TICKS_PER_SEC,

		1200, 1200, 1200, 1200, 1200,  1200, 1200, 1200, 1200, 1200,
		300, 300, 300, 300, 300,  300, 300, 300, 300, 300,

		8, 8, 8, 8, 8,  8, 8, 8, 8, 8,
		8, 8, 8, 8, 8,  8, 8, 8, 8, 8,
		8, 8, 8, 8, 8,  8, 8, 8, 8, 8,

		3, 3, 3, 3, 3,  3, 3, 3, 3, 3,
		3, 3, 3, 3, 3,  3, 3, 3, 3, 3,
		3, 3, 3, 3, 3,  3, 3, 3, 3, 3,

		1, 1, 1, 1, 1,  1, 1, 1, 1, 1,
		0, 0, 0, 0, 0,  0, 0, 0, 0, 0,
	};

	return setPriority2Durable[nPriority];
}

#endif
