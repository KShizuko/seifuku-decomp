#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgConnect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgConnectProbe : CMsgConnect
{
    typedef CMsgConnect::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgConnectProbe, m_pInfo); }
};

static_assert(sizeof(CMsgConnect) == 0x408, "");
static_assert(sizeof(MsgConnectProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgConnectProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgConnectProbe::Info, idAccount) == 0x4, "");
static_assert(offsetof(MsgConnectProbe::Info, dwData) == 0x8, "");
static_assert(offsetof(MsgConnectProbe::Info, szInfo) == 0xC, "");
static_assert(sizeof(MsgConnectProbe::Info::szInfo) == _MAX_NAMESIZE && _MAX_NAMESIZE == 16, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgConnect::*)(OBJID, DWORD, const char*)>(&CMsgConnect::Create)), BOOL (CMsgConnect::*)(OBJID, DWORD, const char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgConnect::*)(char*, DWORD)>(&CMsgConnect::Create)), BOOL (CMsgConnect::*)(char*, DWORD)>::value, "");
static_assert(sizeof(MAC_ADDRESS) == 6 && MAX_MACADDRSIZE == 6, "");
static_assert(_MAX_LANGUAGESIZE == 2, "");
static_assert(sizeof(MsgConnectProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgConnectProbe::Info, game.usVersion) == 0xC, "");
static_assert(offsetof(MsgConnectProbe::Info, game.szLanguage) == 0xE, "");
static_assert(offsetof(MsgConnectProbe::Info, game.abyMac) == 0x10, "");
static_assert(offsetof(MsgConnectProbe::Info, game.dwRes) == 0x18, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgConnect::*)(OBJID, DWORD, int, const char*, const MAC_ADDRESS*, DWORD)>(&CMsgConnect::Create)), BOOL (CMsgConnect::*)(OBJID, DWORD, int, const char*, const MAC_ADDRESS*, DWORD)>::value, "");

TEST_CLASS(ClientMsgConnectHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgConnectProbe::InfoPtr() == 0x404);
    }
};
