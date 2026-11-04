#include "StdAfx.h"
#include <afxmt.h>
#include "CppUnitTest.h"
#include <string.h>
#include <stddef.h>
#include "MsgAccount.h"
#include "MsgConnect.h"
#include "MsgLogin.h"
#include "MsgFee.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	void Put16(unsigned char* p, int off, unsigned short x)	{ memcpy(p + off, &x, 2); }
	void Put32(unsigned char* p, int off, unsigned long x)	{ memcpy(p + off, &x, 4); }
	unsigned long Get32(const char* p, int off)				{ unsigned long x; memcpy(&x, p + off, 4); return x; }
	unsigned short Get16(const char* p, int off)			{ unsigned short x; memcpy(&x, p + off, 2); return x; }
}

TEST_CLASS(AccountMsgTests)
{
public:
	TEST_METHOD(Account1051FromTheClient)
	{
		Assert::AreEqual((size_t)0x34, sizeof(CMsgAccount::MSG_Info));
		Assert::AreEqual((size_t)0x04, offsetof(CMsgAccount::MSG_Info, szAccount));
		Assert::AreEqual((size_t)0x14, offsetof(CMsgAccount::MSG_Info, szPassword));
		Assert::AreEqual((size_t)0x24, offsetof(CMsgAccount::MSG_Info, szServer));
		unsigned char wire[0x34] = { 0 };
		Put16(wire, 0, 0x34); Put16(wire, 2, 1051);
		memcpy(wire + 4, "acc", 3); memcpy(wire + 0x24, "msggate", 7);
		CMsgAccount msg;
		Assert::IsTrue(msg.Create((char*)wire, sizeof(wire)) != FALSE);
		Assert::AreEqual(std::string("acc"), std::string(msg.m_pInfo->szAccount));
		Assert::AreEqual(std::string("msggate"), std::string(msg.m_pInfo->szServer));
		Assert::IsFalse(msg.Create((char*)wire, sizeof(wire) - 1) != FALSE);
	}

	TEST_METHOD(Connect1052Layout)
	{
		CMsgConnect msg;
		char szInfo[] = "127.0.0.1";
		Assert::IsTrue(msg.Create(1000001, 0x12345678, szInfo) != FALSE);
		Assert::AreEqual(0x1C, (int)msg.GetSize());
		Assert::AreEqual(1052, (int)Get16(msg.GetBuf(), 2));
		Assert::AreEqual(1000001UL, Get32(msg.GetBuf(), 4));
		Assert::AreEqual(0x12345678UL, Get32(msg.GetBuf(), 8));
		Assert::AreEqual(std::string("127.0.0.1"), std::string(msg.GetBuf() + 0x0C));
	}

	TEST_METHOD(Connect1052FromTheClient)
	{
		unsigned char wire[0x1C] = { 0 };
		Put16(wire, 0, 0x1C); Put16(wire, 2, 1052); Put32(wire, 4, 1000001); Put32(wire, 8, 3);
		memcpy(wire + 0x0C, "res.dat", 7);
		CMsgConnect msg;
		Assert::IsTrue(msg.Create((char*)wire, sizeof(wire)) != FALSE);
		Assert::AreEqual(1000001UL, (unsigned long)msg.m_pInfo->idAccount);
		Assert::AreEqual(3UL, (unsigned long)msg.m_pInfo->dwData);
	}

	TEST_METHOD(Login1002Layout)
	{
		CMsgLogin msg;
		char szName[] = "msggate";
		Assert::IsTrue(msg.Create(1, 0, szName, 0xC213) != FALSE);
		Assert::AreEqual(0x20, (int)msg.GetSize());
		Assert::AreEqual(1002, (int)Get16(msg.GetBuf(), 2));
		Assert::AreEqual(1UL, Get32(msg.GetBuf(), 4));
		Assert::AreEqual(0xC213, (int)Get16(msg.GetBuf(), 0x0C));
		Assert::AreEqual(std::string("msggate"), std::string(msg.GetBuf() + 0x0E));
	}

	TEST_METHOD(Fee1200Layout)
	{
		CMsgFee msg;
		Assert::IsTrue(msg.Create(1000001, CMsgFee::HEARTBEAT) != FALSE);
		Assert::AreEqual(0x0C, (int)msg.GetSize());
		Assert::AreEqual(1200, (int)Get16(msg.GetBuf(), 2));
		Assert::AreEqual(1000001UL, Get32(msg.GetBuf(), 4));
		Assert::AreEqual(3, (int)(unsigned char)msg.GetBuf()[8]);
		Assert::AreEqual(0, (int)CMsgFee::FEE_BEGIN);
		Assert::AreEqual(5, (int)CMsgFee::FEE_OFFLINE);
		Assert::AreEqual(7, (int)CMsgFee::SERVER_FULL);
		Assert::AreEqual(9, (int)CMsgFee::FEE_TICK);
	}
};
