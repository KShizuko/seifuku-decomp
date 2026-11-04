#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "ClientSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct Probe : public CClientSocket
	{
		typedef CClientSocket::EncryptClient	Cipher;
		static size_t	OffLen()		{ return offsetof(Probe, m_nLen); }
		static size_t	OffSendLen()	{ return offsetof(Probe, m_nSendLen); }
		static size_t	OffState()		{ return offsetof(Probe, m_nState); }
		static size_t	OffSend()		{ return offsetof(Probe, m_cEncryptSend); }
		static size_t	OffRecv()		{ return offsetof(Probe, m_cEncryptRecv); }
		static size_t	OffSock()		{ return offsetof(Probe, m_sock); }
	};

	struct Model
	{
		int p1, p2;
		unsigned char t1[256], t2[256];
		Model() : p1(0), p2(0)
		{
			int x = 0x7E;
			for (int i = 0; i < 256; i++) { t1[i] = (unsigned char)x; x = (x * (18 * x + 0x97) + 0x19) % 256; }
			x = 0xD3;
			for (int i = 0; i < 256; i++) { t2[i] = (unsigned char)x; x = (x * (72 * x + 0x51) + 0x1B) % 256; }
		}
		unsigned char Next()
		{
			unsigned char t = (unsigned char)(t1[p1] ^ t2[p2]);
			if (++p1 >= 256) { p1 = 0; if (++p2 >= 256) p2 = 0; }
			return t;
		}
		void Transform(unsigned char* buf, int len)
		{
			for (int i = 0; i < len; i++)
			{
				unsigned char b = (unsigned char)(buf[i] ^ Next());
				b = (unsigned char)(((b & 0x0F) << 4) + (b >> 4));
				buf[i] = (unsigned char)(b ^ 0xAB);
			}
		}
		void Inverse(unsigned char* buf, int len)
		{
			for (int i = 0; i < len; i++)
			{
				unsigned char b = (unsigned char)(buf[i] ^ 0xAB);
				b = (unsigned char)(((b & 0x0F) << 4) + (b >> 4));
				buf[i] = (unsigned char)(b ^ Next());
			}
		}
	};

	void Fill(unsigned char* buf, int len, unsigned seed)
	{
		for (int i = 0; i < len; i++) { seed = seed * 1103515245u + 12345u; buf[i] = (unsigned char)(seed >> 16); }
	}
}

TEST_CLASS(AccountLinkTests)
{
public:
	TEST_METHOD(ClientSocketLayout)
	{
		Assert::AreEqual((size_t)0x1C, Probe::OffSock());
		Assert::AreEqual((size_t)0x820, Probe::OffLen());
		Assert::AreEqual((size_t)0x17F4, Probe::OffSendLen());
		Assert::AreEqual((size_t)0x17F8, Probe::OffState());
		Assert::AreEqual((size_t)0x17FC, Probe::OffSend());
		Assert::AreEqual((size_t)0x1A04, Probe::OffRecv());
		Assert::AreEqual((size_t)0x1C0C, sizeof(CClientSocket));
	}

	TEST_METHOD(CipherFirstBytesVector)
	{
		Probe::Cipher c;
		unsigned char buf[4] = { 0, 0, 0, 0 };
		c.Encrypt(buf, 4);
		Assert::AreEqual(0x71, (int)buf[0]);
		Model m;
		unsigned char ref[4] = { 0, 0, 0, 0 };
		m.Transform(ref, 4);
		Assert::IsTrue(memcmp(buf, ref, 4) == 0);
	}

	TEST_METHOD(CipherMatchesModelAndInverse)
	{
		const int N = 256 * 2 + 37;
		unsigned char plain[N], wire[N], back[N];
		Fill(plain, N, 7);
		memcpy(wire, plain, N);
		Probe::Cipher send;
		send.Encrypt(wire, 100);
		send.Encrypt(wire + 100, N - 100);
		Model m;
		memcpy(back, plain, N);
		m.Transform(back, N);
		Assert::IsTrue(memcmp(wire, back, N) == 0);

		Model peer;
		memcpy(back, wire, N);
		peer.Inverse(back, N);
		Assert::IsTrue(memcmp(back, plain, N) == 0);
	}

	TEST_METHOD(CipherNoMoveKeepsPosition)
	{
		Probe::Cipher a, b;
		unsigned char x[8], y[8];
		Fill(x, 8, 3); memcpy(y, x, 8);
		a.Encrypt(x, 8, false);
		a.Encrypt(x, 8);
		b.Encrypt(y, 8);
		b.Encrypt(y, 8);
		unsigned char z[8]; Fill(z, 8, 3);
		Model m; m.Transform(z, 8);
		Model n; n.Transform(z, 8);
		Assert::IsTrue(memcmp(x, z, 8) == 0);
		Assert::IsFalse(memcmp(x, y, 8) == 0);
	}
};
