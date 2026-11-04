#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgUserInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgUserInfoProbe : CMsgUserInfo
{
    typedef CMsgUserInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgUserInfoProbe, m_pInfo); }
    static size_t PackerPtr() { return offsetof(MsgUserInfoProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgUserInfo) == 0x414, "");
static_assert(sizeof(MsgUserInfoProbe::Info) == 0x44, "");
static_assert(offsetof(MsgUserInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgUserInfoProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgUserInfoProbe::Info, dwLookFace) == 0x8, "");
static_assert(offsetof(MsgUserInfoProbe::Info, usHair) == 0xC, "");
static_assert(offsetof(MsgUserInfoProbe::Info, dwMoney) == 0x10, "");
static_assert(offsetof(MsgUserInfoProbe::Info, dwEmoney) == 0x14, "");
static_assert(offsetof(MsgUserInfoProbe::Info, nExp) == 0x18, "");
static_assert(offsetof(MsgUserInfoProbe::Info, lDeed) == 0x1C, "");
static_assert(offsetof(MsgUserInfoProbe::Info, dwMedalSelect) == 0x20, "");
static_assert(offsetof(MsgUserInfoProbe::Info, nPotential) == 0x24, "");
static_assert(offsetof(MsgUserInfoProbe::Info, dwVirtue) == 0x28, "");
static_assert(offsetof(MsgUserInfoProbe::Info, usForce) == 0x2C && offsetof(MsgUserInfoProbe::Info, usDexterity) == 0x2E && offsetof(MsgUserInfoProbe::Info, usHealth) == 0x30 && offsetof(MsgUserInfoProbe::Info, usSoul) == 0x32, "");
static_assert(offsetof(MsgUserInfoProbe::Info, usAdditionalPoint) == 0x34, "");
static_assert(offsetof(MsgUserInfoProbe::Info, usLife) == 0x36, "");
static_assert(offsetof(MsgUserInfoProbe::Info, usMana) == 0x38, "");
static_assert(offsetof(MsgUserInfoProbe::Info, sPk) == 0x3A, "");
static_assert(offsetof(MsgUserInfoProbe::Info, ucLevel) == 0x3C && offsetof(MsgUserInfoProbe::Info, ucProfession) == 0x3D, "");
static_assert(offsetof(MsgUserInfoProbe::Info, ucNobility) == 0x3E && offsetof(MsgUserInfoProbe::Info, ucMetempsychosis) == 0x3F && offsetof(MsgUserInfoProbe::Info, ucAutoAllot) == 0x40, "");
static_assert(offsetof(MsgUserInfoProbe::Info, szBuf) == 0x41, "");
static_assert(std::is_same<decltype(&CMsgUserInfo::Create), BOOL (CMsgUserInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgUserInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgUserInfoProbe::InfoPtr() == 0x404);
    }
    TEST_METHOD(PackerFollowsThePointer)
    {
        Assert::IsTrue(MsgUserInfoProbe::PackerPtr() == 0x408);
    }
};
