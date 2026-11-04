#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Item.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ItemProbe : CItem
{
    static size_t Id() { return offsetof(ItemProbe, m_idItem); }
    static size_t Owner() { return offsetof(ItemProbe, m_idOwner); }
    static size_t Type() { return offsetof(ItemProbe, m_infoType); }
    static size_t Position() { return offsetof(ItemProbe, m_ucPosition); }
    static size_t SellPrice() { return offsetof(ItemProbe, m_dwSellPrice); }
    static size_t SellEmoneyPrice() { return offsetof(ItemProbe, m_dwSellEmoneyPrice); }
    static size_t MaxAttackModify() { return offsetof(ItemProbe, m_nMaxAttackModify); }
    static size_t MagicDefenceModify() { return offsetof(ItemProbe, m_nMagicDefenceModify); }
    static size_t Addition() { return offsetof(ItemProbe, m_infoAddition); }
    static size_t Bless() { return offsetof(ItemProbe, m_dwBless); }
    static size_t Restrain() { return offsetof(ItemProbe, m_dwRestrain); }
};

static_assert(sizeof(CItem) == 0x144, "");
static_assert(std::has_virtual_destructor<CItem>::value, "");
static_assert(std::is_same<decltype(&CItem::Create), BOOL (CItem::*)(OBJID, OBJID)>::value, "");
static_assert(std::is_same<decltype(&CItem::ItemGetSort), int (*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CItem::ItemGetSubType), int (*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CItem::ItemGetBaseClass), int (*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CItem::IsAdditionStone), BOOL (CItem::*)()>::value, "");
static_assert(std::is_same<decltype(&CItem::IsNonsuchItem), BOOL (*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CItem::SetBoothSellEmoneyPrice), void (CItem::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CItem::IsHelmet), BOOL (CItem::*)(void)>::value && std::is_same<decltype(&CItem::IsShield), BOOL (CItem::*)(void)>::value, "");
static_assert(_MAX_EQUIPMENT == 9, "");
static_assert(ITEMSORT_MOUNT == 10 && ITEMSORT_SPRITE == 11, "");

TEST_CLASS(ClientItemHeader)
{
public:
    TEST_METHOD(MemberOffsetsMatchCreateAndTheAccessors)
    {
        Assert::IsTrue(ItemProbe::Id() == 0x4);
        Assert::IsTrue(ItemProbe::Owner() == 0x8);
        Assert::IsTrue(ItemProbe::Type() == 0xC);
        Assert::IsTrue(ItemProbe::Position() == 0x100);
        Assert::IsTrue(ItemProbe::SellPrice() == 0x104);
        Assert::IsTrue(ItemProbe::SellEmoneyPrice() == 0x108);
        Assert::IsTrue(ItemProbe::MaxAttackModify() == 0x10C);
        Assert::IsTrue(ItemProbe::MagicDefenceModify() == 0x124);
        Assert::IsTrue(ItemProbe::Bless() == 0x128 && ItemProbe::Restrain() == 0x130);
        Assert::IsTrue(ItemProbe::Addition() == 0x134);
    }
};
