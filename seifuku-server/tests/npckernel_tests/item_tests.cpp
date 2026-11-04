#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
#include "ItemPack.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(ITEMDATA_DB_END == 20, "");
static_assert(sizeof(ItemInfoStruct) == 0x50, "");
static_assert(sizeof(CItem) == 0x5C, "");
static_assert(ITEMDATA_AMOUNT == 4 && ITEMDATA_POSITION == 7 && ITEMDATA_DATA == 13 && ITEMDATA_PLUNDER == 18, "");
static_assert(ITEMDATA_ATKRANGE == 131, "");

static_assert(ITEMPOSITION_BACKPACK == 0 && ITEMPOSITION_EQUIPBEGIN == 1 && ITEMPOSITION_EQUIPEND == 9, "");
static_assert(MAX_EQUIPSIZE == 8 && MAX_USERITEMSIZE == 40, "");

TEST_CLASS(ItemLayoutTests)
{
public:
	TEST_METHOD(EquipPositionsAreConquers)
	{
		Assert::AreEqual(MAX_EQUIPSIZE, ITEMPOSITION_EQUIPEND - ITEMPOSITION_EQUIPBEGIN);
		Assert::AreEqual(4, ITEMPOSITION_WEAPONR);
	}
};
