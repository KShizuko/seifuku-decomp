#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "3DEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(I3DEffect) == 4, "");
static_assert(_EFFECT_PART_MAX_ == 32, "");
static_assert(std::is_same<decltype(&EffectCreate), I3DEffect* (*)(char*)>::value, "");
static_assert(std::is_same<decltype(&DestroyShapeInfo), void (*)(ShapeBackupInfo*&, int)>::value && std::is_same<decltype(&DestroyPtclInfo), void (*)(PTCL_INFO*&)>::value, "");
static_assert(std::is_same<decltype(&DestroyPtcl3Info), void (*)(PTCL3_INFO*&)>::value, "");

namespace
{
    struct FxDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        FxDevice() : d3d(0), dev(0), slot(0)
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
        ~FxDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct FxBlob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void U16(WORD w) { Raw(&w, 2); }
        void F32(float f) { Raw(&f, 4); }
        void Vec(float x, float y, float z) { F32(x); F32(y); F32(z); }
        void Str(const char* s) { U32((DWORD) strlen(s)); Raw(s, strlen(s)); }
        void Mat(const D3DXMATRIX& m) { Raw(&m, 64); }
        void Chunk(const char* id, const FxBlob& body) { Raw(id, 4); U32((DWORD) body.v.size()); Raw(&body.v[0], body.v.size()); }
        void Vertex(float x, float y, float z, float u, float tv)
        {
            F32(x); F32(y); F32(z);
            for (int i = 0; i < 9; i++) F32(0.0f);
            F32(u); F32(tv); U32(0);
            U32(0); U32(0);
            F32(1.0f); F32(0.0f);
        }
    };

    void FxWrite(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string FxDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void BuildFxFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        FxWrite(dir + "\\gfx.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gfx").c_str(), 0);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        D3DXMATRIX tr; D3DXMatrixIdentity(&tr); tr._41 = 2; tr._42 = 4; tr._43 = 6;
        FxBlob ph;
        ph.Str("fxmesh"); ph.U32(0); ph.U32(3); ph.U32(3);
        ph.Vertex(0, 0, 0, 0.0f, 0.0f); ph.Vertex(1, 0, 0, 0.25f, 0.0f); ph.Vertex(0, 1, 0, 0.0f, 0.25f);
        ph.Vertex(0, 0, 1, 0.5f, 0.5f); ph.Vertex(1, 0, 1, 0.75f, 0.5f); ph.Vertex(0, 1, 1, 0.5f, 0.75f);
        ph.U32(1); ph.U32(1);
        for (WORD i = 0; i < 6; i++) ph.U16(i);
        ph.Str("none.bmp");
        ph.Vec(-1, -1, -1); ph.Vec(1, 1, 1);
        ph.Mat(id);
        ph.U32(1); ph.U32(0); ph.U32(0); ph.U32(0);
        ph.Raw("STEP", 4); ph.F32(0.0f); ph.F32(0.0f);
        FxBlob mo;
        mo.U32(1); mo.U32(4); mo.Raw("KKEY", 4); mo.U32(2);
        mo.U32(0); mo.Mat(id);
        mo.U32(2); mo.Mat(tr);
        mo.U32(0);
        FxBlob pt;
        pt.Str("spark"); pt.Str("gfx/tex.bmp"); pt.U32(2); pt.U32(2); pt.U32(2);
        pt.U32(2); pt.Vec(0, 0, 0); pt.Vec(1, 1, 1); pt.F32(0.0f); pt.F32(0.75f); pt.F32(0.5f); pt.F32(1.0f); pt.Mat(id);
        pt.U32(0);
        FxBlob ex;
        ex.Str("sparkex"); ex.Str("gfx/tex.bmp"); ex.U32(2); ex.U32(2); ex.U32(2);
        ex.U32(2); ex.U16(7); ex.U16(9); ex.Vec(0, 0, 0); ex.Vec(1, 1, 1); ex.F32(0.0f); ex.F32(0.75f); ex.F32(0.5f); ex.F32(1.0f); ex.Mat(id);
        ex.U32(0);

        FxBlob p3;
        p3.Str("spark3"); p3.Str("gfx/tex.bmp"); p3.U32(2);
        p3.v.push_back(0); p3.v.push_back(0);
        p3.F32(0.0f); p3.F32(0.0f);
        p3.F32(1.0f); p3.F32(1.0f); p3.F32(1.0f); p3.U32(0); p3.U32(0xFFFFFFFF);
        p3.F32(1.0f); p3.F32(1.0f); p3.F32(1.0f); p3.F32(0.0f); p3.F32(1.0f);
        p3.U32(2); p3.U32(2);
        p3.U32(2); p3.U16(7); p3.U16(9); p3.Vec(0, 0, 0); p3.Vec(1, 1, 1); p3.F32(0.0f); p3.F32(0.75f); p3.F32(0.5f); p3.F32(1.0f); p3.Mat(id);
        p3.U32(0);

        FxBlob fx;
        fx.Raw("MAXFILE C3 00001", 16);
        fx.Chunk("PHY ", ph);
        fx.Chunk("MOTI", mo);
        fx.Chunk("PTCL", pt);
        fx.Chunk("PTCX", ex);
        fx.Chunk("PTC3", p3);
        FxWrite(dir + "\\gfx\\fx.c3", &fx.v[0], (DWORD) fx.v.size());
        FxBlob mesh;
        mesh.Raw("MAXFILE C3 00001", 16);
        mesh.Chunk("PHY ", ph);
        mesh.Chunk("MOTI", mo);
        FxWrite(dir + "\\gfx\\mesh.c3", &mesh.v[0], (DWORD) mesh.v.size());
        FxWrite(dir + "\\gfx\\empty.c3", "MAXFILE C3 00001", 16);

        FxBlob bmp;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        bmp.Raw("BM", 2); bmp.U32(size); bmp.U32(0); bmp.U32(54);
        bmp.U32(40); bmp.U32(w); bmp.U32(h); bmp.U16(1); bmp.U16(24); bmp.U32(0); bmp.U32(pitch * h); bmp.U32(0); bmp.U32(0); bmp.U32(0); bmp.U32(0);
        for (DWORD i = 0; i < pitch * h; i++) bmp.v.push_back(0x80);
        FxWrite(dir + "\\gfx\\tex.bmp", &bmp.v[0], (DWORD) bmp.v.size());
    }

    struct FxCwd
    {
        char old[MAX_PATH];
        FxCwd(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~FxCwd() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(Graphic3DEffect)
{
public:
    TEST_METHOD(LoadBindDrawAndRelease)
    {
        FxDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = FxDir();
        BuildFxFixtures(dir);
        FxCwd cwd(dir);
        CMyBitmap::PackFileOpen("gfx.wdf");

        Assert::IsNull(EffectCreate(0));
        Assert::IsNull(EffectCreate((char*) "gfx/missing.c3"));
        I3DEffect* none = EffectCreate((char*) "gfx/empty.c3");
        Assert::IsNotNull(none);
        Assert::AreEqual<DWORD>(0, none->GetFrames());
        Assert::IsNull(none->CreateShapeInfo());
        Assert::IsNull(none->CreatePtclInfo());
        none->Draw();
        Assert::AreEqual<DWORD>(0, none->Release());

        I3DEffect* fx = EffectCreate((char*) "gfx/fx.c3");
        Assert::IsNotNull(fx);
        Assert::AreEqual<DWORD>(1, fx->GetPhyAmount());
        Assert::AreEqual<DWORD>(1, fx->GetPtclAmount());
        Assert::AreEqual<DWORD>(0, fx->GetShapeAmount());
        Assert::AreEqual<DWORD>(4, fx->GetPhyFrames(0));
        Assert::AreEqual<DWORD>(2, fx->GetPtclFrames(0));
        Assert::AreEqual<DWORD>(4, fx->GetFrames());
        void** vt = *(void***) fx;
        typedef DWORD (__fastcall *FnDword)(void*, void*);
        Assert::AreEqual<DWORD>(1, ((FnDword) vt[12])(fx, 0));
        Assert::AreEqual<DWORD>(1, ((FnDword) vt[15])(fx, 0));
        Assert::AreEqual<DWORD>(4, ((FnDword) vt[18])(fx, 0));
        Assert::AreEqual<DWORD>(0, ((FnDword) vt[20])(fx, 0));

        Assert::AreEqual<DWORD>(1, *(DWORD*) ((BYTE*) fx + 0x210));
        Assert::AreEqual<DWORD>(1, *(DWORD*) ((BYTE*) fx + 0x294));
        PTCL3_INFO* pInfo3 = fx->CreatePtcl3Info();
        Assert::IsNotNull(pInfo3);
        Assert::IsTrue(((void* (__fastcall*)(void*, void*)) vt[26])(fx, 0) != 0);
        fx->SetPtcl3Info(0);
        fx->SetPtcl3Info(pInfo3);
        C3Ptcl3* p3 = *(C3Ptcl3**) ((BYTE*) fx + 0x214);
        Assert::IsTrue(p3->lpInfo == pInfo3);
        ((void (__fastcall*)(void*, void*, PTCL3_INFO*)) vt[27])(fx, 0, pInfo3);
        fx->SetPtcl3Gravity(0.0f, -9.0f, 0.0f);
        Assert::AreEqual(-9.0f, p3->vGravity.y);
        fx->SetPtcl3ExternalSpeed(1.0f, 2.0f, 3.0f);
        Assert::AreEqual(3.0f, p3->vSpeed.z);
        typedef void (__fastcall *FnVec)(void*, void*, float, float, float);
        ((FnVec) vt[28])(fx, 0, 0.0f, -4.0f, 0.0f);
        Assert::AreEqual(-4.0f, p3->vGravity.y);
        ((FnVec) vt[29])(fx, 0, 5.0f, 0.0f, 0.0f);
        Assert::AreEqual(5.0f, p3->vSpeed.x);
        fx->SetStepCount(7);
        Assert::AreEqual<DWORD>(7, *(DWORD*) ((BYTE*) fx + 0x398));
        ((void (__fastcall*)(void*, void*, DWORD)) vt[30])(fx, 0, 9);
        Assert::AreEqual<DWORD>(9, *(DWORD*) ((BYTE*) fx + 0x398));
        Assert::AreEqual<DWORD>(2, p3->dwFrames);
        Assert::AreEqual<DWORD>(4, fx->GetFrames());

        fx->SetFrames(1);
        fx->SetPtclCurFrame(0, 0);
        fx->SetPhyCurFrame(0, 2);
        fx->ResetShape();
        Assert::IsNull(fx->CreateShapeInfo());
        fx->BackupShapeInfo(0); fx->RestoreShapeInfo(0);
        ShapeBackupInfo* pShapeInfo = 0;
        DestroyShapeInfo(pShapeInfo, 3);

        PTCL_INFO* pInfo = fx->CreatePtclInfo();
        Assert::IsNotNull(pInfo);
        fx->SetPtclInfo(0);
        fx->SetPtclInfo(pInfo);

        I3DMotion* mo = MotionCreate((char*) "gfx/mesh.c3", FALSE);
        I3DObj* obj = MeshCreate((char*) "gfx/mesh.c3");
        Assert::IsNotNull(mo); Assert::IsNotNull(obj);
        obj->SetMotion(mo);
        obj->Move(3.0f, 0.0f, 0.0f);
        fx->Bind(obj);
        D3DXMATRIXA16* pMatrix = (D3DXMATRIXA16*) ((BYTE*) fx + 0x39C);
        Assert::AreEqual(3.0f, pMatrix->_41);
        D3DXMATRIXA16 m; D3DXMatrixIdentity(&m); m._42 = 5.0f;
        fx->BindMatrix(&m);
        Assert::AreEqual(5.0f, pMatrix->_42); Assert::AreEqual(0.0f, pMatrix->_41);
        typedef void (__fastcall *FnBind)(void*, void*, void*);
        ((FnBind) vt[3])(fx, 0, obj);
        Assert::AreEqual(3.0f, pMatrix->_41);
        ((FnBind) vt[10])(fx, 0, &m);
        Assert::AreEqual(5.0f, pMatrix->_42);

        I3DTexture* tex = TextureCreate("gfx/tex.bmp");
        Assert::IsNotNull(tex);
        fx->SetTexture(0);
        fx->SetTexture(tex);
        fx->SetColor(0x80FF8040);
        fx->Move(1.0f, 2.0f, 3.0f); fx->Rotate(0.1f, 0.0f, 0.0f); fx->Scale(2.0f, 2.0f, 2.0f);

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        DWORD nTri0 = CMyBitmap::GetShowTriTotal();
        fx->Draw(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        Assert::IsTrue(CMyBitmap::GetShowTriTotal() - nTri0 >= 2);
        fx->Draw(FALSE, D3DBLEND_SRCALPHA, D3DBLEND_ONE);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        DestroyPtclInfo(pInfo);
        Assert::IsNull(pInfo);
        DestroyPtclInfo(pInfo);
        DestroyPtcl3Info(pInfo3);
        Assert::IsNull(pInfo3);
        DestroyPtcl3Info(pInfo3);
        Assert::AreEqual<DWORD>(0, ((FnDword) vt[0])(fx, 0));
        Assert::AreEqual<DWORD>(0, obj->Release());
        Assert::AreEqual<DWORD>(0, mo->Release());
        CMyBitmap::PrepareSprite();
    }
};
