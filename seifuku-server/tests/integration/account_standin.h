#ifndef	ACCOUNT_STANDIN_H
#define	ACCOUNT_STANDIN_H

#include <winsock2.h>
#include <string>
#include <vector>

class CAccountStandIn
{
public:
	enum { MSG_LOGIN = 1002, MSG_CONNECT = 1052, MSG_ACCOUNT = 1099, MSG_FEE = 1200, MSG_QUERYFEE = 1201 };
	enum { SIZE_ACCOUNT = 0x38, SIZE_LOGIN = 0x20, SIZE_CONNECT = 0x1C, SIZE_FEE = 0x0C, SIZE_QUERYFEE = 0x14 };

	struct Message
	{
		unsigned short				usType;
		std::vector<unsigned char>	buf;
	};

public:
	CAccountStandIn();
	~CAccountStandIn();

	bool	Listen(int nPort);
	int		GetPort() const						{ return m_nPort; }
	bool	Accept(int nMilliSecs);
	bool	Recv(Message& msg, int nMilliSecs);
	void	Close();

	bool	SendLoginAnswer(unsigned long idServer, unsigned long dwCheckData, unsigned short usVersion, const char* pszName);
	bool	SendConnect(unsigned long idAccount, unsigned long dwData, const char* pszInfo);
	bool	SendFee(unsigned long idAccount, unsigned char ucType);
	bool	SendQueryFee(unsigned long idAccount, unsigned long dwData, int nTime, unsigned char ucType);
	bool	SendRaw(const void* pBuf, int nLen);

	const std::vector<unsigned char>&	WireIn() const	{ return m_wireIn; }
	const std::vector<unsigned char>&	WireOut() const	{ return m_wireOut; }

	struct Cipher
	{
		int				p1, p2;
		unsigned char	t1[256], t2[256];
		Cipher();
		unsigned char	Next();
		void			Inverse(unsigned char* buf, int nLen);
	};

private:
	SOCKET						m_sockListen;
	SOCKET						m_sock;
	int							m_nPort;
	Cipher						m_cipherIn;
	Cipher						m_cipherOut;
	std::vector<unsigned char>	m_plain;
	std::vector<unsigned char>	m_wireIn;
	std::vector<unsigned char>	m_wireOut;
};

#endif
