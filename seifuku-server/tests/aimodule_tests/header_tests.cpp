#include "CppUnitTest.h"
#include <windows.h>
#include "define.h"
#include "T_SingleMap.h"
#include "AiData.h"
#include "I_AiCenter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct CMapObjStub
{
	CMapObjStub(OBJID id) : m_id(id)	{ s_nAlive++; }
	~CMapObjStub()						{ s_nAlive--; }
	OBJID	GetID()						{ return m_id; }
	void	ReleaseByOwner()			{ delete this; }
	OBJID	m_id;
	static int	s_nAlive;
};
int CMapObjStub::s_nAlive = 0;

TEST_CLASS(AiModuleHeaderTests)
{
public:
	TEST_METHOD(SingleMapOwnerReleases)
	{
		CMapObjStub::s_nAlive = 0;
		CSingleMap<CMapObjStub>* pMap = CSingleMap<CMapObjStub>::CreateNew(true);
		for(OBJID id = 1; id <= 3; id++)
			Assert::IsTrue(pMap->AddObj(new CMapObjStub(id)));
		Assert::AreEqual(3, pMap->GetAmount());
		Assert::AreEqual(3, CMapObjStub::s_nAlive);
		Assert::AreEqual((OBJID)2, pMap->GetObj(2)->GetID());
		Assert::IsTrue(pMap->DelObj(2));
		Assert::IsFalse(pMap->DelObj(2));
		Assert::IsNull(pMap->GetObj(2));
		Assert::AreEqual(2, CMapObjStub::s_nAlive);
		pMap->Release();
		Assert::AreEqual(0, CMapObjStub::s_nAlive);
	}

	TEST_METHOD(SingleMapIteratorWalksAndDeletes)
	{
		CMapObjStub::s_nAlive = 0;
		CSingleMap<CMapObjStub>* pMap = CSingleMap<CMapObjStub>::CreateNew(true);
		OBJID ids[] = { 30, 10, 20 };
		for(int i = 0; i < 3; i++)
			pMap->AddObj(new CMapObjStub(ids[i]));
		CSingleMap<CMapObjStub>::Iterator iter = pMap->NewEnum();
		OBJID seen[3] = { 0, 0, 0 };
		int n = 0;
		while(iter.Next())
		{
			seen[n++] = iter->GetID();
			if(iter->GetID() == 20)
				Assert::IsTrue(pMap->DelObj(iter));
		}
		Assert::IsTrue((CMapObjStub*)iter == NULL);
		Assert::AreEqual(3, n);
		Assert::AreEqual((OBJID)10, seen[0]);
		Assert::AreEqual((OBJID)20, seen[1]);
		Assert::AreEqual((OBJID)30, seen[2]);
		Assert::AreEqual(2, pMap->GetAmount());
		Assert::AreEqual(2, CMapObjStub::s_nAlive);
		pMap->Release();
		Assert::AreEqual(0, CMapObjStub::s_nAlive);
	}

	TEST_METHOD(SingleMapNotOwnerKeepsObjects)
	{
		CMapObjStub::s_nAlive = 0;
		CMapObjStub a(1), b(2);
		CSingleMap<CMapObjStub>* pMap = CSingleMap<CMapObjStub>::CreateNew(false);
		pMap->AddObj(&a);
		pMap->AddObj(&b);
		Assert::IsTrue(pMap->DelObj(1));
		pMap->Release();
		Assert::AreEqual(2, CMapObjStub::s_nAlive);
	}

	TEST_METHOD(SystemFunctionTable)
	{
		const char* names[] = { "dump", "keep", "now", "less", "less_equ", "strstr", "percent" };
		for(int i = 0; i < 7; i++)
			Assert::AreEqual(names[i], (const char*)SystemFunction(i));
		Assert::IsNull(SystemFunction(7));
		Assert::AreEqual((int)SYS_FUNC_PERCENT, 6);
	}
};
