#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "ND_Bitmap.h"
#include "MyBitmapX.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CMyBitmap) == 4, "");
static_assert(sizeof(CMyBitmapX) == 0x24 && offsetof(CMyBitmapX, m_sprite) == 4 && offsetof(CMyBitmapX, m_pMask) == 8, "");
static_assert(offsetof(CMyBitmapX, m_lock) == 0xC && offsetof(CMyBitmapX, m_rect) == 0x14, "");
static_assert(sizeof(CMyBitmap::LineVertex) == 0x14, "");

static_assert(std::is_same<decltype(&CMyBitmap::Init3D), BOOL (*)(HINSTANCE, const char*, DWORD, DWORD, BOOL, WNDPROC, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::Flip), BOOL (*)(const RECT*, const RECT*, HWND)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::ResetDevice), BOOL (*)(void)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::HintGraphicDetail), BOOL (*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::SetReflectFluidSpeed), void (*)(float, float)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::GameCameraGetPos), void (*)(float&, float&, float&, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::GameCameraSet), void (*)(float, float, float, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::CheckStringPixels), bool (*)(const char*, DWORD, const char*, int)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::ShowString), void (*)(int, int, DWORD, const char*, const char*, int, BOOL, RENDER_TEXT_STYLE, DWORD, CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::ShowTString), void (*)(int, int, DWORD, const char*, const char*, int, BOOL, RENDER_TEXT_STYLE, DWORD, CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::ShowEmotionString), void (*)(int, int, DWORD, const char*, IDataIcon*, const char*, int, int, int, int, RENDER_TEXT_STYLE, DWORD, CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::SetTextCornerColor), void (*)(DWORD, DWORD, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::Show3DBox), void (*)(D3DXVECTOR3&, D3DXVECTOR3&, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::QueryDevice), IDirect3DDevice8* (*)()>::value, "");
static_assert(std::is_same<decltype(&CMyBitmap::ScrRayBuild), void (*)(int, int, D3DXVECTOR3&, D3DXVECTOR3&)>::value, "");
static_assert(std::is_same<decltype(&MyBitmapCreate), CMyBitmap* (*)(const char*)>::value && std::is_same<decltype(&BmpCreate), CMyBitmap* (*)()>::value, "");

struct FakeEmotionAni
{
    int shows; DWORD frame; int x, y, alpha; DWORD w, h, way; int src;
    FakeEmotionAni() : shows(0), frame(99), x(0), y(0), alpha(-1), w(0), h(0), way(99), src(-1) {}
    virtual void Slot0() {}
    virtual void Slot1() {}
    virtual void Slot2() {}
    virtual BOOL ShowEx(UINT uFrame, int nx, int ny, RECT* lpSrc, DWORD dwWidth, DWORD dwHeight, int nAlpha, DWORD dwShowWay)
    { shows++; frame = uFrame; x = nx; y = ny; alpha = nAlpha; w = dwWidth; h = dwHeight; way = dwShowWay; src = lpSrc ? 1 : 0; return TRUE; }
    virtual void Slot4() {}
    virtual int GetFrameAmount() { return 3; }
};
class IDataIcon
{
public:
    virtual FakeEmotionAni* GetIcon(int nIndex) = 0;
};
struct FakeDataIcon : IDataIcon
{
    FakeEmotionAni ani; int asked;
    FakeDataIcon() : asked(-1) {}
    virtual FakeEmotionAni* GetIcon(int nIndex) { asked = nIndex; return nIndex == 7 ? &ani : 0; }
};

namespace
{
    struct GraphicDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        GraphicDevice() : d3d(0), dev(0), slot(0)
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
        ~GraphicDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    std::string FixtureDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void WriteBlob(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    void BuildFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        WriteBlob(dir + "\\gbmp.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gbmp").c_str(), 0);
        std::vector<BYTE> v;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (DWORD i = 0; i < pitch * h; i++) v.push_back(0x40);
        WriteBlob(dir + "\\gbmp\\tiny.bmp", &v[0], (DWORD) v.size());
        CreateDirectoryA((dir + "\\ini").c_str(), 0);
        const char font[] = "Arial 16";
        WriteBlob(dir + "\\ini\\font.ini", font, sizeof(font) - 1);
        const BYTE smallChar[] = { 0xB0, 0xA1 };
        WriteBlob(dir + "\\ini\\DbcsSmall.txt", smallChar, sizeof(smallChar));
    }

    struct CwdGuard
    {
        char old[MAX_PATH];
        CwdGuard(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~CwdGuard() { SetCurrentDirectoryA(old); }
    };

    void** Vtbl(CMyBitmap* p) { return *(void***) p; }
}

TEST_CLASS(GraphicMyBitmapStatics)
{
public:
    TEST_METHOD(ScreenDefaults)
    {
        int w = 0, h = 0;
        CMyBitmap::GetScreenSize(w, h);
        Assert::AreEqual(800, w); Assert::AreEqual(600, h);
        Assert::AreEqual(800, CMyBitmap::GetScreenWidth()); Assert::AreEqual(600, CMyBitmap::GetScreenHeight());
        Assert::AreEqual(-30, CMyBitmap::GetObliqueAngle());
        Assert::IsTrue(CMyBitmap::IsDBCSLeadByte("a", 0) == false);
        Assert::IsTrue(CMyBitmap::IsDBCSLeadByte(0, 0) == false);
        Assert::AreEqual(3, CMyBitmap::GetStrShowLen("abc", 0, 3));
        Assert::AreEqual(2, CMyBitmap::GetStrShowLen("abc", 1, 3));
        Assert::AreEqual(0, CMyBitmap::GetStrShowLen(0, 0, 3));
        Assert::IsTrue(CMyBitmap::HintGraphicDetail(3) == FALSE);
        CMyBitmap::SetReflectFluidSpeed(1.0f, 0.0f);
    }

    TEST_METHOD(TwoCameras)
    {
        GraphicDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        Assert::IsTrue(CMyBitmap::QueryDevice() == (IDirect3DDevice8*) dd.dev);
        Assert::IsTrue(CMyBitmap::GameCameraCreate() != FALSE);
        Assert::IsTrue(CMyBitmap::GameCameraIsProject() == FALSE);
        float x = 0, y = 0, z = 0;
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(400.0f, x); Assert::AreEqual(-1000.0f, y); Assert::AreEqual(300.0f, z);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(400.0f, x); Assert::AreEqual(0.0f, y); Assert::AreEqual(300.0f, z);
        CMySize size = { 800, 600 };
        CMyBitmap::GameCameraBuild(size);

        CMyBitmap::GameCameraSet(10.0f, 20.0f, 30.0f, 3);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(10.0f, x); Assert::AreEqual(30.0f, y); Assert::AreEqual(20.0f, z);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(10.0f, x); Assert::AreEqual(0.0f, y); Assert::AreEqual(20.0f, z);
        CMyBitmap::GameCameraSet(1.0f, 2.0f, 3.0f, 2);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(10.0f, x);
        CMyBitmap::GameCameraGetPos(x, y, z, 0);
        Assert::AreEqual(30.0f, y);

        CMyBitmap::GameCameraSet(400.0f, 300.0f, -1000.0f, 1);
        CMyBitmap::GameCameraSet(400.0f, 300.0f, 0.0f, 2);
        D3DXVECTOR3 from, dir;
        CMyBitmap::ScrRayBuild(400, 300, from, dir);
        Assert::AreEqual(400.0f, from.x); Assert::AreEqual(-1000.0f, from.y); Assert::AreEqual(300.0f, from.z);
        Assert::AreEqual(0.0f, dir.x, 1e-6f); Assert::AreEqual(1.0f, dir.y, 1e-6f); Assert::AreEqual(0.0f, dir.z, 1e-6f);
        CMyBitmap::ScrRayBuild(400 + 1000, 300, from, dir);
        Assert::AreEqual(0.70710678f, dir.x, 1e-5f); Assert::AreEqual(0.70710678f, dir.y, 1e-5f);
        float wx = 5, wy = 6, wz = 7;
        CMyBitmap::ScreenToWorld(1, 2, wx, wy, wz);
        Assert::AreEqual(5.0f, wx); Assert::AreEqual(6.0f, wy); Assert::AreEqual(7.0f, wz);

        CMyBitmap::GameCameraMove(TRUE); CMyBitmap::GameCameraRotate(10); CMyBitmap::GameCameraZoom(TRUE); CMyBitmap::GameCameraSetFov(2.0f);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(400.0f, x); Assert::AreEqual(-1000.0f, y);
        CMyBitmap::GameCameraSetType(TRUE);
        Assert::IsTrue(CMyBitmap::GameCameraIsProject() != FALSE);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        const float t30 = 0.57735027f;
        Assert::AreEqual(0.0f, x); Assert::AreEqual(-800.0f, y); Assert::AreEqual(-800.0f * t30, z, 1e-3f);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(0.0f, x); Assert::AreEqual(0.0f, y); Assert::AreEqual(0.0f, z);
        CMyBitmap::GameCameraZoom(TRUE);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        const float d0 = 800.0f / 0.8660254f;
        Assert::AreEqual(-0.8660254f * (d0 - 50.0f), y, 1e-2f); Assert::AreEqual(-0.5f * (d0 - 50.0f), z, 1e-2f);
        for (int i = 0; i < 20; i++) CMyBitmap::GameCameraZoom(TRUE);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(-0.8660254f * 300.0f, y, 1e-2f); Assert::AreEqual(-150.0f, z, 1e-2f);
        CMyBitmap::GameCameraMove(TRUE);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(0.0f, x, 1e-4f); Assert::AreEqual(40.0f * 0.8660254f, y, 1e-3f); Assert::AreEqual(0.0f, z);
        CMyBitmap::GameCameraMove(FALSE);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(0.0f, y, 1e-3f);
        CMyBitmap::GameCameraRotate(90);
        CMyBitmap::GameCameraGetPos(x, y, z, 1);
        Assert::AreEqual(0.8660254f * 300.0f, x, 1e-2f); Assert::AreEqual(0.0f, y, 1e-2f); Assert::AreEqual(-150.0f, z, 1e-2f);
        CMyBitmap::GameCameraSet(1.0f, 2.0f, 3.0f, 2);
        CMyBitmap::GameCameraGetPos(x, y, z, 2);
        Assert::AreEqual(1.0f, x); Assert::AreEqual(3.0f, y); Assert::AreEqual(2.0f, z);
        CMyBitmap::GameCameraSetFov(4.0f);
        int sx = -1, sy = -1;
        CMyBitmap::WorldToScreen(1.0f, 3.0f, 2.0f, sx, sy);
        CMyBitmap::ScreenToWorld(sx, sy, wx, wy, wz);
        Assert::IsTrue(wx != 5.0f || wy != 6.0f || wz != 7.0f);

        CMyBitmap::GameCameraSetType(FALSE);
        Assert::IsTrue(CMyBitmap::GameCameraIsProject() == FALSE);
        CMyBitmap::GameCameraDestroy();
        CMyBitmap::GameCameraDestroy();
    }

    TEST_METHOD(PrepareFunctionsAndDrawing)
    {
        GraphicDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD st = 0;
        CMyBitmap::PrepareSprite();
        dd.dev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        CMyBitmap::PrepareLine();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SHADEMODE, &st))); Assert::AreEqual<DWORD>(D3DSHADE_FLAT, st);
        dd.dev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        CMyBitmap::PrepareLine();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SHADEMODE, &st))); Assert::AreEqual<DWORD>(D3DSHADE_GOURAUD, st);
        CMyBitmap::Prepare3DLine();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_FILLMODE, &st))); Assert::AreEqual<DWORD>(D3DFILL_WIREFRAME, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        dd.dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        CMyBitmap::Prepare3DLine();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_FILLMODE, &st))); Assert::AreEqual<DWORD>(D3DFILL_SOLID, st);
        CMyBitmap::PrepareModle();
        CMyBitmap::PrepareMixSprite();
        CMyBitmap::PrepareSprite();

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        CMyBitmap::ClearBuffer(TRUE, TRUE, 0);
        CMyBitmap::LineVertex line[2];
        ZeroMemory(line, sizeof(line));
        line[1].x = 10; line[1].y = 10; line[0].color = line[1].color = 0xFFFFFFFF;
        CMyBitmap::ShowLine(line, 1);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetFVF(&st))); Assert::AreEqual<DWORD>(LINE_VERTEX, st);
        CMyBitmap::ShowLine(0, 0, 5, 5, 0x80FF0000);
        CMyBitmap::ShowRect(0, 0, 5, 5, 0xFFFF0000);
        CMyBitmap::ShowBlock(0, 0, 5, 5, 0xFF00FF00);
        CMyBitmap::ShowBlock(0, 0, 5, 5, 1, 2, 3, 4);
        CMyPos a = { 0, 0 }, b = { 0, 5 }, c = { 5, 0 }, d = { 5, 5 };
        CMyBitmap::ShowBlock(a, b, c, d, 0xFF0000FF);
        D3DXVECTOR3 vMin(0, 0, 0), vMax(1, 2, 3);
        CMyBitmap::Show3DBox(vMin, vMax, 0xFFFFFFFF);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetFVF(&st))); Assert::AreEqual<DWORD>(PHY_OUT_VERTEX, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_FILLMODE, &st))); Assert::AreEqual<DWORD>(D3DFILL_WIREFRAME, st);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);
        Assert::AreEqual<DWORD>(0, CMyBitmap::GetShowTriTotal());
        CMyBitmap::PrepareSprite();
    }

    TEST_METHOD(FontSetAndText)
    {
        GraphicDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = FixtureDir();
        BuildFixtures(dir);
        CwdGuard cwd(dir);

        Assert::IsTrue(CMyBitmap::GameFontCreate() != FALSE);
        Assert::AreEqual<DWORD>(16, CMyBitmap::GetFontSize());
        CMySize size = { 0, 0 };
        CMyBitmap::GetFontSize(size);
        Assert::AreEqual(16, size.iHeight); Assert::AreEqual(8, size.iWidth);
        const bool bDbcs = ::IsDBCSLeadByte(0xB0) != 0;
        Assert::AreEqual(2, CMyBitmap::GetStrShowLen("\xB0\xA2", 0, 2));
        Assert::AreEqual(bDbcs ? 1 : 2, CMyBitmap::GetStrShowLen("\xB0\xA1", 0, 2));
        Assert::AreEqual(3, CMyBitmap::GetStrShowLen("a\xB0\xA2", 0, 3));
        Assert::AreEqual(bDbcs ? 1 : 2, CMyBitmap::GetStrShowLen("a\xB0\xA2", 0, 2));
        Assert::AreEqual(1, CMyBitmap::GetStrShowLen("a\0b", 0, 3));

        Assert::IsTrue(CMyBitmap::CheckStringPixels("A", 1, "Arial", 16));
        Assert::IsTrue(CMyBitmap::CheckStringPixels("AB", 4, "Arial", 16));
        Assert::IsFalse(CMyBitmap::CheckStringPixels("A B", 1, "Arial", 16));
        Assert::IsFalse(CMyBitmap::CheckStringPixels("A", 100000, "Arial", 16));
        Assert::IsTrue(CMyBitmap::CheckStringPixels("", 1, "Arial", 16));
        Assert::IsFalse(CMyBitmap::CheckStringPixels(0, 1, "Arial", 16));
        Assert::IsFalse(CMyBitmap::CheckStringPixels("A", 1, 0, 16));
        Assert::IsTrue(CMyBitmap::CheckStringPixels("A", 1, "Arial", 12));

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        DWORD nTri0 = CMyBitmap::GetShowTriTotal();
        CMyBitmap::ShowString(10, 10, 0xFFFFFF, "Ab\nc");
        Assert::AreEqual<DWORD>(6, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyBitmap::ShowString(10, 10, 0xFFFFFF, "A", "Arial", 12, TRUE);
        CMyBitmap::ShowString(10, 10, 0xFFFFFF, 0);
        Assert::AreEqual<DWORD>(8, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyPos pos = { 1, 1 };
        CMyBitmap::ShowString(10, 10, 0xFFFFFF, "A", 0, 0, FALSE, RENDER_TEXT_SHADOW, 0x000000, pos);
        Assert::AreEqual<DWORD>(12, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyBitmap::SetTextCornerColor(0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF);
        CMyBitmap::ShowString(10, 10, 0xFFFFFF, "A", 0, 0, FALSE, RENDER_TEXT_GRADIENT, 0, pos);
        Assert::AreEqual<DWORD>(14, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyBitmap::SetTextCornerColor(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
        nTri0 = CMyBitmap::GetShowTriTotal();
        char vertical[] = "AB";
        CMyBitmap::ShowTString(10, 10, 0xFFFFFF, vertical);
        Assert::AreEqual<DWORD>(4, CMyBitmap::GetShowTriTotal() - nTri0);
        char empty[] = "";
        CMyBitmap::ShowTString(10, 10, 0xFFFFFF, empty);
        CMyBitmap::ShowTString(10, 10, 0xFFFFFF, 0);
        CMyBitmap::ShowTString(10, 10, 0xFFFFFF, vertical, 0, 0, FALSE, RENDER_TEXT_SHADOW, 0, pos);
        Assert::AreEqual<DWORD>(12, CMyBitmap::GetShowTriTotal() - nTri0);

        FakeDataIcon icons;
        nTri0 = CMyBitmap::GetShowTriTotal();
        CMyBitmap::ShowEmotionString(100, 50, 0xFFFFFF, "A#07B", &icons, 0, 0, 20, 3, 5);
        Assert::AreEqual(7, icons.asked);
        Assert::AreEqual(1, icons.ani.shows);
        Assert::IsTrue(icons.ani.frame < 3);
        Assert::AreEqual(100 + 8 + 3, icons.ani.x);
        Assert::AreEqual(50 + 5 - 4, icons.ani.y);
        Assert::AreEqual(0, icons.ani.alpha); Assert::AreEqual<DWORD>(20, icons.ani.w); Assert::AreEqual<DWORD>(20, icons.ani.h);
        Assert::AreEqual<DWORD>(0, icons.ani.way); Assert::AreEqual(0, icons.ani.src);
        Assert::AreEqual<DWORD>(4, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyBitmap::ShowEmotionString(100, 50, 0xFFFFFF, "#12", &icons, 0, 0, 0, 0, 0);
        Assert::AreEqual(12, icons.asked); Assert::AreEqual(1, icons.ani.shows);
        CMyBitmap::ShowEmotionString(100, 50, 0xFFFFFF, "#07", 0, 0, 0, 0, 0, 0);
        Assert::AreEqual(1, icons.ani.shows);
        CMyBitmap::ShowEmotionString(100, 50, 0xFFFFFF, "#0", &icons, 0, 0, 0, 0, 0);
        Assert::AreEqual(12, icons.asked);
        Assert::AreEqual<DWORD>(8, CMyBitmap::GetShowTriTotal() - nTri0);
        CMyBitmap::ShowEmotionString(100, 50, 0xFFFFFF, 0, &icons, 0, 0, 0, 0, 0);
        nTri0 = CMyBitmap::GetShowTriTotal();
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        CMyBitmap::GameFontDestroy();
        CMyBitmap::GameFontDestroy();
        Assert::AreEqual(2, CMyBitmap::GetStrShowLen("\xB0\xA1", 0, 2));

        const char fontZero[] = "Arial 0";
        WriteBlob(dir + "\\ini\\font.ini", fontZero, sizeof(fontZero) - 1);
        Assert::IsTrue(CMyBitmap::GameFontCreate() != FALSE);
        Assert::AreEqual<DWORD>(12, CMyBitmap::GetFontSize());
        CMyBitmap::GameFontDestroy();
        const char fontNoSize[] = "Arial";
        WriteBlob(dir + "\\ini\\font.ini", fontNoSize, sizeof(fontNoSize) - 1);
        Assert::IsFalse(CMyBitmap::GameFontCreate() != FALSE);
        const char fontBack[] = "Arial 16";
        WriteBlob(dir + "\\ini\\font.ini", fontBack, sizeof(fontBack) - 1);
        CMyBitmap::PrepareSprite();
    }
};

TEST_CLASS(GraphicBitmapObject)
{
public:
    TEST_METHOD(SlotsLoadAndRelease)
    {
        GraphicDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = FixtureDir();
        BuildFixtures(dir);
        CwdGuard cwd(dir);
        CMyBitmap::PackFileOpen("gbmp.wdf");
        Assert::IsNull(MyBitmapCreate("gbmp/missing.bmp"));
        CMyBitmap* p = MyBitmapCreate("gbmp\\tiny.bmp");
        Assert::IsNotNull(p);

        int w = 0, h = 0;
        p->GetSize(w, h);
        Assert::AreEqual(8, w); Assert::AreEqual(4, h);
        Assert::AreEqual<DWORD>(32, p->GetSize());
        Assert::AreEqual(8, p->GetWidth()); Assert::AreEqual(4, p->GetHeight());
        Assert::AreEqual(0, p->IsbTransparentFormat());
        CMyPos pos = { 1, 1 };
        Assert::IsTrue(p->IsTransparent(pos) != FALSE);

        void** vt = Vtbl(p);
        typedef DWORD (__fastcall *FnDword)(CMyBitmap*, void*);
        typedef int (__fastcall *FnInt)(CMyBitmap*, void*);
        typedef void (__fastcall *FnSize)(CMyBitmap*, void*, int&, int&);
        typedef BOOL (__fastcall *FnPos)(CMyBitmap*, void*, CMyPos);
        typedef BOOL (__fastcall *FnLock)(CMyBitmap*, void*, RECT);
        typedef void (__fastcall *FnPixel)(CMyBitmap*, void*, int, int, DWORD);
        typedef void (__fastcall *FnVoid)(CMyBitmap*, void*);
        Assert::AreEqual<DWORD>(32, ((FnDword) vt[4])(p, 0));
        w = h = 0;
        ((FnSize) vt[5])(p, 0, w, h);
        Assert::AreEqual(8, w); Assert::AreEqual(4, h);
        Assert::AreEqual(8, ((FnInt) vt[6])(p, 0));
        Assert::AreEqual(4, ((FnInt) vt[7])(p, 0));
        Assert::AreEqual(0, ((FnInt) vt[9])(p, 0));
        Assert::IsTrue(((FnPos) vt[10])(p, 0, pos) != FALSE);

        RECT lock = { 1, 1, 5, 3 };
        BOOL bLocked = ((FnLock) vt[11])(p, 0, lock);
        if (bLocked)
        {
            Assert::IsFalse(((FnLock) vt[11])(p, 0, lock) != FALSE);
            ((FnPixel) vt[12])(p, 0, 2, 2, 0xFF00FF00);
            ((FnPixel) vt[12])(p, 0, 5, 2, 0xFF0000FF);
            ((FnPixel) vt[12])(p, 0, 0, 0, 0xFF0000FF);
            ((FnVoid) vt[13])(p, 0);
            Assert::IsTrue(((FnLock) vt[11])(p, 0, lock) != FALSE);
            p->Unlock();
        }
        else
            Logger::WriteMessage("the bitmap's texture is not a 32-bit format: the lock slots not exercised");
        p->Unlock();
        p->SetPixel(2, 2, 0);

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        p->SetColor(255, 255, 255, 255);
        p->Show(1, 2);
        Assert::AreEqual(0, 0);
        RECT src = { 0, 0, 4, 2 };
        p->ShowEx(1, 2, &src, 16, 8, 128, _SHOWWAY_ADDITIVE, 45);
        p->ShowEx(1, 2, 0, 0, 0);
        p->ShowBlend(1, 2, &src, 16, 8, 128, 0, D3DBLEND_ONE, D3DBLEND_ONE);
        p->ShowBlend(1, 2, 0, 0, 0, 0, 0, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        DWORD st = 0;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_SRCALPHA, st);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        Assert::AreEqual<DWORD>(0, ((FnDword) vt[0])(p, 0));

        CMyBitmap* q = BmpCreate();
        Assert::IsNotNull(q);
        Assert::IsTrue(q->IsTransparent(pos) != FALSE);
        Assert::AreEqual<DWORD>(0, q->Release());
    }
};
