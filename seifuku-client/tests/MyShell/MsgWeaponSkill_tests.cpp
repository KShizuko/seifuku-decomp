#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgWeaponSkill.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgWeaponSkillProbe : CMsgWeaponSkill
{
    typedef CMsgWeaponSkill::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgWeaponSkillProbe, m_pInfo); }
};

static_assert(sizeof(CMsgWeaponSkill) == 0x408, "");
static_assert(sizeof(MsgWeaponSkillProbe::Info) == 0x10, "");
static_assert(offsetof(MsgWeaponSkillProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgWeaponSkillProbe::Info, unType) == 0x4, "");
static_assert(offsetof(MsgWeaponSkillProbe::Info, usLevel) == 0x8, "");
static_assert(offsetof(MsgWeaponSkillProbe::Info, unExp) == 0xC, "");
static_assert(std::is_same<decltype(&CMsgWeaponSkill::Create), BOOL (CMsgWeaponSkill::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgWeaponSkillHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgWeaponSkillProbe::InfoPtr() == 0x404);
    }
};
