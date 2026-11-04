#include "CppUnitTest.h"
#include <windows.h>
#include "common.h"
typedef unsigned long OBJID;
#include "AutoPtr.h"
#include "AutoLink.h"
#include "AutoLinkSet.h"
#include "GameObjSet.h"
#include "Index.h"
#include "MyStack.h"
#include "TickOver.h"
#include "T_MyQueue.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct CTestObj
{
	CTestObj(OBJID id) : m_id(id)	{ s_nAlive++; }
	~CTestObj()						{ s_nAlive--; }
	OBJID	GetID()					{ return m_id; }
	void	Release()				{ delete this; }
	void	ReleaseByOwner()		{ delete this; }
	CAutoLink<CTestObj>&	QueryLink(const CAutoLink<CTestObj>&)	{ return m_link; }
	OBJID	m_id;
	CAutoLink<CTestObj>	m_link;
	static int	s_nAlive;
};
int CTestObj::s_nAlive = 0;

TEST_CLASS(HeaderTemplateTests)
{
public:
	TEST_METHOD(GameObjSetOwnsAndDeletesDuringIteration)
	{
		CGameObjSet<CTestObj>* pSet = CGameObjSet<CTestObj>::CreateNew(true);
		for(OBJID id = 1; id <= 4; id++)
			Assert::IsTrue(pSet->AddObj(new CTestObj(id)));
		Assert::AreEqual(4, CTestObj::s_nAlive);
		Assert::IsTrue(pSet->GetObj(3) != NULL);
		for(CGameObjSet<CTestObj>::Iterator it = pSet->NewEnum(); it.Next(); )
		{
			if(it->GetID() % 2)
				Assert::IsTrue(pSet->DelObj(it));
		}
		Assert::AreEqual(2, pSet->GetAmount());
		Assert::AreEqual(2, CTestObj::s_nAlive);
		Assert::IsTrue(pSet->DelObj(2));
		Assert::IsTrue(pSet->GetObj(2) == NULL);
		Assert::IsTrue(pSet->GetObjByIndex(0)->GetID() == 4);
		pSet->Release();
		Assert::AreEqual(0, CTestObj::s_nAlive);
	}

	TEST_METHOD(AutoPtrReleasesOnDestruction)
	{
		{
			CAutoPtr<CTestObj> p;
			p = new CTestObj(9);
			Assert::AreEqual(1, CTestObj::s_nAlive);
			Assert::IsTrue(p->GetID() == 9);
		}
		Assert::AreEqual(0, CTestObj::s_nAlive);
	}

	TEST_METHOD(AutoLinkClearsWhenTheMasterGoes)
	{
		CTestObj* pObj = new CTestObj(5);
		pObj->m_link.Init(pObj);
		CAutoLink<CTestObj> a(pObj);
		CAutoLink<CTestObj> b = a;
		Assert::IsTrue((CTestObj*)b == pObj);
		pObj->Release();
		Assert::IsTrue((CTestObj*)a == NULL);
		Assert::IsTrue((CTestObj*)b == NULL);
	}

	TEST_METHOD(IndexStackQueueTick)
	{
		Index<int, int> idx;
		idx.Add(3, 30);
		Assert::IsTrue(idx.IsIn(3));
		Assert::AreEqual(30, idx[3]);
		CMyStack<int> st;
		st.push(1); st.push(2);
		Assert::AreEqual(2, st.pop());
		CMyQueue<int> q;
		q.push(1); q.push(2);
		Assert::AreEqual(1, q.pop());
		Assert::AreEqual(2, q.pop());
		Assert::AreEqual(0, q.pop());
		CTickOver t;
		t.Startup(100, 10);
		Assert::IsFalse(t.IsTickOver(105));
		Assert::IsTrue(t.ToNextTime(110));
		Assert::AreEqual(120, t.GetEnd());
	}
};
