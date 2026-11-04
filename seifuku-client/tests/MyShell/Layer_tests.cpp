#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Layer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(LayerInfo) == 0xC, "");
static_assert(offsetof(LayerInfo, usTerrain) == 0 && offsetof(LayerInfo, usMask) == 2 && offsetof(LayerInfo, sAltitude) == 4 && offsetof(LayerInfo, pLayer) == 8, "");
static_assert(sizeof(IndexLayerInfo) == 6, "");
static_assert(sizeof(CLayer) == 0xC, "");
static_assert(offsetof(CLayer, usTerrain) == 4 && offsetof(CLayer, usMask) == 6 && offsetof(CLayer, sAltitude) == 8, "");
static_assert(sizeof(CellInfo) == 0x10, "");
static_assert(offsetof(CellInfo, m_infoLayer) == 0 && offsetof(CellInfo, m_bSearched) == 0xC, "");
static_assert(std::is_polymorphic<CLayer>::value, "");
static_assert(std::is_same<decltype(&CLayer::s_hHeap), HANDLE*>::value && std::is_same<decltype(&CLayer::s_uNumAllocsInHeap), UINT*>::value, "");
static_assert(std::is_same<decltype(&CLayer::IsValidPt), BOOL (*)(void*)>::value, "");

TEST_CLASS(ClientLayerHeader)
{
public:
    TEST_METHOD(CellHoldsTheLayerThenTheFlag)
    {
        Assert::IsTrue(offsetof(CellInfo, m_bSearched) == 0xC && sizeof(CellInfo) == 0x10);
    }
};

TEST_CLASS(ClientLayerBodies)
{
public:
    TEST_METHOD(NewCreatesTheHeapAndDeleteDestroysItAtZero)
    {
        Assert::IsTrue(CLayer::s_hHeap == NULL && CLayer::s_uNumAllocsInHeap == 0);
        CLayer* a = new CLayer;
        Assert::IsTrue(a != NULL && CLayer::s_hHeap != NULL && CLayer::s_uNumAllocsInHeap == 1);
        CLayer* b = new CLayer;
        Assert::IsTrue(b != NULL && CLayer::s_uNumAllocsInHeap == 2);
        delete a;
        Assert::IsTrue(CLayer::s_hHeap != NULL && CLayer::s_uNumAllocsInHeap == 1);
        delete b;
        Assert::IsTrue(CLayer::s_hHeap == NULL && CLayer::s_uNumAllocsInHeap == 0);
        CLayer::operator delete(NULL);
        Assert::IsTrue(CLayer::s_hHeap == NULL && CLayer::s_uNumAllocsInHeap == 0);
    }
    TEST_METHOD(IsValidPtKnowsTheHeapsOwnBlocks)
    {
        Assert::IsTrue(CLayer::s_hHeap == NULL);
        Assert::IsFalse(CLayer::IsValidPt(NULL) != 0);
        CLayer* p = new CLayer;
        Assert::IsTrue(CLayer::IsValidPt(p) != 0);
        Assert::IsFalse(CLayer::IsValidPt(NULL) != 0);
        Assert::IsFalse(CLayer::IsValidPt((void*)0x1000) != 0);
        delete p;
        Assert::IsFalse(CLayer::IsValidPt(p) != 0);
    }
    TEST_METHOD(CtorZeroesTheThreeFields)
    {
        CLayer* p = new CLayer;
        Assert::IsTrue(p->usTerrain == 0 && p->usMask == 0 && p->sAltitude == 0);
        p->usTerrain = 7; p->usMask = 3; p->sAltitude = -5;
        delete p;
        Assert::IsTrue(CLayer::s_hHeap == NULL);
    }
};
