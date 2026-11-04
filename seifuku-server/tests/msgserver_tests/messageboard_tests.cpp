#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <string.h>
#include "MessageBoard.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(MessageBoardTests)
{
public:
	TEST_METHOD(NewestFirstAndFull)
	{
		CMessageBoard* p = CMessageBoard::CreateNew(2201, 3);
		Assert::IsTrue(p->AddMsg(1, "one", "a"));
		Assert::IsTrue(p->AddMsg(2, "two", "b"));
		Assert::IsTrue(p->AddMsg(3, "three", "c"));
		Assert::AreEqual(3UL, p->GetMsgID(0));
		Assert::AreEqual(1UL, p->GetMsgID(2));
		Assert::IsTrue(p->AddMsg(4, "four", "d"));
		Assert::AreEqual(4UL, p->GetMsgID(0));
		Assert::AreEqual(2UL, p->GetMsgID(2));
		Assert::IsFalse(p->IsValid(3));
		Assert::AreEqual((OBJID)ID_NONE, p->GetMsgID(3));
		p->Release();
	}

	TEST_METHOD(SenderReplaced)
	{
		CMessageBoard* p = CMessageBoard::CreateNew(2201, 10);
		p->AddMsg(7, "seven", "first");
		p->AddMsg(8, "eight", "x");
		p->AddMsg(7, "seven", "second");
		Assert::AreEqual(7UL, p->GetMsgID(0));
		Assert::AreEqual("second", p->GetMsgWords(0));
		Assert::AreEqual(8UL, p->GetMsgID(1));
		Assert::IsFalse(p->IsValid(2));
		p->AddMsg(ID_NONE, "anon", "y");
		p->AddMsg(ID_NONE, "anon", "z");
		Assert::IsTrue(p->IsValid(3));
		p->Release();
	}

	TEST_METHOD(ByName)
	{
		CMessageBoard* p = CMessageBoard::CreateNew(2201, 10);
		p->AddMsg(1, "alice", "hi");
		p->AddMsg(2, "bob", "yo");
		CMessageBoard::MessageInfo* pInfo = p->GetMsgInfo("alice");
		Assert::IsNotNull(pInfo);
		Assert::AreEqual(1UL, pInfo->id);
		Assert::IsNull(p->GetMsgInfo("carol"));
		Assert::IsFalse(p->DelMsg((LPCTSTR)NULL));
		Assert::IsFalse(p->DelMsg("carol"));
		Assert::IsTrue(p->DelMsg("alice"));
		Assert::IsNull(p->GetMsgInfo("alice"));
		Assert::AreEqual(2UL, p->GetMsgID(0));
		Assert::IsTrue(p->DelMsg((OBJID)2));
		Assert::IsFalse(p->IsValid(0));
		p->Release();
	}
};
