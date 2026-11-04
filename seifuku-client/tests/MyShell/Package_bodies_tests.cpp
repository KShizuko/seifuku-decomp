#include "stdafx.h"
#include "Package.h"
#include "CppUnitTest.h"
#include <cstddef>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

TEST_CLASS(PackageBodies)
{
public:
    TEST_METHOD(EmptyPackageAnswersNothing)
    {
        CPackage package;
        Assert::AreEqual(0, package.GetItemAmount());
        Assert::IsNull(package.GetItemByIndex(0));
        Assert::IsNull(package.GetItemByIndex(-1));
        Assert::IsNull(package.GetItem(1));
        package.DelItem(1);
        package.Reset();
        Assert::AreEqual(0, package.GetItemAmount());
    }

    TEST_METHOD(AddItemIgnoresNull)
    {
        CPackage package;
        package.AddItem(NULL);
        Assert::AreEqual(0, package.GetItemAmount());
    }

    TEST_METHOD(FieldsRoundTrip)
    {
        CPackage package;
        package.SetId(12345);
        package.SetType(PACKAGE_TRUNK);
        package.SetMaxItemAmount(40);
        Assert::AreEqual((OBJID) 12345, package.GetId());
        Assert::AreEqual((int) PACKAGE_TRUNK, package.GetType());
        Assert::AreEqual(40, package.GetMaxItemAmount());
        package.Reset();
        Assert::AreEqual((OBJID) 12345, package.GetId());
    }
};
