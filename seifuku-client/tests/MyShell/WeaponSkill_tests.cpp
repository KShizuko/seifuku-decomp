#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "WeaponSkill.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct WeaponSkillProbe : CWeaponSkill
{
    static size_t Type() { return offsetof(WeaponSkillProbe, m_nWeaponSkillType); }
    static size_t Level() { return offsetof(WeaponSkillProbe, m_dwLevel); }
    static size_t Exp() { return offsetof(WeaponSkillProbe, m_dwExp); }
    static size_t Describe() { return offsetof(WeaponSkillProbe, m_szDescribe); }
};

struct WeaponSkillDataProbe : CWeaponSkillData
{
    static size_t Names() { return offsetof(WeaponSkillDataProbe, m_setWeaponSkillInfo); }
    static size_t LevelExp() { return offsetof(WeaponSkillDataProbe, m_setWeaponSkillLevelExp); }
    static size_t Error() { return offsetof(WeaponSkillDataProbe, m_szError); }
};

static_assert(sizeof(WeaponSkillInfo) == 0x14, "");
static_assert(sizeof(WeaponSkillLevelExp) == 0x8, "");
static_assert(sizeof(CWeaponSkill) == 0x90, "");
static_assert(std::has_virtual_destructor<CWeaponSkill>::value, "");
static_assert(sizeof(CWeaponSkillData) == 4 + 2 * sizeof(DEQUE_WEAPONSKILLINFO) + 0x10, "");
static_assert(std::has_virtual_destructor<CWeaponSkillData>::value, "");
static_assert(std::is_same<decltype(&CWeaponSkillData::GetWeaponSkillName), char* (CWeaponSkillData::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CWeaponSkillData::GetExpRequired), DWORD (CWeaponSkillData::*)(DWORD)>::value, "");

TEST_CLASS(ClientWeaponSkillHeader)
{
public:
    TEST_METHOD(SkillMembersAreByteExact)
    {
        Assert::IsTrue(WeaponSkillProbe::Type() == 0x4);
        Assert::IsTrue(WeaponSkillProbe::Level() == 0x8);
        Assert::IsTrue(WeaponSkillProbe::Exp() == 0xC);
        Assert::IsTrue(WeaponSkillProbe::Describe() == 0x10);
    }

    TEST_METHOD(DataMembersFollowTheTwoDeques)
    {
        const size_t d = sizeof(DEQUE_WEAPONSKILLINFO);
        Assert::IsTrue(WeaponSkillDataProbe::Names() == 0x4);
        Assert::IsTrue(WeaponSkillDataProbe::LevelExp() == 0x4 + d);
        Assert::IsTrue(WeaponSkillDataProbe::Error() == 0x4 + 2 * d);
    }
};
