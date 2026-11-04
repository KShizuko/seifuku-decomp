#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "ItemData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(ItemTypeInfo) == 0xF4, "");
static_assert(offsetof(ItemTypeInfo, uID) == 0x00, "");
static_assert(offsetof(ItemTypeInfo, szName) == 0x04, "");
static_assert(offsetof(ItemTypeInfo, ucRequiredLevel) == 0x16, "");
static_assert(offsetof(ItemTypeInfo, ucMonopoly) == 0x20, "");
static_assert(offsetof(ItemTypeInfo, usWeight) == 0x22, "");
static_assert(offsetof(ItemTypeInfo, uPrice) == 0x24, "");
static_assert(offsetof(ItemTypeInfo, uLuck) == 0x28, "");
static_assert(offsetof(ItemTypeInfo, usMaxAttack) == 0x2C, "");
static_assert(offsetof(ItemTypeInfo, usAmount) == 0x3A, "");
static_assert(offsetof(ItemTypeInfo, ucMagic3) == 0x43, "");
static_assert(offsetof(ItemTypeInfo, usMagicAttack) == 0x44, "");
static_assert(offsetof(ItemTypeInfo, usAttackSpeed) == 0x4A, "");
static_assert(offsetof(ItemTypeInfo, ucFrayMode) == 0x4C, "");
static_assert(offsetof(ItemTypeInfo, ucRepairMode) == 0x4D, "");
static_assert(offsetof(ItemTypeInfo, usTypeMask) == 0x4E, "");
static_assert(offsetof(ItemTypeInfo, uEmoneyPrice) == 0x50, "");
static_assert(offsetof(ItemTypeInfo, szShowName) == 0x54, "");
static_assert(offsetof(ItemTypeInfo, szDesc) == 0x74, "");

static_assert(sizeof(ItemAdditionInfo) == 0x10, "");
static_assert(offsetof(ItemAdditionInfo, nLife) == 0 && offsetof(ItemAdditionInfo, nDefense) == 6 && offsetof(ItemAdditionInfo, nMAttack) == 8 && offsetof(ItemAdditionInfo, nDodge) == 0xE, "");

struct ItemDataProbe : CItemData
{
    static size_t Types() { return offsetof(ItemDataProbe, m_mapItemType); }
    static size_t Additions() { return offsetof(ItemDataProbe, m_mapItemAddition); }
    static size_t Fuse() { return offsetof(ItemDataProbe, m_mapFuse); }
};

static_assert(sizeof(CItemData) == 4 + sizeof(MAP_ITEMINFO) + sizeof(MAP_ITEMADDITIONINFO) + sizeof(MAP_FUSE), "");
static_assert(std::has_virtual_destructor<CItemData>::value, "");
static_assert(std::is_same<decltype(&CItemData::GetItemTypeInfo), BOOL (CItemData::*)(OBJID, ItemTypeInfo&)>::value, "");
static_assert(std::is_same<decltype(&CItemData::GetItemAdditionInfo), BOOL (CItemData::*)(OBJID, DWORD, ItemAdditionInfo&)>::value, "");
static_assert(std::is_same<decltype(&CItemData::GetFuseInfo), DWORD (CItemData::*)(DWORD)>::value, "");

TEST_CLASS(ClientItemDataHeader)
{
public:
    TEST_METHOD(TheThreeMapsFollowTheVtable)
    {
        Assert::IsTrue(ItemDataProbe::Types() == 0x4);
        Assert::IsTrue(ItemDataProbe::Additions() == 0x4 + sizeof(MAP_ITEMINFO));
        Assert::IsTrue(ItemDataProbe::Fuse() == 0x4 + sizeof(MAP_ITEMINFO) + sizeof(MAP_ITEMADDITIONINFO));
    }
};
