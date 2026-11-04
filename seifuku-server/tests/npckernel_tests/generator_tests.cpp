#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include <string.h>
#include "NpcType.h"
#include "NpcGenerator.h"
#include "DynaNpcGen.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct NpcGenLayout : public CNpcGenerator
	{
		static void Check()
		{
			static_assert(offsetof(NpcGenLayout, m_nTotalGen) == 0x0C, "");
			static_assert(offsetof(NpcGenLayout, m_nAmount) == 0x10, "");
			static_assert(offsetof(NpcGenLayout, m_nGenAmount) == 0x14, "");
			static_assert(offsetof(NpcGenLayout, m_nCurrGen) == 0x18, "");
			static_assert(offsetof(NpcGenLayout, m_timeoutRest) == 0x1C, "");
			static_assert(offsetof(NpcGenLayout, m_nMaxNpc) == 0x24, "");
			static_assert(offsetof(NpcGenLayout, m_tBadGen) == 0x28, "");
			static_assert(offsetof(NpcGenLayout, m_pMap) == 0x2C, "");
			static_assert(offsetof(NpcGenLayout, m_pData) == 0x30, "");
		}
	};
	struct DynaLayout : public CDynaNpcGen
	{
		static void Check()
		{
			static_assert(offsetof(DynaLayout, m_pGen) == 4, "");
			static_assert(offsetof(DynaLayout, m_idMap) == 8, "");
			static_assert(offsetof(DynaLayout, m_pos) == 0x0C, "");
		}
	};
	struct NpcTypeLayout : public CNpcType
	{
		static void Check() { static_assert(offsetof(NpcTypeLayout, m_pData) == 8, ""); }
	};
}
static_assert(sizeof(CNpcGenerator) == 0x34, "");
static_assert(sizeof(CDynaNpcGen) == 0x14, "");
static_assert(sizeof(CNpcType) == 0x0C, "");
static_assert(sizeof(CGameData<NPCTYPEDATA, szNpcType, szID>) == 0x0C, "");

namespace
{
	class TypeRecord : public IRecord
	{
	public:
		TypeRecord(OBJID id, int nLevel) : m_id(id), m_nLevel(nLevel) {}
		virtual ~TypeRecord() {}
		virtual void	Release()					{ delete this; }
		virtual UINT	LoadUInt(LPCTSTR szField)	{ Assert::AreEqual(0, strcmp(szField, "id")); return m_id; }
		virtual int		GetInt(int nIdx)			{ return nIdx == NPCTYPEDATA_LEVEL ? m_nLevel : 0; }
	private:
		OBJID	m_id;
		int		m_nLevel;
	};

	class TypeRecordset : public IRecordset
	{
	public:
		virtual ~TypeRecordset() {}
		virtual void		Release()				{ delete this; }
		virtual IRecord*	CreateNewRecord(DWORD idNewKey)	{ Assert::AreEqual((DWORD)NEWKEY_NONE, idNewKey); return new TypeRecord(3101, 42); }
	};

	class FixedGen : public IGenerator
	{
	public:
		virtual ~FixedGen() {}
		virtual int		GetWidth()					{ return 20; }
		virtual int		GetHeight()					{ return 30; }
	};
}

TEST_CLASS(GeneratorTests)
{
public:
	TEST_METHOD(NpcTypeCreate)
	{
		CNpcType* pType = CNpcType::CreateNew();
		Assert::AreEqual((int)OBJ_NPCTYPE, pType->GetObjType());
		TypeRecordset* pRes = new TypeRecordset;
		Assert::IsTrue(pType->Create(pRes));
		pRes->Release();
		Assert::AreEqual((OBJID)3101, pType->GetID());
		Assert::AreEqual(42, pType->GetInt(NPCTYPEDATA_LEVEL));
		pType->ReleaseByOwner();
	}

	TEST_METHOD(DynaNpcGenRegion)
	{
		FixedGen gen;
		IGenerator* pGen = CDynaNpcGen::CreateNew(&gen, 1002, 300, 400);
		Assert::IsNotNull(pGen);
		Assert::AreEqual(290, pGen->GetPosX());
		Assert::AreEqual(385, pGen->GetPosY());
		POINT pos = pGen->GetCenter();
		Assert::AreEqual(300L, pos.x);
		Assert::AreEqual(400L, pos.y);
		Assert::IsTrue(pGen->IsInRegion(290, 385));
		Assert::IsFalse(pGen->IsInRegion(310, 385));
		Assert::IsFalse(pGen->IsTooFar(1002, 280, 380, 10));
		Assert::IsTrue(pGen->IsTooFar(1002, 279, 380, 10));
		Assert::IsTrue(pGen->IsTooFar(1003, 300, 400, 10));
		Assert::AreEqual((OBJID)1234567890, pGen->GetID());
		pGen->Release();

		IGenerator* pNear = CDynaNpcGen::CreateNew(&gen, 1002, 5, 7);
		Assert::AreEqual(5, pNear->GetPosX());
		Assert::AreEqual(7, pNear->GetPosY());
		pNear->Release();
	}
};
