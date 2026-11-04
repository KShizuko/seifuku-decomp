#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Package.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct PackageProbe : CPackage
{
    static size_t Items() { return offsetof(PackageProbe, m_setItem); }
    static size_t Type() { return offsetof(PackageProbe, m_nType); }
    static size_t Id() { return offsetof(PackageProbe, m_idPackage); }
    static size_t MaxItemAmount() { return offsetof(PackageProbe, m_nMaxItemAmount); }
};

static_assert(sizeof(CPackage) == 4 + sizeof(DEQUE_ITEM) + 0xC, "");
static_assert(std::has_virtual_destructor<CPackage>::value, "");
static_assert(std::is_same<decltype(&CPackage::GetItem), CItem* (CPackage::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CPackage::DelItem), void (CPackage::*)(OBJID)>::value, "");

TEST_CLASS(ClientPackageHeader)
{
public:
    TEST_METHOD(ThreeIntsFollowTheDeque)
    {
        const size_t d = sizeof(DEQUE_ITEM);
        Assert::IsTrue(PackageProbe::Items() == 0x4);
        Assert::IsTrue(PackageProbe::Type() == 0x4 + d);
        Assert::IsTrue(PackageProbe::Id() == 0x4 + d + 0x4);
        Assert::IsTrue(PackageProbe::MaxItemAmount() == 0x4 + d + 0x8);
    }
};
