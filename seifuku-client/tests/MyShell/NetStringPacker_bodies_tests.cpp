#include "stdafx.h"
#include "NetStringPacker.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

TEST_CLASS(NetStringPackerBodies)
{
public:
    TEST_METHOD(NoBufferRefusesEverything)
    {
        CNetStringPacker packer;
        char out[8];
        Assert::IsFalse(packer.AddString("a") != FALSE);
        Assert::IsFalse(packer.GetString(0, out, sizeof(out)) != FALSE);
    }

    TEST_METHOD(SetBufStartsEmpty)
    {
        char buf[32];
        memset(buf, 0x55, sizeof(buf));
        CNetStringPacker packer;
        packer.SetBuf(buf, sizeof(buf));
        Assert::AreEqual((UINT) 0, packer.GetStrAmount());
        Assert::AreEqual((UINT) 1, packer.GetSize());
        Assert::AreEqual((int) 0, (int) buf[31]);
    }

    TEST_METHOD(AddAndGetStrings)
    {
        char buf[64];
        CNetStringPacker packer;
        packer.SetBuf(buf, sizeof(buf));
        Assert::IsTrue(packer.AddString("ab") != FALSE);
        Assert::AreEqual((UINT) 1, packer.GetStrAmount());
        Assert::AreEqual((UINT) 4, packer.GetSize());
        Assert::AreEqual((int) 1, (int) (unsigned char) buf[0]);
        Assert::AreEqual((int) 2, (int) (unsigned char) buf[1]);
        Assert::IsTrue(packer.AddString(NULL) != FALSE);
        Assert::AreEqual((UINT) 2, packer.GetStrAmount());
        Assert::AreEqual((UINT) 5, packer.GetSize());
        Assert::IsTrue(packer.AddString("cde") != FALSE);
        Assert::AreEqual((UINT) 3, packer.GetStrAmount());
        Assert::AreEqual((UINT) 9, packer.GetSize());

        char out[16];
        Assert::IsTrue(packer.GetString(0, out, sizeof(out)) != FALSE);
        Assert::AreEqual("ab", out);
        Assert::IsTrue(packer.GetString(1, out, sizeof(out)) != FALSE);
        Assert::AreEqual("", out);
        Assert::IsTrue(packer.GetString(2, out, sizeof(out)) != FALSE);
        Assert::AreEqual("cde", out);
        Assert::IsFalse(packer.GetString(3, out, sizeof(out)) != FALSE);
    }

    TEST_METHOD(GetStringNeedsRoomForTheTerminator)
    {
        char buf[32];
        CNetStringPacker packer;
        packer.SetBuf(buf, sizeof(buf));
        packer.AddString("abc");
        char out[4];
        Assert::IsFalse(packer.GetString(0, out, 3) != FALSE);
        Assert::IsTrue(packer.GetString(0, out, 4) != FALSE);
        Assert::AreEqual("abc", out);
    }

    TEST_METHOD(LimitsOfLengthAndCapacity)
    {
        char big[300];
        memset(big, 'x', 256); big[256] = 0;
        char buf[16];
        CNetStringPacker packer;
        packer.SetBuf(buf, sizeof(buf));
        Assert::IsFalse(packer.AddString(big) != FALSE);
        Assert::AreEqual((UINT) 0, packer.GetStrAmount());

        Assert::IsTrue(packer.AddString("abcdef") != FALSE);
        Assert::IsTrue(packer.AddString("ghijk") != FALSE);
        Assert::AreEqual((UINT) 14, packer.GetSize());
        Assert::IsFalse(packer.AddString("lmn") != FALSE);
        Assert::AreEqual((UINT) 2, packer.GetStrAmount());
    }
};
