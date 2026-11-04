#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgMapItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct MapItemProbe : public CMsgMapItem
	{
		typedef CMsgMapItem::MSG_Info	Body;
	};
	typedef MapItemProbe::Body	Body;
}

namespace mapgroup_msgmapitem_tests
{
	TEST_CLASS(MapGroupMsgMapItemTests)
	{
	public:
		TEST_METHOD(MapItemLayout4351)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, id));
			Assert::AreEqual((size_t)0x04, offsetof(Body, idType));
			Assert::AreEqual((size_t)0x04, offsetof(Body, usLook));
			Assert::AreEqual((size_t)0x08, offsetof(Body, nPosX));
			Assert::AreEqual((size_t)0x0A, offsetof(Body, nPosY));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, nAction));
			Assert::AreEqual((size_t)0x10, sizeof(Body));
		}
	};
}
