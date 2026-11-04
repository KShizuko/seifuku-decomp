#include "CppUnitTest.h"
#include <windows.h>
#include "Fact.h"
#include "SymbolTable.h"
#include "FactSet.h"
#include "RuleSet.h"
#include "AiCenter.h"
#include "Syntax.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(SyntaxTests)
{
public:
	TEST_METHOD(AnalyzeFactReadsKeyAndDataParams)
	{
		CSymbolTable* pTable = CSymbolTable::CreateNew();
		pTable->Create(1000001);
		CFact fact, pattern;
		Assert::IsTrue(AnalyzeFact(pTable, String("!talk< who , 12 >( -3, \"a b\" )"), &fact, &pattern));
		Assert::IsTrue(fact.IsLogicNot());
		Assert::AreEqual((OBJID)1000001, fact.GetType());
		VarTypeSetRef set = fact.GetParamSet();
		Assert::AreEqual(4, set.Size());
		Assert::IsTrue(set[0].IsSymbolType());
		Assert::AreEqual("who", (LPCTSTR)static_cast<String>(set[0]));
		Assert::AreEqual(12, static_cast<int>(set[1]));
		Assert::AreEqual(-3, static_cast<int>(set[2]));
		Assert::IsTrue(set[3].IsStringType());
		Assert::AreEqual("a b", (LPCTSTR)static_cast<String>(set[3]));
		Assert::IsTrue(pattern.IsLogicNot());
		Assert::AreEqual(4, pattern.GetParamSet().Size());
		Assert::IsTrue(pattern.GetParamSet()[1].IsIntType());
		Assert::IsFalse(pattern.GetParamSet()[2].IsValid());
		Assert::IsFalse(pattern.GetParamSet()[3].IsValid());
		pTable->Release();
	}

	TEST_METHOD(AnalyzeFactSetSplitsFacts)
	{
		CSymbolTable* pTable = CSymbolTable::CreateNew();
		pTable->Create(1000001);
		CFactArray setFacts;
		Assert::IsTrue(AnalyzeFactSet(pTable, String("a<x,y>, !b(1,\"p,q\"), c"), &setFacts));
		Assert::AreEqual(3, setFacts.Size());
		Assert::AreEqual(2, setFacts[0].GetParamSet().Size());
		Assert::IsTrue(setFacts[1].IsLogicNot());
		Assert::AreEqual("p,q", (LPCTSTR)static_cast<String>(setFacts[1].GetParamSet()[1]));
		Assert::AreEqual(0, setFacts[2].GetParamSet().Size());
		Assert::AreEqual((OBJID)1000003, setFacts[2].GetType());
		pTable->Release();
	}

	TEST_METHOD(MatchParamBindsSymbols)
	{
		VarTypeSet setFact;
		setFact.Push(VarType(5));
		setFact.Push(VarType(5));
		setFact.Push(VarType(9));
		VarTypeSet setRule;
		setRule.Push(VarType("x", VarType::TYPE_SYMBOL));
		setRule.Push(VarType("x", VarType::TYPE_SYMBOL));
		setRule.Push(VarType("_", VarType::TYPE_SYMBOL));
		ARGUMENT_SET setArg;
		Assert::IsTrue(IAiCenter::MatchParam(setFact, setRule, &setArg));
		Assert::AreEqual(5, static_cast<int>(setArg[String("x")]));

		VarTypeSet setOther;
		setOther.Push(VarType(5));
		setOther.Push(VarType(6));
		setOther.Push(VarType(9));
		ARGUMENT_SET setArg2;
		Assert::IsFalse(IAiCenter::MatchParam(setOther, setRule, &setArg2));

		VarTypeSet setConst;
		setConst.Push(VarType(4));
		ARGUMENT_SET setArg3;
		Assert::IsFalse(IAiCenter::MatchParam(setConst, setFact, &setArg3));
	}

	TEST_METHOD(ReplaceSymbolAndGetValue)
	{
		CSymbolTable* pTable = CSymbolTable::CreateNew();
		pTable->Create(1000001);
		CFact fact;
		Assert::IsTrue(AnalyzeFact(pTable, String("go<x, 7>"), &fact));
		ARGUMENT_SET setArg;
		setArg.Add(String("x"), VarType(42));
		Assert::IsTrue(ReplaceSymbol(&fact, setArg));
		Assert::AreEqual(42, static_cast<int>(fact.GetParamSet()[0]));
		Assert::AreEqual(7, static_cast<int>(fact.GetParamSet()[1]));
		Assert::AreEqual(42, static_cast<int>(IAiCenter::GetValue(VarType("x", VarType::TYPE_SYMBOL), &setArg)));
		Assert::AreEqual(7, static_cast<int>(IAiCenter::GetValue(VarType(7), &setArg)));
		pTable->Release();
	}

	TEST_METHOD(FactStringRoundTrip)
	{
		CSymbolTable* pTable = CSymbolTable::CreateNew();
		pTable->Create(1000001);
		CFactObj* pObj = CFactObj::CreateNew();
		Assert::IsTrue(pObj->Create(pTable, 1000001, (OBJID)0, String("see<a, 3, \"b\">"), 5, 100, 50));
		String str;
		pObj->GetFactString(str, pTable);
		Assert::AreEqual("see<a,3,b>", (LPCTSTR)str);
		Assert::IsTrue(pObj->IsAlive(149));
		Assert::IsFalse(pObj->IsAlive(150));
		pObj->Release();
		pTable->Release();
	}
};
