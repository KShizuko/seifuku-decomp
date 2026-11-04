#if !defined(AFX_GAMEOBJSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_2)
#define AFX_GAMEOBJSET_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_2
#include "I_mydb.h"
#include "Myheap.h"

#include <vector>
using namespace std;

template<class T>
class ISingleObjSet2
{
protected:
	virtual ~ISingleObjSet2() {}
public:
	virtual bool		Create(LPCTSTR szSQL, IDatabase* pDb)	PURE_VIRTUAL_FUNCTION_0
	virtual ULONG		Release()				PURE_VIRTUAL_FUNCTION_0
	virtual int			GetAmount()				PURE_VIRTUAL_FUNCTION_0
	virtual T*			GetObjByIndex(int idx)	PURE_VIRTUAL_FUNCTION_0
	virtual bool		AddObj(T* pObj)			PURE_VIRTUAL_FUNCTION_0
	virtual bool		DelObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0
	virtual T*			GetObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0
	virtual T*			PopObj(OBJID idObj)		PURE_VIRTUAL_FUNCTION_0

};

template<class T>
class CSingleObjSet2 : ISingleObjSet2<T>
{
protected:
	CSingleObjSet2(bool bOwner) { m_bOwner = bOwner; }
	virtual ~CSingleObjSet2();
public:
	static ISingleObjSet2<T>*	CreateNew(bool bOwner) { return new CSingleObjSet2<T>(bOwner); }

public:
	bool		Create(LPCTSTR szSQL, IDatabase* pDb);
	ULONG		Release() { delete this; return 0; }
	int			GetAmount() { return m_setObj.size(); }
	T*			GetObjByIndex(int idx) { CHECKF(0 <= idx && idx < (int)m_setObj.size()); return m_setObj[idx]; }
	bool		AddObj(T* pObj);
	bool		DelObj(OBJID idObj);
	T*			GetObj(OBJID idObj);
	T*			PopObj(OBJID idObj);

protected:
	typedef	vector<T*>	GAMEOBJ_SET;
	GAMEOBJ_SET			m_setObj;
	bool				m_bOwner;
};

template<class T>
CSingleObjSet2<T>::~CSingleObjSet2()
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
}
template<class T>
bool CSingleObjSet2<T>::Create(LPCTSTR szSQL, IDatabase* pDb)
{
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
	if(!pRes)
		return false;

	for(int i = 0; i < (int)pRes->RecordCount(); i++)
	{
		T* pData = T::CreateNew();
		if(!pData)
		{
			pRes->Release();
			return false;
		}

		if(!pData->Create(pRes))
		{
			pData->ReleaseByOwner();
			pRes->Release();
			return false;
		}

		this->AddObj(pData);

		pRes->MoveNext();
	}

	pRes->Release();
	return true;
}

template<class T>
bool CSingleObjSet2<T>::AddObj(T* pObj)
{
	CHECKF(pObj && pObj->GetID());
	MYASSERT(!DelObj(pObj->GetID()));

	m_setObj.push_back(pObj);
	return true;
}

template<class T>
bool CSingleObjSet2<T>::DelObj(OBJID idObj)
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
T* CSingleObjSet2<T>::GetObj(OBJID idObj)
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
T* CSingleObjSet2<T>::PopObj(OBJID idObj)
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

#endif
