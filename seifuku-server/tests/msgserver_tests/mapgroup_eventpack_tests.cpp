#include "CppUnitTest.h"
#include <windows.h>
#include <string.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/EventPack.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_eventpack_tests
{
	class CTestPack : public CEventPack
	{
	public:
		CTestPack(DWORD idEvent, ULONG idKey, DWORD idAction) : CEventPack(idEvent, idKey, idAction) {}
		CTestPack(const char* pBuf, int nSize) : CEventPack(pBuf, nSize) {}
		bool	Split() const				{ return m_bSplit; }
		const char*	Param() const			{ return m_setParam; }
	};

	TEST_CLASS(MapGroupEventPackTests)
	{
	public:
		TEST_METHOD(LayoutAsTheBinary)
		{
			Assert::AreEqual(0x408, EVENTPACK_SIZE);
			Assert::AreEqual((size_t)0x414, sizeof(CEventPack));
			CTestPack pack(12, 0x11223344, 34);
			const char* p = (const char*)&pack;
			Assert::AreEqual((int)(pack.GetBuf() - p), 4);
			Assert::AreEqual((int)(pack.Param() - p), 0x0C);
		}

		TEST_METHOD(UniteHead)
		{
			CTestPack pack(12, 0x11223344, 34);
			Assert::AreEqual(12, pack.GetEventType());
			Assert::AreEqual(34, pack.GetAction());
			Assert::AreEqual((ULONG)0x11223344, pack.GetObjID());
			Assert::IsFalse(pack.Split());
			Assert::AreEqual(8, pack.GetSize());
		}

		TEST_METHOD(SplitCopiesTheBuffer)
		{
			CTestPack src(7, 99, 3);
			CTestPack dst(src.GetBuf(), src.GetSize());
			Assert::AreEqual(7, dst.GetEventType());
			Assert::AreEqual(3, dst.GetAction());
			Assert::AreEqual((ULONG)99, dst.GetObjID());
			Assert::IsTrue(dst.Split());
			Assert::AreEqual(8, dst.GetSize());
		}
	};
}
