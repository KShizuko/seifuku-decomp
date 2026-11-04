#include "stdafx.h"
#include "MyShell.h"
#include "DlgTask.h"
#include "TaskDialog.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgTask>::value, "");
static_assert(std::is_polymorphic<CDlgTask>::value, "");
static_assert(CDlgTask::IDD == 173, "");
static_assert(TEXT_LIMIT == 60 && LINK_LIMIT == 30, "");
static_assert(HEADPIC_WIDTH == 64 && HEADPIC_HEIGHT == 64, "");
static_assert(_TASKDIALOG_TEXT == 0 && _TASKDIALOG_LINK == 1 && _TASKDIALOG_EDIT == 2 && _TASKDIALOG_PIC == 3 && _TASKDIALOG_DEFAULT == 4, "");
static_assert(offsetof(TaskDialogLine, nType) == 0 && offsetof(TaskDialogLine, dwData) == 4 && offsetof(TaskDialogLine, nIndex) == 8 && offsetof(TaskDialogLine, szStr) == 0xC, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgTaskProbe
{
    typedef CDlgTask::TaskText Text;
    typedef CDlgTask::TaskLink Link;
    typedef CDlgTask::TaskEdit Edit;
    static size_t Texts() { return offsetof(CDlgTask, m_TaskText); }
    static size_t TextAmount() { return offsetof(CDlgTask, m_nTextAmount); }
    static size_t Links() { return offsetof(CDlgTask, m_TaskLink); }
    static size_t LinkAmount() { return offsetof(CDlgTask, m_nLinkAmount); }
    static size_t EditInfo() { return offsetof(CDlgTask, m_TaskEdit); }
    static size_t EditAmount() { return offsetof(CDlgTask, m_nEditAmount); }
    static size_t EditIndex() { return offsetof(CDlgTask, m_nEditIndex); }
    static size_t PicIndex() { return offsetof(CDlgTask, m_nPicIndex); }
    static size_t PicSize() { return offsetof(CDlgTask, m_PicSize); }
    static size_t Brush() { return offsetof(CDlgTask, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgTask, m_Pnt); }
    static size_t CloseIndex() { return offsetof(CDlgTask, m_nCloseIndex); }
    static size_t DlgHeight() { return offsetof(CDlgTask, m_nDlgHeight); }
    static size_t MouseMove() { return offsetof(CDlgTask, m_bMouseMove); }
    static size_t CurLink() { return offsetof(CDlgTask, m_btCurLink); }
    static size_t AnswerIndex() { return offsetof(CDlgTask, m_nAnswerIndex); }
    static size_t AnswerAmount() { return offsetof(CDlgTask, m_nAnswerAmount); }
};

static_assert(sizeof(DlgTaskProbe::Text) == 0x224, "");
static_assert(offsetof(DlgTaskProbe::Text, nLineIndex) == 0x200 && offsetof(DlgTaskProbe::Text, nLineNum) == 0x220, "");
static_assert(sizeof(DlgTaskProbe::Link) == 0x138, "");
static_assert(offsetof(DlgTaskProbe::Link, Rect) == 0x100 && offsetof(DlgTaskProbe::Link, nLineIndex) == 0x110 && offsetof(DlgTaskProbe::Link, nLineNum) == 0x130 && offsetof(DlgTaskProbe::Link, nIndex) == 0x134, "");
static_assert(sizeof(DlgTaskProbe::Edit) == 0x28 && offsetof(DlgTaskProbe::Edit, dwData) == 0x20 && offsetof(DlgTaskProbe::Edit, nIndex) == 0x24, "");

TEST_CLASS(ClientDlgTaskHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgTask, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgTaskProbe::Texts() == b + 0x04);
        Assert::IsTrue(DlgTaskProbe::TextAmount() == b + 0x1124);
        Assert::IsTrue(DlgTaskProbe::Links() == b + 0x1128);
        Assert::IsTrue(DlgTaskProbe::LinkAmount() == b + 0x1AE8);
        Assert::IsTrue(DlgTaskProbe::EditInfo() == b + 0x1AEC);
        Assert::IsTrue(DlgTaskProbe::EditAmount() == b + 0x1B14);
        Assert::IsTrue(DlgTaskProbe::EditIndex() == b + 0x1B18);
        Assert::IsTrue(DlgTaskProbe::PicIndex() == b + 0x1B1C);
        Assert::IsTrue(DlgTaskProbe::PicSize() == b + 0x1B20);
        Assert::IsTrue(DlgTaskProbe::Brush() == b + 0x1B28);
        Assert::IsTrue(DlgTaskProbe::Pnt() == b + 0x1B2C);
        Assert::IsTrue(DlgTaskProbe::CloseIndex() == b + 0x1B34);
        Assert::IsTrue(DlgTaskProbe::DlgHeight() == b + 0x1B38);
        Assert::IsTrue(DlgTaskProbe::MouseMove() == b + 0x1B3C);
        Assert::IsTrue(DlgTaskProbe::CurLink() == b + 0x1B40);
        Assert::IsTrue(DlgTaskProbe::AnswerIndex() == b + 0x1B44);
        Assert::IsTrue(DlgTaskProbe::AnswerAmount() == b + 0x1B48);
        const size_t edt = RoundUp(b + 0x1B4C, __alignof(CMyEdit));
        Assert::IsTrue(offsetof(CDlgTask, m_TaskEdtAnswer) == edt);
        const size_t ans = RoundUp(edt + sizeof(CMyEdit), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgTask, m_TaskBtnAnswer) == ans);
        const size_t cls = RoundUp(ans + sizeof(CMyButton), __alignof(CMyButton));
        Assert::IsTrue(offsetof(CDlgTask, m_CloseBtn) == cls);
        Assert::IsTrue(sizeof(CDlgTask) == RoundUp(cls + sizeof(CMyButton), __alignof(CDlgTask)));
    }
};
