#include "stdafx.h"
#include "MyShell.h"
#include "MyEdit.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMyEdit) == sizeof(CEdit) + sizeof(CMyEditRect) + 0xA0, "");
static_assert(std::is_base_of<CEdit, CMyEdit>::value, "");
static_assert(std::is_polymorphic<CMyEdit>::value, "");

struct MyEditProbe
{
    static size_t Cover() { return offsetof(CMyEdit, m_EditRect); }
    static size_t Par() { return offsetof(CMyEdit, m_EdtPar); }
    static size_t Pnt() { return offsetof(CMyEdit, m_EdtPar.m_Pnt); }
    static size_t IntPnt() { return offsetof(CMyEdit, m_EdtPar.m_IntPnt); }
    static size_t Width() { return offsetof(CMyEdit, m_EdtPar.m_nWidth); }
    static size_t Height() { return offsetof(CMyEdit, m_EdtPar.m_nHeight); }
    static size_t SelNum() { return offsetof(CMyEdit, m_EdtPar.m_nSelNum); }
    static size_t RightLeft() { return offsetof(CMyEdit, m_EdtPar.m_btRightLeft); }
    static size_t EndCharPos() { return offsetof(CMyEdit, m_EdtPar.m_btEndCharPos); }
    static size_t SelColor() { return offsetof(CMyEdit, m_EdtPar.m_dwSelColor); }
    static size_t Frame() { return offsetof(CMyEdit, m_EdtPar.m_uFrame); }
    static size_t Id() { return offsetof(CMyEdit, m_EdtPar.m_strEditID); }
    static size_t MultiLine() { return offsetof(CMyEdit, m_EdtPar.m_bMultiLine); }
    static size_t Chinese() { return offsetof(CMyEdit, m_EdtPar.m_bChinese); }
    static size_t LastSelEnd() { return offsetof(CMyEdit, m_EdtPar.m_nLastSelEnd); }
    static size_t Fnt() { return offsetof(CMyEdit, m_EdtFnt); }
    static size_t Password() { return offsetof(CMyEdit, m_EdtFnt.m_bPassword); }
    static size_t FontWidth() { return offsetof(CMyEdit, m_EdtFnt.m_btFontWidth); }
    static size_t CharNum() { return offsetof(CMyEdit, m_EdtFnt.m_nCharNum); }
    static size_t StartPoint() { return offsetof(CMyEdit, m_EdtFnt.m_StartPoint); }
    static size_t EndPoint() { return offsetof(CMyEdit, m_EdtFnt.m_EndPoint); }
    static size_t OffsetPoint() { return offsetof(CMyEdit, m_EdtFnt.m_OffsetPoint); }
    static size_t FontColor() { return offsetof(CMyEdit, m_EdtFnt.m_dwFontColor); }
    static size_t BackColor() { return offsetof(CMyEdit, m_EdtFnt.m_dwBackColor); }
};

TEST_CLASS(ClientMyEditHeader)
{
public:
    TEST_METHOD(TheRecordsSitWhereTheBodiesReadThem)
    {
        const size_t b = sizeof(CEdit);
        Assert::IsTrue(MyEditProbe::Cover() == b);
        const size_t r = b + sizeof(CMyEditRect);
        Assert::IsTrue(MyEditProbe::Par() == r);
        Assert::IsTrue(MyEditProbe::Pnt() == r + 0x00);
        Assert::IsTrue(MyEditProbe::IntPnt() == r + 0x08);
        Assert::IsTrue(MyEditProbe::Width() == r + 0x10);
        Assert::IsTrue(MyEditProbe::Height() == r + 0x14);
        Assert::IsTrue(MyEditProbe::SelNum() == r + 0x18);
        Assert::IsTrue(MyEditProbe::RightLeft() == r + 0x1C);
        Assert::IsTrue(MyEditProbe::EndCharPos() == r + 0x1D);
        Assert::IsTrue(MyEditProbe::SelColor() == r + 0x20);
        Assert::IsTrue(MyEditProbe::Frame() == r + 0x24);
        Assert::IsTrue(MyEditProbe::Id() == r + 0x28);
        Assert::IsTrue(MyEditProbe::MultiLine() == r + 0x68);
        Assert::IsTrue(MyEditProbe::Chinese() == r + 0x6C);
        Assert::IsTrue(MyEditProbe::LastSelEnd() == r + 0x70);
        Assert::IsTrue(MyEditProbe::Fnt() == r + 0x74);
        Assert::IsTrue(MyEditProbe::Password() == r + 0x74);
        Assert::IsTrue(MyEditProbe::FontWidth() == r + 0x78);
        Assert::IsTrue(MyEditProbe::CharNum() == r + 0x7C);
        Assert::IsTrue(MyEditProbe::StartPoint() == r + 0x80);
        Assert::IsTrue(MyEditProbe::EndPoint() == r + 0x88);
        Assert::IsTrue(MyEditProbe::OffsetPoint() == r + 0x90);
        Assert::IsTrue(MyEditProbe::FontColor() == r + 0x98);
        Assert::IsTrue(MyEditProbe::BackColor() == r + 0x9C);
    }
};
