#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/FriendData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_friend_tests
{
	TEST_CLASS(MapGroupFriendTests)
	{
	public:
		TEST_METHOD(FriendRecordLayout)
		{
			Assert::AreEqual((size_t)0x1C, sizeof(FriendInfoStruct));
			Assert::AreEqual((size_t)0x00, offsetof(FriendInfoStruct, id));
			Assert::AreEqual((size_t)0x04, offsetof(FriendInfoStruct, idUser));
			Assert::AreEqual((size_t)0x08, offsetof(FriendInfoStruct, idFriend));
			Assert::AreEqual((size_t)0x0C, offsetof(FriendInfoStruct, szFriendName));
		}
	};
}
