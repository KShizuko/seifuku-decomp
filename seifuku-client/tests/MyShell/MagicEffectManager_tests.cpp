#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MagicEffectManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MagicEffectManagerProbe : CMagicEffectManager
{
    static size_t Effects() { return offsetof(MagicEffectManagerProbe, m_setMagicEffect); }
};

static_assert(sizeof(CMagicEffectManager) == 0x4 + sizeof(DEQUE_MAGICEFFECT), "");
static_assert(std::is_polymorphic<CMagicEffectManager>::value, "");
static_assert(std::is_same<decltype(&CMagicEffectManager::AddMagicEffect), void (CMagicEffectManager::*)(CMagicEffect*)>::value, "");
static_assert(std::is_same<decltype(&CMagicEffectManager::Process), void (CMagicEffectManager::*)()>::value, "");
static_assert(std::is_same<decltype(&CMagicEffectManager::Destroy), void (CMagicEffectManager::*)()>::value, "");

TEST_CLASS(ClientMagicEffectManagerHeader)
{
public:
    TEST_METHOD(TheDequeFollowsTheVtable)
    {
        Assert::IsTrue(MagicEffectManagerProbe::Effects() == 0x4);
    }
};
