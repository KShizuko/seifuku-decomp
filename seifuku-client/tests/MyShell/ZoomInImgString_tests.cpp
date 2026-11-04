#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "ZoomInImgString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ZoomInImgStringProbe
{
    static size_t Prefix() { return offsetof(CZoomInImgString, m_strEffectPrefix); }
    static size_t Step() { return offsetof(CZoomInImgString, m_nStep); }
    static size_t BeginTime() { return offsetof(CZoomInImgString, m_dwBeginTime); }
    static size_t ImgString() { return offsetof(CZoomInImgString, m_pImgString); }
};

static_assert(std::has_virtual_destructor<CZoomInImgString>::value, "");
static_assert(sizeof(CZoomInImgString) == 4 + sizeof(std::string) + 4 + 4 + 4, "");
static_assert(std::is_same<decltype(((CZoomInImgString*) 0)->Add((const char*) 0)), void>::value, "");
static_assert(std::is_same<decltype(((CZoomInImgString*) 0)->Show()), void>::value, "");

TEST_CLASS(ClientZoomInImgString)
{
public:
    TEST_METHOD(LayoutMatchesTheConstructor)
    {
        Assert::IsTrue(ZoomInImgStringProbe::Prefix() == 0x4);
        Assert::IsTrue(ZoomInImgStringProbe::Step() == 0x4 + sizeof(std::string));
        Assert::IsTrue(ZoomInImgStringProbe::BeginTime() == ZoomInImgStringProbe::Step() + 4);
        Assert::IsTrue(ZoomInImgStringProbe::ImgString() == ZoomInImgStringProbe::Step() + 8);
    }
};
