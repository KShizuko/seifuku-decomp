#include "stdafx.h"
#include "MyShell.h"
#include "DlgGroup.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgGroup>::value, "");
static_assert(std::is_polymorphic<CDlgGroup>::value, "");
static_assert(CDlgGroup::IDD == 141, "");
static_assert(IDD_MENU_GROUP == 268, "");
static_assert(TEAM_MODE_NONE == 0 && TEAM_MODE_LEADER == 1 && TEAM_MODE_MEMBER == 2, "");
static_assert(MENU_GROUP_CLICK == 5000 && GEAR_CLICK == 5001 && MONEY_CLICK == 5002 && ITEM_CLICK == 5003, "");
static_assert(ON_GROUP_JOIN == 36 && ON_GROUP_INVITE == 37, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_GROUP == 6, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgGroupProbe
{
    static size_t Brush() { return offsetof(CDlgGroup, m_Hbr); }
    static size_t InitSuc() { return offsetof(CDlgGroup, m_bInitSuc); }
    static size_t Pnt() { return offsetof(CDlgGroup, m_Pnt); }
    static size_t PlayerID() { return offsetof(CDlgGroup, m_PlayerID); }
    static size_t PlayerIDSize() { return sizeof(((CDlgGroup*)0)->m_PlayerID); }
    static size_t KickPlayer() { return offsetof(CDlgGroup, m_uKickPlayer); }
    static size_t FocusPlayer() { return offsetof(CDlgGroup, m_uFocusPlayer); }
};

TEST_CLASS(ClientDlgGroupHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgGroup, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgGroup, m_uTeamMode) == b + 0x04);
        Assert::IsTrue(DlgGroupProbe::Brush() == b + 0x08);
        Assert::IsTrue(DlgGroupProbe::InitSuc() == b + 0x0C);
        Assert::IsTrue(DlgGroupProbe::Pnt() == b + 0x10);
        Assert::IsTrue(DlgGroupProbe::PlayerID() == b + 0x18);
        Assert::IsTrue(DlgGroupProbe::PlayerIDSize() == 5 * sizeof(OBJID));
        Assert::IsTrue(DlgGroupProbe::KickPlayer() == b + 0x2C);
        Assert::IsTrue(DlgGroupProbe::FocusPlayer() == b + 0x30);
        const size_t menu = RoundUp(b + 0x34, __alignof(CMenuGroup));
        Assert::IsTrue(offsetof(CDlgGroup, m_MenuGroup) == menu);
        const size_t ab = __alignof(CMyButton);
        const size_t btn3 = RoundUp(menu + sizeof(CMenuGroup), ab);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupBtn3) == btn3);
        const size_t ai = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), ai);
        const size_t img1 = RoundUp(btn3 + sizeof(CMyButton), ai);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupImg1) == img1);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupImg2) == img1 + 1 * img);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupImg3) == img1 + 2 * img);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupImg4) == img1 + 3 * img);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupImg5) == img1 + 4 * img);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t btn1 = RoundUp(img1 + 5 * img, ab);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupBtn1) == btn1);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupBtn2) == btn1 + 1 * btn);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupBtn4) == btn1 + 2 * btn);
        Assert::IsTrue(offsetof(CDlgGroup, m_GroupCloseBtn) == btn1 + 3 * btn);
        Assert::IsTrue(offsetof(CDlgGroup, m_BtnHelp) == btn1 + 4 * btn);
        Assert::IsTrue(sizeof(CDlgGroup) == RoundUp(btn1 + 5 * btn, __alignof(CDlgGroup)));
    }
};
