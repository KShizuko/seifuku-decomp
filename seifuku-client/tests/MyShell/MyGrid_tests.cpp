#include "stdafx.h"
#include "MyShell.h"
#include "MyGrid.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyGrid) == (sizeof(CButton) + 0x1A74 + __alignof(CButton) - 1) / __alignof(CButton) * __alignof(CButton), "");
static_assert(std::is_base_of<CButton, CMyGrid>::value, "");
static_assert(std::is_polymorphic<CMyGrid>::value, "");

struct MyGridProbe
{
    static size_t Par() { return offsetof(CMyGrid, m_GriPar); }
    static size_t Line() { return offsetof(CMyGrid, m_GriPar.m_btGridLine); }
    static size_t Row() { return offsetof(CMyGrid, m_GriPar.m_btGridRow); }
    static size_t Height() { return offsetof(CMyGrid, m_GriPar.m_btGridHeight); }
    static size_t Width() { return offsetof(CMyGrid, m_GriPar.m_btGridWidth); }
    static size_t PickUp() { return offsetof(CMyGrid, m_GriPar.m_bPickUp); }
    static size_t MousePt() { return offsetof(CMyGrid, m_GriPar.m_PickUpMousePoint); }
    static size_t PickLandR() { return offsetof(CMyGrid, m_GriPar.m_PickUpIconLandR); }
    static size_t MouseMove() { return offsetof(CMyGrid, m_GriPar.m_bMouseMove); }
    static size_t MoveLandR() { return offsetof(CMyGrid, m_GriPar.m_MouseMoveIconLandR); }
    static size_t Owner() { return offsetof(CMyGrid, m_GriPar.m_btOwnerID); }
    static size_t Inset() { return offsetof(CMyGrid, m_GriPar.m_InsetLandR); }
    static size_t TypeID() { return offsetof(CMyGrid, m_GriPar.m_IconTypeID); }
    static size_t RealID() { return offsetof(CMyGrid, m_GriPar.m_IconRealID); }
    static size_t Able() { return offsetof(CMyGrid, m_GriPar.m_bGridAble); }
    static size_t Index() { return offsetof(CMyGrid, m_GriPar.m_IconIndex); }
    static size_t PickTypeID() { return offsetof(CMyGrid, m_GriPar.m_PickUpIconTypeID); }
    static size_t PickType() { return offsetof(CMyGrid, m_GriPar.m_btPickUpIconType); }
    static size_t PickIndex() { return offsetof(CMyGrid, m_GriPar.m_dwPickUpIconIndex); }
    static size_t PickAct() { return offsetof(CMyGrid, m_GriPar.m_btPickUpIconActType); }
    static size_t MoveIndex() { return offsetof(CMyGrid, m_GriPar.m_dwMouseMoveIconIndex); }
    static size_t MoveTypeID() { return offsetof(CMyGrid, m_GriPar.m_dwMouseMoveIconTypeID); }
    static size_t AbleNum() { return offsetof(CMyGrid, m_GriPar.m_nGridAbleNum); }
    static size_t Enable() { return offsetof(CMyGrid, m_GriPar.m_bEnable); }
    static size_t PickEnable() { return offsetof(CMyGrid, m_GriPar.m_bPickEnable); }
    static size_t ClientRect() { return offsetof(CMyGrid, m_GriPar.m_ClientRect); }
    static size_t Num() { return offsetof(CMyGrid, m_GriPar.m_IconNum); }
    static size_t Type() { return offsetof(CMyGrid, m_GriPar.m_btIconType); }
    static size_t Act() { return offsetof(CMyGrid, m_GriPar.m_btIconActType); }
    static size_t ActTime() { return offsetof(CMyGrid, m_GriPar.m_dwIconActTime); }
    static size_t ParSize() { return sizeof(CMyGrid::MyGridParameter); }
};

TEST_CLASS(ClientMyGridHeader)
{
public:
    TEST_METHOD(TheRecordSitsWhereTheBodiesIndexIt)
    {
        const size_t b = sizeof(CButton);
        Assert::IsTrue(MyGridProbe::Par() == b);
        Assert::IsTrue(MyGridProbe::ParSize() == 0x1A74);
        Assert::IsTrue(MyGridProbe::Line() == b + 0x10);
        Assert::IsTrue(MyGridProbe::Row() == b + 0x11);
        Assert::IsTrue(MyGridProbe::Height() == b + 0x12);
        Assert::IsTrue(MyGridProbe::Width() == b + 0x13);
        Assert::IsTrue(MyGridProbe::PickUp() == b + 0x14);
        Assert::IsTrue(MyGridProbe::MousePt() == b + 0x18);
        Assert::IsTrue(MyGridProbe::PickLandR() == b + 0x20);
        Assert::IsTrue(MyGridProbe::MouseMove() == b + 0x28);
        Assert::IsTrue(MyGridProbe::MoveLandR() == b + 0x2C);
        Assert::IsTrue(MyGridProbe::Owner() == b + 0x34);
        Assert::IsTrue(MyGridProbe::Inset() == b + 0x38);
        Assert::IsTrue(MyGridProbe::TypeID() == b + 0x40);
        Assert::IsTrue(MyGridProbe::RealID() == b + 0x440);
        Assert::IsTrue(MyGridProbe::Able() == b + 0x840);
        Assert::IsTrue(MyGridProbe::Index() == b + 0xC40);
        Assert::IsTrue(MyGridProbe::PickTypeID() == b + 0x1040);
        Assert::IsTrue(MyGridProbe::PickType() == b + 0x1044);
        Assert::IsTrue(MyGridProbe::PickIndex() == b + 0x1048);
        Assert::IsTrue(MyGridProbe::PickAct() == b + 0x104C);
        Assert::IsTrue(MyGridProbe::MoveIndex() == b + 0x1050);
        Assert::IsTrue(MyGridProbe::MoveTypeID() == b + 0x1054);
        Assert::IsTrue(MyGridProbe::AbleNum() == b + 0x1058);
        Assert::IsTrue(MyGridProbe::Enable() == b + 0x105C);
        Assert::IsTrue(MyGridProbe::PickEnable() == b + 0x1060);
        Assert::IsTrue(MyGridProbe::ClientRect() == b + 0x1064);
        Assert::IsTrue(MyGridProbe::Num() == b + 0x1074);
        Assert::IsTrue(MyGridProbe::Type() == b + 0x1474);
        Assert::IsTrue(MyGridProbe::Act() == b + 0x1574);
        Assert::IsTrue(MyGridProbe::ActTime() == b + 0x1674);
    }
};
