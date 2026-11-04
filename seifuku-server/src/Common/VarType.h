#if !defined(AFX_VARTYPE_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_)
#define AFX_VARTYPE_H__D9E42F53_A4BD_4BB9_9745_D3FE8666BA41__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif
#include "basetype.h"
#pragma warning(disable:4786)
#include <string>
using namespace std;
class VarType
{
public:
	enum TYPE { TYPE_NONE, TYPE_INT, TYPE_STRING, TYPE_SYMBOL, TYPE_OBJID };
public:
	VarType() : m_ptr(NULL),m_int(0),m_nType(TYPE_NONE)		{}
	VarType(const VarType& obj)	: m_int(obj.m_int),m_nType(obj.m_nType)					{ if(obj.m_ptr) m_ptr=new String(*obj.m_ptr); else m_ptr=NULL; }
	VarType(const char* str,TYPE type=TYPE_STRING)	: m_int(0),m_nType(type)			{ MYASSERT(type == TYPE_STRING || type == TYPE_SYMBOL); MYASSERT(str); m_ptr=new String(str); }
	VarType(int data,TYPE type=TYPE_INT)			: m_ptr(NULL),m_int(data),m_nType(type)		{ MYASSERT(type==TYPE_INT || type==TYPE_OBJID); }
	~VarType()												{ delete m_ptr; }
	VarType& operator=(const VarType& obj);
public:
	bool	operator==(const VarType& obj) const;
	bool	operator!=(const VarType& obj) const			{ return !(operator==(obj)); }
	operator const String&()	const						{ MYASSERT(m_ptr); return *m_ptr; }
	operator int()		const								{ MYASSERT(!m_ptr); return m_int; }
	void	Clear()											{ delete m_ptr; m_nType = TYPE_NONE; }

public:
	bool	IsStringType() const							{ return m_nType == TYPE_STRING; }
	bool	IsIntType() const								{ return m_nType == TYPE_INT; }
	bool	IsSymbolType() const							{ return m_nType == TYPE_SYMBOL; }
	bool	IsOBJIDType() const								{ return m_nType == TYPE_OBJID; }
	bool	IsValid() const									{ return m_nType != TYPE_NONE; }

public:
	String	Dump()	const									{ if(!m_ptr) return String().FormatLong(m_int); return *m_ptr; }

protected:
	String*			m_ptr;
	int				m_int;
	TYPE			m_nType;
};
typedef	const VarType&	VarTypeRef;
template<>
struct dump_traits<VarType> { static String Dump(const VarType& obj)		{ return obj.Dump(); } };

#include "Array.h"
typedef Array<VarType>	VarTypeSet;
typedef	const VarTypeSet&	VarTypeSetRef;
template<>
struct dump_traits<VarTypeSet> { static String Dump(const VarTypeSet& obj)		{ return obj.Dump(); } };

#endif
