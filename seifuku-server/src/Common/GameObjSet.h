#if !defined(AFX_GAMEOBJSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_GAMEOBJSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif
#include <vector>
using namespace std;

template<class T>
class IGameObjSet
{
protected:
	virtual ~IGameObjSet() {}
public:
	virtual void		Release()				PURE_VIRTUAL_FUNCTION
	virtual int			GetAmount()	const			PURE_VIRTUAL_FUNCTION_0
	virtual T*			GetObjByIndex(int idx)	PURE_VIRTUAL_FUNCTION_0
	virtual bool		AddObj(T* pObj)			PURE_VIRTUAL_FUNCTION_0
	virtual bool		DelObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0
	virtual T*			GetObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0
	virtual T*			PopObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0
};

template<class T>
class CGameObjSet : public IGameObjSet<T>
{
protected:
	typedef	vector<T*>	VECTOR_SET;
protected:
	CGameObjSet(bool bOwner) { m_bOwner = bOwner; }
	virtual ~CGameObjSet();
public:
	static CGameObjSet<T>*	CreateNew(bool bOwner=true) { return new CGameObjSet<T>(bOwner); }
	void		Release			()				{ delete this; }

public:
	class	Iterator{
	private:
		Iterator(VECTOR_SET* pSet) : m_pSet(pSet),m_idx(pSet->size()-1),m_pObj(0) {}
		friend class CGameObjSet<T>;
	public:

	public:
		bool	Next()		{ if(m_idx<0) return m_pObj=NULL,false; m_pObj=m_pSet->at(m_idx); MYASSERT(m_pObj); m_idx--; return true; }
		void	Reset()		{ m_idx=m_pSet->size()-1; m_pObj=NULL; }
		operator T*()							{ return m_pObj; }
		T * operator->()						{ return m_pObj; }

	protected:
		const VECTOR_SET*	m_pSet;
		int			m_idx;
		T*			m_pObj;
	};
public:
	Iterator	NewEnum		() 						{ return Iterator(&m_setObj); }
	int			Size		() const				{ return m_setObj.size(); }

public:
	int			GetAmount		() const		{ return m_setObj.size(); }
	T*			GetObjByIndex	(int idx)		{ CHECKF(0 <= idx && idx < (int)m_setObj.size()); return m_setObj[idx]; }
public:
	bool		AddObj			(T* pObj);
	bool		DelObj			(OBJID idObj);
	bool		DelObj			(Iterator& iter);
	T*			GetObj			(OBJID idObj);
	T*			PopObj			(OBJID idObj);
	T*			PopObj			(Iterator& iter);
protected:
	void		Clear			();
protected:
	VECTOR_SET			m_setObj;
	bool				m_bOwner;
};

template<class T>
CGameObjSet<T>::~CGameObjSet()
{
	Clear();
}

template<class T>
void CGameObjSet<T>::Clear()
{
	if(m_bOwner)
	{
		for(int i=0;i<(int)m_setObj.size();i++)
		{ 
			T* pObj=m_setObj[i];
			if(pObj)
				pObj->ReleaseByOwner();
		}
	}
	m_setObj.clear();
}

template<class T>
bool CGameObjSet<T>::AddObj(T* pObj)
{
	CHECKF(pObj && pObj->GetID());
	MYASSERT(!DelObj(pObj->GetID()));
	m_setObj.push_back(pObj);
	return true;
}
template<class T>
bool CGameObjSet<T>::DelObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		if(m_bOwner)
			iter.m_pObj->ReleaseByOwner();
		iter.m_pObj	= NULL;
		int	idx = iter.m_idx+1;
		IF_OK(idx < (int)m_setObj.size())
			m_setObj.erase(m_setObj.begin() + idx);
		return true;
	}
	return false;
}

template<class T>
bool CGameObjSet<T>::DelObj(OBJID idObj)
{
	CHECKF(idObj);
	for(int i=0;i<(int)m_setObj.size();i++)
	{ 
		T* pObj=m_setObj[i];
		if(pObj && pObj->GetID()==idObj)
		{
			if(m_bOwner)
				pObj->ReleaseByOwner();
			m_setObj.erase(m_setObj.begin()+i);
			return true;
		}
	}
	return false;
}
template<class T>
T* CGameObjSet<T>::GetObj(OBJID idObj)
{
	for(int i=0;i<(int)m_setObj.size();i++)
	{
		T* pObj=m_setObj[i];
		if(pObj && pObj->GetID()==idObj)
		{
			return pObj;
		}
	}
	return NULL;
}
template<class T>
T* CGameObjSet<T>::PopObj(OBJID idObj)
{
	MYASSERT(m_bOwner);

	for(int i=0;i<(int)m_setObj.size();i++)
	{
		T* pObj=m_setObj[i];
		if(pObj && pObj->GetID()==idObj)
		{
			if(m_bOwner)
				m_setObj.erase(m_setObj.begin()+i);
			return pObj;
		}
	}
	return NULL;
}

template<class T>
T* CGameObjSet<T>::PopObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj	= NULL;

		int	idx = iter.m_idx+1;
		IF_OK(idx < m_setObj.size())
		{
			m_setObj.erase(m_setObj.begin() + idx);
			return pObj;
		}
	}
	return NULL;
}

#endif
