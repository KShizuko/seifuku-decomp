#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "c3_key.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Frame) == 0x10, "");
static_assert(offsetof(C3Frame, nFrame) == 0x0 && offsetof(C3Frame, fParam) == 0x4 && offsetof(C3Frame, bParam) == 0x8 && offsetof(C3Frame, nParam) == 0xC, "");
static_assert(sizeof(C3Key) == 0x18, "");
static_assert(offsetof(C3Key, dwAlphas) == 0x0 && offsetof(C3Key, lpAlphas) == 0x4, "");
static_assert(offsetof(C3Key, dwDraws) == 0x8 && offsetof(C3Key, lpDraws) == 0xC, "");
static_assert(offsetof(C3Key, dwChangeTexs) == 0x10 && offsetof(C3Key, lpChangeTexs) == 0x14, "");

static_assert(std::is_same<decltype(&Key_ProcessAlpha), BOOL (*)(C3Key*, DWORD, DWORD, float*)>::value, "");
static_assert(std::is_same<decltype(&Key_ProcessDraw), BOOL (*)(C3Key*, DWORD, BOOL*)>::value, "");
static_assert(std::is_same<decltype(&Key_ProcessChangeTex), BOOL (*)(C3Key*, DWORD, int*)>::value, "");
static_assert(std::is_same<decltype(&Key_Clear), void (*)(C3Key*)>::value, "");

TEST_CLASS(C3KeyHeader)
{
public:
    TEST_METHOD(FrameArraysIndexBySixteenBytes)
    {
        C3Frame frames[2] = { { 0, { 0.0f }, { 0 }, { 0 } }, { 10, { 1.0f }, { 1 }, { 7 } } };
        Assert::AreEqual<size_t>(0x14, (size_t) ((char*) &frames[1].fParam[0] - (char*) frames));
        Assert::AreEqual(7, frames[1].nParam[0]);
    }
};

TEST_CLASS(C3KeyFunctions)
{
    static C3Key MakeKey(C3Frame* alphas, DWORD nAlphas, C3Frame* draws, DWORD nDraws, C3Frame* tex, DWORD nTex)
    {
        C3Key k;
        k.dwAlphas = nAlphas; k.lpAlphas = alphas;
        k.dwDraws = nDraws; k.lpDraws = draws;
        k.dwChangeTexs = nTex; k.lpChangeTexs = tex;
        return k;
    }

public:
    TEST_METHOD(ClearZeroesEverything)
    {
        C3Key k;
        memset(&k, 0xCD, sizeof(k));
        Key_Clear(&k);
        const DWORD* w = reinterpret_cast<const DWORD*>(&k);
        for (int i = 0; i < 6; i++)
            Assert::AreEqual<DWORD>(0, w[i]);
    }

    TEST_METHOD(AlphaInterpolatesBetweenTheNeighbouringKeys)
    {
        C3Frame a[3] = { { 10, { 0.0f } }, { 20, { 1.0f } }, { 40, { 0.5f } } };
        C3Key k = MakeKey(a, 3, 0, 0, 0, 0);
        float f = -1.0f;
        Assert::IsTrue(Key_ProcessAlpha(&k, 15, 0, &f));
        Assert::AreEqual(0.5f, f);
        Assert::IsTrue(Key_ProcessAlpha(&k, 30, 0, &f));
        Assert::AreEqual(0.75f, f);
        Assert::IsTrue(Key_ProcessAlpha(&k, 20, 0, &f));
        Assert::AreEqual(1.0f, f);
    }

    TEST_METHOD(AlphaClampsOutsideTheKeyRange)
    {
        C3Frame a[2] = { { 10, { 0.25f } }, { 20, { 1.0f } } };
        C3Key k = MakeKey(a, 2, 0, 0, 0, 0);
        float f = -1.0f;
        Assert::IsTrue(Key_ProcessAlpha(&k, 3, 0, &f));
        Assert::AreEqual(0.25f, f);
        Assert::IsTrue(Key_ProcessAlpha(&k, 99, 0, &f));
        Assert::AreEqual(1.0f, f);
        C3Key empty = MakeKey(0, 0, 0, 0, 0, 0);
        f = -1.0f;
        Assert::IsFalse(Key_ProcessAlpha(&empty, 5, 0, &f));
        Assert::AreEqual(-1.0f, f);
    }

    TEST_METHOD(DrawAndChangeTexMatchExactFramesOnly)
    {
        C3Frame d[2] = { { 3, { 0 }, { TRUE } }, { 7, { 0 }, { FALSE } } };
        C3Frame t[1] = { { 5, { 0 }, { 0 }, { 42 } } };
        C3Key k = MakeKey(0, 0, d, 2, t, 1);
        BOOL b = 99; int n = 99;
        Assert::IsTrue(Key_ProcessDraw(&k, 7, &b));
        Assert::AreEqual<int>(FALSE, b);
        Assert::IsFalse(Key_ProcessDraw(&k, 4, &b));
        Assert::AreEqual<int>(FALSE, b);
        Assert::IsTrue(Key_ProcessChangeTex(&k, 5, &n));
        Assert::AreEqual(42, n);
        Assert::IsFalse(Key_ProcessChangeTex(&k, 6, &n));
        Assert::AreEqual(42, n);

        C3Key e = MakeKey(0, 0, 0, 2, 0, 0);
        b = 99;
        Assert::IsFalse(Key_ProcessDraw(&e, 3, &b));
        Assert::AreEqual<int>(99, b);
    }

    TEST_METHOD(ChangeTexHoldsThePreviousKeyBelowALaterKey)
    {
        C3Frame t[2] = { { 3, { 0 }, { 0 }, { 10 } }, { 7, { 0 }, { 0 }, { 20 } } };
        C3Key k = MakeKey(0, 0, 0, 0, t, 2);
        int n = 99;
        Assert::IsTrue(Key_ProcessChangeTex(&k, 5, &n));
        Assert::AreEqual(10, n);
        Assert::IsTrue(Key_ProcessChangeTex(&k, 7, &n));
        Assert::AreEqual(20, n);
        Assert::IsTrue(Key_ProcessChangeTex(&k, 2, &n));
        Assert::AreEqual(10, n);
        n = 99;
        Assert::IsFalse(Key_ProcessChangeTex(&k, 8, &n));
        Assert::AreEqual(99, n);
    }
};
