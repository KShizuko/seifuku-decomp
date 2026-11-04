#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "IInfoView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(IInfoView) == 0x4, "");
static_assert(std::is_polymorphic<IInfoView>::value, "");
static_assert(std::has_virtual_destructor<IInfoView>::value, "");
static_assert(std::is_same<decltype(&IInfoView::Show), void (IInfoView::*)()>::value, "");

TEST_CLASS(ClientInfoViewHeader)
{
public:
    TEST_METHOD(ShowIsTheSecondSlot)
    {
        Assert::IsTrue(sizeof(IInfoView) == sizeof(void*));
    }
};
