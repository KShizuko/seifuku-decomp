#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
const int	ITEMPOSITION_NONE		= 255;
#include "../../src/MsgServer/MapGroupKernel/Network/MsgItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct ItemProbe : public CMsgItem
	{
		typedef CMsgItem::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef ItemProbe::Body	Body;
}

namespace mapgroup_msgitem_tests
{
	TEST_CLASS(MapGroupMsgItemTests)
	{
	public:
		TEST_METHOD(ItemWireLayout)
		{
			Assert::AreEqual((size_t)0x04, ItemProbe::Wire(offsetof(Body, id)));
			Assert::AreEqual((size_t)0x08, ItemProbe::Wire(offsetof(Body, dwData)));
			Assert::AreEqual((size_t)0x08, ItemProbe::Wire(offsetof(Body, usPosY)));
			Assert::AreEqual((size_t)0x0A, ItemProbe::Wire(offsetof(Body, usPosX)));
			Assert::AreEqual((size_t)0x0C, ItemProbe::Wire(offsetof(Body, usAction)));
			Assert::AreEqual((size_t)0x10, ItemProbe::Wire(offsetof(Body, idTarget)));
			Assert::AreEqual((size_t)0x10, sizeof(Body));
		}
		TEST_METHOD(ItemFieldSizes)
		{
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->id));
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->dwData));
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->usAction));
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->idTarget));
		}
		TEST_METHOD(ItemActionValues)
		{
			Assert::AreEqual(1, (int)ITEMACT_BUY);
			Assert::AreEqual(7, (int)ITEMACT_SPLITITEM);
			Assert::AreEqual(12, (int)ITEMACT_DROPMONEY);
			Assert::AreEqual(21, (int)ITEMACT_BOOTH_QUERY);
			Assert::AreEqual(24, (int)ITEMACT_BOOTH_BUY);
			Assert::AreEqual(26, (int)ITEMACT_FIREWORKS);
			Assert::AreEqual(27, (int)ITEMACT_PING);
			Assert::AreEqual(28, (int)ITEMACT_ENHANCE);
			Assert::AreEqual(29, (int)ITEMACT_BOOTH_ADD_EMONEY);
		}
	};
}
