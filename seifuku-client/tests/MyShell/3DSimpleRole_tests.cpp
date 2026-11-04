#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DSimpleRole.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct SimpleRoleProbe
{
    static size_t Index() { return offsetof(C3DSimpleRole, m_nIndex); }
    static size_t Look() { return offsetof(C3DSimpleRole, m_nLook); }
    static size_t Begin() { return offsetof(C3DSimpleRole, m_dwBegin); }
    static size_t SimpleObjId() { return offsetof(C3DSimpleRole, m_idSimpleObj); }
    static size_t StandBy() { return offsetof(C3DSimpleRole, m_idMotionStandBy); }
    static size_t BlazeId() { return offsetof(C3DSimpleRole, m_idMotionBlaze); }
    static size_t Blaze() { return offsetof(C3DSimpleRole, m_bBlaze); }
    static size_t Pose() { return offsetof(C3DSimpleRole, m_bPose); }
    static size_t Sound() { return offsetof(C3DSimpleRole, m_pSound); }
    static size_t SimpleObj() { return offsetof(C3DSimpleRole, m_obj3DSimpleObj); }
    static size_t EffectF() { return offsetof(C3DSimpleRole, m_objEffectF); }
    static size_t EffectB() { return offsetof(C3DSimpleRole, m_objEffectB); }
    static size_t Role() { return offsetof(C3DSimpleRole, m_obj3DRole); }
    static size_t FrameAmount() { return offsetof(C3DSimpleRole, m_dwFrameAmount); }
    static size_t ResetPose() { return offsetof(C3DSimpleRole, m_bResetPose); }
};

static_assert(std::is_polymorphic<C3DSimpleRole>::value, "");
static_assert(sizeof(IRoleView*) == 4 && sizeof(IGame3DEffect*) == 4 && sizeof(I3DSimpleObj*) == 4, "");
static_assert(std::is_same<decltype(&C3DSimpleRole::Show), void (C3DSimpleRole::*)(int, CMyPos, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&C3DSimpleRole::ShowRole), void (C3DSimpleRole::*)(int, CMyPos, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&C3DSimpleRole::Blaze), void (C3DSimpleRole::*)()>::value, "");
static_assert(_SIMPLEROLE_ROLE_FIRST == 8 && _SIMPLEROLE_ROLE_LAST == 11, "");

TEST_CLASS(ClientSimpleRoleHeader)
{
public:
    TEST_METHOD(TheScalarsPrecedeTheSimpleObject)
    {
        Assert::IsTrue(SimpleRoleProbe::Index() == 0x4);
        Assert::IsTrue(SimpleRoleProbe::Look() == 0x8);
        Assert::IsTrue(SimpleRoleProbe::Begin() == 0xC);
        Assert::IsTrue(SimpleRoleProbe::SimpleObjId() == 0x10);
        Assert::IsTrue(SimpleRoleProbe::StandBy() == 0x14);
        Assert::IsTrue(SimpleRoleProbe::BlazeId() == 0x18);
        Assert::IsTrue(SimpleRoleProbe::Blaze() == 0x1C);
        Assert::IsTrue(SimpleRoleProbe::Pose() == 0x20);
        Assert::IsTrue(SimpleRoleProbe::Sound() == 0x24);
        Assert::IsTrue(SimpleRoleProbe::SimpleObj() == 0x28);
    }
    TEST_METHOD(TheEffectsTheRoleAndTheTwoTailWordsFollow)
    {
        Assert::IsTrue(SimpleRoleProbe::EffectF() == 0x2C);
        Assert::IsTrue(SimpleRoleProbe::EffectB() == 0x30);
        Assert::IsTrue(SimpleRoleProbe::Role() == 0x34);
        Assert::IsTrue(SimpleRoleProbe::FrameAmount() == 0x38);
        Assert::IsTrue(SimpleRoleProbe::ResetPose() == 0x3C);
        Assert::IsTrue(sizeof(C3DSimpleRole) == 0x40);
    }
};
