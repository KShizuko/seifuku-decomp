#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <stddef.h>
#include "UserList.h"
#include "protocol.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(UserListTests)
{
public:
	TEST_METHOD(PlayerLayout)
	{
		Assert::AreEqual((size_t)0x00, offsetof(CPlayer, idUser));
		Assert::AreEqual((size_t)0x04, offsetof(CPlayer, szName));
		Assert::AreEqual((size_t)0x14, offsetof(CPlayer, idProcess));
		Assert::AreEqual((size_t)0x18, offsetof(CPlayer, szAccount));
		Assert::AreEqual((size_t)0x28, offsetof(CPlayer, idSocket));
		Assert::AreEqual((size_t)0x2C, offsetof(CPlayer, m_nReserved));
		Assert::AreEqual((size_t)0x30, offsetof(CPlayer, m_tPoint));
		Assert::AreEqual((size_t)0x34, offsetof(CPlayer, m_tTick));
		Assert::AreEqual((size_t)0x3C, offsetof(CPlayer, m_idAccount));
		Assert::AreEqual((size_t)0x40, offsetof(CPlayer, m_nLevel));
		Assert::AreEqual((size_t)0x44, offsetof(CPlayer, szNotify));
		Assert::AreEqual((size_t)0x54, sizeof(CPlayer));
	}

	TEST_METHOD(AccountLockLayout)
	{
		Assert::AreEqual((size_t)0x00, offsetof(ACCOUNT_LOCK, nCount));
		Assert::AreEqual((size_t)0x04, offsetof(ACCOUNT_LOCK, tLast));
		Assert::AreEqual((size_t)0x08, offsetof(ACCOUNT_LOCK, tFirst));
		Assert::AreEqual((size_t)0x0C, offsetof(ACCOUNT_LOCK, nLoginMinute));
		Assert::AreEqual((size_t)0x10, offsetof(ACCOUNT_LOCK, bufMac));
		Assert::AreEqual((size_t)0x18, offsetof(ACCOUNT_LOCK, dwIP));
		Assert::AreEqual((size_t)0x1C, offsetof(ACCOUNT_LOCK, dwResData));
		Assert::AreEqual((size_t)0x20, sizeof(ACCOUNT_LOCK));
	}

	TEST_METHOD(LoginPacketLayout)
	{
		Assert::AreEqual((size_t)0x0, offsetof(ST_LOGIN, idSocket));
		Assert::AreEqual((size_t)0x4, offsetof(ST_LOGIN, idUser));
		Assert::AreEqual((size_t)0x8, offsetof(ST_LOGIN, nMinorFlag));
		Assert::AreEqual((size_t)0xC, offsetof(ST_LOGIN, nClientVersion));
		Assert::AreEqual((size_t)0x10, sizeof(ST_LOGIN));
	}
};
