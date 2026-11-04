#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/share/define.h"
#include "../../src/MsgServer/MapGroupKernel/TradeBuddy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_tradebuddy_tests
{
	struct TradeBuddyLayout : public CTradeBuddy
	{
		static size_t OffId()		{ return offsetof(TradeBuddyLayout, m_id); }
		static size_t OffUser()		{ return offsetof(TradeBuddyLayout, m_idUser); }
		static size_t OffBuddy()	{ return offsetof(TradeBuddyLayout, m_idBuddy); }
		static size_t OffName()		{ return offsetof(TradeBuddyLayout, m_szName); }
		static size_t OffDate()		{ return offsetof(TradeBuddyLayout, m_dwDate); }
	};

	TEST_CLASS(MapGroupTradeBuddyTests)
	{
	public:
		TEST_METHOD(TradeBuddyLayout)
		{
			Assert::AreEqual((size_t)0x28, sizeof(CTradeBuddy));
			Assert::AreEqual((size_t)0x08, TradeBuddyLayout::OffId());
			Assert::AreEqual((size_t)0x0C, TradeBuddyLayout::OffUser());
			Assert::AreEqual((size_t)0x10, TradeBuddyLayout::OffBuddy());
			Assert::AreEqual((size_t)0x14, TradeBuddyLayout::OffName());
			Assert::AreEqual((size_t)0x24, TradeBuddyLayout::OffDate());
		}
	};
}
