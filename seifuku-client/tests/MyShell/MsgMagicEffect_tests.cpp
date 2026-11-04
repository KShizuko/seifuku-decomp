#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgMagicEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgMagicEffectProbe : CMsgMagicEffect
{
    typedef CMsgMagicEffect::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgMagicEffectProbe, m_pInfo); }
};

static_assert(sizeof(CMsgMagicEffect) == 0x408, "");
static_assert(sizeof(MagicEffectRoleInfo) == 0x8, "");
static_assert(offsetof(MagicEffectRoleInfo, idRole) == 0x0, "");
static_assert(offsetof(MagicEffectRoleInfo, dwData) == 0x4, "");
static_assert(sizeof(MsgMagicEffectProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, idUser) == 0x4, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, idTarget) == 0x8, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, usPosX) == 0x8, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, usPosY) == 0xA, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, usType) == 0xC, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, usLevel) == 0xE, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, ucEffectNum) == 0x10, "");
static_assert(offsetof(MsgMagicEffectProbe::Info, setEffectRole) == 0x14, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMagicEffect::*)(char*, DWORD)>(&CMsgMagicEffect::Create)), BOOL (CMsgMagicEffect::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgMagicEffectHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgMagicEffectProbe::InfoPtr() == 0x404);
    }
};
