#if !defined(AFX_KILLCOUNTER_H__SFK_4294_INCLUDED_)
#define AFX_KILLCOUNTER_H__SFK_4294_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"
#include "Game3DEffectEx.h"
#include "ZoomInImgString.h"
#include <deque>
#include <string>

class CKillCounter
{
	friend struct KillCounterProbe;
public:
	CKillCounter();
	virtual ~CKillCounter();

	void	SetEffectPrefix	(const char* pszPrefix)	{ m_strEffectPrefix = pszPrefix; m_objZoomIn.SetEffectPrefix(pszPrefix); }
	void	Add				(DWORD dwNumber);
	void	AddString		(const char* pszNumber);
	void	Show			(void);
	void	SetString		(const char* pszString);
	void	ShowString		(void);
	void	Flash			(const char* pszNumber);
	void	SetZoomInPrefix	(const char* pszPrefix)	{ m_objZoomIn.SetEffectPrefix(pszPrefix); }

private:
	void	ShowEffect		(int x, int y, IGame3DEffect* pEffect);

private:
	deque<DWORD>		m_setNumber;
	std::string			m_strEffectPrefix;
	DWORD				m_dwBeginTime;
	int					m_nStatus;
	char				m_szNumber[16];
	char				m_szLastNumber[16];
	DWORD				m_dwColor;
	IGame3DEffect*		m_objDigits;
	IGame3DEffect*		m_objTitle;
	IGame3DEffect*		m_objLastDigit;
	CZoomInImgString	m_objZoomIn;
	char				m_szString[16];
	DWORD				m_dwStringTime;
	IGame3DEffect*		m_objString;
};

class CKoCounter
{
	friend struct KoCounterProbe;
public:
	CKoCounter();
	virtual ~CKoCounter();

	void	Add				(DWORD dwNumber);
	void	AddString		(const char* pszNumber);
	void	Show			(void);

private:
	void	ShowEffect		(int x, int y, IGame3DEffect* pEffect);

private:
	deque<DWORD>		m_setNumber;
	int					m_nStatus;
	DWORD				m_dwColor;
	DWORD				m_dwBeginTime;
	char				m_szNumber[16];
	char				m_szLastNumber[16];
	IGame3DEffect*		m_objDigits;
	IGame3DEffect*		m_objTitle;
	IGame3DEffect*		m_objLastDigit;
};

extern BOOL g_bKoShowRecord;

#endif
