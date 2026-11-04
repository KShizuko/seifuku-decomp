#include "stdafx.h"
#include "MyShell.h"
#include "DlgLogin.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgLogin>::value, "");
static_assert(std::is_polymorphic<CDlgLogin>::value, "");
static_assert(IDC_LOGIN_STA_DESC == 1251 && IDC_STCSVR1 == 1247 && IDC_STCSVR2 == 1252 && IDC_STCSVR12 == 1262 && IDC_LOGIN_ROLEIMG == 1229 && IDC_LOGIN_EDTIMG == 1228, "");
static_assert(IDC_LOGIN_BTN_ADD == 1005 && IDC_LOGIN_EDT_ACCOUNT == 1003 && IDC_LOGIN_EDT_PASSWORD == 1004 && IDC_LOGIN_BTN_OK == 1102 && IDC_LOGIN_BTN_EXIT == 1103 && IDC_LOGIN_BTN_NEW == 1001 && IDC_LOGIN_BTN_NET == 1002 && IDC_LOGIN_BTN_RECORD == 1097 && IDC_BTN_HELP == 1667, "");
static_assert(IDC_LOGIN_GROUP1 == 1500 && IDC_LOGIN_GROUP2 == 1248 && IDC_LOGIN_GROUP3 == 1249 && IDC_LOGIN_GROUP4 == 1250 && IDC_LOGIN_SERVER1 == 1225 && IDC_LOGIN_SERVER3 == 1227 && IDC_LOGIN_SERVER4 == 1230 && IDC_LOGIN_SERVER12 == 1238, "");
static_assert(ON_EXIT == 0 && ON_HELPDLG_SHOW == 53 && DLG_LOGIN == 0, "");
static_assert(GI_ENTER == 28 && GI_TABLE == 15 && GI_ESCAPE == 1, "");
static_assert(CONTROL_IMAGE == 10 && SERVER_IMAGE == 11, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgLoginProbe
{
    static constexpr int Idd() { return CDlgLogin::IDD; }
    static size_t Brush() { return offsetof(CDlgLogin, m_Hbr); }
    static size_t BrushEdit() { return offsetof(CDlgLogin, m_HbrEdit); }
    static size_t Load() { return offsetof(CDlgLogin, m_bLoad); }
    static size_t Pnt() { return offsetof(CDlgLogin, m_Pnt); }
    static size_t Input() { return offsetof(CDlgLogin, m_pGameInputStr); }
    static size_t CurrentEdt() { return offsetof(CDlgLogin, m_uCurrentEdt); }
    static size_t B1024() { return offsetof(CDlgLogin, m_b1024); }
    static size_t BkName() { return offsetof(CDlgLogin, m_strBkName); }
    static size_t Group() { return offsetof(CDlgLogin, m_strGroup); }
    static size_t Server() { return offsetof(CDlgLogin, m_strServer); }
    static size_t Password() { return offsetof(CDlgLogin, m_szPassword); }
    static size_t PasswordSize() { return sizeof(CDlgLogin::m_szPassword); }
    static size_t ServerDat() { return offsetof(CDlgLogin, m_strServerDat); }
    static size_t CurrentFocus() { return offsetof(CDlgLogin, m_nCurrentFocus); }
    static size_t CurGroup() { return offsetof(CDlgLogin, m_nCurGroup); }
    static size_t SerSta() { return offsetof(CDlgLogin, m_SerSta); }
    static size_t GroupBase() { return offsetof(CDlgLogin, m_nGroupBase); }
    static size_t GroupAmount() { return offsetof(CDlgLogin, m_nGroupAmount); }
    static size_t ServerAdv() { return offsetof(CDlgLogin, m_ServerAdv); }
    static size_t SetAdv() { return offsetof(CDlgLogin, m_setAdv); }
    static size_t LeftPage() { return offsetof(CDlgLogin, m_BtnLeftPage); }
    static size_t RightPage() { return offsetof(CDlgLogin, m_BtnRightPage); }
    static size_t StaDesc() { return offsetof(CDlgLogin, m_StaDesc); }
    static size_t StaServer1() { return offsetof(CDlgLogin, m_StaServer1); }
    static size_t StaServer12() { return offsetof(CDlgLogin, m_StaServer12); }
    static size_t RoleImg() { return offsetof(CDlgLogin, m_LoginRoleImg); }
    static size_t EdtImg() { return offsetof(CDlgLogin, m_LoginEdtImg); }
    static size_t AddBtn() { return offsetof(CDlgLogin, m_LoginAddBtn); }
    static size_t AccountEdt() { return offsetof(CDlgLogin, m_LoginAccountEdt); }
    static size_t PasswordEdt() { return offsetof(CDlgLogin, m_LoginPasswordEdt); }
    static size_t OkBtn() { return offsetof(CDlgLogin, m_LoginOkBtn); }
    static size_t ExitBtn() { return offsetof(CDlgLogin, m_LoginExitBtn); }
    static size_t NewBtn() { return offsetof(CDlgLogin, m_LoginNewBtn); }
    static size_t NetBtn() { return offsetof(CDlgLogin, m_LoginNetBtn); }
    static size_t RecordBtn() { return offsetof(CDlgLogin, m_LoginRecordBtn); }
    static size_t HelpBtn() { return offsetof(CDlgLogin, m_LoginHelpBtn); }
    static size_t Group1() { return offsetof(CDlgLogin, m_ImgGroup1); }
    static size_t Group4() { return offsetof(CDlgLogin, m_ImgGroup4); }
    static size_t Server1() { return offsetof(CDlgLogin, m_ImgServer1); }
    static size_t Server12() { return offsetof(CDlgLogin, m_ImgServer12); }
};

static_assert(DlgLoginProbe::Idd() == 129, "");
static_assert(std::is_same<decltype(&CDlgLogin::SetAccount), void (CDlgLogin::*)(const char*, const char*)>::value, "");
static_assert(std::is_same<decltype(&CDlgLogin::SetPassword), void (CDlgLogin::*)(const char*)>::value, "");

TEST_CLASS(ClientDlgLoginHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgLogin, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgLoginProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgLoginProbe::BrushEdit() == b + 0x08);
        Assert::IsTrue(DlgLoginProbe::Load() == b + 0x0C);
        Assert::IsTrue(DlgLoginProbe::Pnt() == b + 0x10);
        size_t o = b + 0x18;
        Assert::IsTrue(DlgLoginProbe::Input() == o);
        o += sizeof(IGameInputStr*);
        Assert::IsTrue(DlgLoginProbe::CurrentEdt() == o);         o += 4;
        Assert::IsTrue(DlgLoginProbe::B1024() == o);              o += 4;
        Assert::IsTrue(DlgLoginProbe::BkName() == o);             o += 16;
        Assert::IsTrue(DlgLoginProbe::Group() == o);              o += 16;
        Assert::IsTrue(DlgLoginProbe::Server() == o);             o += 16;
        Assert::IsTrue(DlgLoginProbe::Password() == o);           o += 32;
        Assert::IsTrue(DlgLoginProbe::PasswordSize() == 32);
        Assert::IsTrue(DlgLoginProbe::CurrentFocus() == o);       o += 4;
        Assert::IsTrue(DlgLoginProbe::CurGroup() == o);           o += 4;
        Assert::IsTrue(DlgLoginProbe::GroupBase() == o);          o += 4;
        Assert::IsTrue(DlgLoginProbe::GroupAmount() == o);        o += 4;
        o = RoundUp(o, __alignof(CString));
        Assert::IsTrue(DlgLoginProbe::ServerDat() == o);          o += sizeof(CString);
        const size_t as2 = __alignof(CServerStatus);
        o = RoundUp(o, as2);
        Assert::IsTrue(DlgLoginProbe::SerSta() == o);
        const size_t as = __alignof(CMyStatic);
        const size_t sta = RoundUp(sizeof(CMyStatic), as);
        const size_t am = __alignof(CMyImage);
        const size_t img = RoundUp(sizeof(CMyImage), am);
        const size_t ab = __alignof(CMyButton);
        const size_t btn = RoundUp(sizeof(CMyButton), ab);
        const size_t ae = __alignof(CMyEdit);
        const size_t edt = RoundUp(sizeof(CMyEdit), ae);
        o = RoundUp(o + sizeof(CServerStatus), __alignof(CServerAdv));
        Assert::IsTrue(DlgLoginProbe::ServerAdv() == o);
        o = RoundUp(o + sizeof(CServerAdv), __alignof(std::vector<ServerAdvInfo>));
        Assert::IsTrue(DlgLoginProbe::SetAdv() == o);
        o = RoundUp(o + sizeof(std::vector<ServerAdvInfo>), as);
        Assert::IsTrue(DlgLoginProbe::StaDesc() == o);
        Assert::IsTrue(DlgLoginProbe::StaServer1() == o + sta);
        Assert::IsTrue(DlgLoginProbe::StaServer12() == o + 12 * sta);
        o = RoundUp(o + 13 * sta, am);
        Assert::IsTrue(DlgLoginProbe::RoleImg() == o);            o += img;
        Assert::IsTrue(DlgLoginProbe::EdtImg() == o);             o = RoundUp(o + img, ab);
        Assert::IsTrue(DlgLoginProbe::AddBtn() == o);             o = RoundUp(o + btn, ae);
        Assert::IsTrue(DlgLoginProbe::AccountEdt() == o);         o += edt;
        Assert::IsTrue(DlgLoginProbe::PasswordEdt() == o);        o = RoundUp(o + edt, ab);
        Assert::IsTrue(DlgLoginProbe::OkBtn() == o);              o += btn;
        Assert::IsTrue(DlgLoginProbe::ExitBtn() == o);            o += btn;
        Assert::IsTrue(DlgLoginProbe::NewBtn() == o);             o += btn;
        Assert::IsTrue(DlgLoginProbe::NetBtn() == o);             o += btn;
        Assert::IsTrue(DlgLoginProbe::RecordBtn() == o);          o += btn;
        Assert::IsTrue(DlgLoginProbe::HelpBtn() == o);            o = RoundUp(o + btn, am);
        Assert::IsTrue(DlgLoginProbe::Group1() == o);
        Assert::IsTrue(DlgLoginProbe::Group4() == o + 3 * img);
        Assert::IsTrue(DlgLoginProbe::Server1() == o + 4 * img);
        Assert::IsTrue(DlgLoginProbe::Server12() == o + 15 * img);
        const size_t left = RoundUp(o + 16 * img, ab);
        Assert::IsTrue(DlgLoginProbe::LeftPage() == left);
        Assert::IsTrue(DlgLoginProbe::RightPage() == left + btn);
        Assert::IsTrue(sizeof(CDlgLogin) == RoundUp(left + 2 * btn, __alignof(CDlgLogin)));
    }
};
