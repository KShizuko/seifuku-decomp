#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgItemInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct ItemInfoProbe : public CMsgItemInfo
	{
		typedef CMsgItemInfo::MSG_Info	Body;
	};
	typedef ItemInfoProbe::Body	Body;
}

namespace mapgroup_msgiteminfo_tests
{
	TEST_CLASS(MapGroupMsgItemInfoTests)
	{
	public:
		TEST_METHOD(ItemInfoLayout4351)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, id));
			Assert::AreEqual((size_t)0x04, offsetof(Body, dwType));
			Assert::AreEqual((size_t)0x08, offsetof(Body, usAmount));
			Assert::AreEqual((size_t)0x0A, offsetof(Body, usAmountLimit));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, ucAction));
			Assert::AreEqual((size_t)0x0D, offsetof(Body, ucIdent));
			Assert::AreEqual((size_t)0x0E, offsetof(Body, ucPosition));
			Assert::AreEqual((size_t)0x0F, offsetof(Body, ucGem1));
			Assert::AreEqual((size_t)0x13, offsetof(Body, ucMagic3));
			Assert::AreEqual((size_t)0x14, offsetof(Body, ucReduceDmg));
			Assert::AreEqual((size_t)0x15, offsetof(Body, ucAddLife));
			Assert::AreEqual((size_t)0x18, offsetof(Body, nData));
			Assert::AreEqual((size_t)0x1C, sizeof(Body));
		}
	};
}
