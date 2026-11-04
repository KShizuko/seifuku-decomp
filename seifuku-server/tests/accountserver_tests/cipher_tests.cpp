#include "StdAfx.h"
#include <afxmt.h>
#include "CppUnitTest.h"
#include <string.h>
#include <assert.h>
#include "typedef.h"
#include "EncryptServer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct PeerCipher
	{
		int c1, c2;
		unsigned char t1[256], t2[256];
		PeerCipher(int a1, int b1, int k1, int f1, int a2, int b2, int k2, int f2) : c1(0), c2(0)
		{
			int x = f1;
			for (int i = 0; i < 256; i++) { t1[i] = (unsigned char)x; x = (a1 * x * x + b1 * x + k1) % 256; }
			x = f2;
			for (int i = 0; i < 256; i++) { t2[i] = (unsigned char)x; x = (a2 * x * x + b2 * x + k2) % 256; }
		}
		void Transform(unsigned char* buf, int len)
		{
			for (int i = 0; i < len; i++)
			{
				unsigned char b = buf[i];
				b ^= t1[c1];
				b ^= t2[c2];
				if (++c1 >= 256) { c1 = 0; if (++c2 >= 256) c2 = 0; }
				b = (unsigned char)(((b & 0x0F) << 4) + ((b & 0xF0) >> 4));
				b ^= 0xAB;
				buf[i] = b;
			}
		}
	};

	PeerCipher Client()		{ return PeerCipher(0xFA, 0x0F, 0x13, 0x9D, 0xA4, 0x79, 0x6D, 0x62); }
	PeerCipher MsgServer()	{ return PeerCipher(0x12, 0x97, 0x19, 0x7E, 0x48, 0x51, 0x1B, 0xD3); }

	typedef CEncryptServer<ACCOUNT_KEY1, ACCOUNT_KEY2>	LoginCipher;
	typedef CEncryptServer<POINT_KEY1, POINT_KEY2>		PointCipher;

	void Fill(unsigned char* buf, int len, unsigned seed)
	{
		for (int i = 0; i < len; i++) { seed = seed * 1103515245u + 12345u; buf[i] = (unsigned char)(seed >> 16); }
	}

	template<class SERVER> void BothWays(PeerCipher peerIn, PeerCipher peerOut)
	{
		SERVER serverIn, serverOut;
		unsigned char plain[700], wire[700];
		Fill(plain, sizeof(plain), 7);
		memcpy(wire, plain, sizeof(wire));
		serverOut.Encrypt(wire, 300);
		serverOut.Encrypt(wire + 300, 400);
		Assert::AreNotEqual(0, memcmp(wire, plain, sizeof(wire)));
		peerIn.Transform(wire, sizeof(wire));
		Assert::AreEqual(0, memcmp(wire, plain, sizeof(wire)), L"server to peer");

		Fill(plain, sizeof(plain), 13);
		memcpy(wire, plain, sizeof(wire));
		peerOut.Transform(wire, sizeof(wire));
		serverIn.Encrypt(wire, 52);
		serverIn.Encrypt(wire + 52, sizeof(wire) - 52);
		Assert::AreEqual(0, memcmp(wire, plain, sizeof(wire)), L"peer to server");
	}
}

TEST_CLASS(AccountCipherTests)
{
public:
	TEST_METHOD(LoginFirstByteVector)
	{
		LoginCipher server;
		unsigned char buf[1] = { 0x00 };
		server.Encrypt(buf, 1);
		Assert::AreEqual((int)0x45, (int)buf[0]);
	}

	TEST_METHOD(LoginBothWays)		{ BothWays<LoginCipher>(Client(), Client()); }

	TEST_METHOD(PointFirstByteVector)
	{
		PointCipher server;
		unsigned char buf[1] = { 0x00 };
		server.Encrypt(buf, 1);
		Assert::AreEqual((int)0x17, (int)buf[0]);
	}

	TEST_METHOD(PointBothWays)		{ BothWays<PointCipher>(MsgServer(), MsgServer()); }

	TEST_METHOD(ReferenceKeysDoNotMatchTheClient)
	{
		CEncryptServer<0xa61fce5e, 0x443ffc04> server;
		PeerCipher client = Client();
		unsigned char plain[64], wire[64];
		Fill(plain, sizeof(plain), 3);
		memcpy(wire, plain, sizeof(wire));
		server.Encrypt(wire, sizeof(wire));
		client.Transform(wire, sizeof(wire));
		Assert::AreNotEqual(0, memcmp(wire, plain, sizeof(wire)));
	}

	TEST_METHOD(PeekKeepsCounters)
	{
		LoginCipher a, b;
		unsigned char x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, y[10];
		memcpy(y, x, sizeof(y));
		a.Encrypt(x, 2, false);
		a.Encrypt(x, 10);
		b.Encrypt(y, 10);
		Assert::AreEqual(0, memcmp(x + 2, y + 2, 8));
	}
};
