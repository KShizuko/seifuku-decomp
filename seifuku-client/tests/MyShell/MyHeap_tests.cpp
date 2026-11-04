#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MyHeap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MyHeapProbe
{
    static HANDLE Heap(const CMyHeap& h) { return h.m_hHeap; }
    static LONG Num(const CMyHeap& h) { return h.m_uNumAllocsInHeap; }
    static LONG Max(const CMyHeap& h) { return h.m_uMaxAllocsInHeap; }
    static size_t HeapOffset() { return offsetof(CMyHeap, m_hHeap); }
    static size_t NumOffset() { return offsetof(CMyHeap, m_uNumAllocsInHeap); }
    static size_t MaxOffset() { return offsetof(CMyHeap, m_uMaxAllocsInHeap); }
    static size_t NameOffset() { return offsetof(CMyHeap, m_strName); }
};

static_assert(std::has_virtual_destructor<CMyHeap>::value, "");

TEST_CLASS(ClientMyHeap)
{
public:
    TEST_METHOD(LayoutIsThreeWordsThenTheName)
    {
        Assert::IsTrue(MyHeapProbe::HeapOffset() == sizeof(void*));
        Assert::IsTrue(MyHeapProbe::NumOffset() == sizeof(void*) + 4);
        Assert::IsTrue(MyHeapProbe::MaxOffset() == sizeof(void*) + 8);
        Assert::IsTrue(MyHeapProbe::NameOffset() == sizeof(void*) + 12);
        Assert::IsTrue(sizeof(CMyHeap) == sizeof(void*) + 12 + sizeof(std::string));
    }

    TEST_METHOD(AllocCountsZeroesAndFreeCountsDown)
    {
        CMyHeap heap("MyHeapTest");
        Assert::IsTrue(MyHeapProbe::Heap(heap) != NULL);
        Assert::IsTrue(MyHeapProbe::Num(heap) == 0 && MyHeapProbe::Max(heap) == 0);

        Assert::IsTrue(heap.Alloc(0) == NULL);
        Assert::IsTrue(MyHeapProbe::Num(heap) == 0);

        unsigned char* p1 = (unsigned char*) heap.Alloc(64);
        unsigned char* p2 = (unsigned char*) heap.Alloc(64);
        Assert::IsTrue(p1 != NULL && p2 != NULL && p1 != p2);
        bool bZero = true;
        for (int i = 0; i < 64; i++) bZero = bZero && p1[i] == 0 && p2[i] == 0;
        Assert::IsTrue(bZero);
        Assert::IsTrue(MyHeapProbe::Num(heap) == 2 && MyHeapProbe::Max(heap) == 2);

        heap.Free(p1);
        Assert::IsTrue(MyHeapProbe::Num(heap) == 1 && MyHeapProbe::Max(heap) == 2);
        heap.Free(p2);
        Assert::IsTrue(MyHeapProbe::Num(heap) == 0 && MyHeapProbe::Max(heap) == 2);
    }
};
