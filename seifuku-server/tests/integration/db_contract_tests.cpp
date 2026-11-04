#include <afxwin.h>
#include "CppUnitTest.h"
#include "mysql.h"
#include "Mydb.h"
#include <string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const char*	DB_HOST	= "127.0.0.1";
	const char*	DB_USER	= "root";
	const char*	DB_PASS	= "";
	const unsigned int DB_PORT = 3306;

	MYSQL* RawConnect(const char* pszDb)
	{
		MYSQL* h = mysql_init(NULL);
		SfkSetClientOptions(h);
		if(!mysql_real_connect(h, DB_HOST, DB_USER, DB_PASS, pszDb, DB_PORT, NULL, 0))
		{
			char szMsg[512];
			sprintf(szMsg, "MariaDB not reachable (tools\\db.ps1 start): %s", mysql_error(h));
			mysql_close(h);
			wchar_t wsz[512];
			mbstowcs(wsz, szMsg, 511);
			Assert::Fail(wsz);
		}
		return h;
	}

	void Exec(MYSQL* h, const char* pszSQL)
	{
		if(mysql_query(h, pszSQL) != 0)
		{
			wchar_t wsz[512];
			mbstowcs(wsz, mysql_error(h), 511);
			Assert::Fail(wsz);
		}
		MYSQL_RES* res = mysql_store_result(h);
		if(res)
			mysql_free_result(res);
	}

	IDatabase* GameDb(const char* pszDb)
	{
		mysql_close(RawConnect(pszDb));
		IDatabase* pDb = CreateDatabase(DB_HOST, DB_USER, DB_PASS, pszDb, DB_PORT, false);
		Assert::IsNotNull(pDb);
		return pDb;
	}
}

TEST_CLASS(MySqlContractTests)
{
public:
	TEST_CLASS_INITIALIZE(Setup)
	{
		MYSQL* h = RawConnect(NULL);
		Exec(h, "DROP DATABASE IF EXISTS sfk_test");
		Exec(h, "CREATE DATABASE sfk_test DEFAULT CHARACTER SET latin1");
		Exec(h, "CREATE TABLE sfk_test.t (id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, "
				"name VARCHAR(16) NOT NULL DEFAULT '', num INT NOT NULL DEFAULT 0) ENGINE=MyISAM");
		mysql_close(h);
	}

	TEST_CLASS_CLEANUP(Teardown)
	{
		MYSQL* h = RawConnect(NULL);
		Exec(h, "DROP DATABASE IF EXISTS sfk_test");
		mysql_close(h);
	}

	TEST_METHOD(C1_GbkBytesRoundTrip)
	{
		const char szName[] = "\xD5\xF7\xB7\xFE\xB0\x5C'x";
		char szEsc[64];
		unsigned long n = mysql_escape_string(szEsc, szName, (unsigned long)strlen(szName));
		Assert::AreEqual(0, strcmp(szEsc, "\xD5\xF7\xB7\xFE\xB0\\\\\\'x"));
		Assert::AreEqual((unsigned long)strlen(szEsc), n);

		IDatabase* pDb = GameDb("sfk_test");
		char szSQL[256];
		sprintf(szSQL, "INSERT t SET name='%s', num=7", szEsc);
		Assert::IsTrue(pDb->ExecuteSQL(szSQL));
		int id = pDb->GetInsertId();
		Assert::IsTrue(id > 0);
		sprintf(szSQL, "SELECT * FROM t WHERE id=%d", id);
		IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
		Assert::IsNotNull(pRes);
		Assert::AreEqual(1u, pRes->RecordCount());
		char szBack[32];
		pRes->LoadString(szBack, "name", sizeof(szBack));
		Assert::AreEqual(0, memcmp(szBack, szName, sizeof(szName)));
		Assert::AreEqual(7, pRes->LoadInt("num"));
		pRes->Release();
		pDb->Release();
	}

	TEST_METHOD(C2_NonStrictSqlMode)
	{
		IDatabase* pDb = GameDb("sfk_test");
		Assert::IsTrue(pDb->ExecuteSQL((char*)"INSERT t SET name='0123456789abcdefXYZ', num=''"));
		int id = pDb->GetInsertId();
		char szSQL[128];
		sprintf(szSQL, "SELECT * FROM t WHERE id=%d", id);
		IRecordset* pRes = pDb->CreateNewRecordset(szSQL, false);
		Assert::IsNotNull(pRes);
		char szBack[32];
		pRes->LoadString(szBack, "name", sizeof(szBack));
		Assert::AreEqual("0123456789abcdef", szBack);
		Assert::AreEqual(0, pRes->LoadInt("num"));
		pRes->Release();
		pDb->Release();
	}

	TEST_METHOD(C3_ReconnectKeepsSettings)
	{
		MYSQL* h = RawConnect("sfk_test");
		unsigned long idThread = mysql_thread_id(h);
		MYSQL* hKiller = RawConnect(NULL);
		char szSQL[64];
		sprintf(szSQL, "KILL %lu", idThread);
		Exec(hKiller, szSQL);
		mysql_close(hKiller);
		Assert::AreEqual(0, mysql_query(h, "SELECT @@session.sql_mode, @@session.character_set_client"));
		MYSQL_RES* res = mysql_store_result(h);
		Assert::IsNotNull(res);
		MYSQL_ROW row = mysql_fetch_row(res);
		Assert::AreEqual("", row[0]);
		Assert::AreEqual("latin1", row[1]);
		Assert::AreNotEqual(idThread, mysql_thread_id(h));
		mysql_free_result(res);
		mysql_close(h);
	}

	TEST_METHOD(C5_CqColumnTypesAndLengths)
	{
		MYSQL* h = RawConnect("cq_merged");
		Assert::AreEqual(0, mysql_query(h, "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_SCHEMA='cq_merged'"));
		MYSQL_RES* tables = mysql_store_result(h);
		Assert::IsNotNull(tables);
		int nColumns = 0;
		MYSQL_ROW trow;
		while((trow = mysql_fetch_row(tables)) != NULL)
		{
			MYSQL_RES* f = mysql_list_fields(h, trow[0], NULL);
			Assert::IsNotNull(f);
			MYSQL_FIELD* pField;
			while((pField = mysql_fetch_field(f)) != NULL)
			{
				nColumns++;
				bool bKnown = pField->type == 1 || pField->type == 2 || pField->type == 3 || pField->type == 4
						|| pField->type == 5 || pField->type == 8 || pField->type == 253 || pField->type == 254;
				std::string strCol = std::string(trow[0]) + "." + pField->name;
				if(strCol == "cardbalance.countDate" || strCol == "cq_pk_item.manhunt_time" || strCol == "salelog.ConsumptionTime")
					continue;
				if(!bKnown)
				{
					wchar_t wsz[256];
					swprintf(wsz, 256, L"%S.%S: type %d", trow[0], pField->name, (int)pField->type);
					Assert::Fail(wsz);
				}
			}
			mysql_free_result(f);
		}
		mysql_free_result(tables);
		Assert::IsTrue(nColumns > 1000);
		MYSQL_RES* f = mysql_list_fields(h, "cq_user", "name");
		if(f)
		{
			MYSQL_FIELD* pField = mysql_fetch_field(f);
			Assert::IsNotNull(pField);
			Assert::AreEqual(15ul, (unsigned long)pField->length);
			mysql_free_result(f);
		}
		mysql_close(h);
	}

	TEST_METHOD(C7_DmlAndSeek)
	{
		MYSQL* h = RawConnect("sfk_test");
		Assert::AreEqual(0, mysql_query(h, "UPDATE t SET num=num+1 WHERE id=1 LIMIT 1"));
		Assert::IsNull(mysql_store_result(h));
		Assert::AreEqual(0u, mysql_field_count(h));
		Exec(h, "INSERT t SET name='a'");
		Exec(h, "INSERT t SET name='b'");
		Assert::AreEqual(0, mysql_query(h, "SELECT name FROM t ORDER BY id"));
		MYSQL_RES* res = mysql_store_result(h);
		MYSQL_ROW r0 = mysql_fetch_row(res);
		char szFirst[32];
		strcpy(szFirst, r0[0]);
		mysql_fetch_row(res);
		mysql_data_seek(res, 0);
		Assert::AreEqual(szFirst, mysql_fetch_row(res)[0]);
		mysql_free_result(res);
		mysql_close(h);
	}

	TEST_METHOD(C8_ListFieldsDefaults)
	{
		MYSQL* h = RawConnect("sfk_test");
		MYSQL_RES* f = mysql_list_fields(h, "t", NULL);
		Assert::IsNotNull(f);
		MYSQL_FIELD* pId = mysql_fetch_field(f);
		MYSQL_FIELD* pName = mysql_fetch_field(f);
		MYSQL_FIELD* pNum = mysql_fetch_field(f);
		Assert::IsTrue((pId->flags & PRI_KEY_FLAG) != 0 && (pId->flags & AUTO_INCREMENT_FLAG) != 0 && (pId->flags & UNSIGNED_FLAG) != 0);
		Assert::IsNotNull(pNum->def);
		Assert::AreEqual("0", pNum->def);
		Assert::IsNotNull(pName->def);
		Assert::AreEqual("", pName->def);
		mysql_free_result(f);
		mysql_close(h);
	}
};
