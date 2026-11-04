#ifndef T_COMPACTOBJSET_H
#define T_COMPACTOBJSET_H

#include "common.h"

template<TYPENAME T>
class CompactObjSet
{
public:
	CompactObjSet()	{ m_buf = NULL; }					
	~CompactObjSet() { if(m_buf) delete []m_buf;}
public:
	bool	Create(int nSize)						{ CHECKF(!m_buf); m_nSize = nSize; m_buf = new char[sizeof(T)*m_nSize]; clear(); return m_buf != NULL; }
public:
	int		size()									{ return m_nSize; }
	T*		operator [](int idx)					{ return (T*)(m_buf + sizeof(T)*idx); }
	void	clear()									{ memset(m_buf, 0, sizeof(T)*m_nSize); }
protected:
	char*	m_buf;
	int		m_nSize;
};

#endif
