#include "CppUnitTest.h"
#include <cstddef>
#include <string>
#include "c3_common.h"
#include "c3_main.h"
#include "c3_datafile.h"
#include "c3_datafile.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(ChunkHeader) == 8, "");
static_assert(offsetof(ChunkHeader, byChunkID) == 0 && offsetof(ChunkHeader, dwChunkSize) == 4, "");

static_assert(sizeof(D3DXMATRIX) == 64 && sizeof(D3DXVECTOR3) == 12 && sizeof(D3DXVECTOR4) == 16, "");

namespace
{
    struct FakeCom
    {
        int releases;
        FakeCom() : releases(0) {}
        void Release() { releases++; }
    };

    int g_dtors = 0;
    struct Counted
    {
        ~Counted() { g_dtors++; }
    };

    const float EPS = 1e-5f;
    void Near(float e, float a, const wchar_t* w) { Assert::IsTrue(fabsf(e - a) <= EPS, w); }

    void BuildFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        const char* a = "alpha-content";
        C3DataFileIndex idx[1];
        idx[0].uid = string_id("common/a.txt"); idx[0].size = (DWORD) strlen(a); idx[0].space = 0;
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 1; hdr.offset = sizeof(hdr);
        idx[0].offset = sizeof(hdr) + sizeof(idx);
        std::string blob((const char*) &hdr, sizeof(hdr));
        blob.append((const char*) idx, sizeof(idx)).append(a);
        SeifukuFixtures::WriteBlob(dir + "\\common.wdf", blob.data(), (DWORD) blob.size());
        CreateDirectoryA((dir + "\\common").c_str(), 0);
        std::string c3("MAXFILE C3 00001");
        ChunkHeader ch = { { 'T', 'E', 'S', 'T' }, 4 };
        c3.append((const char*) &ch, sizeof(ch)).append("abcd");
        SeifukuFixtures::WriteBlob(dir + "\\common\\chunk.c3", c3.data(), (DWORD) c3.size());
        SeifukuFixtures::WriteBlob(dir + "\\unregistered.c3", c3.data(), (DWORD) c3.size());
    }
}

TEST_CLASS(C3CommonMacros)
{
public:
    TEST_METHOD(SafeReleaseReleasesOnceAndNulls)
    {
        FakeCom com;
        FakeCom* p = &com;
        SafeRelease(p);
        Assert::IsNull(p);
        Assert::AreEqual(1, com.releases);
        SafeRelease(p);
        Assert::AreEqual(1, com.releases);
    }

    TEST_METHOD(SafeDeleteAndSafeDeleteEx)
    {
        g_dtors = 0;
        Counted* one = new Counted;
        SafeDelete(one);
        Assert::IsNull(one);
        Assert::AreEqual(1, g_dtors);
        Counted* many = new Counted[3];
        SafeDeleteEx(many);
        Assert::IsNull(many);
        Assert::AreEqual(4, g_dtors);
        SafeDelete(one);
        SafeDeleteEx(many);
        Assert::AreEqual(4, g_dtors);
    }

    TEST_METHOD(AssertMacroIsAnExpressionStatement)
    {
        int n = 1;
#ifndef _DEBUG
        Assert(n == 0);
#endif
        Assert(n == 1);
        if (n) Assert(n == 1); else Assert(n == 0);
        Assert::AreEqual(1, n);
    }

    TEST_METHOD(ChunkHeaderIdIsRawBytes)
    {
        ChunkHeader h = { { 'P', 'H', 'Y', ' ' }, 0x1234 };
        Assert::AreEqual<unsigned long>(0x1234, h.dwChunkSize);
        Assert::AreEqual(0, memcmp(h.byChunkID, "PHY ", 4));
        Assert::AreEqual<unsigned long>(0x20594850, *(DWORD*) h.byChunkID);
    }
};

TEST_CLASS(C3CommonFunctions)
{
public:
    TEST_METHOD(RandomFloatCmpCutString)
    {
        for (int i = 0; i < 200; i++)
        {
            int r = Random(3, 7);
            Assert::IsTrue(r >= 3 && r <= 7);
        }
        Assert::AreEqual(5, Random(5, 5));
        Assert::AreEqual(0, FloatCmp(1.0f, 1.00001f));
        Assert::AreEqual(1, FloatCmp(2.0f, 1.0f));
        Assert::AreEqual(-1, FloatCmp(1.0f, 2.0f));
        Assert::AreEqual(0, FloatCmp(1.0f, 1.4f, 0.5f));
        char s[32];
        strcpy(s, "a\\bb\\ccc"); CutString(s, 1); Assert::AreEqual("bb\\ccc", s);
        strcpy(s, "a\\bb\\ccc"); CutString(s, 2); Assert::AreEqual("ccc", s);
        strcpy(s, "a\\bb\\ccc"); CutString(s, 0); Assert::AreEqual("a\\bb\\ccc", s);
        strcpy(s, "a\\bb\\ccc"); CutString(s, 5); Assert::AreEqual("a\\bb\\ccc", s);
    }

    TEST_METHOD(MatrixHelpersPostMultiply)
    {
        D3DXMATRIX m, t;
        D3DXMatrixIdentity(&m);
        Common_Translate(&m, 1.0f, 2.0f, 3.0f);
        Common_Scale(&m, 2.0f, 2.0f, 2.0f);
        Near(2.0f, m._41, L"_41"); Near(4.0f, m._42, L"_42"); Near(6.0f, m._43, L"_43"); Near(2.0f, m._11, L"_11");
        D3DXMatrixIdentity(&m);
        Common_Rotate(&m, 0.0f, 0.0f, D3DXToRadian(90.0f));
        D3DXVECTOR3 v;
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 0, 0), &m);
        Near(0.0f, v.x, L"rot x"); Near(1.0f, v.y, L"rot y");
        D3DXMatrixIdentity(&m);
        D3DXVECTOR4 light(0.0f, 10.0f, 0.0f, 1.0f);
        Common_Shadow(&m, &light, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 5, 1), &m);
        Near(2.0f, v.x, L"shadow x"); Near(0.0f, v.y, L"shadow y"); Near(2.0f, v.z, L"shadow z");
        (void) t;
    }

    TEST_METHOD(TranslateAndScaleInPlace4351)
    {
        D3DXMATRIX m;
        D3DXMatrixIdentity(&m);
        m._14 = 0.5f; m._44 = 2.0f; m._41 = 10.0f;
        Common_Translate(&m, 1.0f, 2.0f, 3.0f);
        Near(11.0f, m._41, L"");
        Near(2.0f, m._42, L"_42 += y"); Near(3.0f, m._43, L"_43 += z");
        Near(1.0f, m._11, L""); Near(0.5f, m._14, L"_14 untouched"); Near(2.0f, m._44, L"_44 untouched");
        D3DXMATRIX s;
        D3DXMatrixIdentity(&s);
        s._21 = 1.0f; s._32 = 1.0f; s._43 = 1.0f; s._14 = 0.5f;
        Common_Scale(&s, 2.0f, 3.0f, 4.0f);
        Near(2.0f, s._11, L"_11 * x"); Near(2.0f, s._21, L"_21 * x"); Near(0.0f, s._41, L"_41 * x");
        Near(3.0f, s._22, L"_22 * y"); Near(3.0f, s._32, L"_32 * y");
        Near(4.0f, s._33, L"_33 * z"); Near(4.0f, s._43, L"_43 * z");
        Near(0.5f, s._14, L"the fourth column untouched"); Near(1.0f, s._44, L"_44 untouched");
    }

    TEST_METHOD(IntersectPlaneHitsTheGround)
    {
        D3DXVECTOR3 org(2, 10, 3), dir(0, -1, 0), nor(0, 1, 0), pt(0, 0, 0), res(9, 9, 9);
        Assert::IsTrue(IntersectPlane(org, dir, nor, pt, res) != FALSE);
        Near(2.0f, res.x, L"x"); Near(0.0f, res.y, L"y"); Near(3.0f, res.z, L"z");
        D3DXVECTOR3 along(1, 0, 0), keep(9, 9, 9);
        Assert::IsFalse(IntersectPlane(org, along, nor, pt, keep) != FALSE);
        Near(9.0f, keep.x, L"kept x"); Near(9.0f, keep.y, L"kept y"); Near(9.0f, keep.z, L"kept z");
    }

    TEST_METHOD(IntersectTriangleFacingRayOnly)
    {
        D3DXVECTOR3 v0(0, 0, 0), v1(1, 0, 0), v2(0, 0, 1), down(0, -1, 0), up(0, 1, 0), res(9, 9, 9);
        Assert::IsTrue(IntersectTriangle(D3DXVECTOR3(0.25f, 5, 0.25f), down, v0, v1, v2, res) != FALSE);
        Near(0.25f, res.x, L"x"); Near(0.0f, res.y, L"y"); Near(0.25f, res.z, L"z");
        D3DXVECTOR3 keep(9, 9, 9);
        Assert::IsFalse(IntersectTriangle(D3DXVECTOR3(2, 5, 2), down, v0, v1, v2, keep) != FALSE);
        Assert::IsFalse(IntersectTriangle(D3DXVECTOR3(0.25f, -5, 0.25f), up, v0, v1, v2, keep) != FALSE);
        Assert::IsFalse(IntersectTriangle(D3DXVECTOR3(0.25f, 5, 0.25f), D3DXVECTOR3(1, 0, 0), v0, v1, v2, keep) != FALSE);
        Near(9.0f, keep.x, L"kept x"); Near(9.0f, keep.y, L"kept y"); Near(9.0f, keep.z, L"kept z");
    }

    TEST_METHOD(ResourceFilesAndTheLooseFileRegistry)
    {
        std::string dir = SeifukuFixtures::Dir();
        BuildFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("common.wdf");
        Assert::IsTrue(DataFile_UnpackSearch("common/chunk.c3") != FALSE);
        Assert::IsTrue(DataFile_UnpackSearch("COMMON\\CHUNK.C3") != FALSE);
        Assert::IsFalse(DataFile_UnpackSearch("unregistered.c3") != FALSE);

        DWORD size = 0;
        Assert::IsNull(Common_OpenRes("unregistered.c3", size, 1));
        void* f = Common_OpenRes("common/chunk.c3", size, 1);
        Assert::IsNotNull(f);
        Assert::AreEqual<DWORD>(28, size);
        Assert::AreEqual<long>(0, Common_GetSeek(f));
        Common_SeekRes(f, 16, SEEK_CUR);
        Assert::IsFalse(Common_IsEofRes(f) != FALSE);
        ChunkHeader ch;
        Common_GetChunk(f, &ch);
        Assert::AreEqual(0, memcmp(ch.byChunkID, "TEST", 4));
        Assert::AreEqual<DWORD>(4, ch.dwChunkSize);
        Assert::AreEqual<long>(24, Common_GetSeek(f));
        Common_SeekRes(f, 4, SEEK_CUR);
        Assert::IsFalse(Common_IsEofRes(f) != FALSE);
        char one;
        Assert::AreEqual<DWORD>(0, Common_ReadRes(f, &one, 1));
        Assert::IsTrue(Common_IsEofRes(f) != FALSE);
        Common_CloseRes(f);

        void* p = Common_OpenRes("common/a.txt", size, 1);
        Assert::IsNotNull(p);
        Assert::AreEqual<DWORD>(13, size);
        char buf[16] = { 0 };
        Assert::AreEqual<DWORD>(13, Common_ReadRes(p, buf, 13));
        Assert::AreEqual(0, memcmp(buf, "alpha-content", 13));
        Common_CloseRes(p);
        Assert::IsNull(Common_OpenRes("common/missing.txt", size, 1));
    }
};
