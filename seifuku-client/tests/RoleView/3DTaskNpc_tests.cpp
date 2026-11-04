#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "3DTaskNpc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct TaskNpcProbe
{
    static int Dir(C3DTaskNpc* p) { return p->m_nDir; }
    static BOOL Hit(C3DTaskNpc* p) { return p->m_bHit; }
    static int Asb(C3DTaskNpc* p) { return p->m_nAsb; }
    static int Adb(C3DTaskNpc* p) { return p->m_nAdb; }
    static DWORD BlazeTime(C3DTaskNpc* p) { return p->m_dwBlazeTime; }
    static I3DSimpleObj* Obj(C3DTaskNpc* p) { return p->m_p3DSimpleObj; }
    static size_t OffName() { return offsetof(C3DTaskNpc, m_szName); }
    static size_t OffTimer() { return offsetof(C3DTaskNpc, m_dwTimer); }
    static size_t OffFixDir() { return offsetof(C3DTaskNpc, m_bFixDir); }
    static size_t OffHit() { return offsetof(C3DTaskNpc, m_bHit); }
    static size_t OffObj() { return offsetof(C3DTaskNpc, m_p3DSimpleObj); }
};

static_assert(sizeof(C3DTaskNpc) == 0x4C, "");
static_assert(_VAR_DIRECT_DIR == 20, "");

TEST_CLASS(RoleView3DTaskNpc)
{
public:
    TEST_METHOD(TheTaskNpcWithoutALook)
    {
        Assert::IsTrue(TaskNpcProbe::OffName() == 0x20 && TaskNpcProbe::OffTimer() == 0x30 && TaskNpcProbe::OffFixDir() == 0x40 && TaskNpcProbe::OffHit() == 0x44 && TaskNpcProbe::OffObj() == 0x48);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_TASKNPC);
        Assert::IsNotNull(pView);
        C3DTaskNpc* p = static_cast<C3DTaskNpc*>(pView);
        Assert::IsTrue(TaskNpcProbe::Dir(p) == -1 && TaskNpcProbe::Hit(p) == FALSE && TaskNpcProbe::Asb(p) == 5 && TaskNpcProbe::Adb(p) == 6 && TaskNpcProbe::BlazeTime(p) == 0 && TaskNpcProbe::Obj(p) == NULL);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 2) != 0 && TaskNpcProbe::Dir(p) == 2);
        Assert::IsTrue(pView->SetVariable(_VAR_DIRECT_DIR, 5) != 0 && TaskNpcProbe::Dir(p) == 5);
        Assert::IsTrue(pView->SetVariable(_VAR_DIR, 3) != 0 && TaskNpcProbe::Dir(p) == 3);
        Assert::IsFalse(pView->SetVariable(_VAR_HAIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(21, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_BLEND, 1) != 0);
        Assert::AreEqual(3u, (unsigned) pView->GetVariable(_VAR_LOOK));
        Assert::AreEqual(3u, (unsigned) pView->GetVariable(_VAR_DIR));
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_DIRECT_DIR));
        Assert::AreEqual(10, pView->SetAction(320));
        Assert::IsNull(TaskNpcProbe::Obj(p));
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEX, 30) != 0);
        Assert::IsNotNull(TaskNpcProbe::Obj(p));
        pView->SetPos(1.0f, 2.0f, 3.0f, 0, 1.0f);
        pView->SetLightOffset(2, 0, 0, 0, 1, 1, 1, 1);
        pView->SetLightOffset(0, 0, 0, 0, 1, 1, 1, 1);
        pView->SetRGBA(1, 1, 1, 1);
        pView->AddPartEffect(_ROLE_PART_BODY, "x");
        pView->ClearPartEffect(_ROLE_PART_BODY, "x");
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
