#include "stdafx.h"
#include "TxtMsgSet.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

TEST_CLASS(TxtMsgSetBodies)
{
public:
    TEST_METHOD(ConstructedSetIsEmpty)
    {
        CTxtMsgSet set;
        Assert::AreEqual((DWORD) 0, set.GetMsgAmount());
        Assert::IsNull(set.GetLastMsg());
    }

    TEST_METHOD(AddThenTakeInOrder)
    {
        CTxtMsgSet set;
        Assert::IsTrue(set.AddMsg("one") != FALSE);
        Assert::IsTrue(set.AddMsg("two") != FALSE);
        Assert::IsTrue(set.AddMsg("three") != FALSE);
        Assert::AreEqual((DWORD) 3, set.GetMsgAmount());
        Assert::AreEqual("one", set.GetLastMsg());
        Assert::AreEqual("two", set.GetLastMsg());
        Assert::AreEqual("three", set.GetLastMsg());
        Assert::IsNull(set.GetLastMsg());
        Assert::AreEqual((DWORD) 3, set.GetMsgAmount());
    }

    TEST_METHOD(RejectsNullAndOverlongText)
    {
        CTxtMsgSet set;
        Assert::IsFalse(set.AddMsg(NULL) != FALSE);
        char szLong[257];
        memset(szLong, 'x', 256); szLong[256] = 0;
        Assert::IsFalse(set.AddMsg(szLong) != FALSE);
        szLong[255] = 0;
        Assert::IsTrue(set.AddMsg(szLong) != FALSE);
        Assert::AreEqual((DWORD) 1, set.GetMsgAmount());
        Assert::AreEqual(szLong, set.GetLastMsg());
    }

    TEST_METHOD(FullSetDropsTheOldestAndPullsTheTakeBack)
    {
        CTxtMsgSet set;
        set.SetMaxMsg(2);
        set.AddMsg("a");
        set.AddMsg("b");
        Assert::AreEqual("a", set.GetLastMsg());
        set.AddMsg("c");
        Assert::AreEqual((DWORD) 2, set.GetMsgAmount());
        Assert::AreEqual("b", set.GetLastMsg());
        Assert::AreEqual("c", set.GetLastMsg());
        Assert::IsNull(set.GetLastMsg());
    }

    TEST_METHOD(SetMaxMsgDropsFromTheFront)
    {
        CTxtMsgSet set;
        set.AddMsg("1"); set.AddMsg("2"); set.AddMsg("3"); set.AddMsg("4");
        Assert::AreEqual("1", set.GetLastMsg());
        Assert::AreEqual("2", set.GetLastMsg());
        set.SetMaxMsg(2);
        Assert::AreEqual((DWORD) 2, set.GetMsgAmount());
        Assert::AreEqual("3", set.GetLastMsg());
        Assert::AreEqual("4", set.GetLastMsg());
        set.SetMaxMsg(10);
        Assert::AreEqual((DWORD) 2, set.GetMsgAmount());
    }

    TEST_METHOD(ClearMsgEmptiesTheSet)
    {
        CTxtMsgSet set;
        set.AddMsg("x"); set.AddMsg("y");
        set.GetLastMsg();
        set.ClearMsg();
        Assert::AreEqual((DWORD) 0, set.GetMsgAmount());
        Assert::IsNull(set.GetLastMsg());
        set.AddMsg("z");
        Assert::AreEqual("z", set.GetLastMsg());
        set.ClearMsg();
        set.ClearMsg();
        Assert::AreEqual((DWORD) 0, set.GetMsgAmount());
    }

    TEST_METHOD(GetMsgReadsByIndexWithoutMovingTheCursor)
    {
        CTxtMsgSet set;
        Assert::IsNull(set.GetMsg(0));
        set.AddMsg("p"); set.AddMsg("q");
        Assert::AreEqual("q", set.GetMsg(1));
        Assert::AreEqual("p", set.GetMsg(0));
        Assert::IsNull(set.GetMsg(2));
        Assert::IsNull(set.GetMsg((DWORD) -1));
        Assert::AreEqual("p", set.GetLastMsg());
    }
};
