#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include <type_traits>
#include "3DRoleEx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(I3DRole) == 4 && !std::has_virtual_destructor<I3DRole>::value, "");
static_assert(std::is_same<decltype(&I3DRoleCreate), I3DRole* (*)(void)>::value, "");
static_assert(std::is_same<decltype(&LoadAdjustConfig), void (*)(void)>::value, "");

TEST_CLASS(RoleEx)
{
public:
    TEST_METHOD(AnEmptyRoleAndItsParts)
    {
        I3DRole* p = I3DRoleCreate();
        Assert::IsNotNull(p);
        C3DRoleEx* pRole = static_cast<C3DRoleEx*>(p);
        Assert::IsNull(pRole->GetPart("armor"));
        Assert::IsFalse(p->HasPart("armor") != 0);
        Assert::IsFalse(p->HasPart(NULL) != 0);
        Assert::IsFalse(p->TestEffect("x", NULL) != 0);
        p->AddEffect(NULL, NULL, FALSE);
        Assert::IsFalse(p->TestEffect("x", "armor") != 0);
        Assert::IsFalse(p->AddPartMesh("armor", "armor", 1) != 0);
        p->DelPart(NULL);
        p->DelPart("armor");
        p->SetFrame(1); p->SetARGB(1, 1, 1, 1); p->Move(1, 2, 3, NULL);
        Assert::IsTrue(p->AddPart("armor", "none", NULL, 0) != 0);
        C3DRolePart* pPart = pRole->GetPart("armor");
        Assert::IsNotNull(pPart);
        Assert::AreEqual(0, pPart->GetInfo()->nPart);
        Assert::AreEqual(1, static_cast<C3DRoleEx*>(p)->GetPartAmount());
        Assert::IsTrue(p->AddPart("weapon", "v_rweapon", "armor", 0) != 0);
        Assert::IsTrue(p->AddPart("weapon", "v_lweapon", "armor", 0) != 0);
        Assert::AreEqual(2, static_cast<C3DRoleEx*>(p)->GetPartAmount());
        Assert::IsTrue(pRole->GetPart("weapon") != pPart);
        Assert::IsTrue(p->HasPart("weapon") != 0 && p->HasPart("armor") != 0 && !p->HasPart("x"));
        Assert::IsFalse(p->TestEffect("x", NULL) != 0);
        Assert::IsTrue(p->IsLoaded("armor") != 0);
        Assert::IsFalse(p->IsLoaded(NULL) != 0);
        Assert::IsFalse(p->IsLoaded("x") != 0);
        D3DXMATRIXA16 scale2;
        D3DXMatrixScaling(&scale2, 2.0f, 2.0f, 2.0f);
        Assert::IsFalse(p->SetPartMatrix("x", &scale2) != 0);
        Assert::IsTrue(p->SetPartMatrix("weapon", &scale2) != 0);
        Assert::IsTrue(pRole->GetPart("weapon")->GetMatrixAdjust()->_22 == 2.0f && pPart->GetMatrixAdjust()->_22 == 1.0f);
        Assert::IsTrue(p->AddPart("misc", "none", "none", 0, &scale2) != 0);
        Assert::IsTrue(pRole->GetPart("misc")->GetMatrixAdjust()->_33 == 1.0f);
        p->SetLodLevel(2);
        Assert::AreEqual(2, pPart->GetLodLevel());
        Assert::AreEqual(2, pRole->GetPart("misc")->GetLodLevel());
        p->SetLightOffset(0, 0, 0, 0, 1, 1, 1, 1);
        p->DelPart("misc");
        p->SetTexture(NULL, NULL, NULL); p->SetDrawType(1, "weapon"); p->ClearMatrix("armor"); p->SetDefaultMotion("armor");
        p->AddEffect("none", NULL, TRUE); p->Clear3DEffect(NULL, "weapon");
        Assert::IsFalse(p->TestEffect("x", "weapon") != 0);
        p->DelPart("weapon");
        Assert::IsNull(pRole->GetPart("weapon"));
        Assert::AreEqual(1, static_cast<C3DRoleEx*>(p)->GetPartAmount());
        Assert::AreEqual(0u, (unsigned) p->Release());

        LoadAdjustConfig();
        Assert::IsTrue(g_nAdjustOblique == CMyBitmap::GetObliqueAngle() && g_nAdjustEffect == g_nAdjustOblique && g_nAdjustEffectOrder == 0);
    }
};
