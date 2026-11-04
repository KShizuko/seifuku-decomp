#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "TerrainObjManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TerrainObjManagerProbe : CTerrainObjManager
{
    static size_t Infos() { return offsetof(TerrainObjManagerProbe, m_setTerrainObjInfo); }
};

typedef CTerrainObjManager::TerrainObjInfo Info;
static_assert(sizeof(Info) == 0x28, "");
static_assert(_MAX_NAMESIZE == 16, "");
static_assert(offsetof(Info, szName) == 0x0, "");
static_assert(offsetof(Info, nDir) == 0x10, "");
static_assert(offsetof(Info, nType) == 0x14, "");
static_assert(offsetof(Info, idNpc) == 0x18, "");
static_assert(offsetof(Info, pObj) == 0x1C, "");
static_assert(offsetof(Info, posCell) == 0x20, "");
static_assert(sizeof(CTerrainObjManager) == 0x4 + sizeof(vector<Info*>), "");
static_assert(std::is_polymorphic<CTerrainObjManager>::value, "");
static_assert(std::is_same<decltype(&CTerrainObjManager::AddNpc), void (CTerrainObjManager::*)(int, int, OBJID, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CTerrainObjManager::SetDir), void (CTerrainObjManager::*)(OBJID, int)>::value, "");
static_assert(std::is_same<decltype(&CTerrainObjManager::SetPos), void (CTerrainObjManager::*)(OBJID, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CTerrainObjManager::GetPos), void (CTerrainObjManager::*)(OBJID, CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&CTerrainObjManager::GetFocus), OBJID (CTerrainObjManager::*)()>::value, "");

TEST_CLASS(ClientTerrainObjManagerHeader)
{
public:
    TEST_METHOD(TheVectorFollowsTheVtable)
    {
        Assert::IsTrue(TerrainObjManagerProbe::Infos() == 0x4);
    }
};
