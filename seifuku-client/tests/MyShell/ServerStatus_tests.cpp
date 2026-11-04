#include "stdafx.h"
#include "ServerStatus.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(ServerInfo) == 0x104, "");
static_assert(offsetof(ServerInfo, nServerStatus) == 0x100, "");
static_assert(std::is_polymorphic<CServerStatus>::value, "");
static_assert(sizeof(CServerStatus) == 4 + sizeof(std::vector<ServerInfo*>), "");

struct ServerStatusProbe
{
    static size_t Set() { return offsetof(CServerStatus, m_setServerInfo); }
    static size_t Count(CServerStatus& s) { return s.m_setServerInfo.size(); }
};

TEST_CLASS(ClientServerStatusHeader)
{
public:
    TEST_METHOD(TheVectorFollowsTheVtable)
    {
        Assert::IsTrue(ServerStatusProbe::Set() == 4);
    }

    TEST_METHOD(AnEmptySetAnswersNone)
    {
        CServerStatus s;
        char szAny[] = "Any";
        Assert::AreEqual(0, s.GetServerStatus(NULL));
        Assert::AreEqual(0, s.GetServerStatus(szAny));
        Assert::IsTrue(ServerStatusProbe::Count(s) == 0);
    }
};
