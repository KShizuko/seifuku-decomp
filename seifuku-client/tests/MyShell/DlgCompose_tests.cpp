#include "stdafx.h"
#include "MyShell.h"
#include "DlgCompose.h"
#include "Item.h"
#include "NetworkDef.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgCompose>::value, "");
static_assert(std::is_polymorphic<CDlgCompose>::value, "");
static_assert(CDlgCompose::IDD == 275, "");
static_assert(IDC_COMPOSE_IMG_ITEM2 == 1441 && IDC_COMPOSE_IMG_ITEM1 == 1440 && IDC_COMPOSE_IMG_ITEM == 1437 && IDC_COMPOSE_IMG_GEM2 == 1439 && IDC_COMPOSE_IMG_GEM1 == 1438, "");
static_assert(IDC_COMPOSE_BTN_OK == 1668 && IDC_BTN_HELP == 1667 && IDC_COMPOSE_BTN_CLOSE == 1724 && IDC_BTN_CLOSE == 1180, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_EXP == 41 && ON_EMBED_CLOSE == 66 && ON_MSG_1003 == 1003 && ON_ICON_DROP == 15 && DLG_ALL == 48 , "");
static_assert(ITEM6_IMAGE == 6, "");
static_assert(ITEMSORT_HELMET == 1 && ITEMSORT_SHOES == 8 && ITEMSORT_OTHER == 9 && OTHER_GEM == 0, "");
static_assert(_TXTATR_SYSTEM == 2005, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgComposeProbe
{
    static size_t Brush() { return offsetof(CDlgCompose, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgCompose, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgCompose, m_nPntX); }
    static size_t PickUp() { return offsetof(CDlgCompose, m_bPickUp); }
    static size_t IconRealID() { return offsetof(CDlgCompose, m_nIconRealID); }
    static size_t Item() { return offsetof(CDlgCompose, m_idItem); }
    static size_t Item1() { return offsetof(CDlgCompose, m_idItem1); }
    static size_t Item2() { return offsetof(CDlgCompose, m_idItem2); }
    static size_t Gem1() { return offsetof(CDlgCompose, m_idGem1); }
    static size_t Gem2() { return offsetof(CDlgCompose, m_idGem2); }
    static size_t ItemType() { return offsetof(CDlgCompose, m_idItemType); }
    static size_t Addition() { return offsetof(CDlgCompose, m_btAddition); }
    static size_t Level() { return offsetof(CDlgCompose, m_btLevel); }
    static size_t LastShowTime() { return offsetof(CDlgCompose, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgCompose, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgCompose, m_nFrameMode); }
};

TEST_CLASS(ClientDlgComposeHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgCompose, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgComposeProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgComposeProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgComposeProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgComposeProbe::PickUp() == b + 0x14);
        Assert::IsTrue(DlgComposeProbe::IconRealID() == b + 0x18);
        Assert::IsTrue(DlgComposeProbe::Item() == b + 0x1C);
        Assert::IsTrue(DlgComposeProbe::Item1() == b + 0x20);
        Assert::IsTrue(DlgComposeProbe::Item2() == b + 0x24);
        Assert::IsTrue(DlgComposeProbe::Gem1() == b + 0x28);
        Assert::IsTrue(DlgComposeProbe::Gem2() == b + 0x2C);
        Assert::IsTrue(DlgComposeProbe::ItemType() == b + 0x30);
        Assert::IsTrue(DlgComposeProbe::Addition() == b + 0x34);
        Assert::IsTrue(DlgComposeProbe::Level() == b + 0x35);
        Assert::IsTrue(DlgComposeProbe::LastShowTime() == b + 0x38);
        Assert::IsTrue(DlgComposeProbe::FrameStep() == b + 0x3C);
        Assert::IsTrue(DlgComposeProbe::FrameMode() == b + 0x40);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t item2 = RoundUp(b + 0x44, ai);
        Assert::IsTrue(offsetof(CDlgCompose, m_ImgItem2) == item2);
        Assert::IsTrue(offsetof(CDlgCompose, m_ImgItem1) == item2 + 1 * img);
        Assert::IsTrue(offsetof(CDlgCompose, m_ImgItem) == item2 + 2 * img);
        Assert::IsTrue(offsetof(CDlgCompose, m_ImgGem2) == item2 + 3 * img);
        Assert::IsTrue(offsetof(CDlgCompose, m_ImgGem1) == item2 + 4 * img);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t ok = RoundUp(item2 + 5 * img, ab);
        Assert::IsTrue(offsetof(CDlgCompose, m_BtnOk) == ok);
        Assert::IsTrue(offsetof(CDlgCompose, m_BtnHelp) == ok + 1 * btn);
        Assert::IsTrue(offsetof(CDlgCompose, m_BtnClose) == ok + 2 * btn);
        Assert::IsTrue(offsetof(CDlgCompose, m_BtnCloseB) == ok + 3 * btn);
        Assert::IsTrue(sizeof(CDlgCompose) == RoundUp(ok + 4 * btn, __alignof(CDlgCompose)));
    }
};
