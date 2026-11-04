#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <type_traits>
#include "3DAniObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(I3DAniObj) == 4, "");
static_assert(std::is_same<decltype(&Scene3DCreate), I3DAniObj* (*)(const char*)>::value, "");

namespace
{
    struct SceneDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        SceneDevice() : d3d(0), dev(0), slot(0)
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
        ~SceneDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct SceneBlob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void U16(WORD w) { Raw(&w, 2); }
        void F32(float f) { Raw(&f, 4); }
        void Str(const char* s) { U32((DWORD) strlen(s)); Raw(s, strlen(s)); }
        void Mat(const D3DXMATRIX& m) { Raw(&m, 64); }
        void Chunk(const char* id, const SceneBlob& body) { Raw(id, 4); U32((DWORD) body.v.size()); Raw(&body.v[0], body.v.size()); }
        void Vertex(float x, float y, float z, float u, float tv)
        {
            F32(x); F32(y); F32(z);
            for (int i = 0; i < 9; i++) F32(0.0f);
            F32(u); F32(tv); U32(0);
            U32(0); U32(0);
            F32(1.0f); F32(0.0f);
        }
    };

    void SceneWrite(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string SceneDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void BuildSceneFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        SceneWrite(dir + "\\gscene.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gscene").c_str(), 0);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        D3DXMATRIX tr; D3DXMatrixIdentity(&tr); tr._41 = 2; tr._42 = 4; tr._43 = 6;
        SceneBlob ph;
        ph.Str("seifuku"); ph.U32(0); ph.U32(3); ph.U32(3);
        ph.Vertex(0, 0, 0, 0.0f, 0.0f); ph.Vertex(1, 0, 0, 0.25f, 0.0f); ph.Vertex(0, 1, 0, 0.0f, 0.25f);
        ph.Vertex(0, 0, 1, 0.5f, 0.5f); ph.Vertex(1, 0, 1, 0.75f, 0.5f); ph.Vertex(0, 1, 1, 0.5f, 0.75f);
        ph.U32(1); ph.U32(1);
        for (WORD i = 0; i < 6; i++) ph.U16(i);
        ph.Str("none.bmp");
        ph.F32(-1); ph.F32(-1); ph.F32(-1); ph.F32(1); ph.F32(1); ph.F32(1);
        ph.Mat(id);
        ph.U32(1);
        ph.U32(0); ph.U32(0); ph.U32(0);
        ph.Raw("STEP", 4); ph.F32(0.0f); ph.F32(0.0f);
        SceneBlob mo;
        mo.U32(1); mo.U32(4); mo.Raw("KKEY", 4); mo.U32(2);
        mo.U32(0); mo.Mat(id);
        mo.U32(2); mo.Mat(tr);
        mo.U32(0);

        SceneBlob file;
        file.Raw("MAXFILE C3 00001", 16);
        file.Chunk("PHY ", ph);
        file.Chunk("MOTI", mo);
        SceneWrite(dir + "\\gscene\\obj.c3", &file.v[0], (DWORD) file.v.size());
        SceneBlob only;
        only.Raw("MAXFILE C3 00001", 16);
        only.Chunk("PHY ", ph);
        SceneWrite(dir + "\\gscene\\phyonly.c3", &only.v[0], (DWORD) only.v.size());

        SceneBlob bmp;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        bmp.Raw("BM", 2); bmp.U32(size); bmp.U32(0); bmp.U32(54);
        bmp.U32(40); bmp.U32(w); bmp.U32(h); bmp.U16(1); bmp.U16(24); bmp.U32(0); bmp.U32(pitch * h); bmp.U32(0); bmp.U32(0); bmp.U32(0); bmp.U32(0);
        for (DWORD i = 0; i < pitch * h; i++) bmp.v.push_back(0x80);
        SceneWrite(dir + "\\gscene\\tex.bmp", &bmp.v[0], (DWORD) bmp.v.size());
    }

    struct SceneCwd
    {
        char old[MAX_PATH];
        SceneCwd(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~SceneCwd() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(Graphic3DAniObj)
{
public:
    TEST_METHOD(CreateDrawHitAndRelease)
    {
        SceneDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SceneDir();
        BuildSceneFixtures(dir);
        SceneCwd cwd(dir);
        CMyBitmap::PackFileOpen("gscene.wdf");

        Assert::IsNull(Scene3DCreate(0));
        Assert::IsNull(Scene3DCreate("gscene/missing.c3"));
        Assert::IsNull(Scene3DCreate("gscene/phyonly.c3"));
        I3DAniObj* obj = Scene3DCreate("gscene/obj.c3");
        Assert::IsNotNull(obj);

        void** vt = *(void***) obj;
        typedef void (__fastcall *FnDword)(void*, void*, DWORD);
        typedef void (__fastcall *FnTwo)(void*, void*, DWORD, DWORD);
        typedef void (__fastcall *FnVoid)(void*, void*);
        typedef bool (__fastcall *FnHit)(void*, void*, int, int, float*, float*, float*);
        ((FnDword) vt[2])(obj, 0, 0);
        ((FnDword) vt[6])(obj, 0, 0);
        ((FnDword) vt[6])(obj, 0, 4);
        ((FnTwo) vt[7])(obj, 0, 5, 6);
        ((FnDword) vt[8])(obj, 0, 0xFFFFFFFF);
        D3DMATERIAL8 mat;
        ZeroMemory(&mat, sizeof(mat));
        mat.Diffuse.a = mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = 1.0f;
        obj->SetMaterial(&mat);

        I3DTexture* tex = TextureCreate("gscene/tex.bmp");
        Assert::IsNotNull(tex);
        obj->ChangeTexture(0);
        obj->ChangeTexture(tex, (char*) "other");
        obj->ChangeTexture(tex, (char*) "seifuku");
        obj->ChangeMixTexture(tex);

        Assert::IsTrue(CMyBitmap::GameCameraCreate() != FALSE);
        CMySize scr = { 800, 600 };
        CMyBitmap::GameCameraBuild(scr);
        int sw = 0, sh = 0;
        CMyBitmap::GetScreenSize(sw, sh);
        CMyBitmap::GameCameraSet(0.25f, -10.0f, 0.25f, 1);
        CMyBitmap::GameCameraSet(0.25f, 10.0f, 0.0f, 2);
        D3DXVECTOR3 from, rdir;
        CMyBitmap::ScrRayBuild(sw / 2, sh / 2, from, rdir);
        if (CMyBitmap::GameCameraIsProject() == FALSE)
        {
            Assert::AreEqual(-10.0f, from.z); Assert::IsTrue(rdir.z > 0.99f);
            float a = -1, b = -1, c = -1;
            Assert::IsTrue(((FnHit) vt[10])(obj, 0, sw / 2, sh / 2, &a, &b, &c));
            D3DXVECTOR3 v0, v1, v2, want;
            v0.x = 0; v0.y = 0; v0.z = 0; v1.x = 1; v1.y = 0; v1.z = 0; v2.x = 0; v2.y = 1; v2.z = 0;
            Assert::IsTrue(IntersectTriangle(from, rdir, v0, v1, v2, want) != FALSE);
            Assert::AreEqual(want.x, a, 1e-4f); Assert::AreEqual(want.y, b, 1e-4f); Assert::AreEqual(want.z, c, 1e-4f);
            CMyBitmap::GameCameraSet(5.0f, -10.0f, 5.0f, 1);
            CMyBitmap::GameCameraSet(5.0f, 10.0f, 0.0f, 2);
            a = b = c = -1;
            Assert::IsFalse(obj->HitTest(sw / 2, sh / 2, &a, &b, &c));
            Assert::AreEqual(-1.0f, a); Assert::AreEqual(-1.0f, c);
        }
        else
            Logger::WriteMessage("the ray is not the plain one (a projection camera is set); the hit test not run");

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        ((FnVoid) vt[3])(obj, 0);
        obj->Draw();
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        typedef DWORD (__fastcall *FnRelease)(void*, void*);
        Assert::AreEqual<DWORD>(0, ((FnRelease) vt[0])(obj, 0));
        Assert::AreEqual<DWORD>(0, tex->Release());
        CMyBitmap::PrepareSprite();
    }
};
