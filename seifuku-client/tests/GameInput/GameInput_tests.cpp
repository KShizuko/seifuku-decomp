#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <type_traits>
#include "GameInput.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(IGameInputStr) == 4, "");
static_assert(std::is_same<decltype(&GameInputCreate), IGameInputStr* (*)(void)>::value, "");
static_assert(GI_ENTER == 28 && GI_TABLE == 15 && GI_ESCAPE == 1, "");

typedef IGameInputStr* (*FnGameInputCreate)(void);

static FnGameInputCreate LoadCreate()
{
    HMODULE hThis = NULL;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &LoadCreate, &hThis);
    char szPath[MAX_PATH];
    GetModuleFileNameA(hThis, szPath, MAX_PATH);
    char* pSlash = strrchr(szPath, '\\');
    Assert::IsNotNull(pSlash);
    strcpy(pSlash + 1, "GameInput.dll");
    HMODULE h = LoadLibraryA(szPath);
    Assert::IsNotNull(h, L"our GameInput.dll beside the test module");
    FnGameInputCreate fn = (FnGameInputCreate) GetProcAddress(h, "?GameInputCreate@@YAPAVIGameInputStr@@XZ");
    Assert::IsNotNull((void*) fn, L"");
    return fn;
}

TEST_CLASS(GameInputStr)
{
public:
    TEST_METHOD(TheExportAndTheStringSlots)
    {
        IGameInputStr* p = LoadCreate()();
        Assert::IsNotNull(p);
        p->SetStr("hello");
        Assert::AreEqual("hello", p->GetStr());
        typedef char* (__fastcall *FnStr)(void*, void*);
        Assert::AreEqual("hello", ((FnStr) (*(void***) p)[4])(p, 0));
        p->SetStr(0);
        Assert::AreEqual("hello", p->GetStr());
        const char* pszShow = ((FnStr) (*(void***) p)[5])(p, 0);
        Assert::IsTrue(pszShow == p->GetShowStr());
        Assert::IsTrue(strcmp(pszShow, "hello") == 0 || (strncmp(pszShow, "*****", 5) == 0 && pszShow[6] == 0));
        Assert::AreEqual('1', p->Key2Char((unsigned char) GI_1));
        Assert::AreEqual('0', p->Key2Char((unsigned char) GI_NUM0));
        Assert::AreEqual((char) 0, p->Key2Char((unsigned char) GI_ENTER));
        typedef char (__fastcall *FnKey)(void*, void*, unsigned char);
        Assert::AreEqual('7', ((FnKey) (*(void***) p)[6])(p, 0, (unsigned char) GI_7));
        typedef int (__fastcall *FnRelease)(void*, void*);
        Assert::AreEqual(0, ((FnRelease) (*(void***) p)[0])(p, 0));
    }
};
