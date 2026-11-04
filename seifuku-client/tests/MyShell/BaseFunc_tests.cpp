#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "BaseFunc.h"
#include "NetworkDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CMouseInfo) == 16 && offsetof(CMouseInfo, dwDownTime) == 12, "");
static_assert(_MOUSE_CLICK == 10 && _MOUSE_RIGHTCLICK == -10 && _MOUSE_ON == 20, "");

static_assert(sizeof(OBJID) == 4, "");
static_assert(sizeof(CMyPos) == 8 && sizeof(CMyRect) == 16 && sizeof(CMy3DDir) == 8 && sizeof(CMy3DPos) == 12 && sizeof(CMy3DPose) == 12 && sizeof(CMySize) == 8, "");
static_assert(_MAX_NAMESIZE == 16 && _MAX_NAMESIZE_CLI == 32 && _MAX_STRING == 256, "");
static_assert(_STATUS_EXIT == 1100, "");
static_assert(_STATUS_NONE == 1000 && _STATUS_NORMAL == 1010 && _STATUS_DESTROY == 1101, "");
static_assert(WM_MY_MESSAGE == WM_USER + 100 && CMD_NORMAL == 1000, "");
static_assert(std::is_same<decltype(&LogMsg), void (*)(const char*, ...)>::value, "");
static_assert(_VERSION == 121, "");

static_assert(_MAX_MSGSIZE == 1024, "");
static_assert(_PORT_ACCOUNTSERVER == 9958 && _PORT_GAMESERVER == 5816, "");
static_assert(_MSG_GENERAL == 1000 && _MSG_LOGIN == 1002 && _MSG_TALK == 1004 && _MSG_ACTION == 1010, "");
static_assert(_MSG_LOGINREPLY == 1055, "");
static_assert(_MSG_MAPITEM == 1101 && _MSG_DICE == 1113 && _MSG_NPCINFO == 2030 && _MSG_DATAARRAY == 2036, "");
static_assert(_TXTATR_NORMAL == 2000 && _TXTATR_SYSTEM == 2005, "");

TEST_CLASS(ClientBaseFunc)
{
public:
    TEST_METHOD(ExchangeShortBitsRotatesTheLowWordRight)
    {
        Assert::AreEqual(0x1234u, ExchangeShortBits(0x1234, 0));
        Assert::AreEqual(0x4123u, ExchangeShortBits(0x1234, 4));
        Assert::AreEqual(0x3412u, ExchangeShortBits(0x1234, 8));
        Assert::AreEqual(0x2341u, ExchangeShortBits(0x1234, 12));
        Assert::AreEqual(0x4123u, ExchangeShortBits(0xABCD1234, 4));
        Assert::AreEqual(0x0002u, ExchangeShortBits(0x0001, 15));
    }

    TEST_METHOD(ExchangeLongBitsRotatesTheDwordRight)
    {
        Assert::AreEqual(0x12345678u, ExchangeLongBits(0x12345678, 0));
        Assert::AreEqual(0x81234567u, ExchangeLongBits(0x12345678, 4));
        Assert::AreEqual(0x56781234u, ExchangeLongBits(0x12345678, 16));
        Assert::AreEqual(0x23456781u, ExchangeLongBits(0x12345678, 28));
        Assert::AreEqual(0x00000002u, ExchangeLongBits(0x00000001, 31));
    }

    TEST_METHOD(MouseEventCodesAreSignedPairs)
    {
        Assert::AreEqual(-_MOUSE_CLICK, _MOUSE_RIGHTCLICK);
        Assert::AreEqual(-_MOUSE_DOWN, _MOUSE_RIGHTDOWN);
        Assert::AreEqual(-_MOUSE_HOLD, _MOUSE_RIGHTHOLD);
    }

    TEST_METHOD(GameIsDBCSLeadByteOverloadIsDeclared)
    {
        static_assert(std::is_same<decltype(IsDBCSLeadByte((const char*) 0, 0)), BOOL>::value, "");
        static_assert(std::is_same<decltype(IsDBCSLeadByte((BYTE) 0)), BOOL>::value, "");
        Assert::IsTrue(true);
    }
};

static const char* FixturePath(char* buf, size_t cb, const char* name)
{
    const char* file = __FILE__;
    const char* slash = strrchr(file, '\\');
    const char* slash2 = strrchr(file, '/');
    if (slash2 > slash) slash = slash2;
    size_t n = slash ? (size_t)(slash - file + 1) : 0;
    if (n + strlen("fixtures\\") + strlen(name) + 1 > cb) { buf[0] = 0; return buf; }
    memcpy(buf, file, n);
    buf[n] = 0;
    strcat(buf, "fixtures\\");
    strcat(buf, name);
    return buf;
}

TEST_CLASS(ClientBaseFuncBodies)
{
public:
    TEST_METHOD(Double2IntRoundsHalfUp)
    {
        Assert::AreEqual(1, Double2Int(0.5));
        Assert::AreEqual(1, Double2Int(1.25));
        Assert::AreEqual(2, Double2Int(1.5));
        Assert::AreEqual(0, Double2Int(0.25));
    }

    TEST_METHOD(ReplaceStringSwapsEveryMatchWithinTheLimit)
    {
        char s[] = "a[b]c[d]";
        ReplaceString(s, '[', '?');
        Assert::AreEqual("a?b]c?d]", s);
        ReplaceString(NULL, 'x', 'y');
        char big[1030];
        memset(big, 'x', sizeof(big) - 1); big[sizeof(big) - 1] = 0;
        ReplaceString(big, 'x', 'y');
        Assert::AreEqual('x', big[0]);
    }

    TEST_METHOD(RandGetOfZeroIsZero)
    {
        Assert::AreEqual(0, RandGet(0));
        int r = RandGet(5);
        Assert::IsTrue(r >= 0 && r < 5);
    }

    TEST_METHOD(Str2IDIsTheTimes33HashOverSignedBytes)
    {
        Assert::AreEqual(0u, (unsigned)Str2ID(""));
        Assert::AreEqual(97u, (unsigned)Str2ID("a"));
        Assert::AreEqual(97u * 33 + 98, (unsigned)Str2ID("ab"));
        Assert::AreEqual(0xFFFFFFB0u, (unsigned)Str2ID("\xB0"));
    }

    TEST_METHOD(GetDateIsADayOfMonth)
    {
        DWORD d = GetDate();
        Assert::IsTrue(d >= 1 && d <= 31);
    }

    TEST_METHOD(MouseSetCheckProcessRoundTrip)
    {
        int x = 0, y = 0;
        MouseSet(12, 34, _MOUSE_CLICK);
        Assert::AreEqual((int)_MOUSE_CLICK, MouseCheck(x, y));
        Assert::AreEqual(12, x); Assert::AreEqual(34, y);
        MouseProcess();
        Assert::AreEqual((int)_MOUSE_NONE, MouseCheck(x, y));
    }

    TEST_METHOD(IniStrGetAndIniDataGetReadTheFixture)
    {
        char path[512];
        FixturePath(path, sizeof(path), "basefunc.ini");
        char value[256] = "";
        Assert::IsTrue(IniStrGet(path, "Section", "Key", value) != 0);
        Assert::AreEqual("Value", value);
        int n = 0;
        Assert::IsTrue(IniDataGet(path, "Section", "Number", n) != 0);
        Assert::AreEqual(42, n);
        Assert::IsTrue(IniDataGet(path, "Other", "Number", n) != 0);
        Assert::AreEqual(7, n);
        Assert::IsTrue(IniStrGet(path, "Other", "Name", value) != 0);
        Assert::AreEqual("Seifuku", value);
        Assert::IsTrue(IniStrGet(path, "Other", "Key", value) == 0);
        Assert::IsTrue(IniStrGet(path, "Missing", "Key", value) == 0);
        Assert::IsTrue(IniStrGet(NULL, "Section", "Key", value) == 0);
        Assert::IsTrue(IniDataGet(path, "Section", NULL, n) == 0);
        Assert::IsTrue(IniStrGet("no-such-file.ini", "Section", "Key", value) == 0);
    }
    TEST_METHOD(RepairStringLeavesTheStringAlone)
    {
        char s[8] = { 'a', 'b', (char)0xB0, 0, 0, 0, 0, 0 };
        RepairString(s);
        Assert::AreEqual('a', s[0]);
        Assert::AreEqual('b', s[1]);
        Assert::AreEqual((char)0xB0, s[2]);
        Assert::AreEqual((char)0, s[3]);
        RepairString(NULL);
    }
    TEST_METHOD(GameIsDBCSLeadByteWalksTheLeadBytes)
    {
        const char s[] = "a\xB0\xA1\xB0\xA1z";
        BOOL bLead = ::IsDBCSLeadByte((BYTE) 0xB0);
        Assert::IsFalse(IsDBCSLeadByte(s, 0) != FALSE);
        Assert::IsTrue((IsDBCSLeadByte(s, 1) != FALSE) == (bLead != FALSE));
        Assert::IsFalse(IsDBCSLeadByte(s, 2) != FALSE);
        Assert::IsTrue((IsDBCSLeadByte(s, 3) != FALSE) == (bLead != FALSE));
        Assert::IsFalse(IsDBCSLeadByte(s, 5) != FALSE);
        Assert::IsFalse(IsDBCSLeadByte(s, 6) != FALSE);
        Assert::IsFalse(IsDBCSLeadByte((const char*) NULL, 0) != FALSE);
        Assert::IsFalse(IsDBCSLeadByte(s, -1) != FALSE);
    }
    TEST_METHOD(SafeCopyCutsToTheSizeAndTerminates)
    {
        char buf[8];
        memset(buf, 'x', sizeof(buf));
        SafeCopy(buf, "abc", 8);
        Assert::AreEqual("abc", buf);
        memset(buf, 'x', sizeof(buf));
        SafeCopy(buf, "abcdefgh", 8);
        Assert::AreEqual("abcdefg", buf);
        memset(buf, 'x', sizeof(buf));
        SafeCopy(buf, "abcdefghijkl", 4);
        Assert::AreEqual("abc", buf);
        Assert::AreEqual('x', buf[4]);
        memset(buf, 'x', sizeof(buf));
        SafeCopy(buf, "abc", 0);
        Assert::AreEqual('x', buf[0]);
        SafeCopy(buf, NULL, 8);
        Assert::AreEqual('x', buf[0]);
        SafeCopy(NULL, "abc", 8);
    }
};
