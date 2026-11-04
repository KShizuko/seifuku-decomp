#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgNpcInfoEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgNpcInfoExProbe : CMsgNpcInfoEX
{
    typedef CMsgNpcInfoEX::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgNpcInfoExProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgNpcInfoExProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgNpcInfoEX) == 0x414, "");
static_assert(sizeof(MsgNpcInfoExProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, dwMaxLife) == 0x8 && offsetof(MsgNpcInfoExProbe::Info, dwLife) == 0xC, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, usCellX) == 0x10 && offsetof(MsgNpcInfoExProbe::Info, usCellY) == 0x12, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, usLook) == 0x14, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, usType) == 0x16, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, usSort) == 0x18, "");
static_assert(offsetof(MsgNpcInfoExProbe::Info, szName) == 0x1A, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgNpcInfoEX::*)(char*, DWORD)>(&CMsgNpcInfoEX::Create)), BOOL (CMsgNpcInfoEX::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgNpcInfoExHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgNpcInfoExProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgNpcInfoExProbe::Packer() == 0x408);
    }
};
