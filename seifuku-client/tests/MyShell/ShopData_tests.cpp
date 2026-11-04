#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "ShopData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ShopDataProbe : CShopData
{
    static size_t Shops() { return offsetof(ShopDataProbe, m_setShopData); }
};

static_assert(sizeof(ShopDataInfo) == 0x24, "");
static_assert(_MAX_NAMESIZE == 16, "");
static_assert(offsetof(ShopDataInfo, nType) == 0x4 && offsetof(ShopDataInfo, nMoneyType) == 0x8 && offsetof(ShopDataInfo, szName) == 0xC && offsetof(ShopDataInfo, dwItemAmount) == 0x1C && offsetof(ShopDataInfo, pidItem) == 0x20, "");
static_assert(_MAX_ITEMTYPE_AMOUNT_OF_SHOP == 256, "");
static_assert(sizeof(CShopData) == 0x4 + sizeof(DEQUE_SHOPDATA), "");
static_assert(std::is_polymorphic<CShopData>::value, "");
static_assert(std::is_same<decltype(&CShopData::GetItemList), BOOL (CShopData::*)(OBJID, DEQUE_ITEM&, char*, int&)>::value, "");

TEST_CLASS(ClientShopDataHeader)
{
public:
    TEST_METHOD(TheDequeFollowsTheVtable)
    {
        Assert::IsTrue(ShopDataProbe::Shops() == 0x4);
    }
};
