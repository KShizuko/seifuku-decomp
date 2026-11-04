#include "stdafx.h"
#include "SayHello.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CSayHello) == 0x404, "");

struct SayHelloProbe : CSayHello
{
    static size_t TimeStart() { return offsetof(SayHelloProbe, m_dwTimeStart); }
    static int Line(const char* pszWords, int* pnOffset, char* pszLine, int nLineSize) { SayHelloProbe o; return o.GetLine(pszWords, pnOffset, pszLine, nLineSize); }
    static size_t Words() { return offsetof(SayHelloProbe, m_szWords); }
};

TEST_CLASS(SayHelloLines)
{
public:
    TEST_METHOD(TheRestThatFitsIsOneLine)
    {
        Assert::IsTrue(SayHelloProbe::Words() == 0 && SayHelloProbe::TimeStart() == 0x400);
        char szLine[32]; int nOffset = 0;
        Assert::AreEqual(5, SayHelloProbe::Line("hello", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "hello"));
        Assert::AreEqual(5, nOffset);
        Assert::AreEqual(0, SayHelloProbe::Line("hello", &nOffset, szLine, 10));
        Assert::AreEqual(0, SayHelloProbe::Line(NULL, &nOffset, szLine, 10));
        Assert::AreEqual(0, SayHelloProbe::Line("hello", &nOffset, NULL, 10));
    }

    TEST_METHOD(ALongTextBreaksAfterTheLastBlank)
    {
        char szLine[32]; int nOffset = 0;
        Assert::AreEqual(9, SayHelloProbe::Line("hello big world", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "hello big"));
        Assert::AreEqual(9, nOffset);
        Assert::AreEqual(6, SayHelloProbe::Line("hello big world", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, " world"));
        nOffset = 0;
        Assert::AreEqual(6, SayHelloProbe::Line("hello brave world", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "hello "));
        Assert::AreEqual(6, nOffset);
        nOffset = 0;
        Assert::AreEqual(9, SayHelloProbe::Line("abcdefghijklmnop", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "abcdefghi"));
    }

    TEST_METHOD(PunctuationAndEmotionsAreKeptWhole)
    {
        char szLine[32]; int nOffset = 0;
        Assert::AreEqual(9, SayHelloProbe::Line("hi there.and more", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "hi there."));
        nOffset = 0;
        Assert::AreEqual(7, SayHelloProbe::Line("abcdefg#12xyz", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "abcdefg"));
        Assert::AreEqual(7, nOffset);
        nOffset = 0;
        Assert::AreEqual(8, SayHelloProbe::Line("abcdefgh#12xyz", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "abcdefgh"));
        nOffset = 0;
        Assert::AreEqual(9, SayHelloProbe::Line("abcdef#12xyz", &nOffset, szLine, 10));
        Assert::AreEqual(0, strcmp(szLine, "abcdef#12"));
    }

    TEST_METHOD(SetWordsCopiesSixtyFourBytes)
    {
        CSayHello hello;
        memset(&hello, 0, sizeof(hello));
        char szLong[1100]; memset(szLong, 'x', sizeof(szLong)); szLong[1099] = 0;
        hello.SetWords(szLong);
        Assert::AreEqual((char) 0, ((char*) &hello)[SayHelloProbe::Words()]);
        char szSeventy[71]; memset(szSeventy, 'y', 70); szSeventy[70] = 0;
        hello.SetWords(szSeventy);
        Assert::AreEqual('y', ((char*) &hello)[SayHelloProbe::Words() + 63]);
        Assert::AreEqual((char) 0, ((char*) &hello)[SayHelloProbe::Words() + 64]);
        hello.SetWords(NULL);
    }
};
