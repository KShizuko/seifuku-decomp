#include "stdafx.h"
#include "MyShell.h"
#include "DlgEnemy.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgEnemy>::value, "");
static_assert(std::is_polymorphic<CDlgEnemy>::value, "");
static_assert(CDlgEnemy::IDD == 269, "");
static_assert(ON_FRIEND_RESUME == 58, "");
static_assert(ON_HELPDLG_SHOW == 53 && DLG_FRIEND == 5, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgEnemyProbe
{
    static size_t Brush() { return offsetof(CDlgEnemy, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgEnemy, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgEnemy, m_nPntX); }
    static size_t LastShowTime() { return offsetof(CDlgEnemy, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgEnemy, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgEnemy, m_nFrameMode); }
};

TEST_CLASS(ClientDlgEnemyHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgEnemy, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgEnemyProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgEnemyProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgEnemyProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgEnemyProbe::LastShowTime() == b + 0x14);
        Assert::IsTrue(DlgEnemyProbe::FrameStep() == b + 0x18);
        Assert::IsTrue(DlgEnemyProbe::FrameMode() == b + 0x1C);
        const size_t card = RoundUp(b + 0x20, __alignof(CDlgEnmState));
        Assert::IsTrue(offsetof(CDlgEnemy, m_DlgEnmState) == card);
        const size_t query = RoundUp(card + sizeof(CDlgEnmState), __alignof(int));
        Assert::IsTrue(offsetof(CDlgEnemy, m_nCurrentQueryIndex) == query);
        const size_t resume = RoundUp(query + sizeof(int), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgEnemy, m_EnemyBtnResume) == resume);
        const size_t list = RoundUp(resume + sizeof(CMyButton), __alignof(CMyList));
        Assert::IsTrue(offsetof(CDlgEnemy, m_EnemyLstData) == list);
        const size_t close = RoundUp(list + sizeof(CMyList), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgEnemy, m_EnemyBtnClose) == close);
        Assert::IsTrue(offsetof(CDlgEnemy, m_BtnHelp) == RoundUp(close + sizeof(CMyButton), __alignof(CMyButton)));
        Assert::IsTrue(sizeof(CDlgEnemy) == RoundUp(offsetof(CDlgEnemy, m_BtnHelp) + sizeof(CMyButton), __alignof(CDlgEnemy)));
    }
};
