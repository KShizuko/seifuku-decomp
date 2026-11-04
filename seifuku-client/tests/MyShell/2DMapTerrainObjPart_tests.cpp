#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapTerrainObjPart.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TerrainObjPartProbe : C2DMapTerrainObjPart
{
    static size_t Offset() { return offsetof(TerrainObjPartProbe, m_posOffset); }
    static size_t Base() { return offsetof(TerrainObjPartProbe, m_sizeBase); }
    static size_t Thickness() { return offsetof(TerrainObjPartProbe, m_nThickness); }
    static size_t AniTitle() { return offsetof(TerrainObjPartProbe, m_szAniTitle); }
    static size_t AniFile() { return offsetof(TerrainObjPartProbe, m_szAniFile); }
    static size_t FrameInterval() { return offsetof(TerrainObjPartProbe, m_dwFrameInterval); }
    static size_t Height() { return offsetof(TerrainObjPartProbe, m_nHeight); }
    static size_t SceneOffset() { return offsetof(TerrainObjPartProbe, m_posSceneOffset); }
    static size_t Show() { return offsetof(TerrainObjPartProbe, m_posShow); }
    static size_t Layers() { return offsetof(TerrainObjPartProbe, m_setLayer); }
    static size_t Index() { return offsetof(TerrainObjPartProbe, m_nIndex); }
    static size_t PreFrame() { return offsetof(TerrainObjPartProbe, m_dwPreFrame); }
    static size_t Objs() { return offsetof(TerrainObjPartProbe, m_setObj); }
};

static_assert(sizeof(C2DMapTerrainObjPart) == sizeof(C2DMapObj) + 0x16C + sizeof(DEQUE_LAYER) + 8 + sizeof(DEQUE_MAPOBJ), "");
static_assert(std::is_polymorphic<C2DMapTerrainObjPart>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::s_bStroke), BOOL*>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::Load), void (C2DMapTerrainObjPart::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::GetLayer), CLayer* (C2DMapTerrainObjPart::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::GetObjAmount), int (C2DMapTerrainObjPart::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::ClearObj), void (C2DMapTerrainObjPart::*)()>::value, "");
static_assert(std::is_same<decltype(&C2DMapTerrainObjPart::GetMapObjByIndex), CMapObj* (C2DMapTerrainObjPart::*)(int)>::value, "");

TEST_CLASS(ClientTerrainObjPartHeader)
{
public:
    TEST_METHOD(FixedMembersFollowTheBase)
    {
        Assert::IsTrue(TerrainObjPartProbe::Offset() == sizeof(C2DMapObj));
        Assert::IsTrue(TerrainObjPartProbe::Base() == sizeof(C2DMapObj) + 0x8);
        Assert::IsTrue(TerrainObjPartProbe::Thickness() == sizeof(C2DMapObj) + 0x10);
        Assert::IsTrue(TerrainObjPartProbe::AniTitle() == sizeof(C2DMapObj) + 0x14);
        Assert::IsTrue(TerrainObjPartProbe::AniFile() == sizeof(C2DMapObj) + 0x54);
        Assert::IsTrue(TerrainObjPartProbe::FrameInterval() == sizeof(C2DMapObj) + 0x154);
        Assert::IsTrue(TerrainObjPartProbe::Height() == sizeof(C2DMapObj) + 0x158);
        Assert::IsTrue(TerrainObjPartProbe::SceneOffset() == sizeof(C2DMapObj) + 0x15C);
        Assert::IsTrue(TerrainObjPartProbe::Show() == sizeof(C2DMapObj) + 0x164);
        Assert::IsTrue(TerrainObjPartProbe::Layers() == sizeof(C2DMapObj) + 0x16C);
    }
    TEST_METHOD(TheTailFollowsTheLayerDeque)
    {
        const size_t d = sizeof(DEQUE_LAYER);
        Assert::IsTrue(TerrainObjPartProbe::Index() == sizeof(C2DMapObj) + 0x16C + d);
        Assert::IsTrue(TerrainObjPartProbe::PreFrame() == sizeof(C2DMapObj) + 0x170 + d);
        Assert::IsTrue(TerrainObjPartProbe::Objs() == sizeof(C2DMapObj) + 0x174 + d);
    }
};
