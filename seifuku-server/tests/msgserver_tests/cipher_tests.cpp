#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <string.h>
#include <assert.h>
#include "EncryptServer.h"
#include "SSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct ClientCipher
	{
		int c1, c2;
		unsigned char t1[256], t2[256];
		ClientCipher() : c1(0), c2(0)
		{
			t1[0] = 0x9D; t2[0] = 0x62;
			for (int i = 0; i < 255; i++)
			{
				t1[i + 1] = (unsigned char)((((t1[i] * 0xFA) & 0xFF) * t1[i] + t1[i] * 0x0F + 0x13) & 0xFF);
				t2[i + 1] = (unsigned char)(((t2[i] * 0xA4 + 0x79) * t2[i] + 0x6D) & 0xFF);
			}
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
		void Rekey(DWORD key)
		{
			for (int i = 0; i < 64; i++)
			{
				((DWORD*)t1)[i] ^= key;
				((DWORD*)t2)[i] ^= key * key;
			}
		}
	};

	typedef CEncryptServer<LOGIN_KEY1, LOGIN_KEY2>	ServerCipher;

	void Fill(unsigned char* buf, int len, unsigned seed)
	{
		for (int i = 0; i < len; i++) { seed = seed * 1103515245u + 12345u; buf[i] = (unsigned char)(seed >> 16); }
	}
}

TEST_CLASS(CipherTests)
{
public:
	TEST_METHOD(ServerSendFirstByteVector)
	{
		ServerCipher server;
		unsigned char buf[1] = { 0x00 };
		server.Encrypt(buf, 1);
		Assert::AreEqual((int)0x45, (int)buf[0]);
	}

	TEST_METHOD(ServerToClient)
	{
		ServerCipher server; ClientCipher client;
		unsigned char plain[700], wire[700];
		Fill(plain, sizeof(plain), 7);
		memcpy(wire, plain, sizeof(wire));
		server.Encrypt(wire, 300);
		server.Encrypt(wire + 300, 400);
		Assert::AreNotEqual(0, memcmp(wire, plain, sizeof(wire)));
		client.Transform(wire, sizeof(wire));
		Assert::AreEqual(0, memcmp(wire, plain, sizeof(wire)));
	}

	TEST_METHOD(ClientToServerAcrossRekey)
	{
		ServerCipher server; ClientCipher client;
		unsigned char first[28], later[600], wire[600];
		Fill(first, sizeof(first), 11);
		Fill(later, sizeof(later), 13);

		memcpy(wire, first, sizeof(first));
		client.Transform(wire, sizeof(first));
		server.Encrypt(wire, sizeof(first));
		Assert::AreEqual(0, memcmp(wire, first, sizeof(first)));

		const DWORD idAccount = 1000123, dwKey = 0x5A3C1E77;
		const DWORD dwRekey = dwKey ^ ((idAccount + dwKey) ^ 0x4321);
		client.Rekey(dwRekey);
		server.ChangeCode(dwRekey);

		memcpy(wire, later, sizeof(later));
		client.Transform(wire, sizeof(later));
		server.Encrypt(wire, sizeof(later));
		Assert::AreEqual(0, memcmp(wire, later, sizeof(later)));
	}

	TEST_METHOD(PeekKeepsCounters)
	{
		ServerCipher a, b;
		unsigned char x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, y[10];
		memcpy(y, x, sizeof(y));
		a.Encrypt(x, sizeof(x), false);
		a.Encrypt(x, 0);
		unsigned char z[10];
		memcpy(z, y, sizeof(z));
		b.Encrypt(z, sizeof(z));
		Assert::AreEqual(0, memcmp(x, z, sizeof(x)), L"a peek transforms like a move");
		memcpy(x, y, sizeof(x));
		a.Encrypt(x, sizeof(x));
		Assert::AreEqual(0, memcmp(x, z, sizeof(x)), L"and leaves the counters at 0");
	}
};
