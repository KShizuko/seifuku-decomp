#include "CppUnitTest.h"
#include <windows.h>
#include <type_traits>
#include "IRoleView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(IRoleView) == 4, "");
static_assert(std::is_same<decltype(&RoleViewCreate), IRoleView* (*)(int)>::value, "");
static_assert(_ROLE_VIEW_TERRAIN == 9 && _ROLE_VIEW_NPCX == 10 && _VAR_BLEND == 31, "");

TEST_CLASS(RoleViewFactory)
{
public:
    TEST_METHOD(TheExportAndItsRefusals)
    {
        HMODULE h = GetModuleHandleA("RoleView.dll");
        Assert::IsNotNull(h);
        Assert::IsTrue(GetProcAddress(h, "RoleViewCreate") == (FARPROC) &RoleViewCreate);
        Assert::IsNull(RoleViewCreate(_ROLE_VIEW_NONE));
        Assert::IsNull(RoleViewCreate(_ROLE_VIEW_MAPMAGICOBJ));
        Assert::IsNull(RoleViewCreate(11));
        Assert::IsNull(RoleViewCreate(-1));
    }
};
