#if !defined(SFK_MD5_H__INCLUDED_)
#define SFK_MD5_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

class CMD5
{
	friend struct MD5Probe;
public:
	CMD5();
	virtual ~CMD5();

	void	Update	(const unsigned char* pInput, unsigned int uInputLen);
	bool	Final	(char* pszOut);

private:
	void	Transform	(const unsigned char block[64]);
	void	Encode		(unsigned char* pOutput, const unsigned long* pInput, unsigned int uLen);
	void	Decode		(unsigned long* pOutput, const unsigned char* pInput, unsigned int uLen);
	void	FF	(unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac);
	void	GG	(unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac);
	void	HH	(unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac);
	void	II	(unsigned long& a, unsigned long b, unsigned long c, unsigned long d, unsigned long x, unsigned long s, unsigned long ac);

private:
	unsigned char	m_buffer[64];
	unsigned long	m_count[2];
	unsigned long	m_state[4];
};

const char*	MD5String	(const char* pszSrc, int nLen);
void		MD5Password	(char* pszOut, const char* pszSrc);

#endif
