#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include "c3_datafile.h"
#include "c3_common.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3DataFileIndex) == 0x10, "");
static_assert(offsetof(C3DataFileIndex, uid) == 0 && offsetof(C3DataFileIndex, offset) == 4 && offsetof(C3DataFileIndex, size) == 8, "");
static_assert(sizeof(C3DataFileHeader) == 0xC, "");
static_assert(offsetof(C3DataFileHeader, number) == 4 && offsetof(C3DataFileHeader, offset) == 8, "");

static_assert(std::is_same<decltype(&string_id), DWORD (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&DataFile_Open), void (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&DataFile_Close), void (*)()>::value, "");
static_assert(std::is_same<decltype(&DataFile_UnpackSearch), BOOL (*)(const char*)>::value, "");
static_assert(std::is_same<decltype(&DataFile_Search), BOOL (*)(const char*, FILE*&, DWORD&)>::value, "");
static_assert(std::is_same<decltype(&DataFile_Load), BOOL (*)(const char*, void*&, DWORD&)>::value, "");
static_assert(std::is_same<decltype(&DataFile_Free), void (*)(void*&)>::value, "");
static_assert(std::is_same<decltype(&Common_OpenRes), void* (*)(LPCSTR, DWORD&, int)>::value, "");
static_assert(std::is_same<decltype(&Common_CloseRes), void (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&Common_ReadRes), DWORD (*)(void*, void*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Common_SeekRes), void (*)(void*, int, int)>::value, "");
static_assert(std::is_same<decltype(&Common_IsEofRes), BOOL (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&Common_GetSeek), long (*)(void*)>::value, "");
static_assert(std::is_same<decltype(&Common_GetChunk), void (*)(void*, ChunkHeader*)>::value, "");

namespace
{
    void BuildLooseFiles(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        CreateDirectoryA((dir + "\\seifuku").c_str(), 0);
        SeifukuFixtures::WriteBlob(dir + "\\seifuku\\loose.txt", "x", 1);
    }

    void BuildPack(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        const char* a = "alpha-content";
        const char* b = "bravo-content-longer";
        C3DataFileIndex idx[2];
        idx[0].uid = string_id("seifuku/a.txt"); idx[0].size = (DWORD) strlen(a); idx[0].space = 0;
        idx[1].uid = string_id("seifuku/b.txt"); idx[1].size = (DWORD) strlen(b); idx[1].space = 0;
        if (idx[0].uid > idx[1].uid) { C3DataFileIndex t = idx[0]; idx[0] = idx[1]; idx[1] = t; const char* s = a; a = b; b = s; }
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 2; hdr.offset = sizeof(hdr);
        DWORD dataStart = sizeof(hdr) + sizeof(idx);
        idx[0].offset = dataStart; idx[1].offset = dataStart + idx[0].size;
        std::string blob((const char*) &hdr, sizeof(hdr));
        blob.append((const char*) idx, sizeof(idx));
        blob.append(a).append(b);
        SeifukuFixtures::WriteBlob(dir + "\\seifuku.wdf", blob.data(), (DWORD) blob.size());
        BuildLooseFiles(dir);
    }
}

TEST_CLASS(C3DataFileFunctions)
{
public:
    TEST_METHOD(StringIdFoldsCaseAndSlashes)
    {
        DWORD abc = string_id("abc");
        Assert::AreEqual(abc, string_id("ABC"));
        Assert::AreNotEqual(abc, string_id("abd"));
        Assert::AreEqual(string_id("a/b"), string_id("A\\B"));
        Assert::AreEqual(string_id(""), string_id(""));
        Assert::AreNotEqual(string_id("data/x"), string_id("x"));
    }

    TEST_METHOD(PackOpenLoadSearchAndFree)
    {
        std::string dir = SeifukuFixtures::Dir();
        BuildPack(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("seifuku.wdf");

        void* p = 0; DWORD size = 0;
        Assert::IsTrue(DataFile_Load("seifuku/a.txt", p, size) != FALSE);
        Assert::IsNotNull(p);
        Assert::AreEqual<DWORD>(13, size);
        Assert::AreEqual(0, memcmp(p, "alpha-content", 13));
        void* kept = p;
        DataFile_Free(p);
        Assert::IsTrue(p == kept);
        Assert::IsTrue(DataFile_Load("SEIFUKU\\B.TXT", p, size) != FALSE);
        Assert::AreEqual<DWORD>(20, size);
        Assert::AreEqual(0, memcmp(p, "bravo-content-longer", 20));
        DataFile_Free(p);
        Assert::IsFalse(DataFile_Load("seifuku/missing.txt", p, size) != FALSE);
        Assert::IsFalse(DataFile_Load("other/a.txt", p, size) != FALSE);
        Assert::IsFalse(DataFile_Load(0, p, size) != FALSE);

        FILE* f = (FILE*) 1; size = 77;
        Assert::IsTrue(DataFile_Search("seifuku/b.txt", f, size) != FALSE);
        Assert::IsTrue(f == (FILE*) 1);
        Assert::AreEqual<DWORD>(77, size);
        Assert::IsFalse(DataFile_Search("seifuku/missing.txt", f, size) != FALSE);
        Assert::IsFalse(DataFile_Search("seifuku/loose.txt", f, size) != FALSE);

        char buf[32] = { 0 };
        void* r = Common_OpenRes("seifuku/a.txt", size, 1);
        Assert::IsNotNull(r);
        Assert::AreEqual<DWORD>(13, size);
        Assert::AreEqual<DWORD>(13, Common_ReadRes(r, buf, 13));
        Assert::AreEqual(0, memcmp(buf, "alpha-content", 13));
        Assert::IsTrue(Common_IsEofRes(r) != FALSE);
        Common_CloseRes(r);
        Assert::IsTrue(DataFile_Load("seifuku/a.txt", p, size) != FALSE);
        DataFile_Free(p);
        Assert::IsTrue(DataFile_UnpackSearch("seifuku/loose.txt") != FALSE);
        Assert::IsTrue(DataFile_UnpackSearch("SEIFUKU\\LOOSE.TXT") != FALSE);
        Assert::IsFalse(DataFile_UnpackSearch("seifuku/nothing") != FALSE);
        Assert::IsFalse(DataFile_UnpackSearch("seifuku/a.txt") != FALSE);
        Assert::IsFalse(DataFile_UnpackSearch(0) != FALSE);
        void* l = Common_OpenRes("seifuku/loose.txt", size, 1);
        Assert::IsNotNull(l);
        Assert::AreEqual<DWORD>(1, size);
        Assert::AreEqual<long>(0, Common_GetSeek(l));
        Assert::IsFalse(Common_IsEofRes(l) != FALSE);
        Common_SeekRes(l, 1, SEEK_CUR);
        Assert::AreEqual<long>(1, Common_GetSeek(l));
        Assert::IsFalse(Common_IsEofRes(l) != FALSE);
        Assert::AreEqual<DWORD>(0, Common_ReadRes(l, buf, 1));
        Assert::IsTrue(Common_IsEofRes(l) != FALSE);
        Common_SeekRes(l, 0, SEEK_SET);
        Assert::AreEqual<DWORD>(1, Common_ReadRes(l, buf, 1));
        Assert::AreEqual('x', buf[0]);
        Common_CloseRes(l);
        Assert::IsNull(Common_OpenRes("seifuku/missing.txt", size, 1));
        Assert::IsNull(Common_OpenRes("seifuku.wdf", size, 0));
        void* w = Common_OpenRes("seifuku/a.txt", size, 0);
        Assert::IsNotNull(w);
        Common_CloseRes(w);
        Assert::AreEqual<long>(-1, Common_GetSeek(0));
        Assert::AreEqual<DWORD>((DWORD) -1, Common_ReadRes(0, buf, 1));
    }

    TEST_METHOD(MissingPackStillRegistersLooseFiles)
    {
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        CreateDirectoryA((dir + "\\missing").c_str(), 0);
        SeifukuFixtures::WriteBlob(dir + "\\missing\\loose.txt", "y", 1);
        DeleteFileA((dir + "\\missing.wdf").c_str());
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("missing.wdf");
        Assert::IsTrue(DataFile_UnpackSearch("missing/loose.txt") != FALSE);
        void* p = 0; DWORD size = 0;
        Assert::IsTrue(DataFile_Load("missing/loose.txt", p, size) != FALSE);
        Assert::AreEqual<DWORD>(1, size);
        Assert::AreEqual('y', *(char*) p);
        DataFile_Free(p);
        FILE* f = 0;
        Assert::IsFalse(DataFile_Search("missing/loose.txt", f, size) != FALSE);
    }
};
