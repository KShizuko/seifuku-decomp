#include "stdafx.h"
#include "MyShell.h"
#include "DlgNpcFace.h"
#include "Role.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgNpcFace>::value, "");
static_assert(std::is_polymorphic<CDlgNpcFace>::value, "");
static_assert(CDlgNpcFace::IDD == 212, "");
static_assert(ON_NPCFACE_CLOSE == 35, "");
static_assert(IDC_NPCFACE_BTN_CLOSEB == 1576, "");
static_assert(SEX_MALE == 1 && SEX_FEMALE == 2, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgNpcFaceProbe
{
    static size_t Brush() { return offsetof(CDlgNpcFace, m_Hbr); }
    static size_t Pnt() { return offsetof(CDlgNpcFace, m_Pnt); }
    static size_t PntX() { return offsetof(CDlgNpcFace, m_nPntX); }
    static size_t Male() { return offsetof(CDlgNpcFace, m_nMaleFaceAmount); }
    static size_t Female() { return offsetof(CDlgNpcFace, m_nFemaleFaceAmount); }
    static size_t Current() { return offsetof(CDlgNpcFace, m_nCurrentAmount); }
    static size_t Focus() { return offsetof(CDlgNpcFace, m_uFocusImage); }
    static size_t LastShowTime() { return offsetof(CDlgNpcFace, m_dwLastShowTime); }
    static size_t FrameStep() { return offsetof(CDlgNpcFace, m_fFrameStep); }
    static size_t FrameMode() { return offsetof(CDlgNpcFace, m_nFrameMode); }
};

TEST_CLASS(ClientDlgNpcFaceHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_bShow) == b + 0x00);
        Assert::IsTrue(DlgNpcFaceProbe::Brush() == b + 0x04);
        Assert::IsTrue(DlgNpcFaceProbe::Pnt() == b + 0x08);
        Assert::IsTrue(DlgNpcFaceProbe::PntX() == b + 0x10);
        Assert::IsTrue(DlgNpcFaceProbe::Male() == b + 0x14);
        Assert::IsTrue(DlgNpcFaceProbe::Female() == b + 0x18);
        Assert::IsTrue(DlgNpcFaceProbe::Current() == b + 0x1C);
        Assert::IsTrue(DlgNpcFaceProbe::Focus() == b + 0x20);
        Assert::IsTrue(DlgNpcFaceProbe::LastShowTime() == b + 0x24);
        Assert::IsTrue(DlgNpcFaceProbe::FrameStep() == b + 0x28);
        Assert::IsTrue(DlgNpcFaceProbe::FrameMode() == b + 0x2C);
        const size_t ab = __alignof(CMyButton);
        const size_t closeb = RoundUp(b + 0x30, ab);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceBtnCloseB) == closeb);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceBtnUp) == closeb + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceBtnDown) == closeb + 2 * RoundUp(sizeof(CMyButton), ab));
        const size_t ai = __alignof(CMyImage);
        const size_t ima6 = RoundUp(closeb + 3 * RoundUp(sizeof(CMyButton), ab), ai);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceIma6) == ima6);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceIma1) == ima6 + 5 * RoundUp(sizeof(CMyImage), ai));
        const size_t close = RoundUp(ima6 + 6 * RoundUp(sizeof(CMyImage), ai), ab);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceBtnClose) == close);
        Assert::IsTrue(offsetof(CDlgNpcFace, m_NpcFaceBtnAgree) == close + 1 * RoundUp(sizeof(CMyButton), ab));
        Assert::IsTrue(sizeof(CDlgNpcFace) == RoundUp(offsetof(CDlgNpcFace, m_NpcFaceBtnAgree) + sizeof(CMyButton), __alignof(CDlgNpcFace)));
    }
};
