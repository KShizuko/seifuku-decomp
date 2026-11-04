#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DMapEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapEffectProbe : C3DMapEffect
{
    static size_t World() { return offsetof(MapEffectProbe, m_posWorld); }
    static size_t Effect() { return offsetof(MapEffectProbe, m_objEffect); }
    static size_t DelSelf() { return offsetof(MapEffectProbe, m_bDelSelf); }
    static size_t Die() { return offsetof(MapEffectProbe, m_bDie); }
    static size_t OffsetY() { return offsetof(MapEffectProbe, m_nOffsetY); }
    static size_t Save() { return offsetof(MapEffectProbe, m_bSave); }
    static size_t Index() { return offsetof(MapEffectProbe, m_szIndex); }
    static size_t Horizontal() { return offsetof(MapEffectProbe, m_fHorizontal); }
    static size_t Vertical() { return offsetof(MapEffectProbe, m_fVertical); }
    static size_t Over() { return offsetof(MapEffectProbe, m_bOver); }
    static size_t Dir() { return offsetof(MapEffectProbe, m_nDir); }
};

static_assert(sizeof(C3DMapEffect) == 0x7C + sizeof(IGame3DEffect*), "");
static_assert(std::is_base_of<C3DMapObj, C3DMapEffect>::value, "");
static_assert(std::is_polymorphic<C3DMapEffect>::value, "");
static_assert(MAP_3DEFFECT == 10, "");
static_assert(std::is_same<decltype(&C3DMapEffect::s_bStroke), BOOL*>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::CreateNew), C3DMapEffect* (*)(CMyPos, char*, BOOL, BOOL, int)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::Create), BOOL (C3DMapEffect::*)(CMyPos, char*, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::SetRotate), void (C3DMapEffect::*)(float, float)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::SetWorldPos), void (C3DMapEffect::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::LoadDataObj), void (C3DMapEffect::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::LoadTextObj), void (C3DMapEffect::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::SaveDataObj), void (C3DMapEffect::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::SaveTextObj), void (C3DMapEffect::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C3DMapEffect::SetDir), void (C3DMapEffect::*)(int)>::value, "");

TEST_CLASS(Client3DMapEffectHeader)
{
public:
    TEST_METHOD(TheWorldPosFollowsTheBaseAndTheEffectComesLast)
    {
        Assert::IsTrue(MapEffectProbe::World() == 0x14);
        Assert::IsTrue(MapEffectProbe::Effect() == 0x7C);
    }
    TEST_METHOD(TheTailKeepsItsOffsets)
    {
        Assert::IsTrue(MapEffectProbe::DelSelf() == 0x1C);
        Assert::IsTrue(MapEffectProbe::Die() == 0x20);
        Assert::IsTrue(MapEffectProbe::OffsetY() == 0x24);
        Assert::IsTrue(MapEffectProbe::Save() == 0x28);
        Assert::IsTrue(MapEffectProbe::Index() == 0x2C);
        Assert::IsTrue(MapEffectProbe::Horizontal() == 0x6C);
        Assert::IsTrue(MapEffectProbe::Vertical() == 0x70);
        Assert::IsTrue(MapEffectProbe::Over() == 0x74);
        Assert::IsTrue(MapEffectProbe::Dir() == 0x78);
    }
};
