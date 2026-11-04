#include "CppUnitTest.h"
#include <windows.h>
#include <string.h>
#include "../../src/MsgServer/MapGroupKernel/GameObj.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_gameobj_tests
{
	class CTestObj : public CGameObj
	{
	public:
		virtual OBJID GetID() { return 7; }
	};

	TEST_CLASS(MapGroupGameObjTests)
	{
	public:
		TEST_METHOD(LayoutTypeAtPlus4)
		{
			Assert::AreEqual((size_t)8, sizeof(CGameObj));
			CTestObj obj;
			obj.SetObjType(0x1234);
			Assert::AreEqual(0x1234, obj.GetObjType());
			Assert::AreEqual(0x1234, *(int*)((char*)&obj + 4));
			Assert::AreEqual((OBJID)7, obj.GetID());
			Assert::AreEqual(0, strcmp(szID, "id"));
		}
	};
}
