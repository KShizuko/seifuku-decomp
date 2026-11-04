#if !defined(AFX_AICENTER_H__D248B8B7_EB60_4EFE_9A6C_A2BD9E621F95__INCLUDED_)
#define AFX_AICENTER_H__D248B8B7_EB60_4EFE_9A6C_A2BD9E621F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "mycom.h"
#include "define.h"
#include "windows.h"
#include "i_mydb.h"
#include "autoptr.h"
#include "I_AiCenter.h"
#include "AiData.h"

enum NPCACTION{
		NPCACTION_TALK,
		NPCACTION_DO_EMOTION,
		NPCACTION_CHANGE_DIR,

		NPCACTION_CLOSE_TARGET,
		NPCACTION_LEAVE_TARGET,

		NPCACTION_CHGMAP_SHOP,
		NPCACTION_FULL_ITEM,
		NPCACTION_USE_ITEM,

		NPCACTION_REBORN,

		NPCACTION_TEAM_ALLY_JOIN,
		NPCACTION_TEAM_ACCEPT_INVITE,
		NPCACTION_TEAM_LEAVE,
		NPCACTION_TEAM_CLOSE_LEADER,

		NPCACTION_ATTACK_TARGET,
};

enum NPCEVENT{
		NPCEVENT_TALK,
		NPCEVENT_LOOK,

		NPCEVENT_LEADER_LEAVE,
		NPCEVENT_INVITE,
		NPCEVENT_FOUND_LEADER,

		NPCEVENT_NEED_CURE,
		NPCEVENT_NEED_BUY_ITEM,
		NPCEVENT_ITEM_EMPTY,

		NPCEVENT_ATTACK_TARGET,
		NPCEVENT_KILL_TARGET,
		NPCEVENT_FOUND_MONSTER,
		NPCEVENT_USER_CURE,
};

class CAiCenter : IAiCenter
{
	NO_COPY(CAiCenter)
protected:
	CAiCenter();
	virtual ~CAiCenter();

public:
	static IAiCenter* CreateNew()							{ return new CAiCenter; }
protected:
	virtual void	Release()								{ delete this; }
	virtual bool	Create(IAiCenterOwner* pOwner, IDatabase* pDb, const Array<String>& setGameFunction, const Array<String>& setGameAction, int nTimeOfLife);
	virtual void	OnTimer();
	virtual void	Persistent(IDatabase* pDb);

	virtual void	AddFact(StringRef strFact, int nPriority);
	virtual bool	FetchAction(int* pidxAction, VarTypeSet* psetParam);
	virtual int		GetTimeOfLife()	const					{ return m_nNow; }
	virtual int		Now()			const					{ return m_nNow; }

protected:
	void	ClearInvalidRule();
	void	Thinking(int nStep);
	bool	ProcessRule(const CRuleObj* pRule);
	bool	ProcessRuleNest(bool bLogicNot, OBJID idFact, VarTypeSetRef setParam, ARGUMENT_SET& setArgument, const CRuleObj* pRule, CFactArray::ConstIterator iterFact);
	bool	CallFunction(OBJID idFact, VarTypeSetRef setParam, ARGUMENT_SET* psetArgument);

public:
	void	ClearAlikeAction(CFactRef cPattern);
	bool	ProcessResult(CFactRef result, CFactRef pattern, int nPriority, ARGUMENT_SET_REF setArgument);
	CSymbolTable*	QuerySymbolTable()				{ return m_pSymbolTable; }
	IAiCenterOwner*	QueryAgent()					{ return m_pOwner; }
protected:
	bool	IsFunction(OBJID idFact)						{ return idFact >= SYSTEM_FUNCTION_ID_BEGIN && idFact < SYSTEM_FUNCTION_ID_END; }
	bool	IsFactFunction(OBJID idFact)					{ return idFact >= GAME_FUNCTION_ID_BEGIN	&& idFact < GAME_FUNCTION_ID_END; }
	bool	IsAction(OBJID idFact)							{ return idFact >= GAME_ACTION_ID_BEGIN	&& idFact < GAME_ACTION_ID_END; }

protected:
	bool InsteadParam(VarTypeSet* pParam, ARGUMENT_SET_REF setArgument);
	CJavaObj<CRuleSet>		m_pRuleSet;
	CJavaObj<CFactSet>		m_pFactSet;
	CFactArray				m_setAction;
	IAiCenterOwner*			m_pOwner;
	int						m_nNow;
	CJavaObj<CSymbolTable>	m_pSymbolTable;

protected:
	bool	m_bDumpRule;
	int		m_nNewPriority;			enum { PRIORITY_NONE = -1 };
};

#endif
