#include "CppUnitTest.h"
#include <windows.h>
#include <cstddef>
#include "3DRolePart.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(RolePartInfo) == 0x74, "");
static_assert(offsetof(C3DRolePart, m_infoPart) == 0x8C + 0x40 + sizeof(CGame3DEffectEx2) + 4 + 4, "");
static_assert(sizeof(C3DRolePart) == offsetof(C3DRolePart, m_infoPart) + sizeof(RolePartInfo), "");

struct RolePartProbe : C3DRolePart
{
    static size_t Type()		{ return offsetof(RolePartProbe, m_idType); }
    static size_t Action()		{ return offsetof(RolePartProbe, m_nAction); }
    static size_t Local()		{ return offsetof(RolePartProbe, m_matrixLocal); }
    static size_t World()		{ return offsetof(RolePartProbe, m_matrixWorld); }
    static size_t Effect()		{ return offsetof(RolePartProbe, m_objGame3DEffectEx); }
    static size_t Flag()		{ return offsetof(RolePartProbe, m_bTextureSet); }
};

TEST_CLASS(RolePart)
{
public:
    TEST_METHOD(TheLayoutAndAnEmptyPart)
    {
        Assert::IsTrue(RolePartProbe::Type() == 0x04);
        Assert::IsTrue(RolePartProbe::Action() == 0x08);
        Assert::IsTrue(RolePartProbe::Local() == 0x0C);
        Assert::IsTrue(RolePartProbe::World() == 0x4C);
        Assert::IsTrue(RolePartProbe::Effect() == 0xCC);
        Assert::IsTrue(RolePartProbe::Flag() == 0xCC + sizeof(CGame3DEffectEx2));

        C3DRolePart part;
        Assert::AreEqual(0u, (unsigned) part.GetTypeID());
        Assert::AreEqual(0, part.GetInfo()->nPart);
        Assert::IsTrue(part.GetMatrixLocal()->_11 == 1.0f && part.GetMatrixLocal()->_44 == 1.0f && part.GetMatrixLocal()->_12 == 0.0f);
        Assert::IsTrue(part.GetMatrixAdjust()->_11 == 1.0f && part.GetMatrixAdjust()->_44 == 1.0f && part.GetMatrixAdjust()->_21 == 0.0f);
        D3DXMATRIXA16 scale2;
        D3DXMatrixScaling(&scale2, 2.0f, 2.0f, 2.0f);
        part.SetMatrix(&scale2);
        Assert::IsTrue(part.GetMatrixAdjust()->_11 == 2.0f && part.GetMatrixAdjust()->_33 == 2.0f);
        D3DXMATRIXA16 move;
        D3DXMatrixTranslation(&move, 1.0f, 0.0f, 0.0f);
        part.MuliplyPhy(&move);
        Assert::IsTrue(part.GetMatrixLocal()->_11 == 2.0f && part.GetMatrixLocal()->_41 == 1.0f);
        part.SetMatrix(NULL);
        Assert::IsTrue(part.GetMatrixAdjust()->_11 == 1.0f && part.GetMatrixAdjust()->_33 == 1.0f);
        Assert::IsTrue(part.IsLoaded() != 0);
        D3DXMATRIXA16 identity;
        D3DXMatrixIdentity(&identity);
        part.MuliplyPhy(&identity);
        part.Move(1, 2, 3); part.SetFrame(1); part.ClearMatrix(); part.SetTexture(); part.SetDrawType(1);
        Assert::IsFalse(part.TestEffect("x") != 0);
        Assert::IsFalse(part.TestEffect(NULL) != 0);
        Assert::IsFalse(part.HitTest(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 1)) != 0);
        C3MotionTracker tracker = part.GetVirtualMotion("v_body");
        Assert::IsNull(tracker.lpMotion);
        Assert::IsNull(tracker.lpBlendMotion);
        D3DXMATRIXA16 matrix;
        part.GetBoneMatrix(&matrix, 0, 0);
        Assert::IsTrue(matrix._11 == 1.0f && matrix._44 == 1.0f);
        part.AddEffect("none", TRUE);
        part.Clear3DEffect();
    }
};
