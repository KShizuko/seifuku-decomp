#include "stdafx.h"
#include "MyShell.h"
#include "MyList.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CListBox, CMyList>::value, "");
static_assert(std::is_polymorphic<CMyList>::value, "");

struct MyListProbe
{
    static size_t Par() { return offsetof(CMyList, m_LstPar); }
    static size_t Type() { return offsetof(CMyList, m_LstPar.m_btType); }
    static size_t Count() { return offsetof(CMyList, m_LstPar.m_btListCount); }
    static size_t LineNum() { return offsetof(CMyList, m_LstPar.m_btListLineNum); }
    static size_t ShowLineNum() { return offsetof(CMyList, m_LstPar.m_btShowLineNum); }
    static size_t Width() { return offsetof(CMyList, m_LstPar.m_nWidth); }
    static size_t Height() { return offsetof(CMyList, m_LstPar.m_nHeight); }
    static size_t ListPoint() { return offsetof(CMyList, m_LstPar.m_ListPoint); }
    static size_t IntPoint() { return offsetof(CMyList, m_LstPar.m_IntPoint); }
    static size_t SelRect() { return offsetof(CMyList, m_LstPar.m_SelRect); }
    static size_t ItemRect() { return offsetof(CMyList, m_LstPar.m_ItemRect); }
    static size_t Top() { return offsetof(CMyList, m_LstPar.m_nTopIndex); }
    static size_t Cur() { return offsetof(CMyList, m_LstPar.m_nCurIndex); }
    static size_t Offset() { return offsetof(CMyList, m_LstPar.m_OffsetPoint); }
    static size_t VShow() { return offsetof(CMyList, m_LstPar.m_bVShow); }
    static size_t ParentCob() { return offsetof(CMyList, m_LstPar.m_ParentCob); }
    static size_t Id() { return offsetof(CMyList, m_LstPar.m_strListID); }
    static size_t LastClk() { return offsetof(CMyList, m_LstPar.m_nLastClkIndex); }
    static size_t Real() { return offsetof(CMyList, m_LstPar.m_nListRealIndex); }
    static size_t Fnt() { return offsetof(CMyList, m_LstFnt); }
    static size_t FontHeight() { return offsetof(CMyList, m_LstFnt.m_btFontHeight); }
    static size_t FontColor() { return offsetof(CMyList, m_LstFnt.m_dwFontColor); }
    static size_t CharNum() { return offsetof(CMyList, m_LstFnt.m_btCharNum); }
    static size_t Scr() { return offsetof(CMyList, m_LstScr); }
    static size_t UpRect() { return offsetof(CMyList, m_LstScr.m_ScrUpRect); }
    static size_t DoRect() { return offsetof(CMyList, m_LstScr.m_ScrDoRect); }
    static size_t UpBtn() { return offsetof(CMyList, m_LstScr.m_ScrUpBtn); }
    static size_t DoBtn() { return offsetof(CMyList, m_LstScr.m_ScrDoBtn); }
    static size_t ParSize() { return sizeof(CMyList::MyListParameter); }
    static size_t FntSize() { return sizeof(CMyList::MyListFont); }
};

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

TEST_CLASS(ClientMyListHeader)
{
public:
    TEST_METHOD(TheRecordsSitWhereTheBodiesReadThem)
    {
        const size_t b = sizeof(CListBox);
        Assert::IsTrue(MyListProbe::Par() == b);
        Assert::IsTrue(MyListProbe::ParSize() == 0x198);
        Assert::IsTrue(MyListProbe::Type() == b + 0x00);
        Assert::IsTrue(MyListProbe::Count() == b + 0x01);
        Assert::IsTrue(MyListProbe::LineNum() == b + 0x02);
        Assert::IsTrue(MyListProbe::ShowLineNum() == b + 0x03);
        Assert::IsTrue(MyListProbe::Width() == b + 0x04);
        Assert::IsTrue(MyListProbe::Height() == b + 0x08);
        Assert::IsTrue(MyListProbe::ListPoint() == b + 0x0C);
        Assert::IsTrue(MyListProbe::IntPoint() == b + 0x14);
        Assert::IsTrue(MyListProbe::SelRect() == b + 0x1C);
        Assert::IsTrue(MyListProbe::ItemRect() == b + 0x2C);
        Assert::IsTrue(MyListProbe::Top() == b + 0x3C);
        Assert::IsTrue(MyListProbe::Cur() == b + 0x40);
        Assert::IsTrue(MyListProbe::Offset() == b + 0x44);
        Assert::IsTrue(MyListProbe::VShow() == b + 0x4C);
        Assert::IsTrue(MyListProbe::ParentCob() == b + 0x50);
        Assert::IsTrue(MyListProbe::Id() == b + 0x54);
        Assert::IsTrue(MyListProbe::LastClk() == b + 0x94);
        Assert::IsTrue(MyListProbe::Real() == b + 0x98);
        Assert::IsTrue(MyListProbe::Fnt() == b + 0x198);
        Assert::IsTrue(MyListProbe::FntSize() == 0x0C);
        Assert::IsTrue(MyListProbe::FontHeight() == b + 0x198);
        Assert::IsTrue(MyListProbe::FontColor() == b + 0x19C);
        Assert::IsTrue(MyListProbe::CharNum() == b + 0x1A0);
        const size_t a = __alignof(CMyScrollButton);
        const size_t scr = RoundUp(b + 0x1A4, a);
        Assert::IsTrue(MyListProbe::Scr() == scr);
        Assert::IsTrue(MyListProbe::UpRect() == scr + 0x00);
        Assert::IsTrue(MyListProbe::DoRect() == scr + 0x10);
        Assert::IsTrue(MyListProbe::UpBtn() == scr + 0x20);
        Assert::IsTrue(MyListProbe::DoBtn() == scr + 0x20 + sizeof(CMyScrollButton));
        Assert::IsTrue(sizeof(CMyList) == RoundUp(scr + 0x20 + 2 * sizeof(CMyScrollButton), __alignof(CMyList)));
    }
};
