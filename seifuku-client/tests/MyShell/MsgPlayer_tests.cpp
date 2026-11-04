#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgPlayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgPlayerProbe : CMsgPlayer
{
    typedef CMsgPlayer::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgPlayerProbe, m_pInfo); }
    static size_t PackerPtr() { return offsetof(MsgPlayerProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgPlayer) == 0x414, "");
static_assert(sizeof(MsgPlayerProbe::Info) == 0x40, "");
static_assert(offsetof(MsgPlayerProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgPlayerProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwLookFace) == 0x8, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwStatus) == 0xC && sizeof(MsgPlayerProbe::Info().dwStatus) == 8, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwSynID_Rank) == 0x14 && offsetof(MsgPlayerProbe::Info, idOwner) == 0x14, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwGarmentType) == 0x18, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwArmetType) == 0x1C, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwArmorType) == 0x20, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwWeaponRType) == 0x24, "");
static_assert(offsetof(MsgPlayerProbe::Info, dwWeaponLType) == 0x28, "");
static_assert(offsetof(MsgPlayerProbe::Info, usRWeaponRestrain) == 0x2C && offsetof(MsgPlayerProbe::Info, usLWeaponRestrain) == 0x2E, "");
static_assert(offsetof(MsgPlayerProbe::Info, ucMetempsychosis) == 0x3C && offsetof(MsgPlayerProbe::Info, ucReserved) == 0x3D && offsetof(MsgPlayerProbe::Info, szBuf) == 0x3E, "");
static_assert(offsetof(MsgPlayerProbe::Info, usLife) == 0x30 && offsetof(MsgPlayerProbe::Info, usLevel) == 0x32 && offsetof(MsgPlayerProbe::Info, nPotential) == 0x30, "");
static_assert(offsetof(MsgPlayerProbe::Info, usPosX) == 0x34 && offsetof(MsgPlayerProbe::Info, usPosY) == 0x36, "");
static_assert(offsetof(MsgPlayerProbe::Info, usHair) == 0x38, "");
static_assert(offsetof(MsgPlayerProbe::Info, ucDir) == 0x3A, "");
static_assert(offsetof(MsgPlayerProbe::Info, ucPose) == 0x3B, "");
static_assert(std::is_same<decltype(&CMsgPlayer::Create), BOOL (CMsgPlayer::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgPlayerHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgPlayerProbe::InfoPtr() == 0x404);
    }
    TEST_METHOD(PackerFollowsThePointer)
    {
        Assert::IsTrue(MsgPlayerProbe::PackerPtr() == 0x408);
    }
};
