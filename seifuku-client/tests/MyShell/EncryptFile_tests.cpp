#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <type_traits>
#include "EncryptFile.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CEncryptFile) == 0x90, "");
static_assert(std::has_virtual_destructor<CEncryptFile>::value, "");
static_assert(std::is_same<decltype(&CEncryptFile::Open), bool (CEncryptFile::*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&CEncryptFile::ReadLine), int (CEncryptFile::*)(char*, int)>::value, "");

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

TEST_CLASS(ClientEncryptFile)
{
public:
    TEST_METHOD(OpenDecodesTheWholeFile)
    {
        char path[512];
        CEncryptFile file(0x2537);
        Assert::IsTrue(file.Open(FixturePath(path, sizeof(path), "encryptfile.dat")));
        Assert::IsTrue(file.GetSize() == 44);
        Assert::IsTrue(file.GetPos() == 0);
        Assert::IsTrue(memcmp(file.GetBuffer(), "Amount=2\r\n1001 First 7 8\r\n1002 Second 9 10\r\n", 44) == 0);
    }

    TEST_METHOD(ReadLineWalksTheLinesAndSkipsTheBreaks)
    {
        char path[512];
        CEncryptFile file(0x2537);
        Assert::IsTrue(file.Open(FixturePath(path, sizeof(path), "encryptfile.dat")));
        char line[64];
        int n = file.ReadLine(line, sizeof(line));
        Assert::IsTrue(strcmp(line, "Amount=2") == 0);
        Assert::IsTrue(n == 7);
        Assert::IsTrue(file.GetPos() == 10);
        Assert::IsFalse(file.IsEnd());
        file.ReadLine(line, sizeof(line));
        Assert::IsTrue(strcmp(line, "1001 First 7 8") == 0);
        file.ReadLine(line, sizeof(line));
        Assert::IsTrue(strcmp(line, "1002 Second 9 10") == 0);
        Assert::IsTrue(file.IsEnd());
    }

    TEST_METHOD(ReadLineIsBoundedAndZeroesTheBuffer)
    {
        char path[512];
        CEncryptFile file(0x2537);
        Assert::IsTrue(file.Open(FixturePath(path, sizeof(path), "encryptfile.dat")));
        char line[8];
        memset(line, 'x', sizeof(line));
        int n = file.ReadLine(line, 4);
        Assert::IsTrue(n == 4);
        Assert::IsTrue(memcmp(line, "Amou", 4) == 0 && line[4] == 'x');
    }

    TEST_METHOD(NothingOpenReadsNothing)
    {
        CEncryptFile file(0x2537);
        char line[8] = "abc";
        Assert::IsTrue(file.ReadLine(line, sizeof(line)) == 0);
        Assert::IsTrue(strcmp(line, "abc") == 0);
        Assert::IsFalse(file.Open(NULL));
        Assert::IsFalse(file.Open("fixtures\\no-such-file.dat"));
    }
};
