#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include <vector>
#include "c3_scene.h"
#include "c3_main.h"
#include "c3_datafile.h"
#include "c3_datafile.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Scene) == 0x70, "");
static_assert(offsetof(C3Scene, lpName) == 0x00 && offsetof(C3Scene, dwVecCount) == 0x04 && offsetof(C3Scene, lpVB) == 0x08, "");
static_assert(offsetof(C3Scene, dwTriCount) == 0x0C && offsetof(C3Scene, lpIB) == 0x10, "");
static_assert(offsetof(C3Scene, lpTexName) == 0x14 && offsetof(C3Scene, nTex) == 0x18, "");
static_assert(offsetof(C3Scene, lplTexName) == 0x1C && offsetof(C3Scene, nlTex) == 0x20, "");
static_assert(offsetof(C3Scene, dwFrameCount) == 0x24 && offsetof(C3Scene, lpFrame) == 0x28 && offsetof(C3Scene, nFrame) == 0x2C, "");
static_assert(offsetof(C3Scene, matrix) == 0x30, "");

static_assert(sizeof(SceneVertex) == 0x28, "");
static_assert(offsetof(SceneVertex, nx) == 0x0C && offsetof(SceneVertex, u) == 0x18 && offsetof(SceneVertex, lu) == 0x20, "");
static_assert(sizeof(SceneEdge) == 8, "");
static_assert(SCENE_VERTEX == 0x212, "");

static_assert(std::is_same<decltype(&Scene_Load), BOOL (*)(C3Scene**, char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Scene_Save), BOOL (*)(char*, C3Scene*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Scene_Muliply), void (*)(C3Scene*, D3DXMATRIX*)>::value, "");
static_assert(std::is_same<decltype(&Scene_NextFrame), void (*)(C3Scene*, int)>::value, "");
static_assert(std::is_same<decltype(&Scene_Optimize), BOOL (*)(C3Scene*)>::value && std::is_same<decltype(&Scene_Draw), BOOL (*)(C3Scene*)>::value, "");
static_assert(std::is_same<decltype(&Scene_Prepare), void (*)(void)>::value && std::is_same<decltype(&Scene_Unload), void (*)(C3Scene**)>::value && std::is_same<decltype(&Scene_Clear), void (*)(C3Scene*)>::value, "");

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
        SeifukuFixtures::WriteBlob(dir + "\\scene.wdf", &hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\scene").c_str(), 0);
        std::vector<BYTE> tex = MakeBmp24(4, 4, 0x00FF00);
        SeifukuFixtures::WriteBlob(dir + "\\scene\\scene.bmp", &tex[0], (DWORD) tex.size());
        SeifukuFixtures::WriteBlob(dir + "\\seifuku_scene.c3", "MAXFILE C3 00001", 16);
    }

    struct TestScene
    {
        C3Scene s;
        SceneVertex vb[6];
        WORD ib[6];
        D3DXMATRIX frames[2];
        char name[8], tex[32];
        TestScene()
        {
            Scene_Clear(&s);
            for (int i = 0; i < 6; i++) { ZeroMemory(&vb[i], sizeof(SceneVertex)); vb[i].nz = 1.0f; }
            vb[0].x = 0; vb[0].y = 0; vb[1].x = 1; vb[1].y = 0; vb[2].x = 0; vb[2].y = 1;
            vb[3].x = 1; vb[3].y = 0; vb[4].x = 1; vb[4].y = 1; vb[5].x = 0; vb[5].y = 1;
            for (WORD i = 0; i < 6; i++) ib[i] = i;
            D3DXMatrixIdentity(&frames[0]); D3DXMatrixTranslation(&frames[1], 1, 2, 3);
            strcpy(name, "quad"); strcpy(tex, "scene/scene.bmp");
            s.lpName = name; s.lpTexName = tex; s.dwVecCount = 6; s.lpVB = vb; s.dwTriCount = 2; s.lpIB = ib;
            s.dwFrameCount = 2; s.lpFrame = frames;
        }
    };
}

TEST_CLASS(C3SceneHeader)
{
public:
    TEST_METHOD(ClearImageMatchesSceneClear)
    {
        C3Scene s;
        memset(&s, 0xCD, sizeof(s));
        Scene_Clear(&s);
        const DWORD* w = reinterpret_cast<const DWORD*>(&s);
        for (int i = 0; i < 12; i++)
            Assert::AreEqual<DWORD>(i == 6 || i == 8 ? 0xFFFFFFFF : 0, w[i]);
        for (int i = 12; i < 28; i++)
            Assert::AreEqual<DWORD>((i - 12) % 5 == 0 ? 0x3F800000 : 0, w[i]);
    }
};

TEST_CLASS(C3SceneFunctions)
{
public:
    TEST_METHOD(FrameAndMatrixHelpers)
    {
        C3Scene s;
        Scene_Clear(&s);
        s.dwFrameCount = 4; s.nFrame = 3;
        Scene_NextFrame(&s, 2);
        Assert::AreEqual(1, s.nFrame);
        D3DXMATRIX t;
        D3DXMatrixTranslation(&t, 1, 2, 3);
        Scene_Muliply(&s, &t);
        Scene_Muliply(&s, &t);
        Assert::AreEqual(2.0f, s.matrix._41); Assert::AreEqual(6.0f, s.matrix._43);
    }

    TEST_METHOD(SaveLoadOptimizeAndDraw)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("scene.wdf");

        TestScene ts;
        Assert::IsTrue(Scene_Save((char*) "seifuku_scene.c3", &ts.s, TRUE));
        C3Scene* l = 0;
        Assert::IsTrue(Scene_Load(&l, (char*) "seifuku_scene.c3", 0));
        Assert::AreEqual("quad", l->lpName);
        Assert::AreEqual<DWORD>(6, l->dwVecCount); Assert::AreEqual<DWORD>(2, l->dwTriCount);
        Assert::AreEqual(1.0f, l->lpVB[4].x); Assert::AreEqual<int>(5, l->lpIB[5]);
        Assert::AreEqual("scene/scene.bmp", l->lpTexName);
        Assert::IsTrue(l->nTex >= 0);
        Assert::AreEqual(-1, l->nlTex);
        Assert::AreEqual<DWORD>(2, l->dwFrameCount); Assert::AreEqual(3.0f, l->lpFrame[1]._43);

        Assert::IsTrue(Scene_Optimize(l));
        Assert::AreEqual<DWORD>(4, l->dwVecCount);
        Assert::AreEqual<int>(1, l->lpIB[3]); Assert::AreEqual<int>(2, l->lpIB[5]);
        Assert::AreEqual<int>(3, l->lpIB[4]);

        Scene_Prepare();
        Assert::IsTrue(Begin3D());
        D3DXMATRIX t; D3DXMatrixTranslation(&t, 5, 0, 0);
        Scene_Muliply(l, &t);
        Assert::IsTrue(Scene_Draw(l));
        Assert::IsTrue(D3DXMatrixIsIdentity(&l->matrix));
        End3D();
        C3Scene* miss = 0;
        Assert::IsFalse(Scene_Load(&miss, (char*) "seifuku_scene.c3", 1));
        Assert::IsNotNull(miss);
        Assert::AreEqual(-1, miss->nTex);
        Scene_Unload(&miss);
        int nTex = l->nTex;
        l->nTex = -1;
        Scene_Unload(&l);
        C3Texture* tex = g_lpTex[nTex];
        Texture_Unload(&tex);
        Assert::IsNull(g_lpTex[nTex]);
    }
};
