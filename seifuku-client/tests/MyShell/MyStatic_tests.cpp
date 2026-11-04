#include "stdafx.h"
#include "MyShell.h"
#include "MyStatic.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyStatic) - sizeof(CStatic) >= 0xE4 && sizeof(CMyStatic) - sizeof(CStatic) <= 0xE8, "");
static_assert(std::is_base_of<CStatic, CMyStatic>::value, "");
static_assert(std::is_polymorphic<CMyStatic>::value, "");

struct MyStaticProbe
{
    static size_t IntPoint() { return offsetof(CMyStatic, m_IntPoint); }
    static size_t FontType() { return offsetof(CMyStatic, m_nFontType); }
    static size_t StaticWidth() { return offsetof(CMyStatic, m_nStaticWidth); }
    static size_t Color() { return offsetof(CMyStatic, m_dwFontColor); }
    static size_t PosType() { return offsetof(CMyStatic, m_nStaticPosType); }
    static size_t Type() { return offsetof(CMyStatic, m_btType); }
    static size_t Load() { return offsetof(CMyStatic, m_bImageLoad); }
    static size_t Id() { return offsetof(CMyStatic, m_strPictureID); }
    static size_t Num() { return offsetof(CMyStatic, m_nNum); }
    static size_t Light() { return offsetof(CMyStatic, m_nLightType); }
    static size_t Frame() { return offsetof(CMyStatic, m_uFrame); }
    static size_t BkOffset() { return offsetof(CMyStatic, m_ptBkOffset); }
    static size_t BkSrc() { return offsetof(CMyStatic, m_ptBkSrc); }
    static size_t BkWidth() { return offsetof(CMyStatic, m_nBkWidth); }
    static size_t BkHeight() { return offsetof(CMyStatic, m_nBkHeight); }
    static size_t BkAni() { return offsetof(CMyStatic, m_szBkAni); }
    static size_t FontName() { return offsetof(CMyStatic, m_szFontName); }
};

TEST_CLASS(ClientMyStaticHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereShowReadsThem)
    {
        const size_t b = sizeof(CStatic);
        Assert::IsTrue(MyStaticProbe::IntPoint() == b + 0x00);
        Assert::IsTrue(MyStaticProbe::FontType() == b + 0x08);
        Assert::IsTrue(MyStaticProbe::StaticWidth() == b + 0x0C);
        Assert::IsTrue(MyStaticProbe::Color() == b + 0x10);
        Assert::IsTrue(MyStaticProbe::PosType() == b + 0x14);
        Assert::IsTrue(MyStaticProbe::Type() == b + 0x18);
        Assert::IsTrue(MyStaticProbe::Load() == b + 0x1C);
        Assert::IsTrue(MyStaticProbe::Id() == b + 0x20);
        Assert::IsTrue(MyStaticProbe::Num() == b + 0x60);
        Assert::IsTrue(MyStaticProbe::Light() == b + 0x64);
        Assert::IsTrue(MyStaticProbe::Frame() == b + 0x68);
        Assert::IsTrue(MyStaticProbe::BkOffset() == b + 0x6C);
        Assert::IsTrue(MyStaticProbe::BkSrc() == b + 0x74);
        Assert::IsTrue(MyStaticProbe::BkWidth() == b + 0x7C);
        Assert::IsTrue(MyStaticProbe::BkHeight() == b + 0x80);
        Assert::IsTrue(MyStaticProbe::BkAni() == b + 0x84);
        Assert::IsTrue(MyStaticProbe::FontName() == b + 0xC4);
    }
};
