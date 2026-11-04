#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "c3_shape.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3SMotion) == 0x4C, "");
static_assert(offsetof(C3SMotion, dwFrames) == 0x00 && offsetof(C3SMotion, lpFrames) == 0x04 && offsetof(C3SMotion, matrix) == 0x08 && offsetof(C3SMotion, nFrame) == 0x48, "");

static_assert(sizeof(C3Shape) == 0x48, "");
static_assert(offsetof(C3Shape, lpName) == 0x00 && offsetof(C3Shape, dwLineCount) == 0x04 && offsetof(C3Shape, lpLine) == 0x08, "");
static_assert(offsetof(C3Shape, lpTexName) == 0x0C && offsetof(C3Shape, nTex) == 0x10, "");
static_assert(offsetof(C3Shape, lpMotion) == 0x14, "");
static_assert(offsetof(C3Shape, vb) == 0x18 && offsetof(C3Shape, dwSegment) == 0x1C && offsetof(C3Shape, dwSegmentMax) == 0x20 && offsetof(C3Shape, dwSegmentCur) == 0x24, "");
static_assert(offsetof(C3Shape, last) == 0x28 && offsetof(C3Shape, bFrist) == 0x40 && offsetof(C3Shape, dwSmooth) == 0x44, "");
static_assert(std::is_same<decltype(C3Shape::vb), LPDIRECT3DVERTEXBUFFER9>::value, "");

static_assert(sizeof(ShapeOutVertex) == 0x18 && SHAPE_OUT_VERTEX == 0x142, "");
static_assert(sizeof(C3Line) == 8, "");
static_assert(sizeof(ShapeBackupInfo) == 0x24, "");

static_assert(std::is_same<decltype(&Shape_Prepare), void (*)(DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Shape_Draw), BOOL (*)(C3Shape*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Shape_SetSegment), void (*)(C3Shape*, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Shape_Load), BOOL (*)(C3Shape**, void*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Shape_Muliply), void (*)(C3Shape*, D3DXMATRIX*)>::value && std::is_same<decltype(&Shape_SetFrame), void (*)(C3Shape*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&SMotion_Load), BOOL (*)(C3SMotion**, void*)>::value, "");

TEST_CLASS(C3ShapeHeader)
{
public:
    TEST_METHOD(ClearImageMatchesShapeClear)
    {
        C3Shape s;
        memset(&s, 0xCD, sizeof(s));
        s.lpName = 0; s.dwLineCount = 0; s.lpLine = 0;
        s.lpTexName = 0; s.nTex = -1; s.lpMotion = 0;
        s.vb = 0; s.dwSegment = 0; s.dwSegmentMax = 0; s.dwSegmentCur = 0;
        s.bFrist = TRUE; s.dwSmooth = 0;
        const DWORD* w = reinterpret_cast<const DWORD*>(&s);
        for (int i = 0; i < 3; i++) Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(0, w[0x0C / 4]);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, w[0x10 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x14 / 4]);
        for (int i = 0x18 / 4; i <= 0x24 / 4; i++) Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, w[0x28 / 4]);
        Assert::AreEqual<DWORD>(1, w[0x40 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x44 / 4]);
    }
};

#include <string>
#include <vector>
#include <cstring>
#include "c3_datafile.h"
#include "c3_datafile.h"
#include "seifuku_fixtures.h"

namespace
{
    struct ShapeDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        ShapeDevice() : d3d(0), dev(0), slot(0)
        {
            HMODULE h = GetModuleHandleA("C3_CORE_DLL.dll");
            slot = h ? (LPDIRECT3DDEVICE9*) GetProcAddress(h, "?g_D3DDevice@@3PAUIDirect3DDevice8@@A") : 0;
            d3d = Direct3DCreate9(D3D_SDK_VERSION);
            if (!d3d || !slot) return;
            D3DPRESENT_PARAMETERS pp;
            ZeroMemory(&pp, sizeof(pp));
            pp.Windowed = TRUE;
            pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            pp.hDeviceWindow = GetDesktopWindow();
            if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, GetDesktopWindow(),
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
                dev = 0;
            if (dev) *slot = dev;
        }
        ~ShapeDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct SBlob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void F32(float f) { Raw(&f, 4); }
        void Str(const char* s) { U32((DWORD) strlen(s)); Raw(s, strlen(s)); }
        void Vec(float x, float y, float z) { F32(x); F32(y); F32(z); }
        void Mat(const D3DXMATRIX& m) { Raw(&m, 64); }
        void Write(const std::string& path, bool header)
        {
            std::vector<BYTE> out;
            if (header) { const char* h = "MAXFILE C3 00001"; out.insert(out.end(), h, h + 16); }
            out.insert(out.end(), v.begin(), v.end());
            SeifukuFixtures::WriteBlob(path, out.empty() ? "" : (const void*) &out[0], (DWORD) out.size());
        }
    };

    D3DXMATRIX ShapeTranslation(float x, float y, float z)
    {
        D3DXMATRIX m;
        D3DXMatrixIdentity(&m);
        m._41 = x; m._42 = y; m._43 = z;
        return m;
    }

    void BuildShapeFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 0; hdr.offset = sizeof(hdr);
        SeifukuFixtures::WriteBlob(dir + "\\shape.wdf", &hdr, sizeof(hdr));
        std::string sub = dir + "\\shape";
        CreateDirectoryA(sub.c_str(), 0);

        SBlob sh;
        sh.Str("blade"); sh.U32(1); sh.U32(2); sh.Vec(0, 0, 0); sh.Vec(0, 1, 0); sh.Str("none"); sh.U32(2);
        sh.Write(sub + "\\shape.c3", true);
        sh.Write(sub + "\\shape.pack", false);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        SBlob sm;
        sm.U32(3); sm.Mat(id); sm.Mat(ShapeTranslation(1, 0, 0)); sm.Mat(ShapeTranslation(2, 0, 0));
        sm.Write(sub + "\\smotion.c3", true);
        sm.Write(sub + "\\smotion.pack", false);

        SBlob empty;
        empty.Write(sub + "\\shape_save.c3", true);
        empty.Write(sub + "\\smotion_save.c3", true);
    }
}

TEST_CLASS(C3ShapeFunctions)
{
public:
    TEST_METHOD(SegmentsFramesAndMatrices)
    {
        C3SMotion m;
        SMotion_Clear(&m);
        Assert::AreEqual<DWORD>(0, m.dwFrames); Assert::IsNull(m.lpFrames); Assert::AreEqual(0, m.nFrame);
        D3DXMatrixIdentity(&m.matrix);
        m.dwFrames = 4;

        C3Shape s;
        Shape_Clear(&s);
        s.lpMotion = &m;
        {
            ShapeDevice dd;
            if (dd.dev)
            {
                Shape_SetSegment(&s, 3, 7);
                Assert::AreEqual<DWORD>(15, s.dwSegment); Assert::AreEqual<DWORD>(16, s.dwSegmentMax); Assert::AreEqual<DWORD>(5, s.dwSmooth); Assert::AreEqual<DWORD>(0, s.dwSegmentCur);
                Assert::IsNotNull(s.vb);
                D3DVERTEXBUFFER_DESC vd;
                Assert::IsTrue(SUCCEEDED(s.vb->GetDesc(&vd)));
                Assert::AreEqual<UINT>(16 * 2 * sizeof(ShapeOutVertex), vd.Size); Assert::AreEqual<DWORD>(SHAPE_OUT_VERTEX, vd.FVF); Assert::AreEqual<int>(D3DPOOL_MANAGED, vd.Pool);
                Shape_SetSegment(&s, 1, 1);
                Assert::AreEqual<DWORD>(5, s.dwSegment);
                Shape_SetSegment(&s, 200, 0);
                Assert::AreEqual<DWORD>(800, s.dwSegment); Assert::AreEqual<DWORD>(801, s.dwSegmentMax);
                s.vb->Release(); s.vb = 0;
            }
            else Logger::WriteMessage("no NULLREF device; Shape_SetSegment skipped");
        }

        Shape_NextFrame(&s, 3); Assert::AreEqual(3, m.nFrame);
        Shape_NextFrame(&s, 2); Assert::AreEqual(1, m.nFrame);
        Shape_SetFrame(&s, 9); Assert::AreEqual(9, m.nFrame);

        D3DXMATRIX t = ShapeTranslation(0, 0, 5);
        Shape_Muliply(&s, &t); Shape_Muliply(&s, &t);
        Assert::AreEqual(10.0f, m.matrix._43);
        Shape_ClearMatrix(&s);
        Assert::AreEqual(0.0f, m.matrix._43); Assert::AreEqual(1.0f, m.matrix._33);
        Shape_ChangeTexture(&s, 4);
        Assert::AreEqual(4, s.nTex);
    }

    TEST_METHOD(LoadDrawSave)
    {
        ShapeDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildShapeFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("shape.wdf");
        int nShape0 = g_nShapeCount, nSMotion0 = g_nSMotionCount;

        void* f = SeifukuFixtures::OpenC3("shape/shape.c3");
        Assert::IsNotNull(f);
        C3Shape* s = 0;
        Assert::IsTrue(Shape_Load(&s, f, FALSE));
        Common_CloseRes(f);
        Assert::AreEqual("blade", s->lpName); Assert::AreEqual("none", s->lpTexName); Assert::AreEqual(-1, s->nTex);
        Assert::AreEqual<DWORD>(1, s->dwLineCount); Assert::AreEqual<DWORD>(2, s->lpLine[0].dwVecCount); Assert::AreEqual(1.0f, s->lpLine[0].lpVB[1].y);
        Assert::AreEqual<DWORD>(10, s->dwSegment); Assert::AreEqual<DWORD>(11, s->dwSegmentMax); Assert::AreEqual<DWORD>(5, s->dwSmooth); Assert::IsNotNull(s->vb); Assert::IsTrue(s->bFrist == TRUE);

        f = SeifukuFixtures::OpenC3("shape/smotion.c3");
        Assert::IsNotNull(f);
        C3SMotion* m = 0;
        Assert::IsTrue(SMotion_Load(&m, f));
        Common_CloseRes(f);
        Assert::AreEqual<DWORD>(3, m->dwFrames); Assert::AreEqual(2.0f, m->lpFrames[2]._41); Assert::AreEqual(1.0f, m->matrix._22); Assert::AreEqual(0, m->nFrame);

        Assert::AreEqual(1, g_nShapeCount - nShape0);
        Assert::AreEqual(1, g_nSMotionCount - nSMotion0);
        f = SeifukuFixtures::OpenC3("shape/shape.c3");
        C3Shape* sfail = 0;
        Assert::IsTrue(Shape_Load(&sfail, f, TRUE));
        Common_CloseRes(f);
        Assert::AreEqual(-1, sfail->nTex);
        Assert::AreEqual(2, g_nShapeCount - nShape0);
        Shape_Unload(&sfail);
        Assert::IsNull(sfail);
        Assert::AreEqual(1, g_nShapeCount - nShape0);

        dd.dev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
        dd.dev->SetRenderState(D3DRS_LIGHTING, TRUE);
        dd.dev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
        Shape_Prepare(D3DBLEND_ONE, D3DBLEND_ZERO);
        DWORD st = 0;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DESTBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ZERO, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SHADEMODE, &st))); Assert::AreEqual<DWORD>(D3DSHADE_GOURAUD, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);

        s->lpMotion = m;
        Assert::IsTrue(Begin3D());
        Shape_SetFrame(s, 1);
        Assert::IsTrue(Shape_Draw(s, FALSE));
        Assert::IsTrue(s->bFrist == FALSE); Assert::AreEqual<DWORD>(1, s->dwSegmentCur);
        Assert::AreEqual(1.0f, s->last[0].x); Assert::AreEqual(1.0f, s->last[1].x); Assert::AreEqual(1.0f, s->last[1].y);
        ShapeOutVertex* pv = 0;
        Assert::IsTrue(SUCCEEDED(s->vb->Lock(0, 0, (void**) &pv, D3DLOCK_READONLY)));
        Assert::AreEqual(1.0f, pv[0].x); Assert::AreEqual(0.0f, pv[0].y); Assert::AreEqual(1.0f, pv[1].y);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, pv[0].color); Assert::AreEqual(0.0f, pv[0].v); Assert::AreEqual(1.0f, pv[1].v);
        Assert::AreEqual(0.0f, pv[2].x); Assert::AreEqual<DWORD>(0, pv[2].color);
        s->vb->Unlock();

        Shape_SetFrame(s, 2);
        Assert::IsTrue(Shape_Draw(s, FALSE));
        Assert::AreEqual<DWORD>(6, s->dwSegmentCur);
        Assert::AreEqual(2.0f, s->last[0].x); Assert::AreEqual(0.0f, s->last[0].y); Assert::AreEqual(1.0f, s->last[1].y);
        Assert::IsTrue(SUCCEEDED(s->vb->Lock(0, 0, (void**) &pv, D3DLOCK_READONLY)));
        Assert::AreEqual(1.2f, pv[2].x, 1e-5f); Assert::AreEqual(0.0f, pv[2].y); Assert::AreEqual(1.2f, pv[3].x, 1e-5f); Assert::AreEqual(1.0f, pv[3].y);
        Assert::AreEqual(2.0f, pv[10].x); Assert::AreEqual(2.0f, pv[11].x); Assert::AreEqual(1.0f, pv[11].y);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, pv[10].color);
        const float step = 0.95f / 10.0f;
        Assert::AreEqual(0.0f, pv[12].u);
        Assert::AreEqual(5 * step, pv[0].u, 1e-5f);
        Assert::AreEqual(10 * step, pv[10].u, 1e-5f); Assert::AreEqual(1.0f, pv[11].v);
        Assert::AreEqual(0.0f, pv[12].x);
        s->vb->Unlock();

        D3DXMATRIX t = ShapeTranslation(0, 0, 5);
        Shape_Muliply(s, &t);
        Assert::IsTrue(Shape_Draw(s, FALSE));
        Assert::AreEqual(5.0f, s->last[0].z); Assert::AreEqual<DWORD>(1, s->dwSegmentCur);
        Assert::IsTrue(SUCCEEDED(s->vb->Lock(0, 0, (void**) &pv, D3DLOCK_READONLY)));
        Assert::AreEqual(5.0f, pv[0].z); Assert::AreEqual(5.0f, pv[1].z);
        s->vb->Unlock();
        Assert::IsTrue(Shape_Draw(s, TRUE));
        Assert::AreEqual(0.0f, s->last[0].z); Assert::AreEqual<DWORD>(6, s->dwSegmentCur);
        Shape_ClearMatrix(s);
        End3D();

        Assert::IsTrue(Shape_Save((char*) "shape\\shape_save.c3", s, FALSE));
        f = SeifukuFixtures::OpenC3("shape/shape_save.c3");
        Assert::IsNotNull(f);
        Common_SeekRes(f, 8, SEEK_CUR);
        C3Shape* s2 = 0;
        Assert::IsTrue(Shape_Load(&s2, f, FALSE));
        Common_CloseRes(f);
        Assert::AreEqual("blade", s2->lpName); Assert::AreEqual<DWORD>(50, s2->dwSegment); Assert::AreEqual(1.0f, s2->lpLine[0].lpVB[1].y);
        Shape_Unload(&s2);

        Assert::IsTrue(SMotion_Save((char*) "shape\\smotion_save.c3", m, FALSE));
        f = SeifukuFixtures::OpenC3("shape/smotion_save.c3");
        Assert::IsNotNull(f);
        Common_SeekRes(f, 8, SEEK_CUR);
        C3SMotion* m2 = 0;
        Assert::IsTrue(SMotion_Load(&m2, f));
        Common_CloseRes(f);
        Assert::AreEqual<DWORD>(3, m2->dwFrames); Assert::AreEqual(2.0f, m2->lpFrames[2]._41);
        SMotion_Unload(&m2);

        s->lpMotion = 0;
        Shape_Unload(&s);
        Assert::IsNull(s);
        SMotion_Unload(&m);
        Assert::IsNull(m);
    }
};
