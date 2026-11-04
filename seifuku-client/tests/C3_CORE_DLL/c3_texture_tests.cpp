#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include <vector>
#include "c3_texture.h"
#include "c3_main.h"
#include "c3_datafile.h"
#include "c3_datafile.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Texture) == 0x2C, "");
static_assert(offsetof(C3Texture, nID) == 0x00, "");
static_assert(offsetof(C3Texture, nDupCount) == 0x04, "");
static_assert(offsetof(C3Texture, lpName) == 0x08, "");
static_assert(offsetof(C3Texture, lpTex) == 0x0C, "");
static_assert(offsetof(C3Texture, Info) == 0x10, "");
static_assert(offsetof(C3Texture, Info.Format) == 0x20, "");
static_assert(offsetof(C3Texture, Info.Width) == 0x10 && offsetof(C3Texture, Info.Height) == 0x14, "");

static_assert(TEX_MAX == 10240, "");
static_assert(sizeof(g_lpTex) == 0xA000, "");

static_assert(std::is_same<decltype(&Texture_Load), int (*)(C3Texture**, const char*, DWORD, D3DPOOL, BOOL, D3DCOLOR, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Texture_Create), BOOL (*)(C3Texture**, DWORD, DWORD, DWORD, D3DFORMAT, D3DPOOL, DWORD, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Texture_Update), BOOL (*)(C3Texture*, C3Texture*)>::value, "");
static_assert(std::is_same<decltype(&Texture_Unload), void (*)(C3Texture**)>::value && std::is_same<decltype(&Texture_Clear), void (*)(C3Texture*)>::value, "");
static_assert(std::is_same<decltype(&Texture_LockRect), BOOL (*)(C3Texture*, RECT, D3DLOCKED_RECT&)>::value, "");
static_assert(std::is_same<decltype(&Texture_Unlock), void (*)(C3Texture*)>::value, "");

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
        std::vector<BYTE> packed = MakeBmp24(4, 4, 0x00FF00), loose = MakeBmp24(8, 2, 0x0000FF);
        C3DataFileIndex idx[1];
        idx[0].uid = string_id("texture/packed.bmp"); idx[0].size = (DWORD) packed.size(); idx[0].space = 0;
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 1; hdr.offset = sizeof(hdr);
        idx[0].offset = sizeof(hdr) + sizeof(idx);
        std::string blob((const char*) &hdr, sizeof(hdr));
        blob.append((const char*) idx, sizeof(idx)).append((const char*) &packed[0], packed.size());
        SeifukuFixtures::WriteBlob(dir + "\\texture.wdf", blob.data(), (DWORD) blob.size());
        CreateDirectoryA((dir + "\\texture").c_str(), 0);
        SeifukuFixtures::WriteBlob(dir + "\\texture\\loose.bmp", &loose[0], (DWORD) loose.size());
        SeifukuFixtures::WriteBlob(dir + "\\texture\\bad.bmp", "BMnope", 6);
    }
}

TEST_CLASS(C3TextureHeader)
{
public:
    TEST_METHOD(ClearedLayoutMatchesTextureClear)
    {
        C3Texture t;
        memset(&t, 0xCD, sizeof(t));
        Texture_Clear(&t);
        const DWORD* w = reinterpret_cast<const DWORD*>(&t);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, w[0]);
        for (int i = 1; i < 11; i++)
            Assert::AreEqual<DWORD>(0, w[i]);
    }
};

TEST_CLASS(C3TextureRegistry)
{
public:
    TEST_METHOD(CreateDoesNotRegister)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        C3Texture* t = 0;
        Assert::IsTrue(Texture_Create(&t, 16, 8, 1, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 0, FALSE));
        Assert::IsNotNull(t); Assert::IsNotNull(t->lpTex);
        Assert::AreEqual(-1, t->nID);
        Assert::AreEqual<unsigned>(16, t->Info.Width); Assert::AreEqual<unsigned>(8, t->Info.Height);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, t->Info.Format);
        Assert::IsNull(t->lpName);
        t->lpTex->Release();
    }

    TEST_METHOD(CreateWithAddTakesAFreeSlot)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD count = g_dwTexCount;
        C3Texture* t = 0;
        Assert::IsTrue(Texture_Create(&t, 8, 8, 1, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 0, TRUE));
        Assert::IsNotNull(t);
        Assert::IsTrue(t->nID >= 0 && t->nID < TEX_MAX);
        Assert::IsTrue(g_lpTex[t->nID] == t);
        for (int i = 0; i < t->nID; i++) Assert::IsNotNull(g_lpTex[i]);
        Assert::AreEqual(count, g_dwTexCount);
        g_lpTex[t->nID] = 0;
        t->lpTex->Release();
    }

    TEST_METHOD(LockRectOnly32BitFormats)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        C3Texture* t = 0;
        Assert::IsTrue(Texture_Create(&t, 8, 4, 1, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, 0, FALSE));
        RECT rc = { 0, 0, 8, 4 };
        D3DLOCKED_RECT lr = { 0, 0 };
        Assert::IsTrue(Texture_LockRect(t, rc, lr) != FALSE);
        Assert::IsNotNull(lr.pBits);
        Assert::IsTrue(lr.Pitch >= 32);
        Texture_Unlock(t);
        t->Info.Format = D3DFMT_X8R8G8B8;
        Assert::IsTrue(Texture_LockRect(t, rc, lr) != FALSE);
        Texture_Unlock(t);
        t->Info.Format = D3DFMT_R5G6B5;
        D3DLOCKED_RECT untouched = { 7, (void*) 7 };
        Assert::IsFalse(Texture_LockRect(t, rc, untouched) != FALSE);
        Assert::AreEqual(7, untouched.Pitch);
        t->lpTex->Release();
    }

    TEST_METHOD(LoadFromLooseFileAndPackWithDuplicates)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("texture.wdf");

        DWORD count0 = g_dwTexCount;
        C3Texture* a = 0;
        int ia = Texture_Load(&a, (char*) "texture/loose.bmp", 1, D3DPOOL_MANAGED, TRUE, 0);
        Assert::IsTrue(ia >= 0);
        Assert::AreEqual<DWORD>(count0 + 1, g_dwTexCount);
        Assert::IsTrue(g_lpTex[ia] == a);
        Assert::AreEqual(ia, a->nID);
        Assert::AreEqual(1, a->nDupCount);
        Assert::AreEqual("texture/loose.bmp", a->lpName);
        Assert::AreEqual<unsigned>(8, a->Info.Width);
        Assert::AreEqual<int>(D3DFMT_X8R8G8B8, a->Info.Format);

        C3Texture* a2 = 0;
        Assert::AreEqual(ia, Texture_Load(&a2, (char*) "TEXTURE/LOOSE.BMP", 1, D3DPOOL_MANAGED, TRUE, 0));
        Assert::IsTrue(a2 == a);
        Assert::AreEqual(2, a->nDupCount);
        Assert::AreEqual<DWORD>(count0 + 1, g_dwTexCount);

        C3Texture* p = 0;
        int ip = Texture_Load(&p, (char*) "texture/packed.bmp", 1, D3DPOOL_MANAGED, TRUE, 0);
        Assert::IsTrue(ip >= 0 && ip != ia);
        Assert::AreEqual<unsigned>(4, p->Info.Width);
        Assert::AreEqual(1, p->nDupCount);
        Assert::AreEqual<DWORD>(count0 + 2, g_dwTexCount);
        C3Texture* q = 0;
        int iq = Texture_Load(&q, "texture/loose.bmp", 1, D3DPOOL_MANAGED, FALSE, 0, D3DX_FILTER_POINT);
        Assert::IsTrue(iq >= 0 && iq != ia);
        Assert::AreEqual<DWORD>(count0 + 3, g_dwTexCount);
        Texture_Unload(&q);
        Assert::IsNull(g_lpTex[iq]);
        Assert::AreEqual<DWORD>(count0 + 2, g_dwTexCount);

        C3Texture* m = 0;
        Assert::AreEqual(-1, Texture_Load(&m, (char*) "texture/missing.bmp", 1, D3DPOOL_MANAGED, TRUE, 0));
        Assert::AreEqual(-1, Texture_Load(&m, (char*) "texture/bad.bmp", 1, D3DPOOL_MANAGED, TRUE, 0));

        C3Texture* ua = a;
        Texture_Unload(&ua);
        Assert::IsTrue(g_lpTex[ia] == a);
        Assert::AreEqual(1, a->nDupCount);
        Texture_Unload(&ua);
        Assert::IsNull(g_lpTex[ia]);
        Assert::AreEqual<DWORD>(count0 + 1, g_dwTexCount);
        C3Texture* up = p;
        Texture_Unload(&up);
        Assert::IsNull(g_lpTex[ip]);
        Assert::AreEqual<DWORD>(count0, g_dwTexCount);
    }
};
