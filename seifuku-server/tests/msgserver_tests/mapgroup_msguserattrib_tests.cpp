#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgUserAttrib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct UserAttribProbe : public CMsgUserAttrib
	{
		typedef CMsgUserAttrib::MSG_Info	Body;
	};
	typedef UserAttribProbe::Body	Body;
}

namespace mapgroup_msguserattrib_tests
{
	TEST_CLASS(MapGroupMsgUserAttribTests)
	{
	public:
		TEST_METHOD(UserAttribLayout4351)
		{
			Assert::AreEqual((size_t)8, sizeof(UserAttribStruct));
			Assert::AreEqual((size_t)0, offsetof(UserAttribStruct, ucAttributeType));
			Assert::AreEqual((size_t)4, offsetof(UserAttribStruct, dwAttributeData));
			Assert::AreEqual((size_t)0, offsetof(Body, idUser));
			Assert::AreEqual((size_t)4, offsetof(Body, dwAttributeNum));
			Assert::AreEqual((size_t)8, offsetof(Body, setUserAttrib));
			Assert::AreEqual((size_t)0x10, sizeof(Body));
		}
	};
}
