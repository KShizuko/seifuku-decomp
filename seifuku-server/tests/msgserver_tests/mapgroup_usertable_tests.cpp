#include "CppUnitTest.h"
#include <windows.h>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/ItemType.h"
#include "../../src/MsgServer/MapGroupKernel/UserTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_usertable_tests
{
	TEST_CLASS(MapGroupUserTableTests)
	{
	public:
		TEST_METHOD(TaskColumns)
		{
			Assert::AreEqual(1, (int)TASKDATA_IDNEXT);
			Assert::AreEqual(5, (int)TASKDATA_MONEY);
			Assert::AreEqual(8, (int)TASKDATA_MINPK);
			Assert::AreEqual(9, (int)TASKDATA_MAXPK);
			Assert::AreEqual(14, (int)TASKDATA_CLIENTACT);
		}
		TEST_METHOD(MonsterTypeColumns)
		{
			Assert::AreEqual(4, (int)NPCTYPEDATA_LIFE);
			Assert::AreEqual(27, (int)NPCTYPEDATA_DROP_ARMET);
			Assert::AreEqual(33, (int)NPCTYPEDATA_DROP_SHOES);
			Assert::AreEqual(39, (int)NPCTYPEDATA_AITYPE);
			Assert::AreEqual(40, (int)NPCTYPEDATA_DEFENCE2);
			Assert::AreEqual(41, (int)NPCTYPEDATA_STC_TYPE);
			Assert::AreEqual(45, (int)NPCTYPEDATA_EXTRA_DAMAGE);
		}
		TEST_METHOD(OtherColumns)
		{
			Assert::AreEqual(13, (int)TRAPTYPEDATA_ATKMODE);
			Assert::AreEqual(5, (int)REBIRTHDATA_METEMPSYCHOSIS);
			Assert::AreEqual(6, (int)ADDPOINTDATA_SOUL);
			Assert::AreEqual(5, (int)ACTIONDATA_PARAM);
		}
	};
}
