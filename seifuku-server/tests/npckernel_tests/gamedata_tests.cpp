#include "CppUnitTest.h"
#include <windows.h>
#include <vector>
#include "GameObj.h"
#include "ItemType.h"
#include "GameData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CGameObj) == 8, "");

namespace
{
	struct Row { int id; int value; };

	class FakeRecord : public IRecord
	{
	public:
		explicit FakeRecord(Row row, int* pLive) : m_row(row), m_pLive(pLive) { ++*m_pLive; }
		virtual ~FakeRecord() { --*m_pLive; }
		virtual void	Release()					{ delete this; }
		virtual int		GetInt(int nIdx)			{ return nIdx == 0 ? m_row.id : m_row.value; }
		virtual LPCTSTR	GetStr(int)					{ return "name"; }
	private:
		Row		m_row;
		int*	m_pLive;
	};

	class FakeRecordset : public IRecordset
	{
	public:
		FakeRecordset(const std::vector<Row>& rows, int* pLive) : m_rows(rows), m_pos(0), m_pLive(pLive) {}
		virtual ~FakeRecordset() {}
		virtual void		Release()				{ delete this; }
		virtual IRecord*	CreateNewConstRecord(DWORD)	{ return new FakeRecord(m_rows[m_pos], m_pLive); }
		virtual UINT		RecordCount()			{ return (UINT)m_rows.size(); }
		virtual bool		MoveNext()				{ return ++m_pos < m_rows.size(); }
	private:
		std::vector<Row>	m_rows;
		size_t				m_pos;
		int*				m_pLive;
	};

	class FakeDatabase : public IDatabase
	{
	public:
		FakeDatabase(const std::vector<Row>& rows, int* pLive) : m_rows(rows), m_pLive(pLive) {}
		virtual ~FakeDatabase() {}
		virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty)
		{ m_strSQL = szSQL; Assert::IsFalse(bNoEmpty); return new FakeRecordset(m_rows, m_pLive); }
		virtual void		Release()				{}
		std::string			m_strSQL;
	private:
		std::vector<Row>	m_rows;
		int*				m_pLive;
	};

	class ItemTypeProbe : public CItemType
	{
	public:
		static ItemTypeProbe*	Make()				{ return new ItemTypeProbe; }
	};

	class FakeRecord64 : public IRecord
	{
	public:
		virtual void	Release()					{ delete this; }
		virtual DWORD	LoadDWord(LPCTSTR)			{ return 7; }
		virtual __int64	GetInt64(int nIdx)			{ return nIdx == 1 ? 0x123456789LL : -1; }
	};
	class FakeRecordset64 : public IRecordset
	{
	public:
		virtual void		Release()				{ delete this; }
		virtual IRecord*	CreateNewRecord(DWORD)	{ return new FakeRecord64; }
	};
}

char szTestTable64[] = "cq_test";
char szTestKey64[] = "id";
enum TESTDATA64 { TESTDATA64_ID = 0, TESTDATA64_EXP };
typedef CGameData<TESTDATA64, szTestTable64, szTestKey64>	CTestData64;

TEST_CLASS(GameDataTests)
{
public:
	TEST_METHOD(GameObjType)
	{
		CGameObj obj;
		obj.SetObjType(OBJ_NPC);
		Assert::AreEqual((int)OBJ_NPC, obj.GetObjType());
	}

	TEST_METHOD(ItemTypeLoadsAndReleases)
	{
		int nLive = 0;
		std::vector<Row> rows = { { 410001, 7 }, { 420002, 9 }, { 130003, 1 } };
		FakeDatabase db(rows, &nLive);
		ItemTypeProbe* pType = ItemTypeProbe::Make();
		Assert::IsTrue(pType->Create(&db));
		Assert::AreEqual(std::string("SELECT * FROM cq_itemtype"), db.m_strSQL);
		Assert::AreEqual(3, nLive);
		CItemTypeData* pData = pType->QueryItemType(420002);
		Assert::IsNotNull(pData);
		Assert::AreEqual((OBJID)420002, pData->GetID());
		Assert::AreEqual(9, pData->GetInt(ITEMTYPEDATA_NAME));
		Assert::IsNull(pType->QueryItemType(999999));
		pType->Release();
		Assert::AreEqual(0, nLive);
	}

	TEST_METHOD(GetInt64ThroughTheRecord)
	{
		FakeRecordset64* pRes = new FakeRecordset64;
		CTestData64* pData = CTestData64::CreateNew();
		Assert::IsTrue(pData->Create(pRes));
		Assert::AreEqual((OBJID)7, pData->GetID());
		Assert::AreEqual(0x123456789LL, pData->GetInt64(TESTDATA64_EXP));
		pData->Release();
		pRes->Release();
	}

	TEST_METHOD(EmptyRecordset)
	{
		int nLive = 0;
		FakeDatabase db(std::vector<Row>(), &nLive);
		ItemTypeProbe* pType = ItemTypeProbe::Make();
		Assert::IsTrue(pType->Create(&db));
		Assert::IsNull(pType->QueryItemType(410001));
		pType->Release();
		Assert::AreEqual(0, nLive);
	}

	TEST_METHOD(OwnerDelObjReleases)
	{
		int nLive = 0;
		std::vector<Row> rows = { { 1, 0 }, { 2, 0 } };
		FakeDatabase db(rows, &nLive);
		IItemTypeSet* pSet = CItemTypeSet::CreateNew(true);
		Assert::IsTrue(pSet->Create("SELECT", &db));
		Assert::IsTrue(pSet->DelObj(1));
		Assert::AreEqual(1, nLive);
		Assert::IsFalse(pSet->DelObj(1));
		pSet->Release();
		Assert::AreEqual(0, nLive);
	}
};
