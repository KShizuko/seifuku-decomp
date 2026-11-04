#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/GameBlock.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_gameblock_tests
{
	struct BlockProbe : public CGameBlock
	{
		static size_t Set()			{ return offsetof(BlockProbe, m_pSet); }
		static size_t Dormancy()	{ return offsetof(BlockProbe, m_bDormancy); }
		void	Wake()				{ m_bDormancy = false; }
	};

	TEST_CLASS(MapGroupGameBlockTests)
	{
	public:
		TEST_METHOD(GameBlockLayout)
		{
			Assert::AreEqual((size_t)0x0C, sizeof(CGameBlock));
			Assert::AreEqual((size_t)0x04, BlockProbe::Set());
			Assert::AreEqual((size_t)0x08, BlockProbe::Dormancy());
		}

		TEST_METHOD(CreateSetsTheDormancyWithTheSet)
		{
			BlockProbe	block;
			Assert::IsNull(block.QuerySet());
			Assert::IsTrue(block.Create());
			Assert::IsNotNull(block.QuerySet());
			Assert::IsTrue(block.IsDormancy());

			IThingSet*	pSet = block.QuerySet();
			block.Wake();
			Assert::IsTrue(block.Create());
			Assert::IsTrue(pSet == block.QuerySet());
			Assert::IsFalse(block.IsDormancy());
		}
	};
}
