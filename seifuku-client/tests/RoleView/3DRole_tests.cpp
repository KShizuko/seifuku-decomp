#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include <type_traits>
#include "3DRole.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(C3DRole) == 0xE8, "");
static_assert(offsetof(C3DRole, m_fPosX) == 0x04 && offsetof(C3DRole, m_nRotateX) == 0x10 && offsetof(C3DRole, m_fScale) == 0x1C, "");
static_assert(offsetof(C3DRole, m_nActionType) == 0x20 && offsetof(C3DRole, m_idArmor) == 0x24 && offsetof(C3DRole, m_idMisc) == 0x38, "");
static_assert(offsetof(C3DRole, m_idArmorType) == 0x3C && offsetof(C3DRole, m_idArmetType) == 0x40, "");
static_assert(offsetof(C3DRole, m_LightOffset) == 0x44 && offsetof(C3DRole, m_nShadowType) == 0x50 && offsetof(C3DRole, m_nLook) == 0x54, "");
static_assert(offsetof(C3DRole, m_idHead) == 0x58 && offsetof(C3DRole, m_nHair) == 0x5C && offsetof(C3DRole, m_a) == 0x60, "");
static_assert(offsetof(C3DRole, m_nAsb) == 0x70 && offsetof(C3DRole, m_nAdb) == 0x74 && offsetof(C3DRole, m_pRole) == 0x78, "");
static_assert(offsetof(C3DRole, m_matrix) == 0x7C && offsetof(C3DRole, m_dwTransStart) == 0xBC && offsetof(C3DRole, m_dwFrame) == 0xC4, "");
static_assert(offsetof(C3DRole, m_pMotionArmor) == 0xCC && offsetof(C3DRole, m_pMotionMisc) == 0xE0 && offsetof(C3DRole, m_pMotionBlend) == 0xE4, "");
static_assert(_VAR_HEAD == 2 && _VAR_MISC == 9 && _VAR_RWEAPON_ID == 12 && _VAR_ACTION_DATA == 30 && _VAR_BLEND == 31 && _VAR_ROTATEZ == 34, "");
static_assert(_ROLE_PART_ARMET == 1 && _ROLE_PART_BODY == 4 && _ROLE_PART_MISC == 9, "");
static_assert(std::is_same<decltype(&IRoleView::SetPos), void (IRoleView::*)(float, float, float, int, float)>::value, "");

TEST_CLASS(RoleView3DRole)
{
public:
    TEST_METHOD(TheFactoryObjectAndItsDefaults)
    {
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_ROLE);
        Assert::IsNotNull(pView);
        C3DRole* p = static_cast<C3DRole*>(pView);
        Assert::IsNotNull(p->m_pRole);
        Assert::IsTrue(p->m_nRotateX == CMyBitmap::GetObliqueAngle() && p->m_nRotateY == 0 && p->m_nRotateZ == 0);
        Assert::IsTrue(p->m_fScale == 1.0f && p->m_a == 1.0f && p->m_r == 0.3f && p->m_g == 0.3f && p->m_b == 0.3f);
        Assert::IsTrue(p->m_LightOffset.x == 0 && p->m_LightOffset.y == 0 && p->m_LightOffset.z == -1000.0f && p->m_nShadowType == 2);
        Assert::IsTrue(p->m_nAsb == -1 && p->m_nAdb == -1);
        Assert::IsTrue(p->m_idArmor == 0 && p->m_idArmet == 0 && p->m_idLWeapon == 0 && p->m_idRWeapon == 0 && p->m_idMount == 0 && p->m_idMisc == 0);
        Assert::IsTrue(p->m_nLook == 0 && p->m_nHair == 0 && p->m_idHead == 0 && p->m_idArmorType == 0 && p->m_idArmetType == 0);
        Assert::IsTrue(p->m_dwTransTime == 0 && p->m_pMotionArmor == NULL && p->m_pMotionBlend == NULL);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }

    TEST_METHOD(TheVariableTables)
    {
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_ROLE);
        Assert::IsNotNull(pView);
        C3DRole* p = static_cast<C3DRole*>(pView);
        for (int i = -1; i < 40; i++)
            if (i != _VAR_ACTION_DATA)
                Assert::AreEqual(0u, (unsigned) pView->GetVariable(i));
        for (int i = 12; i <= 30; i++)
            Assert::IsFalse(pView->SetVariable(i, 1) != 0);
        Assert::IsFalse(pView->SetVariable(35, 1) != 0);
        Assert::IsFalse(pView->SetVariable(-1, 1) != 0);
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEX, 11) != 0 && p->m_nRotateX == 11);
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEY, 22) != 0 && p->m_nRotateY == 22);
        Assert::IsTrue(pView->SetVariable(_VAR_ROTATEZ, 33) != 0 && p->m_nRotateZ == 33);
        Assert::IsTrue(pView->SetVariable(_VAR_BLEND, (6 << 16) | 5) != 0 && p->m_nAsb == 5 && p->m_nAdb == 6);
        Assert::IsTrue(pView->SetVariable(_VAR_FRAME, 7) != 0 && p->m_dwFrame == 7);
        Assert::IsTrue(pView->SetVariable(_VAR_DRAWTYPE, 1) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_RWEAPON, 0) != 0);
        Assert::IsFalse(pView->SetVariable(_VAR_LWEAPON, 0) != 0);
        Assert::IsTrue(pView->SetVariable(_VAR_MOUNT, 0) != 0 && p->m_idMount == 0);
        Assert::IsTrue(pView->SetVariable(_VAR_MISC, 0) != 0 && p->m_idMisc == 0);
        Assert::AreEqual(0u, (unsigned) pView->GetVariable(_VAR_WEAPON_ACTION_DATA));
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }

    TEST_METHOD(TheStoresAndTheEffectSlots)
    {
        IRoleView* pView = RoleViewCreate(_ROLE_VIEW_ROLE);
        Assert::IsNotNull(pView);
        C3DRole* p = static_cast<C3DRole*>(pView);
        pView->SetPos(1.0f, 2.0f, 3.0f, 45, 0.5f);
        float fHeight = CMyBitmap::GameCameraIsProject() ? 3.0f : 6.0f;
        Assert::IsTrue(p->m_fPosX == 1.0f && p->m_fPosY == 2.0f && p->m_fPosH == fHeight && p->m_nRotateZ == 45 && p->m_fScale == 0.5f);
        pView->SetLightOffset(1, 2.0f, 3.0f, 4.0f, 0.5f, 0.6f, 0.7f, 0.8f);
        Assert::IsTrue(p->m_nShadowType == 1 && p->m_LightOffset.x == 2.0f && p->m_LightOffset.y == -3.0f && p->m_LightOffset.z == -4.0f);
        Assert::IsTrue(p->m_a == 0.5f && p->m_r == 0.6f && p->m_g == 0.7f && p->m_b == 0.8f);
        pView->SetRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        pView->AddPartEffect(_ROLE_PART_BODY, NULL);
        pView->AddPartEffect(2, "x"); pView->AddPartEffect(3, "x"); pView->AddPartEffect(5, "x"); pView->AddPartEffect(0, "x"); pView->AddPartEffect(10, "x");
        pView->ClearPartEffect(_ROLE_PART_BODY, "x"); pView->ClearPartEffect(_ROLE_PART_MISC, NULL); pView->ClearPartEffect(2, "x");
        Assert::IsFalse(p->m_pRole->HasPart("body") != 0);
        Assert::IsTrue(pView->Reload() != 0);
        Assert::IsNull(p->m_pMotionArmor);
        pView->SetLightOffset(0, 0, 0, 0, 1, 1, 1, 1);
        Assert::AreEqual(1, p->m_nShadowType);
        pView->SetLightOffset(3, 0, 0, 0, 1, 1, 1, 1);
        Assert::AreEqual(3, p->m_nShadowType);
        Assert::AreEqual(0u, (unsigned) pView->Release());
    }
};
