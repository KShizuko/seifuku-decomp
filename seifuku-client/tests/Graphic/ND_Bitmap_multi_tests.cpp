#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "ND_Bitmap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(IMultiBmp) == 4 && sizeof(IMultiRect) == 4 && sizeof(IPuzzleCell) == 4, "");
static_assert(sizeof(ITargetBmp) == 4 && sizeof(IPuzzleTriangle) == 4 && sizeof(IPuzzleBlock) == 4, "");
static_assert(sizeof(SpriteVertex) == 0x1C, "");
static_assert(sizeof(PhyOutVertex) == 0x24, "");
static_assert(std::is_same<decltype(&TargetBmpCreate), ITargetBmp* (*)(DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&PuzzelBlockCreate), IPuzzleBlock* (*)(DWORD, DWORD, DWORD, DWORD)>::value, "");

namespace
{
    struct MultiDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        MultiDevice() : d3d(0), dev(0), slot(0)
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
        ~MultiDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    void MultiWrite(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string MultiDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    void BuildMultiFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        MultiWrite(dir + "\\gmulti.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gmulti").c_str(), 0);
        std::vector<BYTE> v;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (DWORD i = 0; i < pitch * h; i++) v.push_back(0x60);
        MultiWrite(dir + "\\gmulti\\a.bmp", &v[0], (DWORD) v.size());
    }

    struct MultiCwd
    {
        char old[MAX_PATH];
        MultiCwd(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~MultiCwd() { SetCurrentDirectoryA(old); }
    };

    template <class T> T At(void* obj, size_t off) { T t; memcpy(&t, (BYTE*) obj + off, sizeof(T)); return t; }
    typedef DWORD (__fastcall *FnRelease)(void*, void*);
    DWORD ReleaseBySlot0(void* obj) { return ((FnRelease) (*(void***) obj)[0])(obj, 0); }
}

TEST_CLASS(GraphicMultiPuzzleTarget)
{
public:
    TEST_METHOD(MultiBmpAndMultiRect)
    {
        MultiDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = MultiDir();
        BuildMultiFixtures(dir);
        MultiCwd cwd(dir);
        CMyBitmap::PackFileOpen("gmulti.wdf");
        CMyBitmap* bmp = MyBitmapCreate("gmulti/a.bmp");
        Assert::IsNotNull(bmp);

        IMultiBmp* mb = MultiBmpCreate();
        Assert::IsNotNull(mb);
        Assert::AreEqual<DWORD>(0, At<DWORD>(mb, 4));
        mb->Add(10, 20, 30, 40, 0.0f, 0.0f, 1.0f, 0.125f, 0.25f, 1.0f, 0.5f, 0.75f);
        Assert::AreEqual<DWORD>(1, At<DWORD>(mb, 4));
        SpriteVertex* v = At<SpriteVertex*>(mb, 8);
        const float want[6][4] = { { 10, 20, 0.0f, 0.0f }, { 40, 20, 1.0f, 0.125f }, { 10, 60, 0.25f, 1.0f },
                                   { 40, 20, 1.0f, 0.125f }, { 40, 60, 0.5f, 0.75f }, { 10, 60, 0.25f, 1.0f } };
        for (int i = 0; i < 6; i++)
        {
            Assert::AreEqual(want[i][0], v[i].x); Assert::AreEqual(want[i][1], v[i].y);
            Assert::AreEqual(want[i][2], v[i].u); Assert::AreEqual(want[i][3], v[i].v);
            Assert::AreEqual(0.0f, v[i].z); Assert::AreEqual(1.0f, v[i].rhw); Assert::AreEqual<DWORD>(0xFFFFFFFF, v[i].color);
        }
        typedef void (__fastcall *FnVoid)(void*, void*);
        typedef void (__fastcall *FnShow)(void*, void*, CMyBitmap*);
        void** vt = *(void***) mb;
        ((FnVoid) vt[2])(mb, 0);
        Assert::AreEqual<DWORD>(0, At<DWORD>(mb, 4));
        for (int i = 0; i < 4097; i++) mb->Add(i, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1);
        Assert::AreEqual<DWORD>(4096, At<DWORD>(mb, 4));
        mb->Clear();
        mb->Add(0, 0, 8, 4, 0, 0, 1, 0, 0, 1, 1, 1);

        IMultiRect* mr = MultiRectCreate();
        Assert::IsNotNull(mr);
        D3DXVECTOR3 pos[4];
        pos[0].x = 0; pos[0].y = 0; pos[0].z = 0;
        pos[1].x = 1; pos[1].y = 0; pos[1].z = 0;
        pos[2].x = 0; pos[2].y = 1; pos[2].z = 0;
        pos[3].x = 1; pos[3].y = 1; pos[3].z = 0;
        float uv[8] = { 0, 0, 1, 0, 0, 1, 1, 1 };
        mr->Add(pos, uv);
        Assert::AreEqual<DWORD>(1, At<DWORD>(mr, 4));
        PhyOutVertex* pv = (PhyOutVertex*) ((BYTE*) mr + 8);
        const int order[6] = { 0, 1, 2, 1, 3, 2 };
        for (int i = 0; i < 6; i++)
        {
            Assert::AreEqual(pos[order[i]].x, pv[i].x); Assert::AreEqual(pos[order[i]].y, pv[i].y); Assert::AreEqual(0.0f, pv[i].z);
            Assert::AreEqual(0.0f, pv[i].nx); Assert::AreEqual(0.0f, pv[i].ny);
            Assert::AreEqual(-1.0f, pv[i].nz);
            Assert::AreEqual<DWORD>(0xFFFFFFFF, pv[i].color);
            Assert::AreEqual(uv[order[i] * 2], pv[i].u); Assert::AreEqual(uv[order[i] * 2 + 1], pv[i].v);
        }

        D3DMATERIAL8 mat;
        ZeroMemory(&mat, sizeof(mat));
        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        mb->Show(0);
        ((FnShow) vt[3])(mb, 0, bmp);
        mr->Show(0, &mat, false, 5, 6);
        mr->Show(bmp, &mat, true, 5, 6);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        void** vr = *(void***) mr;
        ((FnVoid) vr[2])(mr, 0);
        Assert::AreEqual<DWORD>(0, At<DWORD>(mr, 4));
        Assert::AreEqual<DWORD>(0, ReleaseBySlot0(mr));
        Assert::AreEqual<DWORD>(0, ReleaseBySlot0(mb));
        Assert::AreEqual<DWORD>(0, bmp->Release());
        CMyBitmap::PrepareSprite();
    }

    TEST_METHOD(PuzzleCellTriangleBlock)
    {
        MultiDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = MultiDir();
        BuildMultiFixtures(dir);
        MultiCwd cwd(dir);
        CMyBitmap::PackFileOpen("gmulti.wdf");
        CMyBitmap* bmp = MyBitmapCreate("gmulti/a.bmp");
        Assert::IsNotNull(bmp);

        IPuzzleCell* cell = PuzzelCellCreate();
        Assert::IsNotNull(cell);
        for (int i = 0; i < 60; i++) Assert::AreEqual<DWORD>(0xFFFFFFFF, At<DWORD>(cell, 4 + i * 4));
        cell->SetColor(1, 2, 0x11223344, 1);
        Assert::AreEqual<DWORD>(0x11223344, At<DWORD>(cell, 4 + (2 * 3 + 1) * 4));
        cell->SetColor(4, 7, 0x55667788, 2);
        Assert::AreEqual<DWORD>(0x55667788, At<DWORD>(cell, 0x40 + (2 * 3 + 1) * 4));
        cell->SetColor(0, 0, 0x01020304, 9);
        Assert::AreEqual<DWORD>(0x01020304, At<DWORD>(cell, 0x7C));
        cell->SetColor(2, 4, 0x0A0B0C0D, 0);
        Assert::AreEqual<DWORD>(0x0A0B0C0D, At<DWORD>(cell, 4 + 14 * 4));
        Assert::AreEqual<DWORD>(0xFFFFFFFF, At<DWORD>(cell, 0xB8));

        IPuzzleTriangle* tri = PuzzelTriangleCreate();
        Assert::IsNotNull(tri);
        Assert::AreEqual<DWORD>(5, At<DWORD>(tri, 0x58)); Assert::AreEqual<DWORD>(6, At<DWORD>(tri, 0x5C)); Assert::AreEqual<DWORD>(4, At<DWORD>(tri, 0x60));
        tri->SetVertex(4, 10, 20, 0x80402010, 0.5f, 0.25f);
        SpriteVertex tv = At<SpriteVertex>(tri, 4 + 0x1C);
        Assert::AreEqual(10.0f, tv.x); Assert::AreEqual(20.0f, tv.y); Assert::AreEqual(0.0f, tv.z); Assert::AreEqual(1.0f, tv.rhw);
        Assert::AreEqual<DWORD>(0x80402010, tv.color); Assert::AreEqual(0.5f, tv.u); Assert::AreEqual(0.25f, tv.v);
        tv = At<SpriteVertex>(tri, 4);
        Assert::AreEqual(0.0f, tv.x); Assert::AreEqual(1.0f, tv.rhw); Assert::AreEqual<DWORD>(0xFFFFFFFF, tv.color);
        tri->SetVertex(0, 0, 0, 0xFFFFFFFF, 0, 0);
        tri->SetVertex(2, 0, 30, 0xFFFFFFFF, 0, 1);
        typedef void (__fastcall *FnBlend)(void*, void*, DWORD, DWORD, DWORD);
        ((FnBlend) (*(void***) tri)[1])(tri, 0, 2, 2, 7);
        Assert::AreEqual<DWORD>(2, At<DWORD>(tri, 0x58)); Assert::AreEqual<DWORD>(2, At<DWORD>(tri, 0x5C)); Assert::AreEqual<DWORD>(7, At<DWORD>(tri, 0x60));
        tri->SetBlend(5, 6, 4);

        IPuzzleBlock* blk = PuzzelBlockCreate(100, 50, 2, 1);
        Assert::IsNotNull(blk);
        Assert::AreEqual<DWORD>(5, At<DWORD>(blk, 0x0C)); Assert::AreEqual<DWORD>(6, At<DWORD>(blk, 0x10)); Assert::AreEqual<DWORD>(4, At<DWORD>(blk, 0x14));
        Assert::AreEqual<DWORD>(100, At<DWORD>(blk, 0x18)); Assert::AreEqual<DWORD>(50, At<DWORD>(blk, 0x1C));
        Assert::AreEqual<DWORD>(2, At<DWORD>(blk, 0x20)); Assert::AreEqual<DWORD>(1, At<DWORD>(blk, 0x24));
        SpriteVertex* bv = At<SpriteVertex*>(blk, 4);
        Assert::AreEqual(0.0f, bv[0].x); Assert::AreEqual(0.0f, bv[0].y); Assert::AreEqual(0.0f, bv[0].u); Assert::AreEqual(0.0f, bv[0].v);
        Assert::AreEqual(50.0f, bv[1].x); Assert::AreEqual(0.5f, bv[1].u);
        Assert::AreEqual(100.0f, bv[5].x); Assert::AreEqual(50.0f, bv[5].y); Assert::AreEqual(1.0f, bv[5].u); Assert::AreEqual(1.0f, bv[5].v);
        Assert::AreEqual(1.0f, bv[5].rhw); Assert::AreEqual<DWORD>(0xFFFFFFFF, bv[5].color);
        WORD* bi = At<WORD*>(blk, 8);
        const WORD wantIdx[12] = { 0, 1, 3, 1, 4, 3, 1, 2, 4, 2, 5, 4 };
        for (int i = 0; i < 12; i++) Assert::AreEqual<int>(wantIdx[i], bi[i]);
        blk->SetColor(4, 3, 0x12345678);
        Assert::AreEqual<DWORD>(0x12345678, bv[4].color);
        blk->SetUV(2, 0, 0.875f, 0.125f);
        Assert::AreEqual(0.875f, bv[2].u); Assert::AreEqual(0.125f, bv[2].v);
        blk->SetSize(200, 80);
        Assert::AreEqual<DWORD>(200, At<DWORD>(blk, 0x18)); Assert::AreEqual<DWORD>(80, At<DWORD>(blk, 0x1C));
        blk->SetBlend(2, 2, 7);
        Assert::AreEqual<DWORD>(7, At<DWORD>(blk, 0x14));

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        cell->Show(0, 0, 0, 0, 0, 0, 0, 0);
        cell->Show(5, 6, 0, 0, bmp, 0, 0, 0);
        cell->Show(5, 6, 16, 8, bmp, bmp, 0, 0);
        typedef void (__fastcall *FnCell)(void*, void*, int, int, int, int, CMyBitmap*, CMyBitmap*, CMyBitmap*, CMyBitmap*);
        ((FnCell) (*(void***) cell)[2])(cell, 0, 5, 6, 16, 8, bmp, bmp, bmp, bmp);
        tri->Show(0);
        tri->Show(bmp);
        blk->Show(1, 2, 0);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        Assert::AreEqual<DWORD>(0, ReleaseBySlot0(blk));
        Assert::AreEqual<DWORD>(0, ReleaseBySlot0(tri));
        Assert::AreEqual<DWORD>(0, ReleaseBySlot0(cell));
        Assert::AreEqual<DWORD>(0, bmp->Release());
        CMyBitmap::PrepareSprite();
    }

    TEST_METHOD(TargetBitmap)
    {
        MultiDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        C3Texture* probe = 0;
        if (!Texture_Create(&probe, 64, 64, 1, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET, TRUE))
        { Logger::WriteMessage("no render-target texture on this device; skipped"); return; }

        ITargetBmp* t = TargetBmpCreate(64, 64);
        Assert::IsNotNull(t);
        C3Texture* tex = t->GetTexture();
        Assert::IsNotNull(tex);
        typedef C3Texture* (__fastcall *FnTex)(void*, void*);
        Assert::IsTrue(((FnTex) (*(void***) t)[6])(t, 0) == tex);
        Assert::IsTrue(At<C3Texture*>(t, 0x3E4) == tex);
        Assert::IsNull(At<void*>(t, 0x3E8)); Assert::IsNull(At<void*>(t, 0x3EC));
        WORD* idx = (WORD*) ((BYTE*) t + 0x324);
        Assert::AreEqual<int>(0, idx[0]); Assert::AreEqual<int>(5, idx[1]); Assert::AreEqual<int>(1, idx[2]);
        Assert::AreEqual<int>(18, idx[45]); Assert::AreEqual<int>(23, idx[46]); Assert::AreEqual<int>(19, idx[47]);
        Assert::AreEqual<int>(1, idx[48]); Assert::AreEqual<int>(5, idx[49]); Assert::AreEqual<int>(6, idx[50]);
        Assert::AreEqual<int>(19, idx[93]); Assert::AreEqual<int>(23, idx[94]); Assert::AreEqual<int>(24, idx[95]);

        for (int i = 0; i < 25; i++) t->SetAlpha((BYTE) i, 0xFF);
        t->SetAlpha(3, 0x80);
        Assert::AreEqual<DWORD>(0x80, At<DWORD>(t, 4 + 3 * 4));
        DWORD after = At<DWORD>(t, 0x68);
        t->SetAlpha(25, 0x11);
        Assert::AreEqual<DWORD>(after, At<DWORD>(t, 0x68));

        LPDIRECT3DSURFACE9 rt = 0, ds = 0;
        bool hasTargets = SUCCEEDED(dd.dev->GetRenderTarget(0, &rt)) && SUCCEEDED(dd.dev->GetDepthStencilSurface(&ds));
        if (rt) rt->Release();
        if (ds) ds->Release();
        if (hasTargets)
        {
            Assert::IsTrue(t->Begin() != FALSE);
            Assert::IsNotNull(At<void*>(t, 0x3E8));
            t->End();
            Assert::IsNull(At<void*>(t, 0x3E8)); Assert::IsNull(At<void*>(t, 0x3EC));
        }
        else
            Logger::WriteMessage("no render target on this device; Begin / End not run");

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        t->Show(1, 2, 30, 40);
        t->ShowAlpha(10, 20, 40, 80, 0xFFFFFF);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);
        SpriteVertex g = At<SpriteVertex>(t, 0x68 + 6 * 0x1C);
        Assert::AreEqual(19.5f, g.x); Assert::AreEqual(39.5f, g.y); Assert::AreEqual(0.0f, g.z); Assert::AreEqual(1.0f, g.rhw);
        Assert::AreEqual(0.25f, g.u); Assert::AreEqual(0.25f, g.v); Assert::AreEqual<DWORD>(0xFFFFFFFF, g.color);
        g = At<SpriteVertex>(t, 0x68 + 3 * 0x1C);
        Assert::AreEqual<DWORD>(0x80FFFFFF, g.color); Assert::AreEqual(39.5f, g.x); Assert::AreEqual(19.5f, g.y);
        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        t->ShowAlpha(10, 20, 40, 80, 0xAA123456);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);
        g = At<SpriteVertex>(t, 0x68 + 3 * 0x1C);
        Assert::AreEqual<DWORD>(0x80123456, g.color);
        CMyBitmap::PrepareSprite();
    }
};
