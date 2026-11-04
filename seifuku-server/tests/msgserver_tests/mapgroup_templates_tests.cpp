#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/MapGroupKernel/T_SingleMap2.h"
#include "../../src/MsgServer/MapGroupKernel/MultiObjSet.h"
#include "../../src/MsgServer/MapGroupKernel/T_SingleObjSet2.h"
#include "../../src/MsgServer/MapGroupKernel/SingleMap64.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	int	g_nReleased = 0;

	class CFakeRecord
	{
	public:
		CFakeRecord(OBJID id) : m_id(id) {}
		static CFakeRecord*	CreateNew()					{ return new CFakeRecord(0); }
		bool				Create(IRecordset* pRes)	{ return false; }
		OBJID				GetID()						{ return m_id; }
		void				ReleaseByOwner()			{ g_nReleased++; delete this; }
	protected:
		OBJID	m_id;
	};
	typedef	ISingleMap2<CFakeRecord>	IFakeSet;
}

TEST_CLASS(MapGroupSingleMap2Tests)
{
public:
	TEST_METHOD(AddFindDeleteAndOwnerRelease)
	{
		g_nReleased = 0;
		IFakeSet* pSet = CSingleMap2<CFakeRecord>::CreateNew(true);
		Assert::IsTrue(pSet->AddObj(new CFakeRecord(7)));
		Assert::IsTrue(pSet->AddObj(new CFakeRecord(3)));
		Assert::AreEqual(7ul, pSet->GetObj(7)->GetID());
		Assert::IsNull(pSet->GetObj(5));
		Assert::AreEqual(3ul, pSet->GetObjByIter(pSet->Begin())->GetID());
		Assert::IsTrue(pSet->DelObj(3));
		Assert::AreEqual(1, g_nReleased);
		Assert::IsFalse(pSet->DelObj(3));
		pSet->Release();
		Assert::AreEqual(2, g_nReleased);
	}

	TEST_METHOD(ANonOwnerLeavesItsObjects)
	{
		g_nReleased = 0;
		CFakeRecord rec(9);
		IFakeSet* pSet = CSingleMap2<CFakeRecord>::CreateNew(false);
		Assert::IsTrue(pSet->AddObj(&rec));
		Assert::IsTrue(pSet->AddObj(&rec));
		Assert::IsTrue(pSet->DelObj(9));
		Assert::IsFalse(pSet->DelObj(9));
		pSet->Release();
		Assert::AreEqual(0, g_nReleased);
	}
};

namespace
{
	class CFakeThing
	{
	public:
		CFakeThing(OBJID id, int nType) : m_id(id), m_nType(nType) {}
		OBJID	GetID()				{ return m_id; }
		int		GetObjType()		{ return m_nType; }
		void	ReleaseByOwner()	{ g_nReleased++; delete this; }
	protected:
		OBJID	m_id;
		int		m_nType;
	};
	typedef	IMultiObjSet<CFakeThing>	IFakeThingSet;
}

TEST_CLASS(MapGroupMultiObjSetTests)
{
public:
	TEST_METHOD(KeyedByIdAndType)
	{
		g_nReleased = 0;
		IFakeThingSet* pSet = CMultiObjSet<CFakeThing>::CreateNew(true);
		Assert::IsTrue(pSet->AddObj(new CFakeThing(5, 1)));
		Assert::IsTrue(pSet->AddObj(new CFakeThing(5, 2)));
		Assert::IsTrue(pSet->AddObj(new CFakeThing(6, 1)));
		Assert::AreEqual(3, pSet->GetAmount());
		Assert::AreEqual(2, pSet->GetObj(5, 2)->GetObjType());
		Assert::IsNull(pSet->GetObj(6, 2));
		Assert::AreEqual(6ul, pSet->GetObjByIndex(2)->GetID());
		Assert::IsTrue(pSet->DelObj(5, 1));
		Assert::AreEqual(1, g_nReleased);
		CFakeThing* pPopped = pSet->PopObj(6, 1);
		Assert::IsNotNull(pPopped);
		Assert::AreEqual(1, pSet->GetAmount());
		Assert::AreEqual(1, g_nReleased);
		delete pPopped;
		pSet->Clear();
		Assert::AreEqual(2, g_nReleased);
		Assert::AreEqual(0, pSet->GetAmount());
		pSet->Release();
	}
};
TEST_CLASS(MapGroupSingleObjSet2Tests)
{
public:
	TEST_METHOD(AVectorKeyedById)
	{
		g_nReleased = 0;
		ISingleObjSet2<CFakeRecord>* pSet = CSingleObjSet2<CFakeRecord>::CreateNew(true);
		Assert::IsTrue(pSet->AddObj(new CFakeRecord(4)));
		Assert::IsTrue(pSet->AddObj(new CFakeRecord(2)));
		Assert::AreEqual(2, pSet->GetAmount());
		Assert::AreEqual(4ul, pSet->GetObjByIndex(0)->GetID());
		Assert::AreEqual(2ul, pSet->GetObj(2)->GetID());
		Assert::IsNull(pSet->GetObj(3));
		CFakeRecord* pPopped = pSet->PopObj(4);
		Assert::IsNotNull(pPopped);
		Assert::AreEqual(0, g_nReleased);
		pPopped->ReleaseByOwner();
		Assert::IsTrue(pSet->DelObj(2));
		Assert::AreEqual(2, g_nReleased);
		Assert::AreEqual(0, pSet->GetAmount());
		pSet->Release();
	}
};
namespace
{
	class CFakePair
	{
	public:
		CFakePair(int nHigh, int nLow) : m_nHigh(nHigh), m_nLow(nLow) {}
		static CFakePair*	CreateNew()					{ return new CFakePair(0, 0); }
		bool				Create(IRecordset* pRes)	{ return false; }
		int					GetInt(int idx)				{ return idx == 1 ? m_nHigh : m_nLow; }
		void				ReleaseByOwner()			{ g_nReleased++; delete this; }
	protected:
		int		m_nHigh, m_nLow;
	};
	typedef	ISingleMap64<CFakePair, int, 1, 2>	IFakePairSet;
}

TEST_CLASS(MapGroupSingleMap64Tests)
{
public:
	TEST_METHOD(KeyOfTwoColumns)
	{
		g_nReleased = 0;
		IFakePairSet* pSet = CSingleMap64<CFakePair, int, 1, 2>::CreateNew(true);
		Assert::IsTrue(pSet->AddObj(new CFakePair(3, 7)));
		Assert::IsTrue(pSet->AddObj(new CFakePair(1, -1)));
		Assert::IsNotNull(pSet->GetObj((((__int64)3) << 32) + 7));
		Assert::IsNotNull(pSet->GetObj((((__int64)1) << 32) - 1));
		Assert::IsNull(pSet->GetObj((((__int64)1) << 32) + 0xFFFFFFFF));
		Assert::IsTrue(pSet->DelObj((((__int64)3) << 32) + 7));
		Assert::AreEqual(1, g_nReleased);
		pSet->Release();
		Assert::AreEqual(2, g_nReleased);
	}
};