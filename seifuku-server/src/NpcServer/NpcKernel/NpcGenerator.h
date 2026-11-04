#if !defined(AFX_NPCGENERATOR_H__66F1D8C3_7833_4C77_AD36_F07544F0D2DE__INCLUDED_)
#define AFX_NPCGENERATOR_H__66F1D8C3_7833_4C77_AD36_F07544F0D2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "GameObj.h"
#include "GameData.h"
#include "Timeout.h"

class IGenerator
{
protected:
	virtual ~IGenerator() {}
public:
	virtual	OBJID	GetID()											PURE_VIRTUAL_FUNCTION_0
	virtual	void	Release()										PURE_VIRTUAL_FUNCTION
	virtual POINT	GetCenter()										PURE_VIRTUAL_FUNCTION_(POINT)
	virtual bool	IsTooFar(OBJID idMap, int x, int y, int nRange)	PURE_VIRTUAL_FUNCTION_0
	virtual bool	IsInRegion(int x, int y)						PURE_VIRTUAL_FUNCTION_0
	virtual int		GetWidth()										PURE_VIRTUAL_FUNCTION_0
	virtual int		GetHeight()										PURE_VIRTUAL_FUNCTION_0
	virtual int		GetPosX()										PURE_VIRTUAL_FUNCTION_0
	virtual int		GetPosY()										PURE_VIRTUAL_FUNCTION_0
	virtual void	GeneratorOK(OBJID idNpc)						PURE_VIRTUAL_FUNCTION
	virtual void	DeleteNpc(OBJID idNpc)							PURE_VIRTUAL_FUNCTION
};

enum NPCGENDATA
{
		NPCGENDATA_MAPID=1,
		NPCGENDATA_BOUNDX, 
		NPCGENDATA_BOUNDY, 
		NPCGENDATA_BOUNDCX, 
		NPCGENDATA_BOUNDCY, 

		NPCGENDATA_GRID,
		NPCGENDATA_RESTSECS,
		NPCGENDATA_MAXPERGEN,

		NPCGENDATA_NPCTYPE, 

		NPCGENDATA_BEGIN,
		NPCGENDATA_END,
		NPCGENDATA_BORNX,
		NPCGENDATA_BORNY,
};

extern char	szGenerator[];
typedef CGameData<NPCGENDATA,szGenerator,szID>	CNpcGenData;

class CNpcGenerator : public CGameObj, IGenerator
{
public:
	CNpcGenerator();
	virtual ~CNpcGenerator();
	static CNpcGenerator*	CreateNew()	{ return new CNpcGenerator; }
	virtual ULONG	ReleaseByOwner()	{ delete this; return 0; }
	IGenerator*		QueryInterface()	{ return (IGenerator*)this; }

	virtual OBJID	GetID()						{ return m_pData->GetKey(); }
protected:
	virtual void	Release()					{}
	virtual POINT	GetCenter()					{ POINT pos; pos.x = GetInt(NPCGENDATA_BOUNDX) + GetInt(NPCGENDATA_BOUNDCX)/2;
													 pos.y = GetInt(NPCGENDATA_BOUNDY) + GetInt(NPCGENDATA_BOUNDCY)/2;
													 return pos; }
	virtual bool	IsTooFar(OBJID idMap, int x, int y, int nRange);
	virtual bool	IsInRegion(int x, int y)	{ return x >= GetInt(NPCGENDATA_BOUNDX) && x < GetInt(NPCGENDATA_BOUNDX) + GetInt(NPCGENDATA_BOUNDCX)
													&& y >= GetInt(NPCGENDATA_BOUNDY) && y < GetInt(NPCGENDATA_BOUNDY) + GetInt(NPCGENDATA_BOUNDCY);
												}
	virtual int		GetWidth()					{ return m_pData->GetInt(NPCGENDATA_BOUNDCX); }
	virtual int		GetHeight()					{ return m_pData->GetInt(NPCGENDATA_BOUNDCY); }
	virtual int		GetPosX()					{ return m_pData->GetInt(NPCGENDATA_BOUNDX); }
	virtual int		GetPosY()					{ return m_pData->GetInt(NPCGENDATA_BOUNDY); }
	virtual void	GeneratorOK(OBJID idNpc);
	virtual void	DeleteNpc(OBJID idNpc);

public:
	virtual bool	Create(IRecordset* pRes);

	int		Generator(int nNum);

public:
	int		GetInt(NPCGENDATA idx)		{ return m_pData->GetInt(idx); }
	LPCTSTR	GetStr(NPCGENDATA idx)		{ return m_pData->GetStr(idx); }
	int		GetAmount()					{ return m_nAmount; }
	int		GetGenAmount()				{ return m_nGenAmount; }
	int		GetMaxNpc()					{ return m_nMaxNpc; }

protected:
	POINT	FindGenPos();

protected:
	int			m_nTotalGen;
	int			m_nAmount;
	int			m_nGenAmount;
	int			m_nCurrGen;
	CTimeOut	m_timeoutRest;
	int			m_nMaxNpc;
	time_t		m_tBadGen;
	CGameMap*	m_pMap;

protected:
	CNpcGenData*		m_pData;
};

#endif
