#if !defined(AFX_ENCRYPTDATA_H__F5417883_9631_4AF1_8829_1A4ABD409790__INCLUDED_)
#define AFX_ENCRYPTDATA_H__F5417883_9631_4AF1_8829_1A4ABD409790__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"

class CEncryptData
{
	friend struct EncryptDataProbe;
public:
	CEncryptData();
	virtual ~CEncryptData();
public:
	DWORD	*set_key(const DWORD in_key[], const DWORD key_len);

public:
	void	SetValue(int nData);
	int		GetValue(void);

	operator int() { return GetValue(); }
	CEncryptData& operator=(int nData) { SetValue(nData); return *this; }
private:
	DWORD  l_key[44];
	inline void	encrypt(const DWORD in_blk[4], DWORD out_blk[4]);
	inline void	decrypt(const DWORD in_blk[4], DWORD out_blk[4]);

private:
	DWORD	m_dwValue[4];
};

struct CMyProtectPos
{
	CEncryptData x;
	CEncryptData y;

	CMyProtectPos& operator=(const CMyPos& pos) { x.SetValue(pos.x); y.SetValue(pos.y); return *this; }
	operator CMyPos() { CMyPos pos = { x.GetValue(), y.GetValue() }; return pos; }
};

#endif
