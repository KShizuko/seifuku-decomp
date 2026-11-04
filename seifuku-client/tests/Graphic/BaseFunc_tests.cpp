#include "CppUnitTest.h"
#include <cstring>
#include <type_traits>
#include "BaseFunc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(std::is_same<decltype(&Str2ID), DWORD (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&ReplaceString), void (*)(char*, char, char)>::value, "");
static_assert(std::is_same<decltype(&RepairString), void (*)(char*)>::value, "");

TEST_CLASS(GraphicBaseFunc)
{
public:
    TEST_METHOD(Str2IDIsTimes33PlusTheSignedByte)
    {
        Assert::AreEqual<DWORD>(0, Str2ID(""));
        Assert::AreEqual<DWORD>('a', Str2ID("a"));
        Assert::AreEqual<DWORD>('a' * 33 + 'b', Str2ID("ab"));
        Assert::AreEqual<DWORD>(('a' * 33 + 'b') * 33 + 'c', Str2ID("abc"));
        Assert::AreEqual<DWORD>((DWORD) (int) (signed char) 0xB0, Str2ID("\xB0"));
        Assert::AreEqual<DWORD>((DWORD) ('a' * 33 + (int) (signed char) 0xB0), Str2ID("a\xB0"));
    }

    TEST_METHOD(ReplaceStringSwapsEveryMatchWithinALimit)
    {
        char s[] = "a/b/c";
        ReplaceString(s, '/', '\\');
        Assert::AreEqual("a\\b\\c", s);
        ReplaceString(0, 'a', 'b');
        char empty[] = "";
        ReplaceString(empty, 'a', 'b');
        Assert::AreEqual("", empty);

        static char big[1026];
        memset(big, 'x', 1025); big[1025] = 0;
        ReplaceString(big, 'x', 'y');
        Assert::AreEqual('x', big[0]); Assert::AreEqual('x', big[1024]);
        big[1024] = 0;
        ReplaceString(big, 'x', 'y');
        Assert::AreEqual('y', big[0]); Assert::AreEqual('y', big[1023]);
    }

    TEST_METHOD(RepairStringDoesNothing)
    {
        char s[] = "\xB0\xA1 text\xB0";
        char copy[sizeof(s)];
        memcpy(copy, s, sizeof(s));
        RepairString(s);
        Assert::IsTrue(memcmp(s, copy, sizeof(s)) == 0);
    }
};
