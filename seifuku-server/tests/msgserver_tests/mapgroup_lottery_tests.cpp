#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/Lottery/Src/ConfigMgr.h"
#include "../../src/MsgServer/Lottery/Src/Lottery.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_lottery_tests
{
	static ST_LOTTERYCONFIG Row(OBJID id, DWORD dwType, int nData1, int nData2, int nData3)
	{
		ST_LOTTERYCONFIG info;
		info.id = id;	info.dwType = dwType;	info.nData1 = nData1;	info.nData2 = nData2;	info.nData3 = nData3;
		return info;
	}

	TEST_CLASS(LotteryConfigSetTests)
	{
	public:
		TEST_METHOD(GuaranteeGivesBothTypesInOrderAndTheChancesSum)
		{
			CLotteryConfigSet set;
			ST_LOTTERYCONFIG rows[] = { Row(1, 11001, 1, 100, 0), Row(2, 10001, 1, 6, 30), Row(3, 11002, 1, 400, 0),
										Row(4, 10000, 1, 4, 200), Row(5, 11005, 2, 999, 0) };
			for (int i = 0; i < 5; i++)
				set.AddConfig(&rows[i]);

			DWORD setInfo[3] = { 0, 0, 0 };
			Assert::IsTrue(set.GetGuarantee(1, setInfo));
			Assert::AreEqual((DWORD)10000, setInfo[0]);
			Assert::AreEqual((DWORD)10001, setInfo[1]);
			Assert::AreEqual((DWORD)500, setInfo[2]);
			Assert::AreEqual(200, set.GetConfig(10000)->nData3);
			Assert::AreEqual(6, set.GetConfig(10001)->nData2);
			Assert::IsNull(set.GetConfig(10002));
		}

		TEST_METHOD(GuaranteeFailsWithoutBothRowsOrWithoutChances)
		{
			CLotteryConfigSet oneGuarantee;
			ST_LOTTERYCONFIG a[] = { Row(1, 10000, 1, 4, 200), Row(2, 11001, 1, 100, 0) };
			for (int i = 0; i < 2; i++)
				oneGuarantee.AddConfig(&a[i]);
			DWORD setInfo[3];
			Assert::IsFalse(oneGuarantee.GetGuarantee(1, setInfo));

			CLotteryConfigSet noChance;
			ST_LOTTERYCONFIG b[] = { Row(1, 10000, 1, 4, 200), Row(2, 10001, 1, 6, 30) };
			for (int i = 0; i < 2; i++)
				noChance.AddConfig(&b[i]);
			Assert::IsFalse(noChance.GetGuarantee(1, setInfo));

			CLotteryConfigSet empty;
			Assert::IsFalse(empty.GetGuarantee(1, setInfo));
		}

		TEST_METHOD(RollFallsInTheRunningSumOfItsLotteryOnly)
		{
			CLotteryConfigSet set;
			ST_LOTTERYCONFIG rows[] = { Row(1, 11002, 1, 400, 0), Row(2, 11001, 1, 100, 0), Row(3, 11003, 2, 50, 0) };
			for (int i = 0; i < 3; i++)
				set.AddConfig(&rows[i]);

			Assert::AreEqual((DWORD)11001, set.RollType(1, 0));
			Assert::AreEqual((DWORD)11001, set.RollType(1, 99));
			Assert::AreEqual((DWORD)11002, set.RollType(1, 100));
			Assert::AreEqual((DWORD)11002, set.RollType(1, 499));
			Assert::AreEqual((DWORD)0, set.RollType(1, 500));
			Assert::AreEqual((DWORD)11003, set.RollType(2, 0));
			Assert::AreEqual((DWORD)0, set.RollType(3, 0));
			Assert::AreEqual(1, (int)(set.RollType(1, 50) % 10));
		}

		TEST_METHOD(LaterRowOfTheSameTypeReplacesTheEarlier)
		{
			CLotteryConfigSet set;
			ST_LOTTERYCONFIG a = Row(1, 11001, 1, 100, 0), b = Row(2, 11001, 1, 300, 0);
			set.AddConfig(&a);
			set.AddConfig(&b);
			Assert::AreEqual(300, set.GetConfig(11001)->nData2);
		}
	};

	TEST_CLASS(LotteryPrizeTests)
	{
	public:
		TEST_METHOD(CreateNewCopiesTheRow)
		{
			ST_LOTTERY info;
			info.id = 7;	info.nType = 1;		info.nRank = 3;		info.nChance = 70;	info.strPrizeName = "SynthPrize";
			info.idPrizeItem = 123456;	info.nColor = 4;	info.nHoleNum = 2;	info.nAdditionLev = 5;
			CLottery* pLottery = CLottery::CreateNew(&info);
			Assert::IsNotNull(pLottery);
			Assert::AreEqual((OBJID)7, pLottery->GetID());
			Assert::AreEqual(1, pLottery->GetType());
			Assert::AreEqual(3, pLottery->GetRank());
			Assert::AreEqual(70, pLottery->GetChance());
			Assert::AreEqual("SynthPrize", pLottery->GetPrizeName());
			Assert::AreEqual((OBJID)123456, pLottery->GetPrizeItem());
			Assert::AreEqual(4, pLottery->GetColor());
			Assert::AreEqual(2, pLottery->GetHoleNum());
			Assert::AreEqual(5, pLottery->GetAdditionLev());
			pLottery->Release();
		}
	};
}
