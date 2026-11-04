#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <cstddef>
#include <type_traits>
#include "Game3DEffectEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(IGame3DEffect) == 4, "");
static_assert(sizeof(IGame3DEffectHandler) == 4, "");
static_assert(std::is_same<decltype(&Game3DEffectCreate), IGame3DEffect* (*)(IGame3DEffectHandler*)>::value, "");
static_assert(sizeof(Game3DEffectExInfo) == 0x180, "");
static_assert(offsetof(Game3DEffectExInfo, szIndex) == 0x08, "");
static_assert(offsetof(Game3DEffectExInfo, dwTimeDelay) == 0x48 && offsetof(Game3DEffectExInfo, dwTimeBegin) == 0x50, "");
static_assert(offsetof(Game3DEffectExInfo, dwLoopTime) == 0x58 && offsetof(Game3DEffectExInfo, dwLoopInterval) == 0x5C, "");
static_assert(offsetof(Game3DEffectExInfo, nOffsetZ) == 0x68 && offsetof(Game3DEffectExInfo, nState) == 0x6C, "");
static_assert(offsetof(Game3DEffectExInfo, bAutoDelete) == 0x70 && offsetof(Game3DEffectExInfo, nASB) == 0x74 && offsetof(Game3DEffectExInfo, nADB) == 0xB4 && offsetof(Game3DEffectExInfo, nScale) == 0xF4, "");
static_assert(offsetof(Game3DEffectExInfo, fPosX) == 0x134 && offsetof(Game3DEffectExInfo, fScaleX) == 0x140 && offsetof(Game3DEffectExInfo, fRotateX) == 0x14C, "");
static_assert(offsetof(Game3DEffectExInfo, dwColor) == 0x158 && offsetof(Game3DEffectExInfo, fGravity) == 0x15C && offsetof(Game3DEffectExInfo, fExternalSpeed) == 0x168, "");
static_assert(offsetof(Game3DEffectExInfo, dwPauseTime) == 0x174 && offsetof(Game3DEffectExInfo, nId) == 0x178 && offsetof(Game3DEffectExInfo, dwStepCount) == 0x17C, "");
static_assert(sizeof(Game3DEffectIdInfo) == 0x1C, "");
static_assert(offsetof(Game3DEffectIdInfo, pPtclInfo) == 0x08 && offsetof(Game3DEffectIdInfo, pPtcl3Info) == 0x0C && offsetof(Game3DEffectIdInfo, pShapeInfoBuf) == 0x10 && offsetof(Game3DEffectIdInfo, nShapes) == 0x14 && offsetof(Game3DEffectIdInfo, bReady) == 0x18, "");

namespace
{
    struct Handler : IGame3DEffectHandler
    {
        int nCalls[5];
        Handler() { memset(nCalls, 0, sizeof(nCalls)); }
        virtual void OnStart(int) { nCalls[0]++; }
        virtual void OnLoop(int) { nCalls[1]++; }
        virtual void OnLoopGap(int) { nCalls[2]++; }
        virtual void OnEnd(int) { nCalls[3]++; }
        virtual void OnDelete(int) { nCalls[4]++; }
    };
}

TEST_CLASS(Game3DEffectEx)
{
public:
    TEST_METHOD(TheExportAndAnEmptyObject)
    {
        HMODULE hThis = NULL;
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &Game3DEffectCreate, &hThis);
        char szPath[MAX_PATH];
        GetModuleFileNameA(hThis, szPath, MAX_PATH);
        strcpy(strrchr(szPath, '\\') + 1, "Role3D.dll");
        HMODULE h = LoadLibraryA(szPath);
        Assert::IsNotNull(h, L"our Role3D.dll beside the test module");
        typedef IGame3DEffect* (*FnCreate)(IGame3DEffectHandler*);
        FnCreate fnCreate = (FnCreate) GetProcAddress(h, "Game3DEffectCreate");
        Assert::IsNotNull((void*) fnCreate, L"");
        IGame3DEffect* pDll = fnCreate(NULL);
        Assert::IsNotNull(pDll);
        Assert::AreEqual(-1, pDll->Add("none"));
        Assert::AreEqual(0u, (unsigned) pDll->Release());

        Handler handler;
        IGame3DEffect* p = Game3DEffectCreate(&handler);
        Assert::IsNotNull(p);
        Assert::AreEqual(-1, p->Add(NULL));
        Assert::AreEqual(-1, p->Add("none"));
        Assert::AreEqual(-1, p->Add("NONE"));
        Assert::IsFalse(p->TestEffect("x") != 0);
        Assert::IsFalse(p->TestEffect(NULL) != 0);
        Assert::AreEqual(-1, p->GetId("x"));
        Assert::AreEqual(0u, (unsigned) p->GetFrames());
        Assert::IsTrue(p->Process() != 0);
        Assert::IsTrue(p->Show(CMyPos(), -1) != 0);
        typedef int (__fastcall *FnAdd)(void*, void*, const char*, bool);
        Assert::AreEqual(-1, ((FnAdd) (*(void***) p)[4])(p, 0, "none", true));
        typedef BOOL (__fastcall *FnTest)(void*, void*, const char*);
        Assert::IsFalse(((FnTest) (*(void***) p)[24])(p, 0, "x") != 0);
        p->SetOffset(3); p->SetHeight(1.0f); p->SetOblique(FALSE);
        p->SetPos(1, 2, 3); p->Rotate(1, 2, 3); p->Scale(2, 2, 2);
        p->Pause(); p->Resume(); p->EverPlay(); p->SetColor(0xFF00FF00); p->SetBlend(5, 6);
        p->SetColor(0xFF0000FF, 0, false);
        p->SetPtcl3Gravity(0, -1, 0); p->SetPtcl3ExternalSpeed(1, 0, 0);
        p->SetTexture(5); p->SetTexture((OBJID) -1, 0, 0);
        Assert::IsTrue(p->Reload() != 0);
        p->Delete(-1);
        typedef void (__fastcall *FnGravity)(void*, void*, float, float, float, int);
        ((FnGravity) (*(void***) p)[28])(p, 0, 0, 0, 0, -1);
        typedef BOOL (__fastcall *FnReload)(void*, void*);
        Assert::IsTrue(((FnReload) (*(void***) p)[31])(p, 0) != 0);
        p->Reset();
        p->Clear();
        Assert::AreEqual(0, handler.nCalls[0] + handler.nCalls[1] + handler.nCalls[2] + handler.nCalls[3] + handler.nCalls[4]);
        Assert::AreEqual(0u, (unsigned) p->Release());
    }
};
