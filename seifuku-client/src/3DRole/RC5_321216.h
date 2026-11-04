#ifndef	RC5_H
#define RC5_H

#ifdef _DEBUG
	#undef	ASSERT
	#define	ASSERT(x)	{ if(!(x)) AfxMessageBox(#x); }
#endif

#define	RC5_32			32

#define	RC5_12			12
#define	RC5_SUB			(RC5_12*2 + 2)

#define	RC5_16			16
#define	RC5_KEY			(RC5_16/4)

const unsigned long	RC5_PW32	= 0xB7E15163;
const unsigned long	RC5_QW32	= 0x9E3779B9;

class	CRc5_321216
{
public:
	void Rc5InitKey(const unsigned char bufKey[RC5_16]);
	void Rc5Encrypt(void* buf, int nLen8);

protected:
	unsigned long	m_bufKey[RC5_KEY];
	unsigned long	m_bufSub[RC5_SUB];
};

#endif