#include "StdAfx.h"
#include <afxmt.h>
#include "CppUnitTest.h"
#include <string.h>
#include "typedef.h"
#include "RC5_321216.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

unsigned long	rotate_left(unsigned long nData, unsigned long nCount);
unsigned long	rotate_right(unsigned long nData, unsigned long nCount);

namespace
{
	unsigned long Rotl(unsigned long v, unsigned long n) { n %= 32; return n ? (v << n) | (v >> (32 - n)) : v; }

	struct ClientRc5
	{
		unsigned long S[26], L[4];
		ClientRc5(const unsigned char* key)
		{
			memcpy(L, key, 16);
			S[0] = 0xB7E15163;
			for (int i = 1; i < 26; i++) S[i] = S[i - 1] + 0x9E3779B9;
			unsigned long A = 0, B = 0;
			int i = 0, j = 0;
			for (int k = 0; k < 3 * 26; k++)
			{
				A = S[i] = Rotl(S[i] + A + B, 3);
				B = L[j] = Rotl(L[j] + A + B, A + B);
				i = (i + 1) % 26; j = (j + 1) % 4;
			}
		}
		void Encrypt(unsigned char* buf, int len)
		{
			for (int k = 0; k < len / 8; k++)
			{
				unsigned long A, B;
				memcpy(&A, buf + 8 * k, 4); memcpy(&B, buf + 8 * k + 4, 4);
				A += S[0]; B += S[1];
				for (int i = 1; i <= 12; i++)
				{
					A = Rotl(A ^ B, B) + S[2 * i];
					B = Rotl(B ^ A, A) + S[2 * i + 1];
				}
				memcpy(buf + 8 * k, &A, 4); memcpy(buf + 8 * k + 4, &B, 4);
			}
		}
	};

	void Password(unsigned char* buf16, const char* psz) { memset(buf16, 0, 16); memcpy(buf16, psz, strlen(psz)); }
}

TEST_CLASS(Rc5Tests)
{
public:
	TEST_METHOD(KeyIsTheClients)
	{
		const unsigned char key[16] = { 0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };
		Assert::AreEqual(0, memcmp(key, RC5PASSWORD_KEY, 16));
	}

	TEST_METHOD(ClientPasswordsDecrypt)
	{
		const char* aPsw[] = { "a", "scenariopw", "123456789012345", "Zz9!", "gbk\xCE\xDE" };
		for (int n = 0; n < (int)(sizeof(aPsw) / sizeof(aPsw[0])); n++)
		{
			unsigned char plain[16], wire[16];
			Password(plain, aPsw[n]);
			memcpy(wire, plain, 16);
			ClientRc5(RC5PASSWORD_KEY).Encrypt(wire, 16);
			Assert::AreNotEqual(0, memcmp(wire, plain, 16));
			CRc5_321216 rc5;
			rc5.Rc5InitKey(RC5PASSWORD_KEY);
			rc5.Rc5Decrypt(wire, 16);
			Assert::AreEqual(0, memcmp(wire, plain, 16));
		}
	}

	TEST_METHOD(EncryptMatchesTheClient)
	{
		unsigned char a[16], b[16];
		Password(a, "scenariopw");
		memcpy(b, a, 16);
		ClientRc5(RC5PASSWORD_KEY).Encrypt(a, 16);
		CRc5_321216 rc5;
		rc5.Rc5InitKey(RC5PASSWORD_KEY);
		rc5.Rc5Encrypt(b, 16);
		Assert::AreEqual(0, memcmp(a, b, 16));
	}

	TEST_METHOD(RotateByZero)
	{
		Assert::AreEqual(0x12345678UL, rotate_left(0x12345678, 0));
		Assert::AreEqual(0x12345678UL, rotate_left(0x12345678, 64));
		Assert::AreEqual(0x12345678UL, rotate_right(0x12345678, 32));
		Assert::AreEqual(0x23456781UL, rotate_left(0x12345678, 4));
		Assert::AreEqual(0x81234567UL, rotate_right(0x12345678, 4));
	}
};
