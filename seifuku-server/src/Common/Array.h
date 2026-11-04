#if !defined(AFX_SAFEARRAY_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_)
#define AFX_SAFEARRAY_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif
#include "basetype.h"
#include "mycom.h"
#pragma warning(disable:4786)
#include <vector>
using namespace std;

template <TYPENAME T>
class Array
{
public:
	class	ConstIterator{
		enum { VALID_MASK=0x80000000, SIZE_MASK=0x7FFFFFFF };
	protected:
		ConstIterator(const vector<T>* pSet) : m_pSet(pSet),m_nValid_Size(pSet->size()),m_idx(0) {}
		friend class Array<T>;

	public:
		bool	Next()								{ if(m_idx>=Size()) return false; if(!IsValid()) m_nValid_Size+=VALID_MASK; else m_idx++; return m_idx<Size(); }
		void	Reset()								{ m_idx=0; m_nValid_Size=pSet->size(); }
		operator const T*()							{ MYASSERT(IsValid()); return &m_pSet->at(m_idx); }
		const T * operator->()						{ MYASSERT(IsValid()); return &m_pSet->operator[](m_idx); }
	protected:
		bool	IsValid()							{ return (m_nValid_Size&VALID_MASK) && m_idx<Size(); }
		int		Size()								{ return m_nValid_Size & SIZE_MASK; }
		void	DelObj()							{ MYASSERT(Size()); m_nValid_Size = Size()-1; }

	protected:
		const vector<T>*	m_pSet;
		unsigned long		m_nValid_Size;
		int					m_idx;
	};
	class	Iterator{
		enum { VALID_MASK=0x80000000, SIZE_MASK=0x7FFFFFFF };
	protected:
		Iterator(vector<T>* pSet) : m_pSet(pSet),m_nValid_Size(pSet->size()),m_idx(0) {}
		friend class Array<T>;

	public:
		bool	Next()								{ if(m_idx>=Size()) return false; if(!IsValid()) m_nValid_Size+=VALID_MASK; else m_idx++; return m_idx<Size(); }
		void	Reset()								{ m_idx=0; m_nValid_Size=m_pSet->size(); }
		operator T*()								{ MYASSERT(IsValid()); return &m_pSet->at(m_idx); }
		T * operator->()							{ MYASSERT(IsValid()); return &m_pSet->operator[](m_idx); }
	protected:
		bool	IsValid()							{ return (m_nValid_Size&0x80000000) && m_idx<(int)m_pSet->size(); }
		int		Size()								{ return m_nValid_Size & 0x7FFFFFFF; }
		void	DelObj()							{ MYASSERT(Size()); m_nValid_Size = Size()-1; }
	protected:
		vector<T>*			m_pSet;
		unsigned long		m_nValid_Size;
		int					m_idx;
	};
public:
	const ConstIterator	NewEnum	() 	const			{ return ConstIterator(&m_set); }
	Iterator	NewEnum	() 							{ return Iterator(&m_set); }
	int			Size	() 	const					{ return m_set.size(); }

public:
	explicit Array(int nSize=0)								{ m_set.resize(nSize); }
	Array(const Array& obj)							{ m_set = obj.m_set; }
	Array& operator=(const Array& obj)				{ if(this==&obj) return *this; m_set = obj.m_set; return *this; }
public:
	T&		operator[](int idx)						{ MYASSERT(idx < (int)m_set.size()); return m_set[idx]; }
	T&		At(int idx)								{ MYASSERT(idx < (int)m_set.size()); return m_set[idx]; }
	const T& operator[](int idx) const				{ MYASSERT(idx < (int)m_set.size()); return m_set[idx]; }
	const T& At(int idx) const						{ MYASSERT(idx < (int)m_set.size()); return m_set[idx]; }
	void	Clear()									{ m_set.clear(); }
	void	Resize(int nSize)						{ m_set.resize(nSize); }
	void	Push(const T& obj)						{ m_set.push_back(obj); }
	bool	DelObj(Iterator& iter)					{ if(!iter.IsValid()) return false; m_set.erase(m_set.begin()+iter.m_idx); iter.DelObj(); return true; }
	bool	DelObj(int idx)							{ MYASSERT(idx<m_set.size()); m_set.erase(m_set.begin()+idx); return true; }

public:
	String	Dump()	const							{ String str="{ "; for(int i=0; i<m_set.size(); i++){ if(i) str+=", "; str+=::DumpString(m_set[i]); } return str+=" }"; }

protected:
	vector<T>		m_set;
};

#endif
