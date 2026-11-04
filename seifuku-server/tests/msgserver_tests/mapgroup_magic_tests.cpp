#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/I_Role.h"
#include "../../src/MsgServer/MapGroupKernel/ItemData.h"
#include "../../src/MsgServer/MapGroupKernel/MagicType.h"
#include "../../src/MsgServer/MapGroupKernel/MagicData.h"
#include "../../src/MsgServer/MapGroupKernel/magic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_magic_tests
{
	struct MagicDataProbe : public CMagicData
	{
		static size_t Type()		{ return offsetof(MagicDataProbe, m_pType); }
	};
	struct MagicProbe : public CMagic
	{
		static size_t SetData()		{ return offsetof(MagicProbe, m_setData); }
		static size_t Magic()		{ return offsetof(MagicProbe, m_pMagic); }
		static size_t Pos()			{ return offsetof(MagicProbe, m_pos); }
		static size_t Intone()		{ return offsetof(MagicProbe, m_tIntone); }
		static size_t Delay()		{ return offsetof(MagicProbe, m_tDelay); }
		static size_t AutoAttack()	{ return offsetof(MagicProbe, m_bAutoAttack); }
		static size_t RawDelay()	{ return offsetof(MagicProbe, m_nRawDelay); }
		static size_t Process()		{ return offsetof(MagicProbe, m_idProcess); }
		static size_t User()		{ return offsetof(MagicProbe, m_pUser); }
	};

	TEST_CLASS(MapGroupMagicTests)
	{
	public:
		TEST_METHOD(MagicTypeColumns)
		{
			Assert::AreEqual(1, (int)MAGICTYPEDATA_TYPE);
			Assert::AreEqual(8, (int)MAGICTYPEDATA_LEVEL);
			Assert::AreEqual(33, (int)MAGICTYPEDATA_ITEM_NUM);
		}
		TEST_METHOD(MagicDataIndexes)
		{
			Assert::AreEqual(2, (int)MAGICDATA_TYPE);
			Assert::AreEqual(4, (int)MAGICDATA_EXP);
			Assert::AreEqual(102, (int)MAGICDATA_SORT);
			Assert::AreEqual(104, (int)MAGICDATA_CRIME);
			Assert::AreEqual(116, (int)MAGICDATA_STATUS);
		}
		TEST_METHOD(RecordAndMagicLayouts)
		{
			Assert::AreEqual((size_t)0x10, sizeof(CMagicData));
			Assert::AreEqual((size_t)0x0C, MagicDataProbe::Type());
			Assert::AreEqual((size_t)0x4C, sizeof(CMagic));
			Assert::AreEqual((size_t)0x04, MagicProbe::SetData());
			Assert::AreEqual((size_t)0x08, MagicProbe::Magic());
			Assert::AreEqual((size_t)0x10, MagicProbe::Pos());
			Assert::AreEqual((size_t)0x1C, MagicProbe::Intone());
			Assert::AreEqual((size_t)0x24, MagicProbe::Delay());
			Assert::AreEqual((size_t)0x2C, MagicProbe::AutoAttack());
			Assert::AreEqual((size_t)0x30, MagicProbe::RawDelay());
			Assert::AreEqual((size_t)0x40, MagicProbe::Process());
			Assert::AreEqual((size_t)0x44, MagicProbe::User());
		}
	};
}
