#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgFuse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct FuseProbe : public CMsgFuse
	{
		typedef CMsgFuse::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef FuseProbe::Body	Body;
}

namespace mapgroup_msgfuse_tests
{
	TEST_CLASS(MapGroupMsgFuseTests)
	{
	public:
		TEST_METHOD(FuseWireLayout)
		{
			Assert::AreEqual((size_t)0x04, FuseProbe::Wire(offsetof(Body, idNpc)));
			Assert::AreEqual((size_t)0x08, FuseProbe::Wire(offsetof(Body, usType)));
			Assert::AreEqual((size_t)0x0A, FuseProbe::Wire(offsetof(Body, usAmount)));
			Assert::AreEqual((size_t)0x0C, FuseProbe::Wire(offsetof(Body, idItem)));
		}
		TEST_METHOD(FuseFieldSizes)
		{
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->idNpc));
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->usType));
			Assert::AreEqual((size_t)2, sizeof(((Body*)0)->usAmount));
			Assert::AreEqual((size_t)4, sizeof(((Body*)0)->idItem[0]));
		}
	};
}
