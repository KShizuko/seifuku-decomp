#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include "AllMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(WorldMsgTests)
{
public:
	TEST_METHOD(ActionValues)
	{
		Assert::AreEqual(11, (int)NAMEACT_MEMBERLIST);
		Assert::AreEqual(1, (int)MSGDIALOG_TEXT);
		Assert::AreEqual(2, (int)MSGDIALOG_LINK);
		Assert::AreEqual(100, (int)MSGDIALOG_CREATE);
		Assert::AreEqual(1, (int)MESSAGEBOARD_DEL);
		Assert::AreEqual(2, (int)MESSAGEBOARD_GETLIST);
		Assert::AreEqual(3, (int)MESSAGEBOARD_LIST);
		Assert::AreEqual(4, (int)MESSAGEBOARD_GETWORDS);
		Assert::AreEqual(12, (int)_FRIEND_ONLINE);
		Assert::AreEqual(16, (int)_ENEMY_ONLINE);
		Assert::AreEqual(1015, (int)_MSG_NAME);
		Assert::AreEqual(2032, (int)_MSG_DIALOG);
		Assert::AreEqual(2034, (int)_MSG_TIME);
		Assert::AreEqual(1111, (int)_MSG_MESSAGEBOARD);
		Assert::AreEqual(1019, (int)_MSG_FRIEND);
	}
	TEST_METHOD(TalkCreateBytes)
	{
		CMsgTalk msg;
		Assert::IsTrue(msg.Create("SYSTEM", "Ann", "hi", NULL, 0x00FF0000, _TXTATR_GM) != FALSE);
		const unsigned char* p = (const unsigned char*)msg.GetBuf();
		const int nStrings = 1 + (1+6) + (1+3) + (1+0) + (1+2);
		Assert::AreEqual(0x0F + nStrings, msg.GetSize());
		Assert::AreEqual(1004, (int)msg.GetType());
		Assert::AreEqual(0x00FF0000u, (unsigned)*(const DWORD*)(p+0));
		Assert::AreEqual((int)_TXTATR_GM, (int)*(const unsigned short*)(p+4));
		Assert::AreEqual(0, (int)*(const unsigned short*)(p+6));
		Assert::AreEqual(4, (int)p[0x0C]);
		Assert::AreEqual(6, (int)p[0x0D]);
		Assert::AreEqual(0, memcmp(p+0x0E, "SYSTEM", 6));
		Assert::AreEqual(3, (int)p[0x14]);
		Assert::AreEqual(0, memcmp(p+0x15, "Ann", 3));
		Assert::AreEqual(0, (int)p[0x18]);
		Assert::AreEqual(2, (int)p[0x19]);
		Assert::AreEqual(0, memcmp(p+0x1A, "hi", 2));
	}};
