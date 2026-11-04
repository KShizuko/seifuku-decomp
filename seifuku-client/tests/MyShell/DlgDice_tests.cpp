#include "stdafx.h"
#include "MyShell.h"
#include "DlgDice.h"
#include "DiceManager.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgDice>::value, "");
static_assert(std::is_polymorphic<CDlgDice>::value, "");
static_assert(IDC_DICE_BTN_RESUME == 1633 && IDC_DICE_STA_CHIP == 1632 && IDC_DICE_BTN_DICELIST == 1422 && IDC_DICE_STA_MPC == 1420 && IDC_DICE_STA_ALL == 1421, "");
static_assert(IDC_DICE_BTN_CHIP100 == 1417 && IDC_DICE_BTN_CHIP10000 == 1419 && IDC_DICE_IMA_SMALL == 1413 && IDC_DICE_IMA_BIG == 1414 && IDC_DICE_BTN_HIDE == 1416 && IDC_DICE_BTN_CHIPIN == 1415, "");
static_assert(IDC_DICE_IMA_ALL1 == 1393 && IDC_DICE_IMA_ALL6 == 1398 && IDC_DICE_IMA_4 == 1399 && IDC_DICE_IMA_17 == 1412, "");
static_assert(DICE_VALUE_SMALL == 0 && DICE_VALUE_BIG == 1 && DICE_VALUE_111 == 2 && DICE_VALUE_ALL == 22, "");
static_assert(ON_DICE_HIDE == 97 && ON_DICE_LIST == 93, "");
static_assert(IDC_CURSOR_H == 185 && IDC_CURSOR_CHIP100 == 265 && IDC_CURSOR_CHIP1000 == 264 && IDC_CURSOR_CHIP10000 == 263, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgDiceProbe
{
    static constexpr int Idd() { return CDlgDice::IDD; }
    static size_t Brush() { return offsetof(CDlgDice, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgDice, m_Pnt); }
    static size_t ChipType() { return offsetof(CDlgDice, m_btChipType); }
    static size_t CurChipIn() { return offsetof(CDlgDice, m_nCurChipIn); }
    static size_t ChipRect() { return offsetof(CDlgDice, m_ChipRect); }
    static size_t ChipRectSize() { return sizeof(CDlgDice::m_ChipRect); }
    static size_t ChipIndex() { return offsetof(CDlgDice, m_nChipIndex); }
    static size_t ShowChipIndex() { return offsetof(CDlgDice, m_nShowChipIndex); }
    static size_t Resume() { return offsetof(CDlgDice, m_DiceBtnResume); }
    static size_t StaChip() { return offsetof(CDlgDice, m_DiceStaChip); }
    static size_t DiceList() { return offsetof(CDlgDice, m_DiceBtnDiceList); }
    static size_t StaMpc() { return offsetof(CDlgDice, m_DiceStaMpc); }
    static size_t StaAll() { return offsetof(CDlgDice, m_DiceStaAll); }
    static size_t Chip100() { return offsetof(CDlgDice, m_DiceBtnChip100); }
    static size_t Chip1000() { return offsetof(CDlgDice, m_DiceBtnChip1000); }
    static size_t Chip10000() { return offsetof(CDlgDice, m_DiceBtnChip10000); }
    static size_t Small() { return offsetof(CDlgDice, m_DiceImaSmall); }
    static size_t Big() { return offsetof(CDlgDice, m_DiceImaBig); }
    static size_t Hide() { return offsetof(CDlgDice, m_DiceBtnHide); }
    static size_t Chipin() { return offsetof(CDlgDice, m_DiceBtnChipin); }
    static size_t All1() { return offsetof(CDlgDice, m_DiceImaAll1); }
    static size_t All6() { return offsetof(CDlgDice, m_DiceImaAll6); }
    static size_t Ima4() { return offsetof(CDlgDice, m_DiceIma4); }
    static size_t Ima17() { return offsetof(CDlgDice, m_DiceIma17); }
};

static_assert(DlgDiceProbe::Idd() == 262, "");

TEST_CLASS(ClientDlgDiceHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgDice, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgDiceProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgDiceProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgDiceProbe::ChipType() == b + 0x10);
        Assert::IsTrue(DlgDiceProbe::CurChipIn() == b + 0x14);
        Assert::IsTrue(DlgDiceProbe::ChipRect() == b + 0x18);
        Assert::IsTrue(DlgDiceProbe::ChipRectSize() == 3 * sizeof(CRect));
        Assert::IsTrue(DlgDiceProbe::ChipIndex() == b + 0x48);
        Assert::IsTrue(DlgDiceProbe::ShowChipIndex() == b + 0x4C);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        size_t o = RoundUp(b + 0x50, ab);
        Assert::IsTrue(DlgDiceProbe::Resume() == o);       o = RoundUp(o + btn, as);
        Assert::IsTrue(DlgDiceProbe::StaChip() == o);      o = RoundUp(o + sta, ab);
        Assert::IsTrue(DlgDiceProbe::DiceList() == o);     o = RoundUp(o + btn, as);
        Assert::IsTrue(DlgDiceProbe::StaMpc() == o);       o = RoundUp(o + sta, as);
        Assert::IsTrue(DlgDiceProbe::StaAll() == o);       o = RoundUp(o + sta, ab);
        Assert::IsTrue(DlgDiceProbe::Chip100() == o);      o = RoundUp(o + btn, ab);
        Assert::IsTrue(DlgDiceProbe::Chip1000() == o);     o = RoundUp(o + btn, ab);
        Assert::IsTrue(DlgDiceProbe::Chip10000() == o);    o = RoundUp(o + btn, ai);
        Assert::IsTrue(DlgDiceProbe::Small() == o);        o = RoundUp(o + img, ai);
        Assert::IsTrue(DlgDiceProbe::Big() == o);          o = RoundUp(o + img, ab);
        Assert::IsTrue(DlgDiceProbe::Hide() == o);         o = RoundUp(o + btn, ab);
        Assert::IsTrue(DlgDiceProbe::Chipin() == o);       o = RoundUp(o + btn, ai);
        Assert::IsTrue(DlgDiceProbe::All1() == o);
        Assert::IsTrue(DlgDiceProbe::All6() == o + 5 * img);
        Assert::IsTrue(DlgDiceProbe::Ima4() == o + 6 * img);
        Assert::IsTrue(DlgDiceProbe::Ima17() == o + 19 * img);
        Assert::IsTrue(sizeof(CDlgDice) == RoundUp(o + 20 * img, __alignof(CDlgDice)));
    }
};
