#if !defined(AFX_SYNMANAGER_H__517F6741_C6D8_44F0_939C_659592647C56__INCLUDED_)
#define AFX_SYNMANAGER_H__517F6741_C6D8_44F0_939C_659592647C56__INCLUDED_

#include "define.h"
#include "GameObjSet.h"
#include <vector>
using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif

const int	MAX_SYNENEMYSIZE		= 5;
const int	MAX_SYNALLYSIZE			= 5;
enum SYNDATA{
		SYNDATA_ID=0,

		SYNDATA_NAME=1,
		SYNDATA_TENET,
		SYNDATA_LEADERTITLE,
		SYNDATA_MEMBERTITLE,
		SYNDATA_LEADERID,
		SYNDATA_LEADERNAME,
		SYNDATA_MONEY,
		SYNDATA_FEALTY,
		SYNDATA_DEL_FLAG,
		SYNDATA_AMOUNT,
		SYNDATA_ENEMY0,
		SYNDATA_ENEMY1,
		SYNDATA_ENEMY2,
		SYNDATA_ENEMY3,
		SYNDATA_ENEMY4,
		SYNDATA_ALLY0,
		SYNDATA_ALLY1,
		SYNDATA_ALLY2,
		SYNDATA_ALLY3,
		SYNDATA_ALLY4,
};

class IRecordset;
class CSyndicate
{
protected:
	CSyndicate()								{}
	~CSyndicate()								{}
public:
	static CSyndicate* CreateNew()				{ return new CSyndicate; }
	void		ReleaseByOwner()				{ delete this; }
	bool		Create(IRecordset* pRes);
	bool		Create(OBJID idSyn, OBJID idFealty)	{ m_id = idSyn; m_idFealty = idFealty; return true; }
public:
	typedef	vector<OBJID>	OBJ_SET;
	OBJID		GetID()							{ return m_id; }
	OBJ_SET*	QueryAllySet()					{ return &m_setAlly; }
	OBJ_SET*	QueryEnemySet()					{ return &m_setEnemy; }
protected:
	OBJID		GetMasterSynID()				{ if(m_idFealty != ID_NONE) return m_idFealty; return GetID(); }

public:
	bool		IsFriendly(OBJID idSyn);
	bool		IsHostile(OBJID idSyn);
	bool		IsNeutral(OBJID idSyn);

protected:
	OBJID		m_id;
	OBJID		m_idFealty;

	OBJ_SET		m_setAlly;
	OBJ_SET		m_setEnemy;
};

typedef		IGameObjSet<CSyndicate>		ISynSet;
typedef		CGameObjSet<CSyndicate>		CSynSet;

class CSynManager  
{
public:
	CSynManager();
	virtual ~CSynManager();

	bool	Create();

public:
	ISynSet*	QuerySynSet()						{ CHECKF(m_setSyn); return m_setSyn; }
	CSyndicate*	QuerySyndicate(OBJID id)			{ return QuerySynSet()->GetObj(id); }

protected:
	ISynSet*	m_setSyn;
};

#endif
