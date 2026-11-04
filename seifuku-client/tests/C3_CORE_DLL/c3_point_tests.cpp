#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "c3_point.h"
#include "c3_line.h"
#include "c3_main.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(offsetof(C3Point, fA) == 0x00 && offsetof(C3Point, fB) == 0x0C && offsetof(C3Point, matrix) == 0x10, "");
static_assert(offsetof(C3Point, dwCount) == 0x50 && offsetof(C3Point, vertex) == 0x54, "");
static_assert(sizeof(PointVertex) == 0x10 && offsetof(PointVertex, color) == 0x0C && POINT_VERTEX == 0x42, "");
static_assert(offsetof(C3Line, fA) == 0x00 && offsetof(C3Line, matrix) == 0x10 && offsetof(C3Line, dwCount) == 0x50 && offsetof(C3Line, vertex) == 0x54, "");
static_assert(sizeof(LineVertex) == 0x10 && offsetof(LineVertex, color) == 0x0C && LINE_VERTEX == 0x42, "");

static_assert(std::is_same<decltype(&Point_Clear), void (*)(C3Point*)>::value && std::is_same<decltype(&Point_ClearMatrix), void (*)(C3Point*)>::value, "");
static_assert(std::is_same<decltype(&Point_Draw), void (*)(C3Point*)>::value && std::is_same<decltype(&Point_Prepare), void (*)(void)>::value, "");
static_assert(std::is_same<decltype(&Point_Muliply), void (*)(C3Point*, D3DXMATRIX*)>::value, "");
static_assert(std::is_same<decltype(&Point_SetColor), void (*)(C3Point*, float, float, float, float)>::value, "");
static_assert(std::is_same<decltype(&Line_Clear), void (*)(C3Line*)>::value && std::is_same<decltype(&Line_ClearMatrix), void (*)(C3Line*)>::value, "");
static_assert(std::is_same<decltype(&Line_Draw), void (*)(C3Line*)>::value && std::is_same<decltype(&Line_Prepare), void (*)(void)>::value, "");
static_assert(std::is_same<decltype(&Line_Muliply), void (*)(C3Line*, D3DXMATRIX*)>::value, "");
static_assert(std::is_same<decltype(&Line_SetColor), void (*)(C3Line*, float, float, float, float)>::value, "");

namespace
{
    struct PointDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        PointDevice() : d3d(0), dev(0), slot(0)
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
        ~PointDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    template <class T, class V>
    void SharedBodies(void (*Clear)(T*), void (*ClearMatrix)(T*), void (*Muliply)(T*, D3DXMATRIX*), void (*SetColor)(T*, float, float, float, float))
    {
        T* p = new T;
        memset(p, 0xCD, sizeof(T));
        Clear(p);
        Assert::AreEqual<DWORD>(0, p->dwCount);
        Assert::AreEqual(1.0f, p->fA); Assert::AreEqual(1.0f, p->fR); Assert::AreEqual(1.0f, p->fG); Assert::AreEqual(1.0f, p->fB);
        for (int i = 0; i < 16; i++) Assert::AreEqual(i % 5 == 0 ? 1.0f : 0.0f, ((float*) &p->matrix)[i]);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, p->vertex[0].color);

        D3DXMATRIX t; D3DXMatrixIdentity(&t); t._41 = 3.0f; t._11 = 2.0f;
        Muliply(p, &t);
        Muliply(p, &t);
        Assert::AreEqual(4.0f, p->matrix._11); Assert::AreEqual(9.0f, p->matrix._41);
        ClearMatrix(p);
        Assert::AreEqual(1.0f, p->matrix._11); Assert::AreEqual(0.0f, p->matrix._41);
        Assert::AreEqual(1.0f, p->fA);

        SetColor(p, 0.5f, 0.25f, 0.125f, 1.0f);
        Assert::AreEqual(0.5f, p->fA); Assert::AreEqual(0.25f, p->fR); Assert::AreEqual(0.125f, p->fG); Assert::AreEqual(1.0f, p->fB);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, p->vertex[0].color);
        p->dwCount = 2;
        SetColor(p, 1.0f, 0.0f, 1.0f, 0.0f);
        Assert::AreEqual<DWORD>(0xFF00FF00, p->vertex[0].color); Assert::AreEqual<DWORD>(0xFF00FF00, p->vertex[1].color);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, p->vertex[2].color);
        SetColor(p, 0.5f, 0.25f, 0.125f, 1.0f);
        Assert::AreEqual<DWORD>(0x7F3F1FFF, p->vertex[1].color);

        Clear(0); ClearMatrix(0); Muliply(0, &t); SetColor(0, 1, 1, 1, 1);
        delete p;
    }
}

TEST_CLASS(C3PointAndLine)
{
public:
    TEST_METHOD(PointBodies)
    {
        SharedBodies<C3Point, PointVertex>(&Point_Clear, &Point_ClearMatrix, &Point_Muliply, &Point_SetColor);
    }

    TEST_METHOD(LineBodies)
    {
        SharedBodies<C3Line, LineVertex>(&Line_Clear, &Line_ClearMatrix, &Line_Muliply, &Line_SetColor);
    }

    TEST_METHOD(PrepareAndDraw)
    {
        PointDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD st = 0;

        dd.dev->SetRenderState(D3DRS_DITHERENABLE, FALSE);
        dd.dev->SetRenderState(D3DRS_LIGHTING, TRUE);
        Point_Prepare();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ZWRITEENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_COLOR2, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_SRCALPHA, st);

        dd.dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        Line_Prepare();
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_FILLMODE, &st))); Assert::AreEqual<DWORD>(D3DFILL_SOLID, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DESTBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_INVSRCALPHA, st);

        C3Point* p = new C3Point;
        ZeroMemory(p, sizeof(C3Point));
        Point_Clear(p);
        p->dwCount = 2; p->vertex[1].x = 1.0f;
        Point_SetColor(p, 1, 1, 1, 1);
        p->matrix._41 = 5.0f;
        C3Line* l = new C3Line;
        ZeroMemory(l, sizeof(C3Line));
        Line_Clear(l);
        l->dwCount = 4; l->vertex[1].x = 1.0f; l->vertex[3].y = 1.0f;
        l->matrix._42 = 6.0f;

        Assert::IsTrue(Begin3D());
        Point_Draw(p);
        D3DMATRIX w;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_WORLD, &w))); Assert::AreEqual(5.0f, w._41);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetFVF(&st))); Assert::AreEqual<DWORD>(0x42, st);
        Line_Draw(l);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_WORLD, &w))); Assert::AreEqual(6.0f, w._42); Assert::AreEqual(0.0f, w._41);
        Point_Draw(0);
        Line_Draw(0);
        End3D();
        delete p;
        delete l;
    }
};
