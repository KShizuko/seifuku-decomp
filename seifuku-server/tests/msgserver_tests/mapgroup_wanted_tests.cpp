#include "CppUnitTest.h"
#include <windows.h>
#include <vector>
#include <string>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/WantedList.h"
#include "I_mydb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct WantedRow { DWORD id; int bounty; };

	class WantedRecord : public IRecord
	{
	public:
		explicit WantedRecord(WantedRow row) : m_row(row) {}
		virtual void	Release()					{ delete this; }
		virtual DWORD	LoadDWord(LPCTSTR)			{ return m_row.id; }
		virtual int		GetInt(int nIdx)			{ return nIdx == DATA_ID ? (int)m_row.id : (nIdx == DATA_BOUNTY ? m_row.bounty : 0); }
		virtual LPCTSTR	GetStr(int)					{ return ""; }
	private:
		WantedRow	m_row;
	};

	class WantedRecordset : public IRecordset
	{
	public:
		explicit WantedRecordset(const std::vector<WantedRow>& rows) : m_rows(rows), m_pos(0) {}
		virtual void		Release()				{ delete this; }
		virtual IRecord*	CreateNewRecord(DWORD)	{ return new WantedRecord(m_rows[m_pos]); }
		virtual UINT		RecordCount()			{ return (UINT)m_rows.size(); }
		virtual bool		MoveNext()				{ return ++m_pos < m_rows.size(); }
	private:
		std::vector<WantedRow>	m_rows;
		size_t					m_pos;
	};

	class WantedDatabase : public IDatabase
	{
	public:
		explicit WantedDatabase(const std::vector<WantedRow>& rows) : m_rows(rows) {}
		virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty)
		{
			m_setSQL.push_back(szSQL);
			m_setNoEmpty.push_back(bNoEmpty);
			return new WantedRecordset(bNoEmpty ? m_rows : std::vector<WantedRow>());
		}
		virtual void		Release()				{}
		std::vector<std::string>	m_setSQL;
		std::vector<bool>			m_setNoEmpty;
	private:
		std::vector<WantedRow>	m_rows;
	};
}

TEST_CLASS(WantedListTests)
{
public:
	TEST_METHOD(LoadOrdersByBounty)
	{
		std::vector<WantedRow> rows = { { 1, 100 }, { 2, 300 }, { 3, 200 }, { 4, 300 } };
		WantedDatabase db(rows);
		CWantedList list;
		Assert::IsTrue(list.Create(&db));
		Assert::AreEqual((size_t)2, db.m_setSQL.size());
		Assert::AreEqual(std::string("SELECT * FROM cq_wanted"), db.m_setSQL[0]);
		Assert::IsTrue(db.m_setNoEmpty[0]);
		Assert::AreEqual(std::string("SELECT * FROM cq_wanted WHERE id=0 LIMIT 1"), db.m_setSQL[1]);
		Assert::IsFalse(db.m_setNoEmpty[1]);

		Assert::AreEqual(4, list.GetWantedAmount());
		const DWORD expected[] = { 4, 2, 3, 1 };
		for(int i = 0; i < 4; i++)
			Assert::AreEqual((int)expected[i], list.GetWantedByIndex(i)->GetInt(DATA_ID));
		list.Release();
	}

	TEST_METHOD(GettersBounds)
	{
		std::vector<WantedRow> rows = { { 7, 500 } };
		WantedDatabase db(rows);
		CWantedList list;
		Assert::IsTrue(list.Create(&db));
		Assert::IsNull(list.GetWantedByIndex(-1));
		Assert::IsNull(list.GetWantedByIndex(1));
		Assert::IsNotNull(list.GetWantedByIndex(0));
		Assert::IsNull(list.GetWanted(ID_NONE));
		Assert::IsNull(list.GetWanted(8));
		Assert::AreEqual(500, list.GetWanted(7)->GetInt(DATA_BOUNTY));
		list.Release();
	}
};
