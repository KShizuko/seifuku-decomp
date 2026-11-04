#include "AlphaMask.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <malloc.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CAlphaMask) == 0x10, "");

struct AlphaMaskProbe : CAlphaMask
{
    static void Fill(CAlphaMask& mask, const BYTE* pData, int nBytes, int w, int h)
    {
        AlphaMaskProbe& p = static_cast<AlphaMaskProbe&>(mask);
        free(p.m_pBuffer);
        p.m_pBuffer = (BYTE*)malloc(nBytes);
        memcpy(p.m_pBuffer, pData, nBytes);
        p.m_sizeInfo.iWidth = w; p.m_sizeInfo.iHeight = h;
    }
    static size_t Buffer() { return offsetof(AlphaMaskProbe, m_pBuffer); }
    static size_t Size() { return offsetof(AlphaMaskProbe, m_sizeInfo); }
};

TEST_CLASS(GraphicAlphaMask)
{
public:
    TEST_METHOD(MembersFollowTheVtable)
    {
        Assert::IsTrue(AlphaMaskProbe::Buffer() == 0x4);
        Assert::IsTrue(AlphaMaskProbe::Size() == 0x8);
    }

    TEST_METHOD(LoadedBitsAnswerOpaqueAndTransparent)
    {
        BYTE data[2] = { 0x05, 0x01 };
        CAlphaMask mask;
        AlphaMaskProbe::Fill(mask, data, 2, 8, 2);
        CMyPos p;
        p.x = 0; p.y = 0; Assert::IsFalse(mask.IsTransparent(p) != FALSE);
        p.x = 1; p.y = 0; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
        p.x = 2; p.y = 0; Assert::IsFalse(mask.IsTransparent(p) != FALSE);
        p.x = 7; p.y = 0; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
        p.x = 0; p.y = 1; Assert::IsFalse(mask.IsTransparent(p) != FALSE);
        p.x = 1; p.y = 1; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
    }

    TEST_METHOD(OutOfBoundsIsTransparent)
    {
        BYTE data[8]; memset(data, 0xFF, sizeof(data));
        CAlphaMask mask;
        AlphaMaskProbe::Fill(mask, data, 8, 8, 8);
        CMyPos p;
        p.x = 3; p.y = 3; Assert::IsFalse(mask.IsTransparent(p) != FALSE);
        p.x = -1; p.y = 3; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
        p.x = 3; p.y = -1; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
        p.x = 8; p.y = 3; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
        p.x = 3; p.y = 8; Assert::IsTrue(mask.IsTransparent(p) != FALSE);
    }

    TEST_METHOD(NullNameRefused)
    {
        CMySize size; size.iWidth = 4; size.iHeight = 1;
        CAlphaMask mask;
        Assert::IsFalse(mask.Load(size, (char*) NULL) != FALSE);
    }
};
