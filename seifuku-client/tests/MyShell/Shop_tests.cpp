#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Shop.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ShopProbe : CShop
{
    static size_t Npc() { return offsetof(ShopProbe, m_idNpc); }
    static size_t Name() { return offsetof(ShopProbe, m_szName); }
    static size_t Active() { return offsetof(ShopProbe, m_bActive); }
    static size_t Type() { return offsetof(ShopProbe, m_nShopType); }
    static size_t MoneyType() { return offsetof(ShopProbe, m_nMoneyType); }
    static size_t Items() { return offsetof(ShopProbe, m_dequeItem); }
    static CShopData* Data() { return &s_ShopData; }
};

static_assert(sizeof(CShop) == 0x24 + sizeof(DEQUE_ITEM), "");
static_assert(std::is_polymorphic<CShop>::value, "");
static_assert(_SHOP_NONE == 0 && _JEWELRY_SHOP == 8, "");
static_assert(std::is_same<decltype(&CShop::Open), BOOL (CShop::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CShop::GetItem), CItem* (CShop::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CShop::GetShopID), OBJID (CShop::*)()>::value, "");
static_assert(std::is_same<decltype(&CShop::GetMoneyType), int (CShop::*)()>::value, "");
static_assert(std::is_same<decltype(&CShop::GetItemByTypeID), CItem* (CShop::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&InsertComma), void (*)(char*)>::value, "");
static_assert(std::is_same<decltype(&CShop::GetShopName), char* (CShop::*)()>::value, "");
static_assert(std::is_same<decltype(&CShop::GetShopType), int (CShop::*)()>::value, "");
static_assert(std::is_same<decltype(&RemoveChar), void (*)(char*, char)>::value, "");

TEST_CLASS(ClientShopHeader)
{
public:
    TEST_METHOD(MembersFollowTheVtable)
    {
        Assert::IsTrue(ShopProbe::Npc() == 0x4);
        Assert::IsTrue(ShopProbe::Name() == 0x8);
        Assert::IsTrue(ShopProbe::Active() == 0x18);
        Assert::IsTrue(ShopProbe::Type() == 0x1C);
        Assert::IsTrue(ShopProbe::MoneyType() == 0x20);
        Assert::IsTrue(ShopProbe::Items() == 0x24);
    }
};
