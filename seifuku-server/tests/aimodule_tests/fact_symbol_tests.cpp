#include "CppUnitTest.h"
#include <windows.h>
#include "Fact.h"
#include "SymbolTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(FactSymbolTests)
{
public:
	TEST_METHOD(FactCreateAndDump)
	{
		CFact empty;
		Assert::IsFalse(empty.IsValid());
		Assert::IsFalse(empty.IsLogicNot());

		VarTypeSet setParam;
		setParam.Push(VarType(7));
		setParam.Push(VarType("abc"));
		setParam.Push(VarType());
		CFact fact(true, 1000005, setParam);
		Assert::IsTrue(fact.IsValid());
		Assert::IsTrue(fact.IsLogicNot());
		Assert::AreEqual((OBJID)1000005, fact.GetType());
		Assert::AreEqual(3, fact.GetParamSet().Size());
		Assert::AreEqual("!1000005(7,abc,(null))", (LPCTSTR)fact.Dump());
	}

	TEST_METHOD(SymbolTableSpawnsIdsInOrder)
	{
		CSymbolTable* pTable = CSymbolTable::CreateNew();
		pTable->Create(1000001);
		Assert::AreEqual((OBJID)1000001, pTable->GetSymbolID(String("alpha")));
		Assert::AreEqual((OBJID)1000002, pTable->GetSymbolID(String("beta")));
		Assert::AreEqual((OBJID)1000001, pTable->GetSymbolID(String("alpha")));
		Assert::IsTrue(pTable->AddSymbol(String("less"), 1004));
		Assert::AreEqual((OBJID)1004, pTable->GetSymbolID(String("less")));
		Assert::AreEqual("beta", pTable->FindSymbolByID(1000002));
		Assert::IsNull(pTable->FindSymbolByID(1000003));
		Assert::AreEqual((OBJID)1000003, pTable->GetSymbolID(String("gamma")));
		pTable->Release();
	}
};
