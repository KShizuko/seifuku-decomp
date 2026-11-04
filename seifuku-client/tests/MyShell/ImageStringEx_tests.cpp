#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "ImageStringEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ImageStringExProbe
{
    static size_t String() { return offsetof(CImageStringEx, m_szString); }
    static size_t Ids() { return offsetof(CImageStringEx, m_setId); }
    static size_t Pos() { return offsetof(CImageStringEx, m_posShow); }
    static size_t Color() { return offsetof(CImageStringEx, m_dwColor); }
    static size_t ScaleX() { return offsetof(CImageStringEx, m_fScaleX); }
    static size_t ScaleY() { return offsetof(CImageStringEx, m_fScaleY); }
    static size_t RotateX() { return offsetof(CImageStringEx, m_nRotateX); }
    static size_t RotateZ() { return offsetof(CImageStringEx, m_nRotateZ); }
    static size_t RotateY() { return offsetof(CImageStringEx, m_nRotateY); }
    static size_t CharWidth() { return offsetof(CImageStringEx, m_nCharWidth); }
    static size_t Effect() { return offsetof(CImageStringEx, m_objEffect); }
};

static_assert(std::has_virtual_destructor<CImageStringEx>::value, "");
static_assert(sizeof(CImageStringEx) == 0x24 + sizeof(std::vector<int>) + 0x24 + sizeof(IGame3DEffect*), "");
static_assert(std::is_same<decltype(&CImageStringEx::SetString), void (CImageStringEx::*)(const char*, vector<string>&)>::value, "");
static_assert(std::is_same<decltype(&CImageStringEx::GetStrLen), int (CImageStringEx::*)()>::value, "");

TEST_CLASS(ClientImageStringEx)
{
public:
    TEST_METHOD(LayoutMatchesTheConstructor)
    {
        const size_t v = sizeof(std::vector<int>);
        Assert::IsTrue(ImageStringExProbe::String() == 0x4);
        Assert::IsTrue(ImageStringExProbe::Ids() == 0x24);
        Assert::IsTrue(ImageStringExProbe::Pos() == 0x24 + v);
        Assert::IsTrue(ImageStringExProbe::Color() == 0x2C + v);
        Assert::IsTrue(ImageStringExProbe::ScaleX() == 0x30 + v && ImageStringExProbe::ScaleY() == 0x34 + v);
        Assert::IsTrue(ImageStringExProbe::RotateX() == 0x38 + v && ImageStringExProbe::RotateZ() == 0x3C + v && ImageStringExProbe::RotateY() == 0x40 + v);
        Assert::IsTrue(ImageStringExProbe::CharWidth() == 0x44 + v);
        Assert::IsTrue(ImageStringExProbe::Effect() == 0x48 + v);
        Assert::IsTrue(0x24 + 0x10 == 0x34 && 0x48 + 0x10 == 0x58 && 0x58 + 4 == 0x5C);
    }
};
