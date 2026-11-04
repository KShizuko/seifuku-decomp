#include "stdafx.h"
#include "MyShell.h"
#include "FontScale.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CFontScale) == 0x50 + sizeof(std::map<HWND, int>), "");
static_assert(std::is_polymorphic<CFontScale>::value, "");
static_assert(offsetof(CFontScale, m_FontCell) == 0x04, "");
static_assert(offsetof(CFontScale, m_bScale) == 0x3C, "");
static_assert(offsetof(CFontScale, m_nUnused) == 0x40, "");
static_assert(offsetof(CFontScale, m_fScaleX) == 0x44, "");
static_assert(offsetof(CFontScale, m_fScaleY) == 0x48, "");
static_assert(offsetof(CFontScale, m_pIni) == 0x4C, "");
static_assert(offsetof(CFontScale, m_mapWndID) == 0x50, "");
static_assert(std::is_same<decltype(&CFontScale::Adjust), void (CFontScale::*)(CWnd*, int)>::value, "");

TEST_CLASS(ClientFontScaleHeader)
{
public:
    TEST_METHOD(TheConstructorFillsSimSunEightToFourteen)
    {
        CFontScale s;
        Assert::AreEqual(6, s.m_FontCell[0].nWidth);   Assert::AreEqual(11, s.m_FontCell[0].nHeight);
        Assert::AreEqual(6, s.m_FontCell[1].nWidth);   Assert::AreEqual(12, s.m_FontCell[1].nHeight);
        Assert::AreEqual(7, s.m_FontCell[2].nWidth);   Assert::AreEqual(13, s.m_FontCell[2].nHeight);
        Assert::AreEqual(8, s.m_FontCell[3].nWidth);   Assert::AreEqual(15, s.m_FontCell[3].nHeight);
        Assert::AreEqual(8, s.m_FontCell[4].nWidth);   Assert::AreEqual(16, s.m_FontCell[4].nHeight);
        Assert::AreEqual(10, s.m_FontCell[5].nWidth);  Assert::AreEqual(19, s.m_FontCell[5].nHeight);
        Assert::AreEqual(11, s.m_FontCell[6].nWidth);  Assert::AreEqual(21, s.m_FontCell[6].nHeight);
        Assert::IsFalse(s.m_bScale != 0);
        Assert::AreEqual(0, s.m_nUnused);
        Assert::AreEqual(0.0f, s.m_fScaleX);
        Assert::AreEqual(0.0f, s.m_fScaleY);
        Assert::IsNull(s.m_pIni);
        Assert::IsTrue(s.m_mapWndID.empty());
    }

    TEST_METHOD(AdjustDoesNothingWithoutAWindow)
    {
        CFontScale s;
        s.Adjust(NULL);
        s.Adjust(NULL, 5);
        Assert::IsFalse(s.m_bScale != 0);
        Assert::IsTrue(s.m_mapWndID.empty());
    }

    TEST_METHOD(SetReservedWritesTheWord)
    {
        CFontScale s;
        s.SetReserved(7);
        Assert::AreEqual(7, s.m_nUnused);
    }
};
