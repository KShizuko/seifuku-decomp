#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include <vector>
#include "c3_sprite.h"
#include "c3_datafile.h"
#include "c3_datafile.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(SpriteVertex) == 0x1C, "");
static_assert(offsetof(SpriteVertex, rhw) == 0x0C && offsetof(SpriteVertex, color) == 0x10 && offsetof(SpriteVertex, u) == 0x14 && offsetof(SpriteVertex, v) == 0x18, "");
static_assert(sizeof(C3Sprite) == 0x74, "");
static_assert(offsetof(C3Sprite, vertex) == 0 && offsetof(C3Sprite, lpTex) == 0x70, "");
static_assert(offsetof(C3Sprite, vertex[1].v) == 0x34 && offsetof(C3Sprite, vertex[2].u) == 0x4C && offsetof(C3Sprite, vertex[3].u) == 0x68 && offsetof(C3Sprite, vertex[3].v) == 0x6C, "");
static_assert(SPRITE_VERTEX == 0x144, "");

static_assert(std::is_same<decltype(&Sprite_Load), BOOL (*)(C3Sprite**, const char*, D3DPOOL, BOOL, D3DCOLOR)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Create), BOOL (*)(C3Sprite**, DWORD, DWORD, DWORD, D3DFORMAT, D3DPOOL)>::value, "");
static_assert(std::is_same<decltype(&Sprite_SetCoor), void (*)(C3Sprite*, RECT*, int, int, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Sprite_SetColor), void (*)(C3Sprite*, BYTE, BYTE, BYTE, BYTE)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Draw), BOOL (*)(C3Sprite*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Sprite_DrawEx), BOOL (*)(C3Sprite*, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Lock), BOOL (*)(C3Sprite*, RECT*, D3DLOCKED_RECT*)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Unlock), BOOL (*)(C3Sprite*)>::value && std::is_same<decltype(&Sprite_Mirror), void (*)(C3Sprite*)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Rotate), void (*)(C3Sprite*, int)>::value, "");
static_assert(std::is_same<decltype(&Sprite_Prepare), void (*)(void)>::value && std::is_same<decltype(&Sprite_Unload), void (*)(C3Sprite**)>::value && std::is_same<decltype(&Sprite_Clear), void (*)(C3Sprite*)>::value, "");

namespace
{
    struct DllDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        DllDevice() : d3d(0), dev(0), slot(0)
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
        ~DllDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    std::vector<BYTE> MakeBmp24(int w, int h, DWORD color)
    {
        std::vector<BYTE> v;
        const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++) { v.push_back((BYTE) color); v.push_back((BYTE) (color >> 8)); v.push_back((BYTE) (color >> 16)); }
            for (DWORD p = w * 3; p < pitch; p++) v.push_back(0);
        }
        return v;
    }

    void BuildFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 0; hdr.offset = sizeof(hdr);
        SeifukuFixtures::WriteBlob(dir + "\\sprite.wdf", &hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\sprite").c_str(), 0);
        std::vector<BYTE> tex = MakeBmp24(8, 4, 0x0000FF);
        SeifukuFixtures::WriteBlob(dir + "\\sprite\\sprite.bmp", &tex[0], (DWORD) tex.size());
    }
}

TEST_CLASS(C3SpriteHeader)
{
public:
    TEST_METHOD(ClearImageMatchesSpriteClear)
    {
        C3Sprite s;
        memset(&s, 0xCD, sizeof(s));
        Sprite_Clear(&s);
        const DWORD* w = reinterpret_cast<const DWORD*>(&s);
        for (int v = 0; v < 4; v++)
        {
            Assert::AreEqual<DWORD>(0x3F800000, w[v * 7 + 3]);
            Assert::AreEqual<DWORD>(0xFFFFFFFF, w[v * 7 + 4]);
        }
        Assert::AreEqual<DWORD>(0x3F800000, w[0x34 / 4]);
        Assert::AreEqual<DWORD>(0x3F800000, w[0x4C / 4]);
        Assert::AreEqual<DWORD>(0x3F800000, w[0x68 / 4]);
        Assert::AreEqual<DWORD>(0x3F800000, w[0x6C / 4]);
        Assert::AreEqual<DWORD>(0, w[0x14 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x70 / 4]);
    }

    TEST_METHOD(SetColorPacksArgb)
    {
        Assert::AreEqual<DWORD>(0x80FF0102, D3DCOLOR_ARGB(0x80, 0xFF, 0x01, 0x02));
    }
};

TEST_CLASS(C3SpriteFunctions)
{
public:
    TEST_METHOD(ColorAndMirror)
    {
        C3Sprite s;
        Sprite_Clear(&s);
        Sprite_SetColor(&s, 0x80, 0x10, 0x20, 0x30);
        for (int v = 0; v < 4; v++) Assert::AreEqual<DWORD>(0x80102030, s.vertex[v].color);
        Sprite_Mirror(&s);
        Assert::AreEqual(1.0f, s.vertex[0].u); Assert::AreEqual(1.0f, s.vertex[1].u);
        Assert::AreEqual(0.0f, s.vertex[2].u); Assert::AreEqual(0.0f, s.vertex[3].u);
        Assert::AreEqual(0.0f, s.vertex[0].v); Assert::AreEqual(1.0f, s.vertex[1].v);
    }

    TEST_METHOD(RotateTurnsTheQuadAboutItsCentre)
    {
        C3Sprite s;
        Sprite_Clear(&s);
        s.vertex[0].x = 0;  s.vertex[0].y = 0;
        s.vertex[1].x = 0;  s.vertex[1].y = 20;
        s.vertex[2].x = 10; s.vertex[2].y = 0;
        s.vertex[3].x = 10; s.vertex[3].y = 20;
        Sprite_Rotate(&s, 90);
        Assert::AreEqual(15.0f, s.vertex[0].x, 1e-3f); Assert::AreEqual(5.0f,  s.vertex[0].y, 1e-3f);
        Assert::AreEqual(-5.0f, s.vertex[1].x, 1e-3f); Assert::AreEqual(5.0f,  s.vertex[1].y, 1e-3f);
        Assert::AreEqual(15.0f, s.vertex[2].x, 1e-3f); Assert::AreEqual(15.0f, s.vertex[2].y, 1e-3f);
        Assert::AreEqual(-5.0f, s.vertex[3].x, 1e-3f); Assert::AreEqual(15.0f, s.vertex[3].y, 1e-3f);

        C3Sprite t;
        Sprite_Clear(&t);
        t.vertex[0].x = 0;  t.vertex[0].y = 0;
        t.vertex[1].x = 0;  t.vertex[1].y = 20;
        t.vertex[2].x = 10; t.vertex[2].y = 0;
        t.vertex[3].x = 10; t.vertex[3].y = 20;
        Sprite_Rotate(&t, 360 + 90);
        for (int v = 0; v < 4; v++) { Assert::AreEqual(s.vertex[v].x, t.vertex[v].x, 1e-3f); Assert::AreEqual(s.vertex[v].y, t.vertex[v].y, 1e-3f); }
        Sprite_Rotate(0, 90);
    }

    TEST_METHOD(LoadSetCoorDrawLockUnload)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("sprite.wdf");

        C3Sprite* s = 0;
        Assert::IsTrue(Sprite_Load(&s, (char*) "sprite/sprite.bmp", D3DPOOL_MANAGED, TRUE, 0));
        Assert::IsNotNull(s); Assert::IsNotNull(s->lpTex);
        int nTex = s->lpTex->nID;
        Assert::IsTrue(nTex >= 0 && g_lpTex[nTex] == s->lpTex);
        Assert::AreEqual(-0.5f, s->vertex[0].x); Assert::AreEqual(7.5f, s->vertex[2].x); Assert::AreEqual(3.5f, s->vertex[1].y);
        Assert::AreEqual(0.0f, s->vertex[0].u); Assert::AreEqual(1.0f, s->vertex[3].u); Assert::AreEqual(1.0f, s->vertex[3].v);

        RECT rc = { 2, 1, 6, 3 };
        Sprite_SetCoor(s, &rc, 10, 20, 40, 10);
        Assert::AreEqual(0.25f, s->vertex[0].u); Assert::AreEqual(0.25f, s->vertex[0].v);
        Assert::AreEqual(0.75f, s->vertex[2].u); Assert::AreEqual(0.75f, s->vertex[1].v);
        Assert::AreEqual(9.5f, s->vertex[0].x); Assert::AreEqual(49.5f, s->vertex[2].x); Assert::AreEqual(9.5f, s->vertex[1].x);
        Assert::AreEqual(19.5f, s->vertex[0].y); Assert::AreEqual(29.5f, s->vertex[1].y); Assert::AreEqual(19.5f, s->vertex[2].y);
        Assert::AreEqual(49.5f, s->vertex[3].x); Assert::AreEqual(29.5f, s->vertex[3].y);
        Sprite_SetCoor(s, &rc, 1, 2, 0, 0);
        Assert::AreEqual(8.5f, s->vertex[2].x); Assert::AreEqual(5.5f, s->vertex[3].y);

        Sprite_Prepare();
        Assert::IsTrue(Begin3D());
        Assert::IsTrue(Sprite_Draw(s, 0));
        Assert::IsTrue(Sprite_Draw(s, 1));
        Assert::IsTrue(Sprite_Draw(s, 2));
        Assert::IsTrue(Sprite_DrawEx(s, D3DBLEND_ONE, D3DBLEND_ONE));
        DWORD v = 0;
        dd.dev->GetRenderState(D3DRS_SRCBLEND, &v);   Assert::AreEqual<DWORD>(D3DBLEND_ONE, v);
        dd.dev->GetRenderState(D3DRS_DESTBLEND, &v);  Assert::AreEqual<DWORD>(D3DBLEND_ONE, v);
        dd.dev->GetRenderState(D3DRS_ALPHAREF, &v);   Assert::AreEqual<DWORD>(8, v);
        dd.dev->GetRenderState(D3DRS_ALPHAFUNC, &v);  Assert::AreEqual<DWORD>(D3DCMP_GREATEREQUAL, v);
        dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &v); Assert::AreEqual<DWORD>(1, v);
        End3D();

        D3DLOCKED_RECT lr;
        ZeroMemory(&lr, sizeof(lr));
        Assert::IsTrue(Sprite_Lock(s, NULL, &lr) != FALSE);
        Assert::IsNotNull(lr.pBits);
        Assert::AreEqual<DWORD>(0xFF0000FF, *(DWORD*) lr.pBits);
        Assert::IsTrue(Sprite_Unlock(s) != FALSE);

        C3Sprite* c = 0;
        Assert::IsTrue(Sprite_Create(&c, 16, 16, 1, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED));
        Assert::IsNotNull(c->lpTex);
        Assert::AreEqual<unsigned>(16, c->lpTex->Info.Width);
        c->lpTex->lpTex->Release();

        Sprite_Unload(&s);
        Assert::IsNull(s);
        Assert::IsNull(g_lpTex[nTex]);
    }
};
