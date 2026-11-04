#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MagicEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MagicEffectProbe : CMagicEffect
{
    static size_t PosTarget() { return offsetof(MagicEffectProbe, m_posTarget); }
    static size_t PosBegin() { return offsetof(MagicEffectProbe, m_posBegin); }
    static size_t PosEnd() { return offsetof(MagicEffectProbe, m_posEnd); }
    static size_t Begin() { return offsetof(MagicEffectProbe, m_dwBegin); }
    static size_t Type() { return offsetof(MagicEffectProbe, m_nType); }
    static size_t Sender() { return offsetof(MagicEffectProbe, m_idSender); }
    static size_t Target() { return offsetof(MagicEffectProbe, m_idTarget); }
    static size_t MagicType() { return offsetof(MagicEffectProbe, m_dwMagicType); }
    static size_t Level() { return offsetof(MagicEffectProbe, m_dwLevel); }
    static size_t BeginFlag() { return offsetof(MagicEffectProbe, m_bBegin); }
    static size_t DelayOk() { return offsetof(MagicEffectProbe, m_bDelayOk); }
    static size_t Data() { return offsetof(MagicEffectProbe, m_pData); }
    static size_t Moment() { return offsetof(MagicEffectProbe, m_dwMoment); }
    static size_t DwData() { return offsetof(MagicEffectProbe, m_dwData); }
    static size_t Delay() { return offsetof(MagicEffectProbe, m_dwDelay); }
    static size_t RoleInfo() { return offsetof(MagicEffectProbe, m_setRoleInfo); }
};

static_assert(sizeof(MagicEffectRoleInfo) == 8, "");
static_assert(sizeof(MagicEffectSoulInfo) == 4, "");
static_assert(sizeof(CMagicEffect) == 0x4C + sizeof(VECTOR_MAGICEFFECTROLEINFO), "");
static_assert(std::has_virtual_destructor<CMagicEffect>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::CreateNew), CMagicEffect* (*)(OBJID, OBJID, DWORD, DWORD, int)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::CreateNewArrow), CMagicEffect* (*)(OBJID, OBJID, int, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::CreateNewTerrain), CMagicEffect* (*)(OBJID, CMyPos, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::Create), BOOL (CMagicEffect::*)(OBJID, OBJID, DWORD, DWORD, int)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::CreateArrow), BOOL (CMagicEffect::*)(OBJID, OBJID, int, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::CreateTerrain), BOOL (CMagicEffect::*)(OBJID, CMyPos, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::AddRoleInfo), void (CMagicEffect::*)(OBJID, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffect::BeginScreenEffect), void (CMagicEffect::*)(DWORD)>::value, "");
static_assert(MAGICEFFECT_BODY_TERRAIN_BODY == 3, "");
static_assert(MAGICEFFECT_ARROW == 98, "");
static_assert(MAGICEFFECT_SOUL == 99, "");

TEST_CLASS(ClientMagicEffectHeader)
{
public:
    TEST_METHOD(MemberOffsetsMatchTheCreatorsWrites)
    {
        Assert::IsTrue(MagicEffectProbe::PosTarget() == 0x4);
        Assert::IsTrue(MagicEffectProbe::PosBegin() == 0xC);
        Assert::IsTrue(MagicEffectProbe::PosEnd() == 0x14);
        Assert::IsTrue(MagicEffectProbe::Begin() == 0x1C);
        Assert::IsTrue(MagicEffectProbe::Type() == 0x20);
        Assert::IsTrue(MagicEffectProbe::Sender() == 0x24);
        Assert::IsTrue(MagicEffectProbe::Target() == 0x28);
        Assert::IsTrue(MagicEffectProbe::MagicType() == 0x2C);
        Assert::IsTrue(MagicEffectProbe::Level() == 0x30);
        Assert::IsTrue(MagicEffectProbe::BeginFlag() == 0x34);
        Assert::IsTrue(MagicEffectProbe::DelayOk() == 0x38);
        Assert::IsTrue(MagicEffectProbe::Data() == 0x3C);
        Assert::IsTrue(MagicEffectProbe::Moment() == 0x40);
        Assert::IsTrue(MagicEffectProbe::DwData() == 0x44);
        Assert::IsTrue(MagicEffectProbe::Delay() == 0x48);
        Assert::IsTrue(MagicEffectProbe::RoleInfo() == 0x4C);
    }
};
