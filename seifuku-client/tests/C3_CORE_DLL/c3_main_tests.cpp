#include "CppUnitTest.h"
#include <type_traits>
#include "c3_main.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(D3DDISPLAYMODE) == 16, "");
static_assert(sizeof(D3DVIEWPORT9) == 24, "");
static_assert(sizeof(D3DXMATRIX) == 64, "");
static_assert(sizeof(CRITICAL_SECTION) == 24, "");
static_assert(sizeof(D3DPRESENT_PARAMETERS) == 56, "");
static_assert(sizeof(D3DCAPS9) == 304, "");

static_assert(std::is_same<decltype(&Init3D), int (*)(HINSTANCE, const char*, DWORD, DWORD, BOOL, WNDPROC, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Init3DEx), int (*)(HWND, DWORD, DWORD, BOOL, DWORD)>::value, "");
static_assert(std::is_same<decltype(&ClearBuffer), BOOL (*)(BOOL, BOOL, D3DCOLOR)>::value, "");
static_assert(std::is_same<decltype(&SetRenderState), void (*)(D3DRENDERSTATETYPE, DWORD)>::value, "");
static_assert(std::is_same<decltype(&SetTextureStageState), void (*)(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CalcRate), DWORD (*)(void)>::value && std::is_same<decltype(&LimitRate), BOOL (*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(g_D3DDevice), LPDIRECT3DDEVICE9>::value && std::is_same<decltype(g_ViewMatrix), D3DXMATRIX>::value, "");
static_assert(std::is_same<decltype(&HintGraphicDetail), int (*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(g_InvViewMatrix), D3DXMATRIX>::value && std::is_same<decltype(g_IDMatrix), D3DXMATRIX>::value, "");

TEST_CLASS(C3MainHeader)
{
public:
    TEST_METHOD(VersionStringIsTheBinaryOne)
    {
        Assert::AreEqual<size_t>(64, sizeof(C3_VERSION));
        Assert::AreEqual("MAXFILE C3 00001", C3_VERSION);
        Assert::AreEqual<size_t>(16, strlen(C3_VERSION));
        for (int i = 17; i < 64; i++)
            Assert::AreEqual('\0', C3_VERSION[i]);
    }

    TEST_METHOD(ExportMacrosAreImportsHere)
    {
#ifdef C3_CORE_DLL_EXPORTS
        Assert::Fail(L"tests must not define C3_CORE_DLL_EXPORTS");
#endif
        Assert::AreEqual("__declspec ( dllimport )", _CRT_STRINGIZE(C3_CORE_DLL_API));
        Assert::AreEqual("__declspec ( dllimport )", _CRT_STRINGIZE(C3_CORE_DLL_D3DDATA));
    }
};
