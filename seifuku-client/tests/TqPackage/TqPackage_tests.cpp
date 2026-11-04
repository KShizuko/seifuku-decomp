#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <type_traits>
#include "CppUnitTest.h"
#include "TqPackage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

DWORD string_id(const char* filename);

static_assert(std::is_same<decltype(&TqPackagesOpen), TQPRESULT (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&TqPackagesClose), void (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&TqFCheck), bool (*)(const char*, FILETYPE)>::value, "");
static_assert(std::is_same<decltype(&TqFDump), TQPRESULT (*)(const char*, void*&, unsigned long&)>::value, "");
static_assert(std::is_same<decltype(&TqFUndump), TQPRESULT (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&TqFOpen), TQPRESULT (*)(const char*, void*&)>::value, "");
static_assert(std::is_same<decltype(&TqFClose), TQPRESULT (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&TQFRead), long (*)(void*, unsigned long, unsigned long, void*)>::value, "");
static_assert(std::is_same<decltype(&TqFSeek), bool (*)(void*, long, TQSEEKMODE)>::value, "");
static_assert(std::is_same<decltype(&TqFTell), unsigned long (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&TqFIsEof), bool (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&TqFGetLen), long (*)(void*)>::value, "");

namespace
{
    std::string FixtureDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void WriteBlob(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        Assert::IsTrue(h != INVALID_HANDLE_VALUE, L"fixture write");
        DWORD w = 0;
        WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    void BuildPackage(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        const char* a = "alpha-content";
        const char* b = "bravo-content-longer";
        struct { DWORD id; int number; unsigned offset; } hdr = { 0, 2, 12 };
        struct { DWORD uid, offset, size, space; } idx[2];
        idx[0].uid = string_id("tqp/a.txt"); idx[0].offset = 12 + 32;      idx[0].size = 13; idx[0].space = 0;
        idx[1].uid = string_id("tqp/b.txt"); idx[1].offset = 12 + 32 + 13; idx[1].size = 20; idx[1].space = 0;
        std::string blob((const char*) &hdr, sizeof(hdr));
        blob.append((const char*) idx, sizeof(idx));
        blob.append(a).append(b);
        WriteBlob(dir + "\\tqp.wdf", blob.data(), (DWORD) blob.size());

        CreateDirectoryA((dir + "\\tqp").c_str(), 0);
        CreateDirectoryA((dir + "\\tqp\\sub").c_str(), 0);
        WriteBlob(dir + "\\tqp\\loose.txt", "loose!", 6);
        WriteBlob(dir + "\\tqp\\sub\\Deep.TXT", "deep", 4);
    }

    struct Package
    {
        char old[MAX_PATH];
        Package()
        {
            static bool open = false;
            std::string dir = FixtureDir();
            if (!open) BuildPackage(dir);
            GetCurrentDirectoryA(MAX_PATH, old);
            SetCurrentDirectoryA(dir.c_str());
            if (!open) { Assert::AreEqual<int>(TQP_OK, TqPackagesOpen("TQP.wdf")); open = true; }
        }
        ~Package() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(TqPackageExports)
{
public:
    TEST_METHOD(StringIdFoldsCaseAndSlashes)
    {
        Assert::AreEqual(string_id("abc/d"), string_id("ABC\\D"));
        Assert::AreNotEqual(string_id("abc/d"), string_id("abc/e"));
    }

    TEST_METHOD(CheckKnowsPackedAndLooseFiles)
    {
        Package p;
        Assert::IsTrue(TqFCheck("tqp/a.txt", FILETYPE_ANY));
        Assert::IsTrue(TqFCheck("tqp/a.txt", FILETYPE_PACK));
        Assert::IsFalse(TqFCheck("tqp/a.txt", FILETYPE_UNPACK));
        Assert::IsTrue(TqFCheck("tqp/loose.txt", FILETYPE_ANY));
        Assert::IsTrue(TqFCheck("tqp/loose.txt", FILETYPE_UNPACK));
        Assert::IsFalse(TqFCheck("tqp/loose.txt", FILETYPE_PACK));
        Assert::IsTrue(TqFCheck("TQP\\SUB\\DEEP.TXT", FILETYPE_UNPACK));
        Assert::IsTrue(TqFCheck("Tqp\\B.txt", FILETYPE_PACK));
        Assert::IsFalse(TqFCheck("tqp/missing.txt", FILETYPE_ANY));
        Assert::IsFalse(TqFCheck("other/a.txt", FILETYPE_ANY));
        Assert::IsFalse(TqFCheck("tqp/a.txt", (FILETYPE) 3));
        Assert::IsFalse(TqFCheck("", FILETYPE_ANY));
        Assert::IsFalse(TqFCheck(0, FILETYPE_ANY));
    }

    TEST_METHOD(DumpReadsWholeFiles)
    {
        Package p;
        void* buf = 0; unsigned long size = 0;
        Assert::AreEqual<int>(TQP_OK, TqFDump("tqp/a.txt", buf, size));
        Assert::AreEqual<unsigned long>(13, size);
        Assert::AreEqual(0, memcmp(buf, "alpha-content", 13));
        Assert::AreEqual<int>(TQP_OK, TqFUndump(buf));

        buf = 0; size = 0;
        Assert::AreEqual<int>(TQP_OK, TqFDump("tqp/loose.txt", buf, size));
        Assert::AreEqual<unsigned long>(6, size);
        Assert::AreEqual(0, memcmp(buf, "loose!", 6));
        Assert::AreEqual<int>(TQP_OK, TqFUndump(buf));

        buf = 0;
        Assert::AreEqual<int>(TQP_NOTFOUND, TqFDump("tqp/missing.txt", buf, size));
        Assert::IsNull(buf);
        Assert::AreEqual<int>(TQP_NOTFOUND, TqFDump("other/a.txt", buf, size));
        Assert::AreEqual<int>(TQP_INVALID, TqFDump("", buf, size));
        Assert::AreEqual<int>(TQP_INVALID, TqFDump(0, buf, size));
        Assert::AreEqual<int>(TQP_OK, TqFUndump(0));
    }

    TEST_METHOD(PackedFileReadSeekTell)
    {
        Package p;
        void* h = 0;
        Assert::AreEqual<int>(TQP_OK, TqFOpen("tqp/b.txt", h));
        Assert::IsNotNull(h);
        Assert::AreEqual<long>(20, TqFGetLen(h));
        Assert::AreEqual<unsigned long>(0, TqFTell(h));
        Assert::IsFalse(TqFIsEof(h));

        char buf[32] = { 0 };
        Assert::AreEqual<long>(5, TQFRead(buf, 1, 5, h));
        Assert::AreEqual(0, memcmp(buf, "bravo", 5));
        Assert::AreEqual<unsigned long>(5, TqFTell(h));
        Assert::AreEqual<long>(3, TQFRead(buf, 2, 3, h));
        Assert::AreEqual(0, memcmp(buf, "-conte", 6));

        Assert::IsTrue(TqFSeek(h, -7, TQSEEK_END));
        Assert::AreEqual<unsigned long>(13, TqFTell(h));
        Assert::AreEqual<long>(7, TQFRead(buf, 1, 7, h));
        Assert::AreEqual(0, memcmp(buf, "-longer", 7));
        Assert::IsTrue(TqFIsEof(h));
        Assert::AreEqual<long>(-1, TQFRead(buf, 1, 1, h));

        Assert::IsTrue(TqFSeek(h, 0, TQSEEK_SET));
        Assert::IsTrue(TqFSeek(h, 2, TQSEEK_CUR));
        Assert::AreEqual<unsigned long>(2, TqFTell(h));
        Assert::AreEqual<long>(-1, TQFRead(buf, 1, 19, h));
        Assert::AreEqual<unsigned long>(2, TqFTell(h));
        Assert::AreEqual<int>(TQP_OK, TqFClose(h));
    }

    TEST_METHOD(LooseFileReadSeekTell)
    {
        Package p;
        void* h = 0;
        Assert::AreEqual<int>(TQP_OK, TqFOpen("tqp/loose.txt", h));
        Assert::IsNotNull(h);
        Assert::AreEqual<long>(6, TqFGetLen(h));

        char buf[32] = { 0 };
        Assert::AreEqual<long>(4, TQFRead(buf, 1, 4, h));
        Assert::AreEqual(0, memcmp(buf, "loos", 4));
        Assert::AreEqual<unsigned long>(4, TqFTell(h));
        Assert::IsFalse(TqFIsEof(h));
        Assert::AreEqual<long>(2, TQFRead(buf, 1, 10, h));
        Assert::IsTrue(TqFIsEof(h));
        Assert::IsTrue(TqFSeek(h, 0, TQSEEK_SET));
        Assert::AreEqual<unsigned long>(0, TqFTell(h));
        Assert::AreEqual<int>(TQP_OK, TqFClose(h));
    }

    TEST_METHOD(OpenFailuresAndNullHandles)
    {
        Package p;
        void* h = (void*) 1;
        Assert::AreEqual<int>(TQP_NOTFOUND, TqFOpen("other/a.txt", h));
        Assert::IsNull(h);
        Assert::AreEqual<int>(TQP_NOTFOUND, TqFOpen("tqp/missing.txt", h));
        Assert::AreEqual<int>(TQP_INVALID, TqFOpen("", h));
        Assert::AreEqual<int>(TQP_INVALID, TqFOpen(0, h));

        char buf[4];
        Assert::AreEqual<int>(TQP_INVALID, TqFClose(0));
        Assert::AreEqual<long>(-1, TQFRead(buf, 1, 1, 0));
        Assert::IsFalse(TqFSeek(0, 0, TQSEEK_SET));
        Assert::AreEqual<unsigned long>(0xFFFFFFFF, TqFTell(0));
        Assert::IsTrue(TqFIsEof(0));
        Assert::AreEqual<long>(-1, TqFGetLen(0));
    }
};
