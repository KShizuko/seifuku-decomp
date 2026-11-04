#ifndef	ENCRYPTCLIENT_H
#define ENCRYPTCLIENT_H

#include <stdlib.h>
#include "logfile.h"

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
					unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
class	CEncryptClient
{
public:
	CEncryptClient(){ Init(); }
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

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
					unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
inline void CEncryptClient<a1, b1, c1, fst1, a2, b2, c2, fst2>::Encrypt(unsigned char * bufMsg, int nLen, bool bMove )
{
	try{
		int		nOldPos1 = m_nPos1;
		int		nOldPos2 = m_nPos2;
		for(int i = 0; i < nLen; i++)
		{
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
			bufMsg[i] = (unsigned char)(((bufMsg[i] & 0x0F) << 4) + (bufMsg[i] >> 4));
			bufMsg[i] ^= 0xAB;
		}

		if(!bMove)
		{
			m_nPos1 = nOldPos1;
			m_nPos2 = nOldPos2;
		}
	}catch(...){  }
}

template <unsigned char a1, unsigned char b1, unsigned char c1, unsigned char fst1, 
					unsigned char a2, unsigned char b2, unsigned char c2, unsigned char fst2>
inline void CEncryptClient<a1, b1, c1, fst1, a2, b2, c2, fst2>::ChangeCode(DWORD dwData)
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
