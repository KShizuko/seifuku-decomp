#include "CppUnitTest.h"
#include "c3_main.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
            pp.EnableAutoDepthStencil = TRUE;
            pp.AutoDepthStencilFormat = D3DFMT_D16;
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
}

TEST_CLASS(C3MainDeviceWrappers)
{
public:
    TEST_METHOD(SceneClearFlipAndLostState)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        Assert::IsTrue(Begin3D());
        ClearBuffer(TRUE, TRUE, 0xFF102030);
        Assert::IsTrue(End3D());
        Flip(0, 0, 0);
        Assert::AreEqual(0, IfDeviceLost());
    }

    TEST_METHOD(RenderStateWrappersWriteThrough)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD v = 0;
        SetRenderState(D3DRS_ALPHAREF, 0x2A);
        dd.dev->GetRenderState(D3DRS_ALPHAREF, &v);
        Assert::AreEqual<DWORD>(0x2A, v);
        SetRenderState(D3DRS_ALPHAREF, 0x2A);
        dd.dev->GetRenderState(D3DRS_ALPHAREF, &v);
        Assert::AreEqual<DWORD>(0x2A, v);
        SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        dd.dev->GetTextureStageState(1, D3DTSS_COLOROP, &v);
        Assert::AreEqual<DWORD>(D3DTOP_DISABLE, v);
    }

    TEST_METHOD(HintGraphicDetailSwitchesAntialiasing)
    {
        Assert::AreEqual(0, HintGraphicDetail(0));
        Assert::AreEqual(0, HintGraphicDetail(2));
        Assert::AreEqual(0, HintGraphicDetail(2));
        Assert::AreEqual(0, HintGraphicDetail(0));
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD v = 0;
        Assert::AreEqual(1, HintGraphicDetail(1));
        dd.dev->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &v);
        Assert::AreEqual<DWORD>(1, v);
        Assert::AreEqual(0, HintGraphicDetail(1));
        Assert::AreEqual(0, HintGraphicDetail(3));
        Assert::AreEqual(1, HintGraphicDetail(2));
        Assert::AreEqual(0, HintGraphicDetail(0));
        dd.dev->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &v);
        Assert::AreEqual<DWORD>(0, v);
    }

    TEST_METHOD(LimitRateAndCalcRate)
    {
        Assert::IsTrue(LimitRate(0));
        Assert::IsTrue(LimitRate(0));
        Assert::IsFalse(LimitRate(1000000));
        DWORD r = CalcRate();
        Assert::IsTrue(r >= 0);
    }
};
