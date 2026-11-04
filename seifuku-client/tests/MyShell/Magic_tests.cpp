#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Magic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(MagicTypeInfo) == 0x630, "");
static_assert(offsetof(MagicTypeInfo, szName) == 8 && offsetof(MagicTypeInfo, dwTarget) == 0x24 && offsetof(MagicTypeInfo, dwLevel) == 0x28, "");
static_assert(offsetof(MagicTypeInfo, dwExpRequired) == 0x50 && offsetof(MagicTypeInfo, dwXp) == 0x58 && offsetof(MagicTypeInfo, dwWeaponSubType) == 0x5C, "");
static_assert(offsetof(MagicTypeInfo, dwIntoneDuration) == 0x34 && offsetof(MagicTypeInfo, dwMagicBreak) == 0x88, "");
static_assert(offsetof(MagicTypeInfo, szDisc) == 0x94 && offsetof(MagicTypeInfo, szIntoneEffect) == 0x1D4 && offsetof(MagicTypeInfo, szSenderEffect) == 0x318, "");
static_assert(offsetof(MagicTypeInfo, dwCanBeusedInMarket) == 0x628, "");
static_assert(std::is_pod<MagicTypeInfo>::value, "");

static_assert(sizeof(CMagicData) == 4 + 3 * 4 + sizeof(MAP_MAGICTYPE), "");

static_assert(offsetof(CMagic, m_dwExp) == 8 && offsetof(CMagic, m_infoMagicType) == 0xC && sizeof(CMagic) == 0x63C, "");
static_assert(std::has_virtual_destructor<CMagic>::value, "");
static_assert(std::is_same<decltype(CMagic::CreateNew(0, 0, 0)), CMagic*>::value, "");
static_assert(MAGICSORT_TRANSFORM == 19, "");
static_assert(TYPE_XPSKILL == 1, "");

TEST_CLASS(ClientMagicHeaders)
{
public:
    TEST_METHOD(MagicDataSizeWithVc6Map)
    {
        Assert::IsTrue(sizeof(CMagicData) - sizeof(MAP_MAGICTYPE) + 0x10 == 0x20);
        Assert::IsTrue(sizeof(CMagic) == 0x63C);
    }
};
