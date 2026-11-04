#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "c3_ptcl.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Ptcl) == 0x64, "");
static_assert(offsetof(C3Ptcl, lpName) == 0x00 && offsetof(C3Ptcl, lpVB) == 0x04 && offsetof(C3Ptcl, nTex) == 0x08, "");
static_assert(std::is_same<decltype(C3Ptcl::lpVB), LPDIRECT3DVERTEXBUFFER9>::value, "");
static_assert(offsetof(C3Ptcl, lpTexName) == 0x0C && offsetof(C3Ptcl, dwCount) == 0x10 && offsetof(C3Ptcl, dwRow) == 0x14, "");
static_assert(offsetof(C3Ptcl, lpPtcl) == 0x18 && offsetof(C3Ptcl, nFrame) == 0x1C && offsetof(C3Ptcl, dwFrames) == 0x20 && offsetof(C3Ptcl, matrix) == 0x24, "");

static_assert(sizeof(PtclVertex) == 0x18 && PTCL_VERTEX == 0x142, "");
static_assert(sizeof(PtclFrame) == 0x50, "");
static_assert(offsetof(PtclFrame, lpPos) == 4 && offsetof(PtclFrame, lpAge) == 8 && offsetof(PtclFrame, lpSize) == 0xC && offsetof(PtclFrame, matrix) == 0x10, "");

static BOOL (*const s_PtclDraw)(C3Ptcl*, DWORD, DWORD) = &Ptcl_Draw;
static BOOL (*const s_PtclLoad)(C3Ptcl**, void*, BOOL) = &Ptcl_Load;
static void (*const s_PtclMuliply)(C3Ptcl*, D3DXMATRIX*) = &Ptcl_Muliply;
static void (*const s_PtclSetFrame)(C3Ptcl*, DWORD) = &Ptcl_SetFrame;
static void (*const s_PtclChangeTexture)(C3Ptcl*, int) = &Ptcl_ChangeTexture;
static_assert(std::is_same<decltype(&Ptcl_NextFrame), void (*)(C3Ptcl*, int)>::value, "");
static_assert(std::is_same<decltype(&Ptcl_Save), BOOL (*)(char*, C3Ptcl*, BOOL)>::value && std::is_same<decltype(&Ptcl_Prepare), void (*)(void)>::value, "");

static_assert(sizeof(C3PtclEx) == 0x68 && offsetof(C3PtclEx, lpInfo) == 0x64, "");
static_assert(offsetof(C3PtclEx, nTex) == offsetof(C3Ptcl, nTex) && offsetof(C3PtclEx, lpPtcl) == offsetof(C3Ptcl, lpPtcl) && offsetof(C3PtclEx, nFrame) == offsetof(C3Ptcl, nFrame) && offsetof(C3PtclEx, dwFrames) == offsetof(C3Ptcl, dwFrames) && offsetof(C3PtclEx, matrix) == offsetof(C3Ptcl, matrix), "");
static_assert(sizeof(PtclFrameEx) == 0x54 && offsetof(PtclFrameEx, lpID) == 4 && offsetof(PtclFrameEx, lpPos) == 8 && offsetof(PtclFrameEx, lpAge) == 0xC && offsetof(PtclFrameEx, lpSize) == 0x10 && offsetof(PtclFrameEx, matrix) == 0x14, "");
static_assert(sizeof(PTCL_MATRIX) == 0x44 && offsetof(PTCL_MATRIX, bUse) == 0x40, "");

static_assert(sizeof(C3Ptcl3) == 0xB4 && offsetof(C3Ptcl3, lpInfo) == 0x64, "");
static_assert(offsetof(C3Ptcl3, bAlignSpeed) == 0x68 && offsetof(C3Ptcl3, bBornMatrix) == 0x69 && offsetof(C3Ptcl3, fSpin) == 0x6C && offsetof(C3Ptcl3, fSpinRandom) == 0x70, "");
static_assert(offsetof(C3Ptcl3, fAlphaStart) == 0x74 && offsetof(C3Ptcl3, dwFadeInFrames) == 0x80 && offsetof(C3Ptcl3, dwFadeOutFrame) == 0x84 && offsetof(C3Ptcl3, fAgeAlpha0) == 0x88, "");
static_assert(offsetof(C3Ptcl3, fAgeFadeIn) == 0x94 && offsetof(C3Ptcl3, fAgeFadeOut) == 0x98 && offsetof(C3Ptcl3, vGravity) == 0x9C && offsetof(C3Ptcl3, vSpeed) == 0xA8, "");
static_assert(sizeof(PTCL3_MATRIX) == 0x58 && offsetof(PTCL3_MATRIX, bUse) == 0x40 && offsetof(PTCL3_MATRIX, pos) == 0x44 && offsetof(PTCL3_MATRIX, dwTime) == 0x50 && offsetof(PTCL3_MATRIX, fSpin) == 0x54, "");
static BOOL (*const s_PtclDraw3)(C3Ptcl3*, DWORD, DWORD) = &Ptcl_Draw;
static BOOL (*const s_PtclLoad3)(C3Ptcl3**, void*, BOOL) = &Ptcl_Load;
static void (*const s_PtclClear3)(C3Ptcl3*) = &Ptcl_Clear;
static void (*const s_PtclUnload3)(C3Ptcl3**) = &Ptcl_Unload;
static_assert(std::is_same<decltype(&Ptcl3_CreateInfo), PTCL3_INFO* (*)(int)>::value && std::is_same<decltype(&Ptcl3_DestroyInfo), void (*)(PTCL3_INFO*)>::value, "");
static_assert(std::is_same<decltype(&Ptcl3_SetGravity), void (*)(C3Ptcl3*, D3DXVECTOR3)>::value && std::is_same<decltype(&Ptrl3_SetExternalSpeed), void (*)(C3Ptcl3*, D3DXVECTOR3)>::value, "");
static void (*const s_PtclClearEx)(C3PtclEx*) = &Ptcl_Clear;
static BOOL (*const s_PtclLoadEx)(C3PtclEx**, void*, BOOL) = &Ptcl_Load;
static void (*const s_PtclUnloadEx)(C3PtclEx**) = &Ptcl_Unload;
static BOOL (*const s_PtclDrawEx)(C3PtclEx*, DWORD, DWORD) = &Ptcl_Draw;
static void (*const s_PtclSetFrameEx)(C3PtclEx*, DWORD) = &Ptcl_SetFrame;
static void (*const s_PtclMuliplyEx)(C3PtclEx*, D3DXMATRIX*) = &Ptcl_Muliply;
static void (*const s_PtclClearMatrixEx)(C3PtclEx*) = &Ptcl_ClearMatrix;
static void (*const s_PtclChangeTextureEx)(C3PtclEx*, int) = &Ptcl_ChangeTexture;
static PTCL_INFO* (*const s_PtclCreateInfo)(int) = &Ptcl_CreateInfo;
static void (*const s_PtclDestroyInfo)(PTCL_INFO*) = &Ptcl_DestroyInfo;

TEST_CLASS(C3PtclHeader)
{
public:
    TEST_METHOD(ClearImageMatchesPtclClear)
    {
        C3Ptcl p;
        memset(&p, 0xCD, sizeof(p));
        p.lpName = 0; p.lpVB = 0; p.nTex = -1; p.lpTexName = 0; p.dwCount = 0; p.dwRow = 1;
        p.lpPtcl = 0; p.nFrame = 0; p.dwFrames = 0;
        D3DXMatrixIdentity(&p.matrix);
        const DWORD* w = reinterpret_cast<const DWORD*>(&p);
        for (int i = 0; i < 9; i++)
            Assert::AreEqual<DWORD>(i == 2 ? 0xFFFFFFFF : i == 5 ? 1 : 0, w[i]);
        for (int i = 0; i < 16; i++)
            Assert::AreEqual<DWORD>(i % 5 == 0 ? 0x3F800000 : 0, w[9 + i]);
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
    struct PtclDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        PtclDevice() : d3d(0), dev(0), slot(0)
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
        ~PtclDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct PBlob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void U16(WORD w) { Raw(&w, 2); }
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

    struct VBRead
    {
        LPDIRECT3DVERTEXBUFFER9 vb;
        PtclVertex* v;
        VBRead(LPDIRECT3DVERTEXBUFFER9 b) : vb(b), v(0) { Assert::IsTrue(SUCCEEDED(vb->Lock(0, 0, (void**) &v, D3DLOCK_READONLY))); }
        ~VBRead() { vb->Unlock(); }
    };

    std::vector<BYTE> PtclBmp24(int w, int h, DWORD color)
    {
        PBlob b;
        const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        b.Raw("BM", 2); b.U32(size); b.U32(0); b.U32(54);
        b.U32(40); b.U32(w); b.U32(h); b.U16(1); b.U16(24); b.U32(0); b.U32(pitch * h); b.U32(0); b.U32(0); b.U32(0); b.U32(0);
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++) { b.v.push_back((BYTE) color); b.v.push_back((BYTE) (color >> 8)); b.v.push_back((BYTE) (color >> 16)); }
            for (DWORD p = w * 3; p < pitch; p++) b.v.push_back(0);
        }
        return b.v;
    }

    void BuildPtclFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 0; hdr.offset = sizeof(hdr);
        SeifukuFixtures::WriteBlob(dir + "\\ptcl.wdf", &hdr, sizeof(hdr));
        std::string sub = dir + "\\ptcl";
        CreateDirectoryA(sub.c_str(), 0);
        std::vector<BYTE> tex = PtclBmp24(8, 4, 0x0000FF);
        SeifukuFixtures::WriteBlob(sub + "\\sprite.bmp", &tex[0], (DWORD) tex.size());
        PBlob tga;
        const BYTE tgaHdr[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 32, 0x28 };
        tga.Raw(tgaHdr, 18);
        for (int i = 0; i < 16; i++) tga.U32(0x80FFFFFF);
        tga.Write(sub + "\\spark.tga", false);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        PBlob pt;
        pt.Str("spark"); pt.Str("ptcl/sprite.bmp"); pt.U32(2); pt.U32(2); pt.U32(2);
        pt.U32(2); pt.Vec(0, 0, 0); pt.Vec(1, 1, 1); pt.F32(0.0f); pt.F32(0.75f); pt.F32(0.5f); pt.F32(1.0f); pt.Mat(id);
        pt.U32(0);
        pt.Write(sub + "\\ptcl.c3", true);
        PBlob nt;
        nt.Str("spark"); nt.Str("ptcl/missing.bmp"); nt.U32(2); nt.U32(2); nt.U32(2);
        nt.U32(2); nt.Vec(0, 0, 0); nt.Vec(1, 1, 1); nt.F32(0.0f); nt.F32(0.75f); nt.F32(0.5f); nt.F32(1.0f); nt.Mat(id);
        nt.U32(0);
        nt.Write(sub + "\\ptcl_notex.c3", true);
        PBlob empty;
        empty.Write(sub + "\\ptcl_save.c3", true);
        PBlob ex;
        ex.Str("sparkex"); ex.Str("ptcl/sprite.bmp"); ex.U32(2); ex.U32(2); ex.U32(2);
        ex.U32(2); ex.U16(7); ex.U16(9); ex.Vec(0, 0, 0); ex.Vec(1, 1, 1); ex.F32(0.0f); ex.F32(0.75f); ex.F32(0.5f); ex.F32(1.0f); ex.Mat(id);
        ex.U32(0);
        ex.Write(sub + "\\ptclex.c3", true);
        PBlob exnt;
        exnt.Str("sparkex"); exnt.Str("ptcl/missing.bmp"); exnt.U32(2); exnt.U32(2); exnt.U32(0);
        exnt.Write(sub + "\\ptclex_notex.c3", true);
        PBlob p3;
        p3.Str("spark3"); p3.Str("ptcl/sprite.bmp"); p3.U32(2);
        p3.v.push_back(0); p3.v.push_back(0);
        p3.F32(0.0f); p3.F32(0.0f);
        p3.F32(1.0f); p3.F32(1.0f); p3.F32(1.0f); p3.U32(0); p3.U32(0xFFFFFFFF);
        p3.F32(1.0f); p3.F32(1.0f); p3.F32(1.0f); p3.F32(0.0f); p3.F32(1.0f);
        p3.U32(2); p3.U32(2);
        p3.U32(2); p3.U16(7); p3.U16(9); p3.Vec(0, 0, 0); p3.Vec(1, 1, 1); p3.F32(0.0f); p3.F32(0.75f); p3.F32(0.5f); p3.F32(1.0f); p3.Mat(id);
        p3.U32(0);
        p3.Write(sub + "\\ptcl3.c3", true);
    }
}

TEST_CLASS(C3PtclFunctions)
{
public:
    TEST_METHOD(FramesAndMatrices)
    {
        C3Ptcl p;
        Ptcl_Clear(&p);
        p.dwFrames = 3;
        Ptcl_NextFrame(&p, 2); Assert::AreEqual(2, p.nFrame);
        Ptcl_NextFrame(&p, 2); Assert::AreEqual(1, p.nFrame);
        Ptcl_SetFrame(&p, 7); Assert::AreEqual(1, p.nFrame);
        D3DXMATRIX t; D3DXMatrixIdentity(&t); t._42 = 3;
        Ptcl_Muliply(&p, &t); Ptcl_Muliply(&p, &t);
        Assert::AreEqual(6.0f, p.matrix._42);
        Ptcl_ClearMatrix(&p);
        Assert::AreEqual(0.0f, p.matrix._42); Assert::AreEqual(1.0f, p.matrix._44);
        Ptcl_ChangeTexture(&p, 5);
        Assert::AreEqual(5, p.nTex);
    }

    TEST_METHOD(LoadDrawSave)
    {
        PtclDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildPtclFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("ptcl.wdf");
        int nPtcl0 = g_nPtclCount;

        void* f = SeifukuFixtures::OpenC3("ptcl/ptcl.c3");
        Assert::IsNotNull(f);
        C3Ptcl* p = 0;
        Assert::IsTrue(Ptcl_Load(&p, f, TRUE));
        Common_CloseRes(f);
        Assert::AreEqual("spark", p->lpName); Assert::AreEqual("ptcl/sprite.bmp", p->lpTexName);
        Assert::IsTrue(p->nTex >= 0 && g_lpTex[p->nTex] != 0); Assert::AreEqual<unsigned>(8, g_lpTex[p->nTex]->Info.Width);
        Assert::AreEqual<DWORD>(2, p->dwRow); Assert::AreEqual<DWORD>(2, p->dwCount); Assert::AreEqual<DWORD>(2, p->dwFrames);
        Assert::IsNotNull(p->lpVB);
        D3DVERTEXBUFFER_DESC vd;
        Assert::IsTrue(SUCCEEDED(p->lpVB->GetDesc(&vd)));
        Assert::AreEqual<UINT>(2 * 4 * sizeof(PtclVertex), vd.Size); Assert::AreEqual<DWORD>(PTCL_VERTEX, vd.FVF); Assert::AreEqual<DWORD>(D3DUSAGE_WRITEONLY, vd.Usage);
        Assert::AreEqual<DWORD>(2, p->lpPtcl[0].dwCount); Assert::AreEqual(1.0f, p->lpPtcl[0].lpPos[1].z);
        Assert::AreEqual(0.75f, p->lpPtcl[0].lpAge[1]); Assert::AreEqual(0.5f, p->lpPtcl[0].lpSize[0]); Assert::AreEqual(1.0f, p->lpPtcl[0].matrix._11);
        Assert::AreEqual<DWORD>(0, p->lpPtcl[1].dwCount); Assert::IsNull(p->lpPtcl[1].lpPos); Assert::IsNull(p->lpPtcl[1].lpSize);
        int nTex = p->nTex;

        Assert::AreEqual(1, g_nPtclCount - nPtcl0);
        f = SeifukuFixtures::OpenC3("ptcl/ptcl_notex.c3");
        Assert::IsNotNull(f);
        C3Ptcl* pfail = 0;
        Assert::IsTrue(Ptcl_Load(&pfail, f, TRUE));
        Common_CloseRes(f);
        Assert::AreEqual(-1, pfail->nTex);
        Assert::AreEqual(2, g_nPtclCount - nPtcl0);
        Ptcl_Unload(&pfail);
        Assert::IsNull(pfail);
        Assert::AreEqual(1, g_nPtclCount - nPtcl0);

        dd.dev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
        dd.dev->SetRenderState(D3DRS_LIGHTING, TRUE);
        Ptcl_Prepare();
        DWORD st = 1;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ZWRITEENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MIPFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MINFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_LINEAR, st);

        D3DXMatrixIdentity(&g_ViewMatrix);
        Assert::IsTrue(Begin3D());
        dd.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        Assert::IsTrue(Ptcl_Draw(p));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHATESTENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        {
            VBRead r(p->lpVB);
            Assert::AreEqual(-0.5f, r.v[0].x, 1e-5f); Assert::AreEqual(-0.5f, r.v[0].y, 1e-5f); Assert::AreEqual(0.0f, r.v[0].z);
            Assert::AreEqual(0.0f, r.v[0].u); Assert::AreEqual(0.5f, r.v[0].v); Assert::AreEqual<DWORD>(0xFFFFFFFF, r.v[0].color);
            Assert::AreEqual(0.5f, r.v[3].x, 1e-5f); Assert::AreEqual(0.5f, r.v[3].y, 1e-5f); Assert::AreEqual(0.5f, r.v[3].u); Assert::AreEqual(0.0f, r.v[3].v);
            Assert::AreEqual(0.0f, r.v[4].x, 1e-5f); Assert::AreEqual(1.0f, r.v[4].z);
            Assert::AreEqual(0.5f, r.v[4].u); Assert::AreEqual(1.0f, r.v[4].v);
            Assert::AreEqual(2.0f, r.v[7].x, 1e-5f); Assert::AreEqual(2.0f, r.v[7].y, 1e-5f); Assert::AreEqual(1.0f, r.v[7].u); Assert::AreEqual(0.5f, r.v[7].v);
        }
        D3DXMATRIX sc; D3DXMatrixIdentity(&sc); sc._11 = sc._22 = sc._33 = 2.0f;
        Ptcl_Muliply(p, &sc);
        Assert::IsTrue(Ptcl_Draw(p));
        {
            VBRead r(p->lpVB);
            Assert::AreEqual(-1.0f, r.v[0].x, 1e-5f); Assert::AreEqual(1.0f, r.v[3].y, 1e-5f);
            Assert::AreEqual(0.0f, r.v[4].x, 1e-5f); Assert::AreEqual(2.0f, r.v[4].z, 1e-5f);
        }
        Ptcl_ClearMatrix(p);

        C3Texture* alpha = 0;
        Assert::IsTrue(Texture_Load(&alpha, (char*) "ptcl/spark.tga", 1, D3DPOOL_MANAGED, TRUE, 0) >= 0);
        Assert::IsTrue(alpha->Info.Format == D3DFMT_A8R8G8B8);
        Ptcl_ChangeTexture(p, alpha->nID);
        Assert::IsTrue(Ptcl_Draw(p, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHAREF, &st))); Assert::AreEqual<DWORD>(8, st);
        Ptcl_ChangeTexture(p, nTex);

        Ptcl_ChangeTexture(p, -1);
        {
            PtclVertex* pv = 0;
            Assert::IsTrue(SUCCEEDED(p->lpVB->Lock(0, 0, (void**) &pv, 0)));
            pv[0].x = 123.0f;
            p->lpVB->Unlock();
        }
        dd.dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        Assert::IsTrue(Ptcl_Draw(p));
        { VBRead r(p->lpVB); Assert::AreEqual(123.0f, r.v[0].x); }
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Ptcl_ChangeTexture(p, nTex);

        Ptcl_SetFrame(p, 1);
        Assert::IsTrue(Ptcl_Draw(p));
        Ptcl_NextFrame(p, 1);
        Assert::AreEqual(0, p->nFrame);
        End3D();

        Assert::IsTrue(Ptcl_Save((char*) "ptcl\\ptcl_save.c3", p, FALSE));
        f = SeifukuFixtures::OpenC3("ptcl/ptcl_save.c3");
        Assert::IsNotNull(f);
        Common_SeekRes(f, 8, SEEK_CUR);
        C3Ptcl* p2 = 0;
        Assert::IsTrue(Ptcl_Load(&p2, f, FALSE));
        Common_CloseRes(f);
        Assert::AreEqual("spark", p2->lpName); Assert::AreEqual<DWORD>(2, p2->dwRow); Assert::AreEqual<DWORD>(2, p2->dwFrames);
        Assert::AreEqual(0.75f, p2->lpPtcl[0].lpAge[1]); Assert::AreEqual(1.0f, p2->lpPtcl[0].lpSize[1]); Assert::AreEqual<DWORD>(0, p2->lpPtcl[1].dwCount);
        Ptcl_Unload(&p2);
        Assert::IsNull(p2);

        Ptcl_Unload(&p);
        Assert::IsNull(p);
        C3Texture* ta = g_lpTex[alpha->nID];
        Texture_Unload(&ta);
        C3Texture* tn = g_lpTex[nTex];
        Texture_Unload(&tn);
        Assert::IsNull(g_lpTex[nTex]);
        void* fx = SeifukuFixtures::OpenC3("ptcl/ptclex.c3");
        Assert::IsNotNull(fx);
        int nPtclEx0 = g_nPtclCount;
        C3PtclEx* px = 0;
        Assert::IsTrue(Ptcl_Load(&px, fx, TRUE));
        Common_CloseRes(fx);
        Assert::AreEqual(nPtclEx0, g_nPtclCount);
        Assert::AreEqual(0, strcmp(px->lpName, "sparkex"));
        Assert::IsTrue(px->nTex >= 0 && g_lpTex[px->nTex] != 0);
        Assert::AreEqual<DWORD>(2, px->dwRow); Assert::AreEqual<DWORD>(2, px->dwCount); Assert::AreEqual<DWORD>(2, px->dwFrames);
        Assert::IsNull(px->lpInfo);
        Assert::AreEqual<DWORD>(2, px->lpPtcl[0].dwCount);
        Assert::AreEqual<WORD>(7, px->lpPtcl[0].lpID[0]); Assert::AreEqual<WORD>(9, px->lpPtcl[0].lpID[1]);
        Assert::AreEqual(1.0f, px->lpPtcl[0].lpPos[1].z); Assert::AreEqual(0.75f, px->lpPtcl[0].lpAge[1]); Assert::AreEqual(1.0f, px->lpPtcl[0].lpSize[1]);
        Assert::AreEqual(1.0f, px->lpPtcl[0].matrix._33);
        Assert::AreEqual<DWORD>(0, px->lpPtcl[1].dwCount);
        Assert::IsNull(px->lpPtcl[1].lpID); Assert::IsNull(px->lpPtcl[1].lpPos);
        Ptcl_SetFrame(px, 3);
        Assert::AreEqual(1, px->nFrame);
        D3DXMATRIX mx; D3DXMatrixTranslation(&mx, 1, 2, 3);
        Ptcl_Muliply(px, &mx);
        Assert::AreEqual(3.0f, px->matrix._43);
        Ptcl_ClearMatrix(px);
        Assert::AreEqual(0.0f, px->matrix._43);
        px->lpInfo = Ptcl_CreateInfo(1);
        Ptcl_SetFrame(px, 0);
        D3DXMatrixIdentity(&g_ViewMatrix);
        Assert::IsTrue(Begin3D());
        Assert::IsTrue(Ptcl_Draw(px, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::AreEqual<size_t>(2, px->lpInfo->size());
        Assert::IsTrue(px->lpInfo->count(7) == 1 && px->lpInfo->count(9) == 1);
        Assert::IsTrue((*px->lpInfo)[7].bUse == FALSE);
        {
            VBRead r(px->lpVB);
            Assert::AreEqual(-0.5f, r.v[0].x, 1e-5f); Assert::AreEqual(0.0f, r.v[4].x, 1e-5f);
            Assert::AreEqual(0.5f, r.v[4].u); Assert::AreEqual(1.0f, r.v[4].v);
        }
        D3DXMatrixTranslation(&mx, 10, 0, 0);
        Ptcl_Muliply(px, &mx);
        Assert::IsTrue(Ptcl_Draw(px, D3DBLEND_ONE, D3DBLEND_ONE));
        { VBRead r(px->lpVB); Assert::AreEqual(-0.5f, r.v[0].x, 1e-5f); Assert::AreEqual(0.0f, r.v[4].x, 1e-5f); }
        px->lpPtcl[0].lpID[1] = 11;
        Assert::IsTrue(Ptcl_Draw(px, D3DBLEND_ONE, D3DBLEND_ONE));
        {
            VBRead r(px->lpVB);
            Assert::AreEqual(-0.5f, r.v[0].x, 1e-5f);
            Assert::AreEqual(10.0f, r.v[4].x, 1e-5f);
        }
        Assert::AreEqual<size_t>(2, px->lpInfo->size());
        Assert::IsTrue(px->lpInfo->count(9) == 0 && px->lpInfo->count(11) == 1);
        DWORD stx = 1;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &stx))); Assert::AreEqual<DWORD>(FALSE, stx);
        Ptcl_SetFrame(px, 1);
        Assert::IsTrue(Ptcl_Draw(px, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::AreEqual<size_t>(0, px->lpInfo->size());
        End3D();
        Ptcl_DestroyInfo(px->lpInfo);
        px->lpInfo = 0;

        int nTexEx = px->nTex;
        Ptcl_ChangeTexture(px, -1);
        Assert::AreEqual(-1, px->nTex);
        Ptcl_Unload(&px);
        Assert::IsNull(px);
        Assert::AreEqual(nPtclEx0, g_nPtclCount);
        Assert::IsNotNull(g_lpTex[nTexEx]);
        C3Texture* tx = g_lpTex[nTexEx];
        Texture_Unload(&tx);

        fx = SeifukuFixtures::OpenC3("ptcl/ptclex_notex.c3");
        Assert::IsNotNull(fx);
        C3PtclEx* pxfail = 0;
        Assert::IsFalse(Ptcl_Load(&pxfail, fx, TRUE));
        Common_CloseRes(fx);
        Assert::IsNotNull(pxfail);
        Assert::AreEqual(-1, pxfail->nTex);

        C3Ptcl3 c3;
        memset(&c3, 0xCD, sizeof(c3));
        Ptcl_Clear(&c3);
        Assert::AreEqual(-1, c3.nTex); Assert::IsNull(c3.lpInfo); Assert::AreEqual<int>(0, c3.bAlignSpeed); Assert::AreEqual<int>(0, c3.bBornMatrix);
        Assert::AreEqual(0.0f, c3.fSpin); Assert::AreEqual(1.0f, c3.fAlphaStart); Assert::AreEqual(1.0f, c3.fAlphaEnd);
        Assert::AreEqual<DWORD>(0, c3.dwFadeInFrames); Assert::AreEqual<DWORD>(0xFFFFFFFF, c3.dwFadeOutFrame);
        Assert::AreEqual(1.0f, c3.fAgeAlpha2); Assert::AreEqual(0.0f, c3.fAgeFadeIn); Assert::AreEqual(1.0f, c3.fAgeFadeOut);
        Assert::AreEqual(0.0f, c3.vGravity.y); Assert::AreEqual(0.0f, c3.vSpeed.z);
        Ptcl3_SetGravity(&c3, D3DXVECTOR3(0, -10, 0));
        Ptrl3_SetExternalSpeed(&c3, D3DXVECTOR3(3, 0, 0));
        Assert::AreEqual(-10.0f, c3.vGravity.y); Assert::AreEqual(3.0f, c3.vSpeed.x);

        void* f3 = SeifukuFixtures::OpenC3("ptcl/ptcl3.c3");
        Assert::IsNotNull(f3);
        C3Ptcl3* p3 = 0;
        Assert::IsTrue(Ptcl_Load(&p3, f3, TRUE));
        Common_CloseRes(f3);
        Assert::AreEqual(0, strcmp(p3->lpName, "spark3"));
        Assert::IsTrue(p3->nTex >= 0);
        Assert::AreEqual<DWORD>(2, p3->dwRow); Assert::AreEqual<DWORD>(2, p3->dwCount); Assert::AreEqual<DWORD>(2, p3->dwFrames);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, p3->dwFadeOutFrame); Assert::AreEqual(1.0f, p3->fAgeFadeOut); Assert::AreEqual(0.0f, p3->fSpin);
        Assert::IsNotNull(p3->lpVB); Assert::AreEqual<WORD>(9, p3->lpPtcl[0].lpID[1]); Assert::AreEqual(0.75f, p3->lpPtcl[0].lpAge[1]);
        Assert::IsNull(p3->lpInfo);
        p3->lpInfo = Ptcl3_CreateInfo(1);
        Assert::IsNotNull(p3->lpInfo);
        Assert::IsNull(Ptcl3_CreateInfo(0));

        D3DXMatrixIdentity(&g_ViewMatrix);
        D3DXMatrixIdentity(&g_InvViewMatrix);
        Assert::IsTrue(Begin3D());
        Assert::IsTrue(Ptcl_Draw(p3, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::AreEqual<size_t>(2, p3->lpInfo->size());
        Assert::IsTrue(p3->lpInfo->count(7) == 1 && p3->lpInfo->count(9) == 1);
        Assert::IsTrue((*p3->lpInfo)[7].bUse == FALSE);
        Assert::AreEqual(1.0f, (*p3->lpInfo)[9].pos.x);
        Assert::AreEqual(0.0f, (*p3->lpInfo)[9].fSpin);
        {
            VBRead r(p3->lpVB);
            Assert::AreEqual(0.5f, r.v[0].x, 1e-5f); Assert::AreEqual(-0.5f, r.v[0].y, 1e-5f); Assert::AreEqual(0.0f, r.v[0].z, 1e-5f);
            Assert::AreEqual(-0.5f, r.v[3].x, 1e-5f); Assert::AreEqual(0.5f, r.v[3].y, 1e-5f);
            Assert::AreEqual(-0.5f, r.v[1].x, 1e-5f); Assert::AreEqual(-0.5f, r.v[1].y, 1e-5f);
            Assert::AreEqual<DWORD>(0xFFFFFFFF, r.v[0].color);
            Assert::AreEqual(0.0f, r.v[0].u); Assert::AreEqual(0.5f, r.v[0].v); Assert::AreEqual(0.5f, r.v[3].u); Assert::AreEqual(0.0f, r.v[3].v);
            Assert::AreEqual(2.0f, r.v[4].x, 1e-5f); Assert::AreEqual(1.0f, r.v[4].z, 1e-5f);
            Assert::AreEqual(0.5f, r.v[4].u); Assert::AreEqual(1.0f, r.v[4].v);
        }
        Ptcl3_SetGravity(p3, D3DXVECTOR3(0, -10, 0));
        Ptrl3_SetExternalSpeed(p3, D3DXVECTOR3(100, 0, 0));
        Sleep(60);
        Assert::IsTrue(Ptcl_Draw(p3, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::AreEqual<size_t>(2, p3->lpInfo->size());
        Assert::IsTrue((*p3->lpInfo)[7].pos.x > 1.0f);
        Assert::IsTrue((*p3->lpInfo)[7].pos.y < 0.0f);
        {
            VBRead r(p3->lpVB);
            Assert::IsTrue(r.v[0].x > 1.0f && r.v[0].y < -0.5f);
        }
        p3->fAgeFadeOut = 0.5f; p3->fAgeAlpha2 = 0.0f;
        p3->lpPtcl[0].lpID[0] = 11;
        Assert::IsTrue(Ptcl_Draw(p3, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::IsTrue(p3->lpInfo->count(7) == 0 && p3->lpInfo->count(11) == 1);
        {
            VBRead r(p3->lpVB);
            Assert::AreEqual<DWORD>(0x7F7F7F7F, r.v[4].color);
            Assert::AreEqual<DWORD>(0xFFFFFFFF, r.v[0].color);
        }
        p3->dwFadeOutFrame = 1; p3->fAlphaEnd = 0.0f;
        Ptcl_SetFrame((C3PtclEx*) p3, 1);
        Assert::IsTrue(Ptcl_Draw(p3, D3DBLEND_ONE, D3DBLEND_ONE));
        Assert::AreEqual<size_t>(0, p3->lpInfo->size());
        End3D();
        Ptcl3_DestroyInfo(p3->lpInfo);
        p3->lpInfo = 0;
        Ptcl3_DestroyInfo(0);
        int nTex3 = p3->nTex;
        Ptcl_Unload(&p3);
        Assert::IsNull(p3);
        C3Texture* t3 = g_lpTex[nTex3];
        Texture_Unload(&t3);

        Assert::IsNull(Ptcl_CreateInfo(0));
        Assert::IsNull(Ptcl_CreateInfo(-3));
        PTCL_INFO* info = Ptcl_CreateInfo(3);
        Assert::IsNotNull(info);
        Assert::IsTrue(info[0].empty() && info[2].empty());
        PTCL_MATRIX pm; D3DXMatrixIdentity(&pm.matrix); pm.bUse = TRUE;
        info[1][7] = pm;
        Assert::AreEqual<size_t>(1, info[1].size());
        Ptcl_DestroyInfo(info);
        Ptcl_DestroyInfo(0);
    }
};
