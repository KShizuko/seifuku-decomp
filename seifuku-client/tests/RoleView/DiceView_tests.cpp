#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "DiceView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct DiceViewProbe
{
    static BOOL Run(CDiceView* p) { return p->m_bRun; }
    static BOOL DiceRun(CDiceView* p, int i) { return p->m_bDiceRun[i]; }
    static DWORD Dice(CDiceView* p, int i) { return p->m_dwDice[i]; }
    static DWORD End(CDiceView* p, int i) { return p->m_dwDiceEnd[i]; }
    static DWORD Frame(CDiceView* p, int i) { return p->m_dwDiceFrame[i]; }
    static DWORD Offset(CDiceView* p, int i) { return p->m_dwOffset[i]; }
    static BOOL Up(CDiceView* p, int i) { return p->m_bUp[i]; }
    static I3DSimpleObj* Desk(CDiceView* p) { return p->m_objDesk.p; }
    static I3DSimpleObj* Roll(CDiceView* p, int i) { return p->m_objDiceRoll[i].p; }
    static size_t OffDesk() { return offsetof(CDiceView, m_objDesk); }
    static size_t OffRoll() { return offsetof(CDiceView, m_objDiceRoll); }
    static size_t OffStatic() { return offsetof(CDiceView, m_objDiceStatic); }
    static size_t OffRun() { return offsetof(CDiceView, m_bRun); }
    static size_t OffDice() { return offsetof(CDiceView, m_dwDice); }
    static size_t OffEnd() { return offsetof(CDiceView, m_dwDiceEnd); }
    static size_t OffFrame() { return offsetof(CDiceView, m_dwDiceFrame); }
    static size_t OffOffset() { return offsetof(CDiceView, m_dwOffset); }
    static size_t OffUp() { return offsetof(CDiceView, m_bUp); }
};

static_assert(sizeof(CDiceView) == 0xB4, "");
static_assert(sizeof(CSimpleObjPtr) == 4, "");
static_assert(_VAR_DICE0 == 27 && _VAR_DICE2 == 29, "");

TEST_CLASS(RoleViewDiceView)
{
public:
    TEST_METHOD(TheDiceAndTheirThrow)
    {
        Assert::IsTrue(DiceViewProbe::OffDesk() == 0x4C && DiceViewProbe::OffRoll() == 0x50 && DiceViewProbe::OffStatic() == 0x5C && DiceViewProbe::OffRun() == 0x68);
        Assert::IsTrue(DiceViewProbe::OffDice() == 0x78 && DiceViewProbe::OffEnd() == 0x84 && DiceViewProbe::OffFrame() == 0x90 && DiceViewProbe::OffOffset() == 0x9C && DiceViewProbe::OffUp() == 0xA8);
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_DICENPC);
        Assert::IsNotNull(pView);
        CDiceView* p = static_cast<CDiceView*>(pView);
        Assert::IsNotNull(DiceViewProbe::Desk(p));
        Assert::IsNotNull(DiceViewProbe::Roll(p, 2));
        Assert::IsTrue(DiceViewProbe::Run(p) == FALSE && DiceViewProbe::DiceRun(p, 1) == FALSE && DiceViewProbe::Dice(p, 0) == 0 && DiceViewProbe::Dice(p, 2) == 2);
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_DIR));
        DWORD dwNow = ::GetTickCount();
        Assert::IsTrue(pView->SetVariable(_VAR_DICE1, 5) != 0);
        Assert::IsTrue(DiceViewProbe::Run(p) == TRUE && DiceViewProbe::DiceRun(p, 1) == TRUE && DiceViewProbe::Dice(p, 1) == 5);
        Assert::IsTrue(DiceViewProbe::End(p, 1) >= dwNow + 2900 && DiceViewProbe::End(p, 1) < dwNow + 4200);
        Assert::IsTrue(DiceViewProbe::Frame(p, 1) < 16 && DiceViewProbe::Offset(p, 1) == 0 && DiceViewProbe::Up(p, 1) == 1);
        Assert::IsTrue(DiceViewProbe::DiceRun(p, 0) == FALSE && DiceViewProbe::DiceRun(p, 2) == FALSE);
        Assert::IsTrue(pView->SetVariable(_VAR_DICE2, 6) != 0 && DiceViewProbe::Up(p, 2) == 2);
        Assert::IsFalse(pView->SetVariable(_VAR_HAIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_DIR, 1) != 0);
        Assert::IsFalse(pView->SetVariable(30, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_ROTATEX, 30) != 0);
        Assert::AreEqual(10, pView->SetAction(100));
        pView->SetPos(100.0f, 200.0f, 3.0f, 0, 1.0f);
        pView->SetLightOffset(2, 0, 0, 0, 1, 1, 1, 1);
        pView->SetRGBA(1, 1, 1, 1);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
