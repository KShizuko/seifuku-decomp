#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcEquip.h"
#include "Item.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgNpcEquip>::value, "");
static_assert(std::is_polymorphic<CDlgNpcEquip>::value, "");
static_assert(CDlgNpcEquip::IDD == 258, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_NPCEQUIP == 36, "");
static_assert(ITEM12_IMAGE == 12, "");
static_assert(ITEMPOSITION_HELMET == 1 && ITEMPOSITION_NECKLACE == 2 && ITEMPOSITION_ARMOR == 3 && ITEMPOSITION_WEAPONR == 4 && ITEMPOSITION_WEAPONL == 5 && ITEMPOSITION_RINGR == 6 && ITEMPOSITION_SHOES == 8, "");
static_assert(ITEMPOSITION_TREASURE == 7 && ITEMPOSITION_MOUNT == 9, "");
static_assert(IDC_NPCEQUIP_IMA_TREASURE == 1385 && IDC_NPCEQUIP_IMA_GARMENT == 1386, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgNpcEquipProbe
{
    static size_t Brush() { return offsetof(CDlgNpcEquip, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgNpcEquip, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgNpcEquip, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgNpcEquip, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgNpcEquip, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgNpcEquip, m_nFrameMode); }
};

TEST_CLASS(ClientDlgNpcEquipHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgNpcEquipProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgNpcEquipProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgNpcEquipProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgNpcEquipProbe::LastShowTime() == b + 0x14);
        Assert::IsTrue(DlgNpcEquipProbe::FrameStep() == b + 0x18);
        Assert::IsTrue(DlgNpcEquipProbe::FrameMode() == b + 0x1C);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t garment = RoundUp(b + 0x20, ai);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgGarment) == garment);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgTreasure) == garment + img);
        const size_t name = RoundUp(garment + 2 * img, as);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_StaName) == name);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_StaFaction) == name + sta);
        const size_t body = RoundUp(name + 2 * sta, ai);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgBody) == body);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgHead) == body + 1 * img);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgNeck) == body + 2 * img);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgGear) == body + 3 * img);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgLHand) == body + 4 * img);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgRHand) == body + 5 * img);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_ImgShoe) == body + 6 * img);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t closeb = RoundUp(body + 7 * img, ab);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_BtnCloseB) == closeb);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_BtnClose) == closeb + 1 * btn);
        Assert::IsTrue(offsetof(CDlgNpcEquip, m_BtnHelp) == closeb + 2 * btn);
        Assert::IsTrue(sizeof(CDlgNpcEquip) == RoundUp(closeb + 3 * btn, __alignof(CDlgNpcEquip)));
    }
};
