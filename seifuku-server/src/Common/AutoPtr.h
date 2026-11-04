#if !defined(AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_)
#define AFX_GAMEAUTOPTR_H__5708545C_E8F2_4155_AC66_4191BE0F150E__INCLUDED_
#include "common.h"
#include "basetype.h"

template<TYPENAME T>
class CAutoPtr
{
public:
	CAutoPtr() : m_ptr(NULL)					{}
	CAutoPtr(T* ptr) : m_ptr(ptr)				{}
	~CAutoPtr()									{ if(m_ptr) m_ptr->Release(); }
	CAutoPtr& operator=(T* ptr)					{ MYASSERT(!m_ptr && ptr); m_ptr=ptr; return *this; }
	T*	pop()									{ T* p=m_ptr; m_ptr=NULL; return p; }
private:
	CAutoPtr& operator=(const CAutoPtr& ptr);
	CAutoPtr(const CAutoPtr&);
public:
	operator T*()									{ return m_ptr; }
	POINT_NO_RELEASE<T>*	operator->()								{ MYASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }
	operator const T*()	 	 const					{ return m_ptr; }
	const POINT_NO_RELEASE<T>*	operator->() const					{ MYASSERT(m_ptr); return static_cast<POINT_NO_RELEASE<T>* >(m_ptr); }

protected:
	T*			m_ptr;
};

template<TYPENAME T>
class CJavaObj
{
public:
	CJavaObj() : m_ptr(NULL)					{}
	~CJavaObj()									{ if(m_ptr) m_ptr->Release(); }
private:
	CJavaObj& operator=(const CJavaObj& ptr);
	CJavaObj(const CJavaObj&);

public:
	operator T*()									{ if(!m_ptr) m_ptr=T::CreateNew(); MYASSERT(m_ptr); return m_ptr; }
	T*	operator->()								{ if(!m_ptr) m_ptr=T::CreateNew(); MYASSERT(m_ptr); return m_ptr; }

protected:
	T*			m_ptr;
};

#endif
