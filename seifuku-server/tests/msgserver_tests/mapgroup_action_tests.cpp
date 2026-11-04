#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgAction.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct ActionProbe : public CMsgAction
	{
		typedef CMsgAction::MSG_Info	Body;
		static size_t	Wire(size_t nBody)	{ return nBody + 4; }
	};
	typedef ActionProbe::Body	Body;
}

namespace mapgroup_action_tests
{
	TEST_CLASS(MapGroupActionTests)
	{
	public:
		TEST_METHOD(ActionWireLayout)
		{
			Assert::AreEqual((size_t)0x04, ActionProbe::Wire(offsetof(Body, dwTimeStamp)));
			Assert::AreEqual((size_t)0x08, ActionProbe::Wire(offsetof(Body, idUser)));
			Assert::AreEqual((size_t)0x0C, ActionProbe::Wire(offsetof(Body, dwData)));
			Assert::AreEqual((size_t)0x0C, ActionProbe::Wire(offsetof(Body, usTargetPosX)));
			Assert::AreEqual((size_t)0x0E, ActionProbe::Wire(offsetof(Body, usTargetPosY)));
			Assert::AreEqual((size_t)0x10, ActionProbe::Wire(offsetof(Body, unPosX)));
			Assert::AreEqual((size_t)0x12, ActionProbe::Wire(offsetof(Body, unPosY)));
			Assert::AreEqual((size_t)0x14, ActionProbe::Wire(offsetof(Body, unDir)));
			Assert::AreEqual((size_t)0x16, ActionProbe::Wire(offsetof(Body, usAction)));
			Assert::AreEqual((size_t)0x18, ActionProbe::Wire(sizeof(Body)));
		}
		TEST_METHOD(ActionValues)
		{
			Assert::AreEqual(74, (int)actionEnterMap);
			Assert::AreEqual(86, (int)actionFlyMap);
			Assert::AreEqual(101, (int)actionTeamMemeberPos);
			Assert::AreEqual(108, (int)actionKickBack);
			Assert::AreEqual(138, (int)actionSynchro);
			Assert::AreEqual(142, (int)actionChangeFace);
			Assert::AreEqual(310, (int)actionQueryPlayerInfo);
		}
	};
}
