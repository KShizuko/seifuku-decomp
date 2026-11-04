#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <string.h>
#include <stddef.h>
#include "NetMsg.h"
using namespace world_kernel;
#include "MsgAccount_A.h"
#include "MsgConnect_A.h"
#include "MsgLogin_A.h"
#include "MsgFee_A.h"
#include "MsgQueryFee_A.h"
#include "ConnectUser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CMsgAccount_A::MSG_Info) == 0x38, "");
static_assert(offsetof(CMsgAccount_A::MSG_Info, szAccount) == 0x04, "");
static_assert(offsetof(CMsgAccount_A::MSG_Info, szPassword) == 0x14, "");
static_assert(offsetof(CMsgAccount_A::MSG_Info, szServer) == 0x24, "");
static_assert(offsetof(CMsgAccount_A::MSG_Info, nServerPort) == 0x34, "");
static_assert(sizeof(CMsgLogin_A::MSG_Info) == 0x20, "");
static_assert(offsetof(CMsgLogin_A::MSG_Info, idAccount) == 0x04, "");
static_assert(offsetof(CMsgLogin_A::MSG_Info, dwCheckData) == 0x08, "");
static_assert(offsetof(CMsgLogin_A::MSG_Info, usVersion) == 0x0C, "");
static_assert(offsetof(CMsgLogin_A::MSG_Info, szUserName) == 0x0E, "");
static_assert(sizeof(CMsgConnect_A::MSG_Info) == 0x1C, "");
static_assert(offsetof(CMsgConnect_A::MSG_Info, idAccount) == 0x04, "");
static_assert(offsetof(CMsgConnect_A::MSG_Info, dwData) == 0x08, "");
static_assert(offsetof(CMsgConnect_A::MSG_Info, szInfo) == 0x0C, "");
static_assert(sizeof(CMsgFee_A::MSG_Info) == 0x0C, "");
static_assert(offsetof(CMsgFee_A::MSG_Info, idAccount) == 0x04, "");
static_assert(offsetof(CMsgFee_A::MSG_Info, ucType) == 0x08, "");
static_assert(sizeof(CMsgQueryFee_A::MSG_Info) == 0x14, "");
static_assert(offsetof(CMsgQueryFee_A::MSG_Info, idAccount) == 0x04, "");
static_assert(offsetof(CMsgQueryFee_A::MSG_Info, dwData) == 0x08, "");
static_assert(offsetof(CMsgQueryFee_A::MSG_Info, nTime) == 0x0C, "");
static_assert(offsetof(CMsgQueryFee_A::MSG_Info, ucType) == 0x10, "");

namespace
{
	struct ConnectProbe : public CConnectUser
	{
		static size_t	OffAccount()	{ return offsetof(ConnectProbe, m_idAccount); }
		static size_t	OffCheck()		{ return offsetof(ConnectProbe, m_dwCheckData); }
		static size_t	OffInfo()		{ return offsetof(ConnectProbe, m_szInfo); }
		static size_t	OffSocket()		{ return offsetof(ConnectProbe, m_idSocket); }
		static size_t	OffDelete()		{ return offsetof(ConnectProbe, m_tDelete); }
	};
}

TEST_CLASS(AccountMsgTests)
{
public:
	TEST_METHOD(AccountLinkTypes)
	{
		Assert::AreEqual(1099, (int)_MSG_ACCOUNT_A);
		Assert::AreEqual(1002, (int)_MSG_LOGIN_A);
		Assert::AreEqual(1052, (int)_MSG_CONNECT);
		Assert::AreEqual(1200, (int)_MSG_FEE_A);
		Assert::AreEqual(1201, (int)_MSG_QUERYFEE_A);
		Assert::AreEqual(3, (int)CMsgFee_A::HEARTBEAT);
		Assert::AreEqual(4, (int)CMsgFee_A::FEE_KICKOUT);
		Assert::AreEqual(5, (int)CMsgFee_A::FEE_OFFLINE);
		Assert::AreEqual(7, (int)CMsgFee_A::SERVER_FULL);
		Assert::AreEqual(8, (int)CMsgFee_A::SERVER_CLOSE);
	}

	TEST_METHOD(ConnectUserLayout)
	{
		Assert::AreEqual((size_t)0x28, sizeof(CConnectUser));
		Assert::AreEqual((size_t)0x04, ConnectProbe::OffAccount());
		Assert::AreEqual((size_t)0x08, ConnectProbe::OffCheck());
		Assert::AreEqual((size_t)0x0C, ConnectProbe::OffInfo());
		Assert::AreEqual((size_t)0x1C, ConnectProbe::OffSocket());
		Assert::AreEqual((size_t)0x20, ConnectProbe::OffDelete());
	}

	TEST_METHOD(ConnectUserCreate)
	{
		CConnectUser* p = CConnectUser::CreateNew();
		Assert::IsTrue(p->Create(123, 0xABCD, "0123456789ABCDEFGH", 7));
		Assert::AreEqual(123UL, p->GetID());
		Assert::IsTrue(p->CheckData(0xABCD));
		Assert::IsFalse(p->CheckData(0xABCE));
		Assert::AreEqual(7L, p->GetSocketID());
		Assert::AreEqual("0123456789ABCDE", p->GetInfo());
		Assert::IsTrue(p->IsValid());
		p->ReleaseByOwner();

		p = CConnectUser::CreateNew();
		Assert::IsTrue(p->Create(1, 2, ""));
		Assert::AreEqual((long)SOCKET_NONE, p->GetSocketID());
		p->ReleaseByOwner();
	}
};
