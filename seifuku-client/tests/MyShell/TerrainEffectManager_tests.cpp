#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "TerrainEffectManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TerrainEffectUnitProbe : CTerainEffectUnit
{
    static size_t Units() { return offsetof(TerrainEffectUnitProbe, m_setUnit); }
    static size_t World() { return offsetof(TerrainEffectUnitProbe, m_posWorld); }
    static size_t TimeBegin() { return offsetof(TerrainEffectUnitProbe, m_dwTimeBegin); }
    static size_t Dir() { return offsetof(TerrainEffectUnitProbe, m_nDir); }
    static size_t Effects() { return offsetof(TerrainEffectUnitProbe, m_setMapEffect); }
};
struct TerrainEffectManagerProbe : CTerrainEffectManager
{
    static size_t Effects() { return offsetof(TerrainEffectManagerProbe, m_setEffect); }
};

static_assert(sizeof(TerrainEffectUnitInfo) == 0x90, "");
static_assert(offsetof(TerrainEffectUnitInfo, dwDelay) == 0x80, "");
static_assert(offsetof(TerrainEffectUnitInfo, nOffsetY) == 0x8C, "");
static_assert(sizeof(CTerainEffectUnit) == 2 * sizeof(VECTOR_TERRAINEFFECTUNIT) + 0x10, "");
static_assert(!std::is_polymorphic<CTerainEffectUnit>::value, "");
static_assert(sizeof(CTerrainEffectManager) == 0x4 + sizeof(VECTOR_TERRAINEFFECT), "");
static_assert(std::is_polymorphic<CTerrainEffectManager>::value, "");
static_assert(std::is_same<decltype(&CTerainEffectUnit::CreateNew), CTerainEffectUnit* (*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CTerainEffectUnit::Create), BOOL (CTerainEffectUnit::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CTerainEffectUnit::Rotate), void (CTerainEffectUnit::*)(CMyPos, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CTerainEffectUnit::Process), BOOL (CTerainEffectUnit::*)()>::value, "");

TEST_CLASS(ClientTerrainEffectHeader)
{
public:
    TEST_METHOD(TheUnitMembersFollowTheFirstVector)
    {
        const size_t v = sizeof(VECTOR_TERRAINEFFECTUNIT);
        Assert::IsTrue(TerrainEffectUnitProbe::Units() == 0x0);
        Assert::IsTrue(TerrainEffectUnitProbe::World() == v);
        Assert::IsTrue(TerrainEffectUnitProbe::TimeBegin() == v + 0x8);
        Assert::IsTrue(TerrainEffectUnitProbe::Dir() == v + 0xC);
        Assert::IsTrue(TerrainEffectUnitProbe::Effects() == v + 0x10);
    }
    TEST_METHOD(TheManagerVectorFollowsTheVtable)
    {
        Assert::IsTrue(TerrainEffectManagerProbe::Effects() == 0x4);
    }
};
