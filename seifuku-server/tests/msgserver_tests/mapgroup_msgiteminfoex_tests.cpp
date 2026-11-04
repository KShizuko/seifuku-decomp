#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgItemInfoEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct ItemInfoExProbe : public CMsgItemInfoEx
	{
		typedef CMsgItemInfoEx::MSG_Info	Body;
	};
	typedef ItemInfoExProbe::Body	Body;
}

namespace mapgroup_msgiteminfoex_tests
{
	TEST_CLASS(MapGroupMsgItemInfoExTests)
	{
	public:
		TEST_METHOD(ItemInfoExLayout4351)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, id));
			Assert::AreEqual((size_t)0x04, offsetof(Body, idOwner));
			Assert::AreEqual((size_t)0x08, offsetof(Body, dwPrice));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, dwType));
			Assert::AreEqual((size_t)0x10, offsetof(Body, usAmount));
			Assert::AreEqual((size_t)0x12, offsetof(Body, usAmountLimit));
			Assert::AreEqual((size_t)0x14, offsetof(Body, ucAction));
			Assert::AreEqual((size_t)0x15, offsetof(Body, ucIdent));
			Assert::AreEqual((size_t)0x16, offsetof(Body, ucPosition));
			Assert::AreEqual((size_t)0x17, offsetof(Body, ucGem1));
			Assert::AreEqual((size_t)0x1B, offsetof(Body, ucMagic3));
			Assert::AreEqual((size_t)0x1C, offsetof(Body, ucReduceDmg));
			Assert::AreEqual((size_t)0x1D, offsetof(Body, ucAddLife));
			Assert::AreEqual((size_t)0x20, offsetof(Body, nData));
			Assert::AreEqual((size_t)0x24, sizeof(Body));
		}
	};
}
