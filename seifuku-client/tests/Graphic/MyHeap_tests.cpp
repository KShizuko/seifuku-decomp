#include "CppUnitTest.h"
#include "MyHeap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct MyHeapProbe
{
    static HANDLE Heap(const CMyHeap& h) { return h.m_hHeap; }
    static LONG Num(const CMyHeap& h) { return h.m_uNumAllocsInHeap; }
    static LONG Max(const CMyHeap& h) { return h.m_uMaxAllocsInHeap; }
};

TEST_CLASS(GraphicMyHeap)
{
public:
    TEST_METHOD(AllocCountsZeroesAndFreeCountsDown)
    {
        {
            CMyHeap heap("GraphicMyHeapTest");
            Assert::IsTrue(MyHeapProbe::Heap(heap) != NULL);
            Assert::IsTrue(heap.Alloc(0) == NULL);
            Assert::IsTrue(MyHeapProbe::Num(heap) == 0 && MyHeapProbe::Max(heap) == 0);

            unsigned char* p1 = (unsigned char*) heap.Alloc(32);
            unsigned char* p2 = (unsigned char*) heap.Alloc(32);
            Assert::IsTrue(p1 != NULL && p2 != NULL && p1 != p2);
            bool bZero = true;
            for (int i = 0; i < 32; i++) bZero = bZero && p1[i] == 0 && p2[i] == 0;
            Assert::IsTrue(bZero);
            Assert::IsTrue(MyHeapProbe::Num(heap) == 2 && MyHeapProbe::Max(heap) == 2);
            Assert::AreEqual<LONG>(2, heap.GetAllocCount());

            heap.Free(p1);
            Assert::IsTrue(MyHeapProbe::Num(heap) == 1 && MyHeapProbe::Max(heap) == 2);
        }
        CMyHeap unnamed(NULL);
        void* p = unnamed.Alloc(8);
        Assert::IsNotNull(p);
        unnamed.Free(p);
        Assert::AreEqual<LONG>(0, unnamed.GetAllocCount());
    }
};
