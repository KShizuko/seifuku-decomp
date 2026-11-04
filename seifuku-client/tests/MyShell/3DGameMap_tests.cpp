#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DGameMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

#define O(m) offsetof(GameMapProbe, m)
#define S(m) sizeof(((GameMapProbe*)0)->m)

struct GameMapProbe : CGameMap
{
    static void Check()
    {
        Assert::IsTrue(O(m_setMapLayer) == 0x4);
        Assert::IsTrue(O(m_sizeMap) == O(m_setMapLayer) + S(m_setMapLayer));
        Assert::IsTrue(O(m_szieWorld) == O(m_sizeMap) + 0x8);
        Assert::IsTrue(O(m_bCreate) == O(m_szieWorld) + 0x8);
        Assert::IsTrue(O(m_posOrigin) == O(m_bCreate) + 0x4);
        Assert::IsTrue(O(m_posView) == O(m_posOrigin) + 0x8);
        Assert::IsTrue(O(m_pCellInfo) == O(m_posView) + 0x8);
        Assert::IsTrue(O(m_posViewOffset) == O(m_pCellInfo) + 0x4);
        Assert::IsTrue(O(m_dwVersion) == O(m_posViewOffset) + 0x8);
        Assert::IsTrue(O(m_setNode) == O(m_dwVersion) + 0x4);
        Assert::IsTrue(O(m_nNodeIndex) == O(m_setNode) + S(m_setNode));
        Assert::IsTrue(O(m_setOpen) == O(m_nNodeIndex) + 0x4);
        Assert::IsTrue(O(m_setClose) == O(m_setOpen) + S(m_setOpen));
        Assert::IsTrue(O(m_nCloseNodeAmount) == O(m_setClose) + S(m_setClose));
        Assert::IsTrue(O(m_setPath) == O(m_nCloseNodeAmount) + 0x4);
        Assert::IsTrue(O(m_posStart) == O(m_setPath) + S(m_setPath));
        Assert::IsTrue(O(m_posTarget) == O(m_posStart) + 0x8);
        Assert::IsTrue(O(m_posRealTarget) == O(m_posTarget) + 0x8);
        Assert::IsTrue(O(m_setCellAStar) == O(m_posRealTarget) + 0x8);
        Assert::IsTrue(O(m_nPathMode) == O(m_setCellAStar) + S(m_setCellAStar));
        Assert::IsTrue(O(m_nLeaveDisc) == O(m_nPathMode) + 0x4);
        Assert::IsTrue(O(m_bShowMapObjInfo) == O(m_nLeaveDisc) + 0x4);
        Assert::IsTrue(O(m_objItemManager) == O(m_bShowMapObjInfo) + 0x4);
        Assert::IsTrue(O(m_objTerrainObjManager) == O(m_objItemManager) + S(m_objItemManager));
        Assert::IsTrue(O(m_objMagicEffectManager) == O(m_objTerrainObjManager) + S(m_objTerrainObjManager));
        Assert::IsTrue(O(m_objTerrainEffectManager) == O(m_objMagicEffectManager) + S(m_objMagicEffectManager));
        Assert::IsTrue(O(m_idMap) == O(m_objTerrainEffectManager) + S(m_objTerrainEffectManager));
        Assert::IsTrue(O(m_idDoc) == O(m_idMap) + 0x4);
        Assert::IsTrue(O(m_dwType) == O(m_idDoc) + 0x4);
        Assert::IsTrue(O(m_bShowMiniMap) == O(m_dwType) + 0x4);
        Assert::IsTrue(O(m_objRadar) == O(m_bShowMiniMap) + 0x4);
        Assert::IsTrue(O(m_dwMapARGB) == O(m_objRadar) + S(m_objRadar));
        Assert::IsTrue(O(m_nScale) == O(m_dwMapARGB) + 0x4);
        Assert::IsTrue(O(m_nRealScreenWidth) == O(m_nScale) + 0x4);
        Assert::IsTrue(O(m_nRealScreenHeight) == O(m_nRealScreenWidth) + 0x4);
        Assert::IsTrue(O(m_nZoom) == O(m_nRealScreenHeight) + 0x4);
        Assert::IsTrue(O(m_szMediaRegion) == O(m_nZoom) + 0x4);
        Assert::IsTrue(O(m_szRegionMedia) == O(m_szMediaRegion) + 0x40);
        Assert::IsTrue(O(m_setRegionData) == O(m_szRegionMedia) + 0x100);
        Assert::IsTrue(O(m_bRegionNameChangeCount) == O(m_setRegionData) + S(m_setRegionData));
        Assert::IsTrue(O(m_szRegionName) == O(m_bRegionNameChangeCount) + 0x4);
        Assert::IsTrue(O(m_szRegionEffect) == O(m_szRegionName) + 0x40);
        Assert::IsTrue(O(m_dwRegionEffectBegin) == O(m_szRegionEffect) + 0x40);
        Assert::IsTrue(O(m_bShowRegionEffect) == O(m_dwRegionEffectBegin) + 0x4);
        Assert::IsTrue(O(m_bShowGobalRegionEffect) == O(m_bShowRegionEffect) + 0x4);
        Assert::IsTrue(O(m_dwRegionNameChangeBegin) == O(m_bShowGobalRegionEffect) + 0x4);
        Assert::IsTrue(O(m_bShowExp) == O(m_dwRegionNameChangeBegin) + 0x4);
        Assert::IsTrue(O(m_dwOldExp) == O(m_bShowExp) + 0x4);
        Assert::IsTrue(O(m_dwTimeExpBegin) == O(m_dwOldExp) + 0x4);
        Assert::IsTrue(O(m_dwShowExp) == O(m_dwTimeExpBegin) + 0x4);
        Assert::IsTrue(O(m_objRegionEffect) == O(m_dwShowExp) + 0x4);
        Assert::IsTrue(O(m_setIndexLayerInfo) == O(m_objRegionEffect) + S(m_objRegionEffect));
        Assert::IsTrue(O(m_dwBeginShowFriendPos) == O(m_setIndexLayerInfo) + S(m_setIndexLayerInfo));
        Assert::IsTrue(O(m_posFriend) == O(m_dwBeginShowFriendPos) + 0x4);
        Assert::IsTrue(O(ShakeEffectInfo) == O(m_posFriend) + 0x8);
        Assert::IsTrue(O(ScaleEffectInfo) == O(ShakeEffectInfo) + 0x14);
        Assert::IsTrue(O(ColorEffectInfo) == O(ScaleEffectInfo) + 0x18);
        Assert::IsTrue(O(m_InfoViewManager) == O(ColorEffectInfo) + 0x20);
        Assert::IsTrue(sizeof(CGameMap) == O(m_InfoViewManager) + S(m_InfoViewManager));
    }
};

static_assert(sizeof(NodeInfo) == 0x20, "");
static_assert(sizeof(IndexLayerInfo) == 6, "");
static_assert(std::is_polymorphic<CGameMap>::value, "");
static_assert(_NORMAL_SCALE == 256, "");
static_assert(_CELL_WIDTH == 64 && _CELL_HEIGHT == 32, "");
static_assert(std::is_same<decltype(&CGameMap::SetViewPos), void (CGameMap::*)(CMyPos, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::IsPosVisible), BOOL (CGameMap::*)(CMyPos, int)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::GetScale), int (CGameMap::*)()>::value, "");
static_assert(std::is_same<decltype(&CGameMap::SaveTextMap), void (CGameMap::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::SaveDataMap), void (CGameMap::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::LoadTextMap), BOOL (CGameMap::*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::Add3DMapEffect), C3DMapEffect* (CGameMap::*)(CMyPos, char*, BOOL, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CGameMap::*)(CPath&, CMyPos, CMyPos)>(&CGameMap::FindPath)), void (CGameMap::*)(CPath&, CMyPos, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::TestPath), BOOL (CGameMap::*)(CMapObj*, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::DDALine), void (CGameMap::*)(int, int, int, int, vector<POINT>&)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::GetDirection), int (*)(int, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&CGameMap::GetLastLayerInfo), LayerInfo* (*)(CellInfo*)>::value, "");

TEST_CLASS(ClientGameMapHeader)
{
public:
    TEST_METHOD(MembersFollowTheReferenceOrderWithoutTheAbsentOnes)
    {
        GameMapProbe::Check();
    }
};
