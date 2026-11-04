#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgFriend.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct FriendProbe : public CMsgFriend
	{
		typedef CMsgFriend::MSG_Info	Body;
	};
	typedef FriendProbe::Body	Body;
}

namespace mapgroup_msgfriend_tests
{
	TEST_CLASS(MapGroupMsgFriendTests)
	{
	public:
		TEST_METHOD(FriendBodyLayout4351)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, idFriend));
			Assert::AreEqual((size_t)0x04, offsetof(Body, ucAction));
			Assert::AreEqual((size_t)0x05, offsetof(Body, ucOnlineFlag));
			Assert::AreEqual((size_t)0x08, offsetof(Body, szName));
			Assert::AreEqual((size_t)0x08, offsetof(Body, idTransmit));
			Assert::AreEqual((size_t)0x18, sizeof(Body));
		}
	};
}
