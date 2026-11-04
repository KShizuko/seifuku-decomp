#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/MapTrap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_maptrap_tests
{
	struct MapTrapProbe : public CMapTrap
	{
		static size_t Process()		{ return offsetof(MapTrapProbe, m_idProcess); }
		static size_t Info()		{ return offsetof(MapTrapProbe, m_info); }
		static size_t Fight()		{ return offsetof(MapTrapProbe, m_tFight); }
		static size_t Die()			{ return offsetof(MapTrapProbe, m_bDie); }
		static size_t Times()		{ return offsetof(MapTrapProbe, m_nTimes); }
		static size_t User()		{ return offsetof(MapTrapProbe, m_pUser); }
		static size_t Status()		{ return offsetof(MapTrapProbe, m_setStatus); }
		static size_t Map()			{ return offsetof(MapTrapProbe, m_pMap); }
		static size_t Type()		{ return offsetof(MapTrapProbe, m_pType); }
		static size_t Battle()		{ return offsetof(MapTrapProbe, m_pBattleSystem); }
	};

	TEST_CLASS(MapTrapTests)
	{
	public:
		TEST_METHOD(TrapInfoLayout)
		{
			Assert::AreEqual((size_t)0x14, sizeof(TRAP_INFO));
			Assert::AreEqual((size_t)0x04, offsetof(TRAP_INFO, usType));
			Assert::AreEqual((size_t)0x06, offsetof(TRAP_INFO, usLook));
			Assert::AreEqual((size_t)0x08, offsetof(TRAP_INFO, idOwner));
			Assert::AreEqual((size_t)0x0C, offsetof(TRAP_INFO, idMap));
			Assert::AreEqual((size_t)0x10, offsetof(TRAP_INFO, usPosX));
			Assert::AreEqual((size_t)0x12, offsetof(TRAP_INFO, usPosY));
		}

		TEST_METHOD(MapTrapLayout)
		{
			Assert::AreEqual((size_t)0x54, sizeof(CMapTrap));
			Assert::AreEqual((size_t)0x10, MapTrapProbe::Process());
			Assert::AreEqual((size_t)0x14, MapTrapProbe::Info());
			Assert::AreEqual((size_t)0x28, MapTrapProbe::Fight());
			Assert::AreEqual((size_t)0x30, MapTrapProbe::Die());
			Assert::AreEqual((size_t)0x34, MapTrapProbe::Times());
			Assert::AreEqual((size_t)0x38, MapTrapProbe::User());
			Assert::AreEqual((size_t)0x44, MapTrapProbe::Status());
			Assert::AreEqual((size_t)0x48, MapTrapProbe::Map());
			Assert::AreEqual((size_t)0x4C, MapTrapProbe::Type());
			Assert::AreEqual((size_t)0x50, MapTrapProbe::Battle());
		}
	};
}
