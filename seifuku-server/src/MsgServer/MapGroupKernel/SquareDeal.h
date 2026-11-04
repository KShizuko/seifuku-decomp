#if !defined(AFX_SQUAREDEAL_H__A5B68023_45DD_4048_8818_AD1778901164__INCLUDED_)
#define AFX_SQUAREDEAL_H__A5B68023_45DD_4048_8818_AD1778901164__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "Myheap.h"

#include <vector>
using namespace std;

class ISquareDeal;
class CUser;
class CItem;
class IUserSquareDeal
{
public:
	virtual bool	Create(ISquareDeal* pSquareDeal)				PURE_VIRTUAL_FUNCTION_0
	virtual ULONG	Release()										PURE_VIRTUAL_FUNCTION_0
	virtual CUser*	QueryUser()										PURE_VIRTUAL_FUNCTION_0

public:
	virtual bool	AddItem(CItem* pItem, int nAmount, int nAllWeight)			PURE_VIRTUAL_FUNCTION_0
	virtual bool	AddMoney(int nAllMoney)							PURE_VIRTUAL_FUNCTION_0
	virtual bool	AddEmoney(int nAllEmoney)						PURE_VIRTUAL_FUNCTION_0
	virtual void	ClickOK()										PURE_VIRTUAL_FUNCTION
	virtual OBJID	GetAccountID()									PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetEmoney()										PURE_VIRTUAL_FUNCTION_0
	virtual DWORD	GetMoney()										PURE_VIRTUAL_FUNCTION_0
	virtual LPCTSTR	GetName()										PURE_VIRTUAL_FUNCTION_0
};

class ISquareDeal
{
public:
	virtual bool	Create(IUserSquareDeal* pUser, IUserSquareDeal* pTarget)	PURE_VIRTUAL_FUNCTION_0
	virtual ULONG	Release()										PURE_VIRTUAL_FUNCTION_0

public:
	virtual bool	AddItem(CItem* pItem)							PURE_VIRTUAL_FUNCTION_0
	virtual int 	AddMoney(int nMoney)							PURE_VIRTUAL_FUNCTION_0
	virtual int 	AddEmoney(int nEmoney)							PURE_VIRTUAL_FUNCTION_0
	virtual bool	ClickOK()										PURE_VIRTUAL_FUNCTION_0
	virtual bool	ClickUnfairOK()									PURE_VIRTUAL_FUNCTION_0
	virtual CUser*	GetTarget()										PURE_VIRTUAL_FUNCTION_0
};

class CItem;
class CSquareDeal
{
protected:
	CSquareDeal();
	virtual ~CSquareDeal();
public:
	static ISquareDeal* CreateNew()		{ CSquareDeal* ptr = new CSquareDeal; if(ptr) return &ptr->m_iface1; else return NULL; }

protected:
	bool	TradeOK();

	class XSquareDeal1 : public ISquareDeal
	{
	protected:
		CSquareDeal*	This()							{ return m_pParent; }
		virtual bool	Create(IUserSquareDeal* pUser, IUserSquareDeal* pTarget);
		virtual ULONG	Release		()					{ delete m_pParent; return 0; }
	protected:
		virtual bool	AddItem		(CItem* pItem);
		virtual int		AddMoney	(int nMoney);
		virtual int		AddEmoney	(int nEmoney);
		virtual bool	ClickOK		();
		virtual bool	ClickUnfairOK	();
		virtual CUser*	GetTarget	();

	private:
		CSquareDeal*	m_pParent;
		friend class CSquareDeal;
	}			m_iface1;

	class XSquareDeal2 : public ISquareDeal
	{
	protected:
		CSquareDeal*	This()							{ return m_pParent; }
		virtual bool	Create(IUserSquareDeal* pUser, IUserSquareDeal* pTarget);
		virtual ULONG	Release		()					{ delete m_pParent; return 0; }
	protected:
		virtual bool	AddItem		(CItem* pItem);
		virtual int		AddMoney	(int nMoney);
		virtual int		AddEmoney	(int nEmoney);
		virtual bool	ClickOK		();
		virtual bool	ClickUnfairOK	();
		virtual CUser*	GetTarget	();

	private:
		CSquareDeal*	m_pParent;
		friend class CSquareDeal;
	}			m_iface2;

protected:
	IUserSquareDeal*	m_pUser1;
	IUserSquareDeal*	m_pUser2;
	typedef vector<OBJID>	ITEM_SET;
	ITEM_SET		m_setItem1;
	ITEM_SET		m_setItem2;
	int				m_nAllWeight1;
	int				m_nAllWeight2;
	int				m_nMoney1;
	int				m_nMoney2;
	int				m_nEmoney1;
	int				m_nEmoney2;
	bool			m_bClickOK1;
	bool			m_bClickOK2;
	bool			m_bUnfairOK1;
	bool			m_bUnfairOK2;
	bool			m_bTradeSuccess;
	friend class CSquareDeal::XSquareDeal1;
	friend class CSquareDeal::XSquareDeal2;

	MYHEAP_DECLARATION(s_heap)
};

#endif
