#include "stdafx.h"
#include "MyShell.h"
#include "MyImage.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyImage) == (sizeof(CButton) + 0xE4 + __alignof(CButton) - 1) / __alignof(CButton) * __alignof(CButton), "");
static_assert(std::is_base_of<CButton, CMyImage>::value, "");
static_assert(std::is_polymorphic<CMyImage>::value, "");

struct MyImageProbe
{
    static size_t MouseMove() { return offsetof(CMyImage, m_bMouseMove); }
    static size_t IntPoint() { return offsetof(CMyImage, m_IntPoint); }
    static size_t Point() { return offsetof(CMyImage, m_Point); }
    static size_t Load() { return offsetof(CMyImage, m_bImageLoad); }
    static size_t Frame() { return offsetof(CMyImage, m_uFrame); }
    static size_t Id() { return offsetof(CMyImage, m_strImageID); }
    static size_t DwId() { return offsetof(CMyImage, m_dwImageID); }
    static size_t Width() { return offsetof(CMyImage, m_nWidth); }
    static size_t Height() { return offsetof(CMyImage, m_nHeight); }
    static size_t Rect() { return offsetof(CMyImage, m_ShowRect); }
    static size_t Type() { return offsetof(CMyImage, m_btItemType); }
    static size_t Ani() { return offsetof(CMyImage, m_strAni); }
    static size_t ItemPos() { return offsetof(CMyImage, m_nItemPos); }
    static size_t TypeID() { return offsetof(CMyImage, m_nImageTypeID); }
    static size_t Act() { return offsetof(CMyImage, m_btActType); }
    static size_t ActTime() { return offsetof(CMyImage, m_dwActTime); }
    static size_t Zoom() { return offsetof(CMyImage, m_bImgZoom); }
    static size_t Money() { return offsetof(CMyImage, m_nCurMoney); }
    static size_t Mil() { return offsetof(CMyImage, m_btMilNum); }
    static size_t Tho() { return offsetof(CMyImage, m_btThoNum); }
    static size_t Hun() { return offsetof(CMyImage, m_btHunNum); }
    static size_t Level() { return offsetof(CMyImage, m_nWeaponLevel); }
    static size_t Gold() { return offsetof(CMyImage, m_btGoldStar); }
    static size_t Siller() { return offsetof(CMyImage, m_btSillerStar); }
    static size_t Copper() { return offsetof(CMyImage, m_btCopperStar); }
    static size_t SetCursor() { return offsetof(CMyImage, m_bSetCursor); }
};

TEST_CLASS(ClientMyImageHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheBodiesReadThem)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(MyImageProbe::MouseMove() == b + 0x00);
        Assert::IsTrue(MyImageProbe::IntPoint() == b + 0x04);
        Assert::IsTrue(MyImageProbe::Point() == b + 0x0C);
        Assert::IsTrue(MyImageProbe::Load() == b + 0x14);
        Assert::IsTrue(MyImageProbe::Frame() == b + 0x18);
        Assert::IsTrue(MyImageProbe::Id() == b + 0x1C);
        Assert::IsTrue(MyImageProbe::DwId() == b + 0x5C);
        Assert::IsTrue(MyImageProbe::Width() == b + 0x60);
        Assert::IsTrue(MyImageProbe::Height() == b + 0x64);
        Assert::IsTrue(MyImageProbe::Rect() == b + 0x68);
        Assert::IsTrue(MyImageProbe::Type() == b + 0x78);
        Assert::IsTrue(MyImageProbe::Ani() == b + 0x7C);
        Assert::IsTrue(MyImageProbe::ItemPos() == b + 0xBC);
        Assert::IsTrue(MyImageProbe::TypeID() == b + 0xC0);
        Assert::IsTrue(MyImageProbe::Act() == b + 0xC4);
        Assert::IsTrue(MyImageProbe::ActTime() == b + 0xC8);
        Assert::IsTrue(MyImageProbe::Zoom() == b + 0xCC);
        Assert::IsTrue(MyImageProbe::Money() == b + 0xD0);
        Assert::IsTrue(MyImageProbe::Mil() == b + 0xD4);
        Assert::IsTrue(MyImageProbe::Tho() == b + 0xD5);
        Assert::IsTrue(MyImageProbe::Hun() == b + 0xD6);
        Assert::IsTrue(MyImageProbe::Level() == b + 0xD8);
        Assert::IsTrue(MyImageProbe::Gold() == b + 0xDC);
        Assert::IsTrue(MyImageProbe::Siller() == b + 0xDD);
        Assert::IsTrue(MyImageProbe::Copper() == b + 0xDE);
        Assert::IsTrue(MyImageProbe::SetCursor() == b + 0xE0);
    }
};
