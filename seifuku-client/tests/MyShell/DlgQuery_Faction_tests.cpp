#include "stdafx.h"
#include "MyShell.h"
#include "DlgQuery_Faction.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgQuery_Faction>::value, "");
static_assert(std::is_polymorphic<CDlgQuery_Faction>::value, "");
static_assert(IDC_BTN_DEL == 1669 && IDC_FACTION_RIGHT == 1372 && IDC_FACTION_LEFT == 1371 && IDC_FACTION_LST_LIST == 1370 && IDC_BTN_OK == 1668 && IDC_BTN_CANCEL == 1671 && IDC_FACTION_EDT_DATA == 1512, "");
static_assert(IDC_FACTION_CLOSEB == 1511 && IDC_BTN_HELP == 1667 && IDC_FACTION_BTN_STATE == 1052 && IDC_FACTION_BTN_FRIEND == 1048 && IDC_FACTION_BTN_ENEMY == 1049 && IDC_FACTION_BTN_WORD == 1047 && IDC_FACTION_CLOSE == 1510, "");
static_assert(IDC_STATIC8 == 1039 && IDC_STATIC12 == 1038 && IDC_STATIC11 == 1037 && IDC_STATIC13 == 1040 && IDC_STATIC55 == 1041 && IDC_STATIC66 == 1042 && IDC_STATIC1 == 1507 && IDC_STATIC5 == 1032, "");
static_assert(IDC_FACTION_BTN_JOIN == 1046 && IDC_FACTION_BTN_OUT == 1043 && IDC_FACTION_BTN_SUB == 1044 && IDC_FACTION_BTN_INVITE == 2000, "");
static_assert(ON_FACTION_KICK == 43 && ON_FACTION_SUB == 44 && ON_FACTION_INVITE == 45 && ON_FACTION_JOIN == 46 && ON_FACTION_OUT == 47 && ON_HELPDLG_SHOW == 53 && DLG_QUERY_FACTION == 8, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgQuery_FactionProbe
{
    static constexpr int Idd() { return CDlgQuery_Faction::IDD; }
    static size_t Brush() { return offsetof(CDlgQuery_Faction, m_Hbr); }
    static size_t BrushEdit() { return offsetof(CDlgQuery_Faction, m_HbrEdit); }
    static size_t Pnt() { return offsetof(CDlgQuery_Faction, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgQuery_Faction, m_nPntX); }
    static size_t ShowType() { return offsetof(CDlgQuery_Faction, m_nShowType); }
    static size_t Rank() { return offsetof(CDlgQuery_Faction, m_nRank); }
    static size_t CurBase() { return offsetof(CDlgQuery_Faction, m_nCurBase); }
    static size_t LastShowTime() { return offsetof(CDlgQuery_Faction, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgQuery_Faction, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgQuery_Faction, m_nFrameMode); }
    static size_t Member() { return offsetof(CDlgQuery_Faction, m_strMember); }
    static size_t MemberSize() { return sizeof(CDlgQuery_Faction::m_strMember); }
    static size_t MemberOnline() { return offsetof(CDlgQuery_Faction, m_bMemberOnline); }
    static size_t MemberAmount() { return offsetof(CDlgQuery_Faction, m_nMemberAmount); }
    static size_t FacState() { return offsetof(CDlgQuery_Faction, m_DlgFacState); }
    static size_t Del() { return offsetof(CDlgQuery_Faction, m_BtnDel); }
    static size_t Right() { return offsetof(CDlgQuery_Faction, m_BtnRight); }
    static size_t Left() { return offsetof(CDlgQuery_Faction, m_BtnLeft); }
    static size_t List() { return offsetof(CDlgQuery_Faction, m_LstMember); }
    static size_t Ok() { return offsetof(CDlgQuery_Faction, m_BtnOk); }
    static size_t Cancel() { return offsetof(CDlgQuery_Faction, m_BtnCancel); }
    static size_t Edit() { return offsetof(CDlgQuery_Faction, m_EdtWord); }
    static size_t CloseB() { return offsetof(CDlgQuery_Faction, m_BtnCloseB); }
    static size_t Help() { return offsetof(CDlgQuery_Faction, m_BtnHelp); }
    static size_t State() { return offsetof(CDlgQuery_Faction, m_BtnState); }
    static size_t Friend() { return offsetof(CDlgQuery_Faction, m_BtnFriend); }
    static size_t Enemy() { return offsetof(CDlgQuery_Faction, m_BtnEnemy); }
    static size_t Word() { return offsetof(CDlgQuery_Faction, m_BtnWord); }
    static size_t Close() { return offsetof(CDlgQuery_Faction, m_BtnClose); }
    static size_t Proffer() { return offsetof(CDlgQuery_Faction, m_StaProffer); }
    static size_t Population() { return offsetof(CDlgQuery_Faction, m_StaPopulation); }
    static size_t Line1() { return offsetof(CDlgQuery_Faction, m_StaLine1); }
    static size_t Line5() { return offsetof(CDlgQuery_Faction, m_StaLine5); }
    static size_t Join() { return offsetof(CDlgQuery_Faction, m_BtnJoin); }
    static size_t Out() { return offsetof(CDlgQuery_Faction, m_BtnOut); }
    static size_t Sub() { return offsetof(CDlgQuery_Faction, m_BtnSub); }
};

static_assert(DlgQuery_FactionProbe::Idd() == 133, "");

TEST_CLASS(ClientDlgQuery_FactionHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgQuery_Faction, m_bShow) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgQuery_Faction, m_strCurName) == b + 0x04);
        Assert::IsTrue(sizeof(CDlgQuery_Faction::m_strCurName) == 16);
        Assert::IsTrue(DlgQuery_FactionProbe::Brush() == b + 0x14);
        Assert::IsTrue(DlgQuery_FactionProbe::BrushEdit() == b + 0x18);
        Assert::IsTrue(DlgQuery_FactionProbe::Pnt() == b + 0x1C);
        Assert::IsTrue(DlgQuery_FactionProbe::PntX() == b + 0x24);
        Assert::IsTrue(DlgQuery_FactionProbe::ShowType() == b + 0x28);
        Assert::IsTrue(DlgQuery_FactionProbe::Rank() == b + 0x2C);
        Assert::IsTrue(DlgQuery_FactionProbe::CurBase() == b + 0x30);
        Assert::IsTrue(DlgQuery_FactionProbe::LastShowTime() == b + 0x34);
        Assert::IsTrue(DlgQuery_FactionProbe::FrameStep() == b + 0x38);
        Assert::IsTrue(DlgQuery_FactionProbe::FrameMode() == b + 0x3C);
        Assert::IsTrue(DlgQuery_FactionProbe::Member() == b + 0x40);
        Assert::IsTrue(DlgQuery_FactionProbe::MemberSize() == 800 * 32);
        Assert::IsTrue(DlgQuery_FactionProbe::MemberOnline() == b + 0x40 + 800 * 32);
        Assert::IsTrue(DlgQuery_FactionProbe::MemberAmount() == b + 0x40 + 800 * 36);
        const size_t af = __alignof(CDlgFacState);
        size_t o = RoundUp(b + 0x44 + 800 * 36, af);
        Assert::IsTrue(DlgQuery_FactionProbe::FacState() == o);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t al = __alignof(CMyList);
        const size_t ae = __alignof(CMyEdit);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        o = RoundUp(o + sizeof(CDlgFacState), ab);
        Assert::IsTrue(DlgQuery_FactionProbe::Del() == o);         o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Right() == o);       o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Left() == o);        o = RoundUp(o + btn, al);
        Assert::IsTrue(DlgQuery_FactionProbe::List() == o);        o = RoundUp(o + sizeof(CMyList), ab);
        Assert::IsTrue(DlgQuery_FactionProbe::Ok() == o);          o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Cancel() == o);      o = RoundUp(o + btn, ae);
        Assert::IsTrue(DlgQuery_FactionProbe::Edit() == o);        o = RoundUp(o + sizeof(CMyEdit), ab);
        Assert::IsTrue(DlgQuery_FactionProbe::CloseB() == o);      o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Help() == o);        o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::State() == o);       o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Friend() == o);      o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Enemy() == o);       o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Word() == o);        o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Close() == o);       o = RoundUp(o + btn, as);
        Assert::IsTrue(DlgQuery_FactionProbe::Proffer() == o);
        Assert::IsTrue(DlgQuery_FactionProbe::Population() == o + 5 * sta);
        Assert::IsTrue(DlgQuery_FactionProbe::Line1() == o + 6 * sta);
        Assert::IsTrue(DlgQuery_FactionProbe::Line5() == o + 10 * sta);
        o = RoundUp(o + 11 * sta, ab);
        Assert::IsTrue(DlgQuery_FactionProbe::Join() == o);        o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Out() == o);         o += btn;
        Assert::IsTrue(DlgQuery_FactionProbe::Sub() == o);         o += btn;
        Assert::IsTrue(sizeof(CDlgQuery_Faction) == RoundUp(o, __alignof(CDlgQuery_Faction)));
    }
};
