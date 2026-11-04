#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Pet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct PetProbe : CPet
{
    static size_t Behavior() { return offsetof(PetProbe, m_nBehaviorType); }
    static size_t FirstAtk() { return offsetof(PetProbe, m_bFirstAtk); }
    static size_t IdleEnd() { return offsetof(PetProbe, m_dwIdleEnd); }
    static size_t Target() { return offsetof(PetProbe, m_idTarget); }
    static size_t AIInfo() { return offsetof(PetProbe, m_AIInfo); }
    static size_t Align() { return offsetof(PetProbe, m_posAlign); }
    static size_t HeroTarget() { return offsetof(PetProbe, m_idHeroTarget); }
    static size_t HeroTargetTime() { return offsetof(PetProbe, m_dwHeroTargetTime); }
    static size_t LastJump() { return offsetof(PetProbe, m_dwLastJumpTime); }
};

static_assert(sizeof(PetInfo) == 4 * 5 + sizeof(vector<int>), "");
static_assert(std::is_same<decltype(&CPet::CreateNewPet), CPet* (*)(OBJID, int)>::value, "");
static_assert(std::is_same<decltype(((CPet*) 0)->SetCommand(0)), DWORD>::value && std::is_same<decltype(((CPet*) 0)->ProcessCommand()), BOOL>::value, "");
static_assert(std::is_same<decltype(((CPet*) 0)->GetAtkRange()), int>::value, "");
static_assert(std::has_virtual_destructor<CPet>::value, "");
static_assert(CPet::_PET_IDLE == 0 && CPet::_PET_BEHAVIOR_READY == 1 && CPet::_PET_BEHAVIOR_FIND_HOST == 2 && CPet::_PET_ATTACK == 3, "");
static_assert(CPet::_PET_SE_STATUS_ASSISTANT == 1 && CPet::_PET_SE_JUSTICAL_C == 8, "");

TEST_CLASS(ClientPetHeader)
{
public:
    TEST_METHOD(MemberOrderMatchesTheConstructor)
    {
        Assert::IsTrue(PetProbe::Behavior() == sizeof(CPlayer));
        Assert::IsTrue(PetProbe::FirstAtk() == PetProbe::Behavior() + 4);
        Assert::IsTrue(PetProbe::IdleEnd() == PetProbe::Behavior() + 8);
        Assert::IsTrue(PetProbe::Target() == PetProbe::Behavior() + 0xC);
        Assert::IsTrue(PetProbe::AIInfo() == PetProbe::Behavior() + 0x10);
        Assert::IsTrue(PetProbe::Align() == PetProbe::AIInfo() + sizeof(PetInfo));
        Assert::IsTrue(PetProbe::HeroTarget() == PetProbe::Align() + 8);
        Assert::IsTrue(PetProbe::HeroTargetTime() == PetProbe::Align() + 0xC);
        Assert::IsTrue(PetProbe::LastJump() == PetProbe::Align() + 0x10);
        Assert::IsTrue(sizeof(CPet) == PetProbe::Align() + 0x18);
        Assert::IsTrue(0xD88 - 0xD3C == 0x10 + 0x24 + 0x18);
    }
};

static_assert(std::is_same<decltype(((CPet*) 0)->SetHeroTarget(0)), void>::value, "");
