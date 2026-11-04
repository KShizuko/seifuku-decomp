#if !defined(AFX_CHECK_H__BD357BB3_6170_4E69_8778_C55BCA1682B8__INCLUDED_)
#define AFX_CHECK_H__BD357BB3_6170_4E69_8778_C55BCA1682B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"

class CCheck
{
	friend struct CheckProbe;
public:
	enum
	{
			_WS_MAINWINDOW_	= 0x00000001,
	};
	enum
	{
		_TYPE_WS				=1,
		_TYPE_FY				=2,
		_TYPE_USE_LIFE_MEDICINE	=3,
		_TYPE_USE_MANA_MEDICINE	=4,
		_TYPE_WS2_LIBRARY		=5,
	};
public:
	CCheck();
	virtual ~CCheck();
public:
	BOOL Process();
	void Reset();
	void DecLife();
	void DecMana();
	void AddLife();
	void AddMana();
private:
	DWORD	m_dwTimeCheck;
	BOOL	m_bReportWSProcess;
	BOOL	m_bReportFYProcess;
	BOOL	m_bReportUseLifeMedicine;
	BOOL	m_bReportUseManaMedicine;
	BOOL	m_bReportWS2Library;
	int		m_nUseLifeMedicineAmount;
	int		m_nUseManaMedicineAmount;
	DWORD	m_dwLastDecLife;
	DWORD	m_dwLastDecMana;
	union
	{
		DWORD	m_dwData;
		struct
		{
			UCHAR	m_ucType;
		};
	};
};

const int _MAX_CHECKKEYS = 20;
class CCheckKeys
{
	friend struct CheckKeysProbe;
public:
	CCheckKeys();
	virtual ~CCheckKeys();
public:
	void	Clear();
	void	SetKey(int nIndex, DWORD dwKey);
	DWORD	GetKey(int nIndex);
private:
	DWORD	m_dwKeys[_MAX_CHECKKEYS];
};

#endif
