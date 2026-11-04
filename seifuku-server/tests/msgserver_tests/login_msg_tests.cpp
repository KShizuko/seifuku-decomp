#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include "CppUnitTest.h"
#include "windows.h"
#define private public
#define protected public
#include "NetMsg.h"
using namespace world_kernel;
#include "MsgTalk.h"
#include "MsgConnect.h"
#include "MsgLogin.h"
#include "MsgRegister.h"
#undef protected
#undef private

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(world_kernel::CNetMsg) == 0x41C, "");
static_assert(sizeof(CMsgConnect) == 0x420 && sizeof(CMsgLogin) == 0x420 && sizeof(CMsgRegister) == 0x420, "");
static_assert(sizeof(CMsgTalk) == 0x42C, "");
static_assert(sizeof(CMsgTalk::MSG_Info) == 0x10, "");

TEST_CLASS(LoginMsgLayoutTests)
{
public:
	TEST_METHOD(NetMsgMembers)
	{
		Assert::AreEqual((size_t)0x04, offsetof(world_kernel::CNetMsg, m_unMsgType));
		Assert::AreEqual((size_t)0x08, offsetof(world_kernel::CNetMsg, m_unMsgSize));
		Assert::AreEqual((size_t)0x0C, offsetof(world_kernel::CNetMsg, m_bufMsg));
		Assert::AreEqual((size_t)0x40C, offsetof(world_kernel::CNetMsg, m_idProcess));
		Assert::AreEqual((size_t)0x410, offsetof(world_kernel::CNetMsg, m_idSocket));
		Assert::AreEqual((size_t)0x414, offsetof(world_kernel::CNetMsg, m_idNpc));
		Assert::AreEqual((size_t)0x418, offsetof(world_kernel::CNetMsg, m_nTransData));
	}
	TEST_METHOD(ConnectAsBothRead)
	{
		Assert::AreEqual((size_t)0x00, offsetof(CMsgConnect::MSG_Info, idAccount));
		Assert::AreEqual((size_t)0x04, offsetof(CMsgConnect::MSG_Info, dwData));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgConnect::MSG_Info, szInfo));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgConnect::MSG_Info, usVersion));
		Assert::AreEqual((size_t)0x0A, offsetof(CMsgConnect::MSG_Info, szLanguage));
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgConnect::MSG_Info, bufClient));
		Assert::AreEqual((size_t)0x14, offsetof(CMsgConnect::MSG_Info, dwResData));
		Assert::AreEqual((size_t)0x18, sizeof(CMsgConnect::MSG_Info));
	}
	TEST_METHOD(LoginAndRegister)
	{
		Assert::AreEqual((size_t)0x04, offsetof(CMsgLogin::MSG_Info, szAccount));
		Assert::AreEqual((size_t)0x14, offsetof(CMsgLogin::MSG_Info, szPassword));
		Assert::AreEqual((size_t)0x10, offsetof(CMsgRegister::MSG_Info, szName));
		Assert::AreEqual((size_t)0x20, offsetof(CMsgRegister::MSG_Info, szPassword));
		Assert::AreEqual((size_t)0x30, offsetof(CMsgRegister::MSG_Info, usLook));
		Assert::AreEqual((size_t)0x32, offsetof(CMsgRegister::MSG_Info, usProfession));
		Assert::AreEqual((size_t)0x34, offsetof(CMsgRegister::MSG_Info, idAccount));
	}
	TEST_METHOD(TalkOf4351)
	{
		Assert::AreEqual((size_t)0x04, offsetof(CMsgTalk::MSG_Info, unTxtAttribute));
		Assert::AreEqual((size_t)0x06, offsetof(CMsgTalk::MSG_Info, unTxtStyle));
		Assert::AreEqual((size_t)0x08, offsetof(CMsgTalk::MSG_Info, dwTime));
		Assert::AreEqual((size_t)0x0C, offsetof(CMsgTalk::MSG_Info, szBuf));
	}
};

namespace
{
	std::vector<char> ClientTalk(DWORD dwColor, USHORT usChannel, USHORT usStyle, DWORD dwTime, const char* const* ppStr, int nStr)
	{
		std::vector<char> v(0x10, 0);
		memcpy(&v[4], &dwColor, 4);
		memcpy(&v[8], &usChannel, 2);
		memcpy(&v[0x0A], &usStyle, 2);
		memcpy(&v[0x0C], &dwTime, 4);
		v.push_back((char)nStr);
		for(int i = 0; i < nStr; i++)
		{
			v.push_back((char)strlen(ppStr[i]));
			v.insert(v.end(), ppStr[i], ppStr[i] + strlen(ppStr[i]));
		}
		v.resize(v.size() + 3, 0);
		USHORT usSize = (USHORT)v.size(), usType = 1004;
		memcpy(&v[0], &usSize, 2);
		memcpy(&v[2], &usType, 2);
		return v;
	}
}

TEST_CLASS(TalkWireTests)
{
public:
	TEST_METHOD(AnswerOkIsBuiltAsTheClientReadsIt)
	{
		const char* strs[] = { "SYSTEM", "ALLUSERS", "", "ANSWER_OK" };
		CMsgTalk msg;
		Assert::IsTrue(msg.Create("SYSTEM", "ALLUSERS", "ANSWER_OK", "", 0xFFFFFF, 2101) != FALSE);
		std::vector<char> want = ClientTalk(0xFFFFFF, 2101, 0, msg.m_pInfo->dwTime, strs, 4);
		Assert::AreEqual((int)want.size() - 4, (int)msg.GetSize());
		Assert::IsTrue(memcmp(msg.GetBuf(), &want[4], want.size() - 4) == 0);
	}
	TEST_METHOD(ClientTalkIsReadAsItCame)
	{
		const char* strs[] = { "Hero", "ALLUSERS", "", "hello" };
		std::vector<char> pkt = ClientTalk(0xFF, 2000, 0, 7, strs, 4);
		CMsgTalk msg;
		Assert::IsTrue(msg.Create(&pkt[4], (DWORD)pkt.size() - 4) != FALSE);
		Assert::AreEqual(2000, (int)msg.m_pInfo->unTxtAttribute);
		Assert::AreEqual(7u, (unsigned)msg.m_pInfo->dwTime);
		char sz[64];
		msg.m_StrPacker.GetString(0, sz, sizeof(sz));
		Assert::AreEqual(std::string("Hero"), std::string(sz));
		msg.m_StrPacker.GetString(3, sz, sizeof(sz));
		Assert::AreEqual(std::string("hello"), std::string(sz));
	}
};
