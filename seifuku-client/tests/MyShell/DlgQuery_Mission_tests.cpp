#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Mission.h"
#include "Item.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery_Mission>::value, "");
static_assert(std::is_polymorphic<CDlgQuery_Mission>::value, "");
static_assert(ON_ICON_DROP == 15 && DLG_ALL == 48 , "");
static_assert(ITEM_IMAGE == 1, "");
static_assert(ITEMPOSITION_HELMET == 1 && ITEMPOSITION_NECKLACE == 2 && ITEMPOSITION_ARMOR == 3 && ITEMPOSITION_WEAPONR == 4 && ITEMPOSITION_WEAPONL == 5 && ITEMPOSITION_RINGR == 6 && ITEMPOSITION_SHOES == 8, "");
static_assert(ITEMSORT_HELMET == 1 && ITEMSORT_NECKLACE == 2 && ITEMSORT_ARMOR == 3 && ITEMSORT_WEAPON_SINGLE_HAND == 4 && ITEMSORT_WEAPON_DOUBLE_HAND == 5 && ITEMSORT_SHIELD == 6 && ITEMSORT_SHOES == 8, "");
static_assert(ITEMSORT_RING == 7 && ITEMSORT_MOUNT == 10 && ITEMSORT_SPRITE == 11 && ITEMPOSITION_TREASURE == 7 && ITEMPOSITION_MOUNT == 9, "");
static_assert(IDC_MISSION_IMA_TREASURE == 1679 && IDC_MISSION_IMA_GARMENT == 1680, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQuery_MissionProbe
{
    static constexpr int Idd() { return CDlgQuery_Mission::IDD; }
    static size_t Brush() { return offsetof(CDlgQuery_Mission, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgQuery_Mission, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgQuery_Mission, m_Pnt); }
    static size_t PickUp() { return offsetof(CDlgQuery_Mission, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgQuery_Mission, m_nIconRealID); }
    static size_t LHandEnable() { return offsetof(CDlgQuery_Mission, m_bLHandEnable); }
    static size_t Garment() { return offsetof(CDlgQuery_Mission, m_ImgGarment); }
    static size_t Treasure() { return offsetof(CDlgQuery_Mission, m_ImgTreasure); }
    static size_t Head() { return offsetof(CDlgQuery_Mission, m_ImgHead); }
    static size_t Neck() { return offsetof(CDlgQuery_Mission, m_ImgNeck); }
    static size_t Gear() { return offsetof(CDlgQuery_Mission, m_ImgGear); }
    static size_t Body() { return offsetof(CDlgQuery_Mission, m_ImgBody); }
    static size_t LHand() { return offsetof(CDlgQuery_Mission, m_ImgLHand); }
    static size_t RHand() { return offsetof(CDlgQuery_Mission, m_ImgRHand); }
    static size_t Shoe() { return offsetof(CDlgQuery_Mission, m_ImgShoe); }
};

static_assert(DlgQuery_MissionProbe::Idd() == 135, "");

TEST_CLASS(ClientDlgQuery_MissionHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgQuery_Mission, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgQuery_MissionProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgQuery_MissionProbe::InitSuc() == b + 0x08);
        Assert::IsTrue(DlgQuery_MissionProbe::Pnt() == b + 0x0C);
        Assert::IsTrue(DlgQuery_MissionProbe::PickUp() == b + 0x14);
        Assert::IsTrue(DlgQuery_MissionProbe::IconRealID() == b + 0x18);
        Assert::IsTrue(DlgQuery_MissionProbe::LHandEnable() == b + 0x1C);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t garment = RoundUp(b + 0x20, ai);
        Assert::IsTrue(DlgQuery_MissionProbe::Garment() == garment);
        Assert::IsTrue(DlgQuery_MissionProbe::Treasure() == garment + img);
        const size_t head = garment + 2 * img;
        Assert::IsTrue(DlgQuery_MissionProbe::Head() == head);
        Assert::IsTrue(DlgQuery_MissionProbe::Neck() == head + 1 * img);
        Assert::IsTrue(DlgQuery_MissionProbe::Gear() == head + 2 * img);
        Assert::IsTrue(DlgQuery_MissionProbe::Body() == head + 3 * img);
        Assert::IsTrue(DlgQuery_MissionProbe::LHand() == head + 4 * img);
        Assert::IsTrue(DlgQuery_MissionProbe::RHand() == head + 5 * img);
        Assert::IsTrue(DlgQuery_MissionProbe::Shoe() == head + 6 * img);
        Assert::IsTrue(sizeof(CDlgQuery_Mission) == RoundUp(head + 7 * img, __alignof(CDlgQuery_Mission)));
    }
};
