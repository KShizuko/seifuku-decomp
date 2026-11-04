#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/ItemType.h"
#include "../../src/MsgServer/MapGroupKernel/ItemAddition.h"
#include "../../src/MsgServer/MapGroupKernel/ItemData.h"
#include "../../src/MsgServer/MapGroupKernel/Item.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_item_tests
{
	struct ItemDataProbe : public CItemData
	{
		static size_t Data()		{ return offsetof(ItemDataProbe, m_pData); }
		static size_t Type()		{ return offsetof(ItemDataProbe, m_pType); }
		static size_t Addition()	{ return offsetof(ItemDataProbe, m_pAddition); }
	};

	TEST_CLASS(MapGroupItemTests)
	{
	public:
		TEST_METHOD(ItemInfoLayout)
		{
			Assert::AreEqual((size_t)0x4C, sizeof(ItemInfoStruct));
			Assert::AreEqual((size_t)0x20, offsetof(ItemInfoStruct, nData));
			Assert::AreEqual((size_t)0x34, offsetof(ItemInfoStruct, nMagic3));
			Assert::AreEqual((size_t)0x38, offsetof(ItemInfoStruct, nReduceDmg));
			Assert::AreEqual((size_t)0x3C, offsetof(ItemInfoStruct, nAddLife));
			Assert::AreEqual((size_t)0x40, offsetof(ItemInfoStruct, nAntiMonster));
			Assert::AreEqual((size_t)0x44, offsetof(ItemInfoStruct, usPlunder));
			Assert::AreEqual((size_t)0x46, offsetof(ItemInfoStruct, usSpecialFlag));
			Assert::AreEqual((size_t)0x48, offsetof(ItemInfoStruct, nColor));
		}

		TEST_METHOD(ItemDataLayout)
		{
			Assert::AreEqual((size_t)0x14, sizeof(CItemData));
			Assert::AreEqual((size_t)0x08, ItemDataProbe::Data());
			Assert::AreEqual((size_t)0x0C, ItemDataProbe::Type());
			Assert::AreEqual((size_t)0x10, ItemDataProbe::Addition());
			Assert::AreEqual(17, (int)ITEMDATA_CHKSUM);
			Assert::AreEqual(20, (int)ITEMDATA_COLOR);
			Assert::AreEqual(129, (int)ITEMDATA_MAGICATK);
			Assert::AreEqual(10, ITEMPOSITION_EQUIPEND);
		}

		TEST_METHOD(TypeAndAdditionColumns)
		{
			Assert::AreEqual(12, (int)ITEMTYPEDATA_PRICE);
			Assert::AreEqual(21, (int)ITEMTYPEDATA_AMOUNT);
			Assert::AreEqual(28, (int)ITEMTYPEDATA_MAGIC3);
			Assert::AreEqual(36, (int)ITEMTYPEDATA_EMONEY_PRICE);
			Assert::AreEqual(7, (int)ITEMADDITIONDATA_MAGICATK);
			Assert::AreEqual(10, (int)ITEMADDITIONDATA_DODGE);
		}

		TEST_METHOD(ItemLayoutAndSorts)
		{
			Assert::AreEqual((size_t)0x14, sizeof(CItem));
			Assert::AreEqual(0, ITEMSORT_EXPEND);
			Assert::AreEqual(5, ITEMSORT_WEAPON2);
			Assert::AreEqual(6, ITEMSORT_SHIELD);
			Assert::AreEqual(9, ITEMSORT_OTHER);
			Assert::AreEqual(11, ITEMSORT_MANTLE);
		}
	};
}
