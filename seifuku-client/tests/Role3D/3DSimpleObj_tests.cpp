#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include <type_traits>
#include "3DSimpleObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(I3DSimpleObj) == 4, "");
static_assert(sizeof(C3DSimpleObj) == 0x60, "");
static_assert(std::is_same<decltype(&Simple3DObjCreate), I3DSimpleObj* (*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&Simple3DObjCreate2), I3DSimpleObj* (*)(OBJID, OBJID, OBJID, int, int, int)>::value, "");
static_assert(offsetof(C3DSimpleObj, m_nPosH) == 0x0C && offsetof(C3DSimpleObj, m_nRotate) == 0x10, "");
static_assert(offsetof(C3DSimpleObj, m_nObliqueAngle) == 0x14 && offsetof(C3DSimpleObj, m_vScale) == 0x18, "");
static_assert(offsetof(C3DSimpleObj, m_vRotate) == 0x24 && offsetof(C3DSimpleObj, m_LightOffset) == 0x30, "");
static_assert(offsetof(C3DSimpleObj, m_nShadowType) == 0x3C && offsetof(C3DSimpleObj, m_a) == 0x40, "");
static_assert(offsetof(C3DSimpleObj, m_bOblique) == 0x50 && offsetof(C3DSimpleObj, m_fPosH) == 0x54 && offsetof(C3DSimpleObj, m_p3DRolePart) == 0x58, "");
static_assert(offsetof(C3DSimpleObj, m_idType) == 0x5C, "");

TEST_CLASS(SimpleObj)
{
public:
    TEST_METHOD(TheExportsAndTheSetters)
    {
        I3DSimpleObj* p = Simple3DObjCreate(0);
        Assert::IsNotNull(p);
        C3DSimpleObj* pObj = static_cast<C3DSimpleObj*>(p);
        Assert::IsNull(pObj->m_p3DRolePart);
        Assert::IsTrue(pObj->m_vScale.x == 1.0f && pObj->m_LightOffset.z == 1000.0f && pObj->m_nShadowType == 2 && pObj->m_a == 0.3f && pObj->m_bOblique);
        Assert::AreEqual(0u, (unsigned) pObj->m_idType);
        p->SetPos(10, 20, 7, 90, 2.0f);
        Assert::IsTrue(pObj->m_nPosX == 10 && pObj->m_nPosY == 20 && pObj->m_nPosH == 14 && pObj->m_nRotate == 90 && pObj->m_vScale.y == 2.0f);
        p->Scale(1, 2, 3);
        Assert::IsTrue(pObj->m_vScale.z == 3.0f);
        p->Rotate(4, 5, 6);
        Assert::IsTrue(pObj->m_vRotate.x == 4.0f);
        p->SetLightOffset(1, 2, 3, 4, 5, 6, 7, 8);
        Assert::IsTrue(pObj->m_nShadowType == 1 && pObj->m_LightOffset.y == -3.0f && pObj->m_LightOffset.z == -4.0f && pObj->m_b == 8.0f);
        p->SetPosH(2.5f); p->SetObliqueAngle(30); p->SetOblique(FALSE); p->SetShadowType(3);
        Assert::IsTrue(pObj->m_fPosH == 2.5f && pObj->m_nObliqueAngle == 30 && !pObj->m_bOblique && pObj->m_nShadowType == 3);
        typedef void (__fastcall *FnScale)(void*, void*, float, float, float);
        ((FnScale) (*(void***) p)[17])(p, 0, 7.0f, 8.0f, 9.0f);
        Assert::IsTrue(pObj->m_vScale.x == 7.0f && pObj->m_vScale.z == 9.0f);
        Assert::AreEqual(0u, (unsigned) p->Release());

        I3DSimpleObj* p2 = Simple3DObjCreate2(1001, 2002, 3003, 12345, 5, 6);
        Assert::IsNotNull(p2);
        C3DRolePart* pPart = static_cast<C3DSimpleObj*>(p2)->m_p3DRolePart;
        Assert::IsNotNull(pPart);
        RolePartInfo* pInfo = pPart->GetInfo();
        Assert::IsTrue(pInfo->nPart == 1 && pInfo->idMesh[0] == 1001 && pInfo->idTexture[0] == 2002 && pInfo->idMixTex[0] == 3003 && pInfo->nMixOpt[0] == 2 && pInfo->nAsb[0] == 5 && pInfo->nAdb[0] == 6);
        Assert::AreEqual(0u, (unsigned) static_cast<C3DSimpleObj*>(p2)->m_idType);
        typedef int (__fastcall *FnReload)(void*, void*);
        Assert::IsTrue(((FnReload) (*(void***) p2)[18])(p2, 0) != 0);
        Assert::IsTrue(pInfo->nPart == 1 && pInfo->idMesh[0] == 1001);
        p2->SetARGB(1, 1, 1, 1); p2->SetFrame(1); p2->SetMotion(NULL); p2->AddEffect("none"); p2->SetTexture(NULL, NULL, 0);
        Assert::AreEqual(0u, (unsigned) p2->Release());
    }
};
