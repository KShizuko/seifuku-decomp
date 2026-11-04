#include "CppUnitTest.h"
#include <windows.h>
#include <vector>
#include <string>
#include "define.h"
#include "../../src/MsgServer/MapGroupKernel/GMToolCfg.h"
#include "I_mydb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct CfgRow { int idMap, x, y, cx, cy; };

	class CfgRecordset : public IRecordset
	{
	public:
		explicit CfgRecordset(const std::vector<CfgRow>& rows) : m_rows(rows), m_pos(0) {}
		virtual void		Release()				{ delete this; }
		virtual UINT		RecordCount()			{ return (UINT)m_rows.size(); }
		virtual bool		MoveNext()				{ return ++m_pos < m_rows.size(); }
		virtual int			GetInt(int nIdx)
		{
			const CfgRow& r = m_rows[m_pos];
			switch(nIdx) { case 2: return r.idMap; case 3: return r.x; case 4: return r.y; case 5: return r.cx; case 6: return r.cy; }
			return 0;
		}
	private:
		std::vector<CfgRow>	m_rows;
		size_t				m_pos;
	};

	class CfgDatabase : public IDatabase
	{
	public:
		explicit CfgDatabase(const std::vector<CfgRow>& rows) : m_rows(rows) {}
		virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty)
		{ m_strSQL = szSQL; m_bNoEmpty = bNoEmpty; return new CfgRecordset(m_rows); }
		virtual void		Release()				{}
		std::string			m_strSQL;
		bool				m_bNoEmpty = false;
	private:
		std::vector<CfgRow>	m_rows;
	};
}

TEST_CLASS(GMToolCfgTests)
{
public:
	TEST_METHOD(RegionsFromConfig)
	{
		std::vector<CfgRow> rows = { { 1002, 100, 200, 50, 40 }, { 1000, 0, 0, 10, 10 } };
		CfgDatabase db(rows);
		CGMToolCfg* pCfg = CGMToolCfg::CreateNew(&db);
		Assert::IsNotNull(pCfg);
		Assert::AreEqual(std::string("SELECT * FROM cq_config where type between 1020 and 1029"), db.m_strSQL);
		Assert::IsTrue(db.m_bNoEmpty);

		POINT inside = { 120, 210 }, rightEdge = { 150, 210 }, bottomEdge = { 120, 240 }, origin = { 0, 0 };
		Assert::IsTrue(pCfg->IsInRegion(1002, inside));
		Assert::IsFalse(pCfg->IsInRegion(1002, rightEdge));
		Assert::IsFalse(pCfg->IsInRegion(1002, bottomEdge));
		Assert::IsFalse(pCfg->IsInRegion(1001, inside));
		Assert::IsTrue(pCfg->IsInRegion(1000, origin));
		pCfg->Release();
	}

	TEST_METHOD(EmptyConfig)
	{
		CfgDatabase db((std::vector<CfgRow>()));
		CGMToolCfg* pCfg = CGMToolCfg::CreateNew(&db);
		Assert::IsNotNull(pCfg);
		POINT pos = { 1, 1 };
		Assert::IsFalse(pCfg->IsInRegion(1002, pos));
		pCfg->Release();
	}
};
