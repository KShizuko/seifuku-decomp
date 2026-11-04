#ifndef	ENCRYPTSERVER_HEAD
#define ENCRYPTSERVER_HEAD

#define		aa	0x7E
#define		bb	0x33
#define		cc	0xA1

template <unsigned long key1, unsigned long key2>
class	CEncryptServer
{
public:
	CEncryptServer(){ Init(); }
public:
	void Init() { m_nPos1 = m_nPos2 = 0; m_cGlobalEncrypt.Init(); }
	void Encrypt(unsigned char * bufMsg, int nLen, bool bMove = true);
	void ChangeCode(DWORD dwData);
protected:
	int		m_nPos1;
	int		m_nPos2;
protected:
	class	CEncryptCode
	{
	public:
		CEncryptCode() { Init(); }
		void Init()
		{
			try{
				int	a1, b1, c1, fst1;
				a1		= ((key1 >> 0) & 0xFF) ^ aa;
				b1		= ((key1 >> 8) & 0xFF) ^ bb;
				c1		= ((key1 >> 24) & 0xFF) ^ cc;
				fst1	= (key1 >> 16) & 0xFF;

				int	a2, b2, c2, fst2;
				a2		= ((key2 >> 0) & 0xFF) ^ aa;
				b2		= ((key2 >> 8) & 0xFF) ^ bb;
				c2		= ((key2 >> 24) & 0xFF) ^ cc;
				fst2	= (key2 >> 16) & 0xFF;

				unsigned char	nCode = fst1;
				for(int i = 0; i < 256; i++)
				{
					m_bufEncrypt1[i] = nCode;
					nCode = (a1*nCode*nCode + b1*nCode + c1) % 256;
				}
				assert(nCode == fst1);

					nCode = fst2;
				for(int i = 0; i < 256; i++)
				{
					m_bufEncrypt2[i] = nCode;
					nCode = (a2*nCode*nCode + b2*nCode + c2) % 256;
				}
				assert(nCode == fst2);
			}catch(...){  }
		}
		unsigned char m_bufEncrypt1[256];
		unsigned char m_bufEncrypt2[256];
	}m_cGlobalEncrypt;
};

template <unsigned long key1, unsigned long key2>
inline void CEncryptServer<key1, key2>::Encrypt(unsigned char * bufMsg, int nLen, bool bMove )
{
	try{
		int		nOldPos1 = m_nPos1;
		int		nOldPos2 = m_nPos2;
		for(int i = 0; i < nLen; i++)
		{
			bufMsg[i] ^= 0xAB;
			bufMsg[i] = (unsigned char)(((bufMsg[i] & 0x0F) << 4) + ((bufMsg[i] & 0xF0) >> 4));
			bufMsg[i] ^= m_cGlobalEncrypt.m_bufEncrypt1[m_nPos1];
			bufMsg[i] ^= m_cGlobalEncrypt.m_bufEncrypt2[m_nPos2];
			if(++m_nPos1 >= 256)
			{
				m_nPos1 = 0;
				if(++m_nPos2 >= 256)
					m_nPos2 = 0;
			}
			assert(m_nPos1 >=0 && m_nPos1 < 256);
			assert(m_nPos2 >=0 && m_nPos2 < 256);
		}

		if(!bMove)
		{
			m_nPos1 = nOldPos1;
			m_nPos2 = nOldPos2;
		}
	}catch(...){  }
}

template <unsigned long key1, unsigned long key2>
inline void CEncryptServer<key1, key2>::ChangeCode(DWORD dwData)
{
	try{
		DWORD	dwData2 = dwData*dwData;
		for(int i = 0; i < 256; i += 4)
		{
			*(DWORD*)(&m_cGlobalEncrypt.m_bufEncrypt1[i]) ^= dwData;
			*(DWORD*)(&m_cGlobalEncrypt.m_bufEncrypt2[i]) ^= dwData2;
		}
	}catch(...){  }
}

#endif
