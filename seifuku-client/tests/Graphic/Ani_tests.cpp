#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "Ani.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(IAni) == 4, "");
static_assert(_MAX_FRAME == 64, "");
static_assert(std::is_same<decltype(&AniCreate), IAni* (*)(const char*, const char*, BOOL)>::value, "");

namespace
{
    struct AniDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        AniDevice() : d3d(0), dev(0), slot(0)
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
        ~AniDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    void AniWrite(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string AniDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    void BuildAniFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        AniWrite(dir + "\\gani.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gani").c_str(), 0);
        std::vector<BYTE> v;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (DWORD i = 0; i < pitch * h; i++) v.push_back(0x60);
        AniWrite(dir + "\\gani\\a.bmp", &v[0], (DWORD) v.size());
        AniWrite(dir + "\\gani\\b.bmp", &v[0], (DWORD) v.size());
        const char ani[] =
            "[Two]\n"
            "FrameAmount=2\n"
            "Frame0=gani/a.bmp\n"
            "Frame1=gani/b.bmp\n"
            "\n"
            "[Three]\n"
            "FrameAmount=3\n"
            "Frame0=gani/a.bmp\n"
            "Frame1=gani/b.bmp\n"
            "Frame2=gani/b.bmp\n";
        AniWrite(dir + "\\gani\\test.ani", ani, sizeof(ani) - 1);
    }

    struct AniCwd
    {
        char old[MAX_PATH];
        AniCwd(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~AniCwd() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(GraphicAni)
{
public:
    TEST_METHOD(CreateShowAndRelease)
    {
        AniDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = AniDir();
        BuildAniFixtures(dir);
        AniCwd cwd(dir);
        CMyBitmap::PackFileOpen("gani.wdf");

        IAni* dyn = AniCreate(0, 0, FALSE);
        Assert::IsNotNull(dyn);
        Assert::AreEqual<int>(ANISTATUS_INIT, dyn->GetStatus());
        Assert::AreEqual(0, dyn->GetFrameAmount());
        Assert::IsNull(dyn->GetFrame(0));
        Assert::IsFalse(dyn->Show(0, 0, 0) != FALSE);
        Assert::AreEqual<DWORD>(0, dyn->GetSize());
        CMySize none = dyn->GetSize(0);
        Assert::AreEqual(0, none.iWidth); Assert::AreEqual(0, none.iHeight);
        Assert::IsTrue(dyn->SetAniFilename("gani/test.ani", "Two") != FALSE);
        Assert::IsTrue(dyn->Load() != FALSE);
        Assert::AreEqual<int>(ANISTATUS_LOADED, dyn->GetStatus());
        Assert::AreEqual(2, dyn->GetFrameAmount());
        DWORD born = dyn->GetTimeBorn();
        dyn->SetLife(5000);
        Assert::AreEqual<DWORD>(5000, dyn->GetLife());
        Assert::IsTrue(dyn->GetTimeBorn() >= born);
        dyn->SetStatus(ANISTATUS_LOADED);
        dyn->Free();
        Assert::AreEqual<int>(ANISTATUS_INIT, dyn->GetStatus());
        Assert::AreEqual(0, dyn->GetFrameAmount());
        Assert::IsFalse(dyn->Create("gani/test.ani", "NoSuchTitle", FALSE) != FALSE);
        Assert::AreEqual<int>(ANISTATUS_FAILED, dyn->GetStatus());
        Assert::AreEqual<DWORD>(0, dyn->Release());

        Assert::IsNull(AniCreate("gani/test.ani", "NoSuchTitle", FALSE));
        IAni* ani = AniCreate("gani/test.ani", "Two", FALSE);
        Assert::IsNotNull(ani);
        Assert::AreEqual(2, ani->GetFrameAmount());
        Assert::IsNotNull(ani->GetFrame(1));
        Assert::IsTrue(ani->GetFrame(3) == ani->GetFrame(1));
        CMySize size = ani->GetSize(0);
        Assert::AreEqual(8, size.iWidth); Assert::AreEqual(4, size.iHeight);
        Assert::AreEqual<DWORD>(64, ani->GetSize());
        void** vt = *(void***) ani;
        typedef DWORD (__fastcall *FnDword)(void*, void*);
        typedef int (__fastcall *FnInt)(void*, void*);
        typedef CMySize* (__fastcall *FnSize)(void*, void*, CMySize*, UINT);
        Assert::AreEqual(2, ((FnInt) vt[5])(ani, 0));
        Assert::AreEqual<int>(ANISTATUS_LOADED, ((FnInt) vt[6])(ani, 0));
        CMySize byslot = { 0, 0 };
        ((FnSize) vt[8])(ani, 0, &byslot, 1);
        Assert::AreEqual(8, byslot.iWidth); Assert::AreEqual(4, byslot.iHeight);
        Assert::AreEqual<DWORD>(64, ((FnDword) vt[9])(ani, 0));

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        Assert::IsFalse(ani->Show(0, 10, 10) != FALSE);
        Assert::IsTrue(ani->Show(1, 10, 10) != FALSE);
        Assert::IsTrue(ani->Show(3, 10, 10, 128, _SHOWWAY_ADDITIVE) != FALSE);
        RECT src = { 0, 0, 4, 2 };
        Assert::IsFalse(ani->ShowEx(0, 1, 2, &src, 16, 8) != FALSE);
        Assert::IsTrue(ani->ShowEx(1, 1, 2, 0, 20, 20, 0, _SHOWWAY_NORMAL) != FALSE);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);
        Assert::AreEqual<DWORD>(0, ani->Release());

        IAni* half = AniCreate("gani/test.ani", "Three", TRUE);
        Assert::IsNotNull(half);
        Assert::AreEqual(3, half->GetFrameAmount());
        Assert::IsTrue(half->GetFrame(2) == half->GetFrame(1));
        Assert::IsTrue(half->GetFrame(0) != half->GetFrame(1));
        Assert::AreEqual<DWORD>(64, half->GetSize());
        Assert::AreEqual<DWORD>(0, ((FnDword) (*(void***) half)[0])(half, 0));
        CMyBitmap::PrepareSprite();
    }
};
