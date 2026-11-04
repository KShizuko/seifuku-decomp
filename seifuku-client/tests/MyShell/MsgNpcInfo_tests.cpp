#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgNpcInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgNpcInfoProbe : CMsgNpcInfo
{
    typedef CMsgNpcInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgNpcInfoProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgNpcInfoProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgNpcInfo) == 0x414, "");
static_assert(sizeof(MsgNpcInfoProbe::Info) == 0x14, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, usCellX) == 0x8 && offsetof(MsgNpcInfoProbe::Info, usCellY) == 0xA, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, usLook) == 0xC, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, usType) == 0xE, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, usSort) == 0x10, "");
static_assert(offsetof(MsgNpcInfoProbe::Info, szName) == 0x12, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpcInfo::*)(CMyPos, int, int, int)>(&CMsgNpcInfo::Create)), BOOL (CMsgNpcInfo::*)(CMyPos, int, int, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpcInfo::*)(CMyPos, int)>(&CMsgNpcInfo::Create)), BOOL (CMsgNpcInfo::*)(CMyPos, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpcInfo::*)(char*, DWORD)>(&CMsgNpcInfo::Create)), BOOL (CMsgNpcInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgNpcInfoHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgNpcInfoProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgNpcInfoProbe::Packer() == 0x408);
    }
};
