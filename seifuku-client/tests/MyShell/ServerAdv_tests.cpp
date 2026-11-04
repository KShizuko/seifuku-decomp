#include "stdafx.h"
#include "ServerAdv.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(ServerAdvInfo) == 0x250, "");
static_assert(offsetof(ServerAdvInfo, nPosX) == 0x20 && offsetof(ServerAdvInfo, nPosY) == 0x24, "");
static_assert(offsetof(ServerAdvInfo, szFont) == 0x28 && offsetof(ServerAdvInfo, nFontSize) == 0x48 && offsetof(ServerAdvInfo, dwColor) == 0x4C, "");
static_assert(offsetof(ServerAdvInfo, szText) == 0x50 && offsetof(ServerAdvInfo, szUrl) == 0x150, "");
static_assert(std::is_polymorphic<CServerAdv>::value, "");
static_assert(sizeof(CServerAdv) == 4 + sizeof(std::vector<ServerAdvInfo>), "");

struct ServerAdvProbe
{
    static size_t Set() { return offsetof(CServerAdv, m_setAdv); }
    static void Add(CServerAdv& s, const char* pszServer, const char* pszText)
    {
        ServerAdvInfo info;
        memset(&info, 0, sizeof(info));
        strcpy(info.szServerName, pszServer);
        strcpy(info.szText, pszText);
        s.m_setAdv.push_back(info);
    }
};

TEST_CLASS(ClientServerAdv)
{
public:
    TEST_METHOD(TheVectorFollowsTheVtable)
    {
        Assert::IsTrue(ServerAdvProbe::Set() == 4);
    }
    TEST_METHOD(GetAdvCopiesTheLinesOfOneServer)
    {
        CServerAdv s;
        std::vector<ServerAdvInfo> out;
        Assert::IsFalse(s.GetAdv(NULL, out));
        Assert::IsFalse(s.GetAdv("Dragon", out));
        ServerAdvProbe::Add(s, "Dragon", "one");
        ServerAdvProbe::Add(s, "Tiger", "two");
        ServerAdvProbe::Add(s, "Dragon", "three");
        Assert::IsTrue(s.GetAdv("Dragon", out));
        Assert::IsTrue(out.size() == 2);
        Assert::AreEqual("one", out[0].szText);
        Assert::AreEqual("three", out[1].szText);
        Assert::IsFalse(s.GetAdv("Phoenix", out));
        Assert::IsTrue(out.size() == 0);
    }
};
