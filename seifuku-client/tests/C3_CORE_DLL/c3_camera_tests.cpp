#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include "c3_camera.h"
#include "c3_main.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Camera) == 0x20, "");
static_assert(offsetof(C3Camera, lpName) == 0x00 && offsetof(C3Camera, lpFrom) == 0x04 && offsetof(C3Camera, lpTo) == 0x08, "");
static_assert(offsetof(C3Camera, fNear) == 0x0C && offsetof(C3Camera, fFar) == 0x10 && offsetof(C3Camera, fFov) == 0x14, "");
static_assert(offsetof(C3Camera, dwFrameCount) == 0x18, "");
static_assert(offsetof(C3Camera, nFrame) == 0x1C, "");

static_assert(std::is_same<decltype(&Camera_Load), BOOL (*)(C3Camera**, char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Camera_Save), BOOL (*)(char*, C3Camera*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Camera_BuildOrtho), BOOL (*)(C3Camera*, float, float, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Camera_BuildView), BOOL (*)(C3Camera*, BOOL)>::value && std::is_same<decltype(&Camera_BuildProject), BOOL (*)(C3Camera*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Camera_NextFrame), void (*)(C3Camera*, int)>::value, "");
static_assert(std::is_same<decltype(&Camera_Process1stRotate), void (*)(C3Camera*, float, float)>::value, "");
static_assert(std::is_same<decltype(&Camera_Unload), void (*)(C3Camera**)>::value && std::is_same<decltype(&Camera_Clear), void (*)(C3Camera*)>::value, "");

namespace
{
    DWORD Bits(float f) { DWORD d; memcpy(&d, &f, 4); return d; }
    const float EPS = 1e-4f;
    void Near(float e, float a, const wchar_t* w) { Assert::IsTrue(fabsf(e - a) <= EPS, w); }

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

    struct TestCamera
    {
        C3Camera cam;
        D3DXVECTOR3 from[2], to[2];
        TestCamera()
        {
            Camera_Clear(&cam);
            from[0] = D3DXVECTOR3(0, 0, 0); to[0] = D3DXVECTOR3(0, 10, 0);
            from[1] = D3DXVECTOR3(5, 5, 5); to[1] = D3DXVECTOR3(6, 5, 5);
            cam.lpFrom = from; cam.lpTo = to; cam.dwFrameCount = 2; cam.nFrame = 0;
        }
    };
}

TEST_CLASS(C3CameraHeader)
{
public:
    TEST_METHOD(ClearDefaultsAreTheBinaryConstants)
    {
        Assert::AreEqual<DWORD>(0x41200000, Bits(10.0f));
        Assert::AreEqual<DWORD>(0x461C4000, Bits(10000.0f));
        Assert::AreEqual<DWORD>(0x3F490FDB, Bits(D3DXToRadian(45)));
    }

    TEST_METHOD(FrameArithmeticAsNextFrame)
    {
        C3Camera c; c.dwFrameCount = 10; c.nFrame = 8;
        c.nFrame = (DWORD) (c.nFrame + 5) % c.dwFrameCount;
        Assert::AreEqual(3, c.nFrame);
    }
};

TEST_CLASS(C3CameraFunctions)
{
public:
    TEST_METHOD(ClearAndNextFrame)
    {
        C3Camera c;
        memset(&c, 0xCD, sizeof(c));
        Camera_Clear(&c);
        Assert::IsNull(c.lpName); Assert::IsNull(c.lpFrom); Assert::IsNull(c.lpTo);
        Assert::AreEqual(10.0f, c.fNear); Assert::AreEqual(10000.0f, c.fFar);
        Assert::AreEqual<DWORD>(0x3F490FDB, Bits(c.fFov));
        Assert::AreEqual<DWORD>(0, c.dwFrameCount); Assert::AreEqual(0, c.nFrame);
        c.dwFrameCount = 3; c.nFrame = 2;
        Camera_NextFrame(&c, 2);
        Assert::AreEqual(1, c.nFrame);
    }

    TEST_METHOD(FirstPersonRotateAndItsVerticalLimit)
    {
        TestCamera t;
        Camera_Process1stRotate(&t.cam, D3DXToRadian(90.0f), 0.0f);
        Near(0.0f, t.from[0].x, L"eye x"); Near(0.0f, t.from[0].y, L"eye y");
        Near(-10.0f, t.to[0].x, L"target x"); Near(0.0f, t.to[0].y, L"target y"); Near(0.0f, t.to[0].z, L"target z");
        Camera_Process1stRotate(&t.cam, 0.0f, D3DXToRadian(85.0f));
        Near(-10.0f, t.to[0].x, L"limited x"); Near(0.0f, t.to[0].z, L"limited z");
        Camera_Process1stRotate(&t.cam, 0.0f, D3DXToRadian(45.0f));
        Near(10.0f, D3DXVec3Length(&(t.to[0] - t.from[0])), L"length kept");
        Assert::IsTrue(fabsf(t.to[0].z) > 6.0f);
    }

    TEST_METHOD(TranslationsMoveBothPointsByStep)
    {
        TestCamera t;
        Camera_Process1stTranslate(&t.cam, 0.0f, 3.0f);
        Near(0.0f, t.from[0].x, L"x"); Near(3.0f, t.from[0].y, L"eye y"); Near(13.0f, t.to[0].y, L"target y");
        Camera_ProcessXYTranslate(&t.cam, D3DXToRadian(90.0f), 2.0f);
        Near(2.0f, t.from[0].x, L"xy eye x"); Near(3.0f, t.from[0].y, L"xy eye y");
        Near(2.0f, t.to[0].x, L"xy target x"); Near(13.0f, t.to[0].y, L"xy target y");
    }

    TEST_METHOD(BuildMatricesOnADevice)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        g_DisplayMode.Width = 800; g_DisplayMode.Height = 600;
        TestCamera t;
        t.cam.nFrame = 1;
        Assert::IsTrue(Camera_BuildView(&t.cam, TRUE));
        D3DXVECTOR3 v;
        D3DXVec3TransformCoord(&v, &t.from[1], &g_ViewMatrix);
        Near(0.0f, v.x, L"eye -> origin"); Near(0.0f, v.y, L""); Near(0.0f, v.z, L"");
        D3DXVec3TransformCoord(&v, &t.to[1], &g_ViewMatrix);
        Near(1.0f, v.z, L"target -> +z");
        D3DXMATRIX dev;
        dd.dev->GetTransform(D3DTS_VIEW, &dev);
        Assert::IsTrue(dev == g_ViewMatrix);
        D3DXMATRIX prod;
        D3DXMatrixMultiply(&prod, &g_ViewMatrix, &g_InvViewMatrix);
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                Near(r == c ? 1.0f : 0.0f, (&prod._11)[r * 4 + c], L"view * inverse");
        Near(5.0f, g_InvViewMatrix._41, L"eye.x"); Near(5.0f, g_InvViewMatrix._42, L"eye.y"); Near(5.0f, g_InvViewMatrix._43, L"eye.z");
        Assert::IsTrue(Camera_BuildProject(&t.cam, TRUE));
        Near(1.0f / tanf(t.cam.fFov / 2) / (800.0f / 600.0f), g_ProjectMatrix._11, L"_11");
        Near(1.0f / tanf(t.cam.fFov / 2), g_ProjectMatrix._22, L"_22");
        Assert::IsTrue(Camera_BuildOrtho(&t.cam, 20.0f, 10.0f, FALSE));
        Near(0.1f, g_ProjectMatrix._11, L"ortho 2/w"); Near(0.2f, g_ProjectMatrix._22, L"ortho 2/h");
    }

    TEST_METHOD(SaveThenLoadRoundTrip)
    {
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        std::string path = dir + "\\seifuku_camera.c3";
        SeifukuFixtures::WriteBlob(path, "MAXFILE C3 00001", 16);
        TestCamera t;
        char name[] = "cam01";
        t.cam.lpName = name;
        Assert::IsTrue(Camera_Save((char*) path.c_str(), &t.cam, FALSE));
        C3Camera* loaded = 0;
        Assert::IsTrue(Camera_Load(&loaded, (char*) path.c_str(), 0));
        Assert::IsNotNull(loaded);
        Assert::AreEqual("cam01", loaded->lpName);
        Assert::AreEqual<DWORD>(2, loaded->dwFrameCount);
        Near(6.0f, loaded->lpTo[1].x, L"to[1].x"); Near(5.0f, loaded->lpFrom[1].z, L"from[1].z");
        Assert::AreEqual<DWORD>(0x3F490FDB, Bits(loaded->fFov));
        Assert::IsFalse(Camera_Load(&loaded, (char*) path.c_str(), 1));
        Assert::IsFalse(Camera_Load(&loaded, (char*) (dir + "\\seifuku_missing.c3").c_str(), 0));
        Camera_Unload(&loaded);
    }
};
