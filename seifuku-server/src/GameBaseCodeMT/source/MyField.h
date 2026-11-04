#ifndef _MYFIELD_H
#define _MYFIELD_H
class CCriticalSection;
class CMyRecordset;

class CMyField
{
public:
	CMyField();
	virtual ~CMyField();

public:
	MYHEAP_DECLARATION(s_heap)

public:
#ifdef	MULTITHREAD_SAFE
	static CCriticalSection	m_xCtrl;
#endif

protected:
	unsigned long			m_ulType;
	union
	{
		unsigned char		m_ucVal;
		char				m_cVal;
		short int			m_nVal;
		int					m_iVal;
		long				m_lVal;
		unsigned long		m_ulVal;

		float				m_fVal;
		double				m_dVal;
		__int64				m_i64Val;

	};
	char*				m_szVal;
	int					m_nIndex;
	MYSQL_FIELD			m_Field;
	CMyField *			m_pMyField;

	friend class	CMyRecordset;
	friend class	CMyRecord;
	friend class	CMyConstRecord;

public:
	operator int	()
	{	if ( m_ulType == FIELD_TYPE_SHORT
				|| m_ulType == FIELD_TYPE_TINY
				|| m_ulType == FIELD_TYPE_LONG )
			return m_iVal;
		else return -1;
	};

	operator long()
	{
		if (m_ulType==FIELD_TYPE_SHORT 
				|| m_ulType == FIELD_TYPE_TINY
				|| m_ulType==FIELD_TYPE_LONG)
			return m_lVal;
		else return -1;
	};

	operator unsigned char ()
	{	if (m_ulType==FIELD_TYPE_TINY )
			return m_ucVal;
		else return 0;
	};

	operator char ()
	{	if (m_ulType==FIELD_TYPE_TINY )
			return m_cVal;
		else return 0;
	};

	operator float()
	{	
		if (m_ulType==FIELD_TYPE_DOUBLE ||
			m_ulType==FIELD_TYPE_FLOAT)
			return m_fVal;
		else return 0;
	};

	operator double()
	{
		if (m_ulType==FIELD_TYPE_DOUBLE ||
			m_ulType==FIELD_TYPE_FLOAT)
			return m_dVal;
		else return 0; 

	};

	operator unsigned int()
	{
		if (m_ulType == FIELD_TYPE_SHORT 
				|| m_ulType == FIELD_TYPE_TINY
				|| m_ulType == FIELD_TYPE_LONG)
			return m_iVal;
		else return 0; 

	};

	operator unsigned long()
	{
		if (m_ulType == FIELD_TYPE_SHORT 
				|| m_ulType == FIELD_TYPE_TINY
				|| m_ulType == FIELD_TYPE_LONG)
			return m_lVal;
		else return 0; 
	};

	operator unsigned short()
	{
		if (m_ulType == FIELD_TYPE_SHORT 
				|| m_ulType == FIELD_TYPE_TINY
				|| m_ulType == FIELD_TYPE_LONG)
			return m_nVal;
		else return 0; 
	};

#define EXTERN_C_DEFINE

#ifdef	EXTERN_C_DEFINE

	operator LPCTSTR ()
	{
		if (m_ulType==FIELD_TYPE_STRING ||
			m_ulType==FIELD_TYPE_VAR_STRING)
			return (m_szVal);
		else return "";
	}
	
#else

	operator = (CString strOp);
	operator CString()
	{	CString str;
		switch (m_ulType)
		{
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
				return (m_szVal);
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_FLOAT:
				str.Format("%f",m_fVal);
				break;
		case FIELD_TYPE_TINY:
		case FIELD_TYPE_SHORT:
		case FIELD_TYPE_LONG:
				str.Format("%d",m_lVal);
				break;
		}
		return str;
	};
#endif

};
	
#endif
