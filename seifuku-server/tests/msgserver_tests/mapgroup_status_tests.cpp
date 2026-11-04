#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/share/define.h"
#include "../../src/MsgServer/MapGroupKernel/StatusOnce.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_status_tests
{
	TEST_CLASS(MapGroupStatusTests)
	{
	public:
		TEST_METHOD(StatusRecordLayout)
		{
			Assert::AreEqual((size_t)0x10, sizeof(StatusInfoStruct));
			Assert::AreEqual((size_t)0x00, offsetof(StatusInfoStruct, nStatus));
			Assert::AreEqual((size_t)0x04, offsetof(StatusInfoStruct, nPower));
			Assert::AreEqual((size_t)0x08, offsetof(StatusInfoStruct, nSecs));
			Assert::AreEqual((size_t)0x0C, offsetof(StatusInfoStruct, nTimes));
		}

		TEST_METHOD(StatusObjectSizes)
		{
			Assert::AreEqual((size_t)0x20, sizeof(CStatusMore));
			Assert::AreEqual((size_t)0x1C, sizeof(CStatusOnce));
		}
	};
}
