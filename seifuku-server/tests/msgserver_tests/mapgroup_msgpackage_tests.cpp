#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgPackage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct PackageProbe : public CMsgPackage
	{
		typedef CMsgPackage::MSG_Info	Body;
	};
	typedef PackageProbe::Body	Body;
}

namespace mapgroup_msgpackage_tests
{
	TEST_CLASS(MapGroupMsgPackageTests)
	{
	public:
		TEST_METHOD(PackageLayout)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, id));
			Assert::AreEqual((size_t)0x04, offsetof(Body, ucAction));
			Assert::AreEqual((size_t)0x05, offsetof(Body, ucType));
			Assert::AreEqual((size_t)0x08, offsetof(Body, idItem));
			Assert::AreEqual((size_t)0x08, offsetof(Body, usSize));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, setItemInfo));
			Assert::AreEqual((size_t)0x20, sizeof(Body));
		}
		TEST_METHOD(PackageItemInfoLayout)
		{
			Assert::AreEqual((size_t)0x08, offsetof(MsgPackageItemInfo, ucIdent));
			Assert::AreEqual((size_t)0x0D, offsetof(MsgPackageItemInfo, ucMagic3));
			Assert::AreEqual((size_t)0x0E, offsetof(MsgPackageItemInfo, ucReduceDmg));
			Assert::AreEqual((size_t)0x10, offsetof(MsgPackageItemInfo, usAddLife));
			Assert::AreEqual((size_t)0x12, offsetof(MsgPackageItemInfo, usAntiMonster));
			Assert::AreEqual((size_t)0x14, sizeof(MsgPackageItemInfo));
		}
	};
}
