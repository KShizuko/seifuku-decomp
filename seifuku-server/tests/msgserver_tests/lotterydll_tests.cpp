#include "CppUnitTest.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include "I_mydb.h"
#include "I_Adapter.h"
#include "../../src/MsgServer/Lottery/Src/Lottery.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace lotterydll_tests
{
	typedef bool		(__fastcall* PFN_LOTTERY)		(void* pThis, void* pEdx, int nLotteryType, int nColor, ILotteryActor* pActor, bool bTest);
	typedef bool		(__fastcall* PFN_RANK)			(void* pThis, void* pEdx, int nLotteryType, ILotteryActor* pActor, int* pRank, int* pBigRank);
	typedef CLottery*	(__fastcall* PFN_GETLOTTERY)	(void* pThis, void* pEdx, int nLotteryType, int nRank, int nColor);
	typedef bool		(__fastcall* PFN_AWARD)			(void* pThis, void* pEdx, CLottery* pLottery, ILotteryActor* pActor);
	typedef int			(__fastcall* PFN_SUM)			(void* pThis, void* pEdx, int nLotteryType, int nRank);
	typedef CLottery*	(__fastcall* PFN_BYROLL)		(void* pThis, void* pEdx, int nLotteryType, int nRank, int nRoll);

	void** Vtbl(void* p)	{ return *(void***)p; }

	typedef std::vector<std::vector<std::string> >	ROWS;

	class CFakeRecordset : public IRecordset
	{
	public:
		CFakeRecordset(const ROWS& rows, int* pReleased) : m_rows(rows), m_nPos(0), m_pReleased(pReleased) {}
		virtual void		Release()				{ (*m_pReleased)++; delete this; }
		virtual UINT		RecordCount()			{ return (UINT)m_rows.size(); }
		virtual bool		MoveNext()				{ return ++m_nPos < m_rows.size(); }
		virtual int			GetInt(int nIdx)		{ return atoi(m_rows[m_nPos][nIdx].c_str()); }
		virtual LPCTSTR		GetStr(int nIdx)		{ return m_rows[m_nPos][nIdx].c_str(); }
	private:
		ROWS		m_rows;
		size_t		m_nPos;
		int*		m_pReleased;
	};

	class CFakeDatabase : public IDatabase
	{
	public:
		CFakeDatabase() : m_nReleased(0) {}
		virtual IRecordset*	CreateNewRecordset(LPCTSTR szSQL, bool bNoEmpty)
		{
			m_setSQL.push_back(szSQL);
			m_setNoEmpty.push_back(bNoEmpty);
			if (strstr(szSQL, "cq_config"))
				return new CFakeRecordset(m_rowsConfig, &m_nReleased);
			if (strstr(szSQL, "cq_lottery"))
				return new CFakeRecordset(m_rowsLottery, &m_nReleased);
			return NULL;
		}
		virtual void		Release()				{}
		ROWS						m_rowsConfig, m_rowsLottery;
		std::vector<std::string>	m_setSQL;
		std::vector<bool>			m_setNoEmpty;
		int							m_nReleased;
	};

	class CRegisterProbe : public IRegister
	{
	public:
		CRegisterProbe() : m_idModule(0), m_pModule(NULL) {}
		virtual bool	Register(int idModule, void* pModule)	{ m_idModule = idModule; m_pModule = pModule; return true; }
		int		m_idModule;
		void*	m_pModule;
	};

	class CPrizeProbe : public ILotteryPrize
	{
	public:
		CPrizeProbe() : m_bEquip(true) {}
		virtual OBJID	GetID		()						{ m_setSlot.push_back(0); return 9001; }
		virtual bool	IsEquip		()						{ m_setSlot.push_back(1); return m_bEquip; }
		virtual bool	SetInt		(int nIdx, int nData)	{ m_setSlot.push_back(2); m_setInt.push_back(nIdx); m_setInt.push_back(nData); return true; }
		virtual bool	GetInfo		(PLUGIN_ITEMINFO* pInfo)
		{
			m_setSlot.push_back(3);
			memset(pInfo, 0, sizeof(*pInfo));
			pInfo->id = 9001;	pInfo->idType = 410001;	pInfo->nAmount = 5000;	pInfo->nAmountLimit = 6000;	pInfo->nGem1 = 1;
			return true;
		}
		virtual USHORT	Release		()						{ m_setSlot.push_back(4); return 0; }
		virtual ~CPrizeProbe()								{}
		bool				m_bEquip;
		std::vector<int>	m_setSlot;
		std::vector<int>	m_setInt;
	};

	class CActorProbe : public ILotteryActor
	{
	public:
		CActorProbe() : m_nBigFails(0), m_nSmallFails(0), m_pPrize(NULL), m_nItemType(0) {}
		virtual USHORT	Release				()						{ m_setSlot.push_back(0); return 0; }
		virtual OBJID	GetID				()						{ m_setSlot.push_back(1); return 1000001; }
		virtual LPCTSTR	GetName				()						{ m_setSlot.push_back(2); return "Tester"; }
		virtual OBJID	GetMapID			()						{ m_setSlot.push_back(3); return 1002; }
		virtual DWORD	GetLev				()						{ m_setSlot.push_back(4); return 70; }
		virtual __int64	GetExp				()						{ m_setSlot.push_back(5); return 0; }
		virtual int		GetBigPrizeFails	()						{ m_setSlot.push_back(6); return m_nBigFails; }
		virtual void	SetBigPrizeFails	(int nData, bool bUpdate)	{ m_setSlot.push_back(7); m_setSet.push_back(nData); m_setSet.push_back(bUpdate); }
		virtual int		GetSmallPrizeFails	()						{ m_setSlot.push_back(8); return m_nSmallFails; }
		virtual void	SetSmallPrizeFails	(int nData, bool bUpdate)	{ m_setSlot.push_back(9); m_setSet.push_back(nData); m_setSet.push_back(bUpdate); }
		virtual ILotteryPrize*	AwardItem	(int nItemType)			{ m_setSlot.push_back(10); m_nItemType = nItemType; return m_pPrize; }
		virtual bool	SendSysMsg			(const char* fmt, ...)	{ m_setSlot.push_back(11); m_setText.push_back(Format(fmt, (va_list)(&fmt + 1))); return true; }
		virtual bool	SendMsg				(PLUGIN_MSG* pMsg)		{ m_setSlot.push_back(12); Keep(pMsg); return true; }
		virtual bool	BroadcastMsg		(const char* fmt, ...)	{ m_setSlot.push_back(13); m_setText.push_back(Format(fmt, (va_list)(&fmt + 1))); return true; }
		virtual bool	BroadcastRoomMsg	(PLUGIN_MSG* pMsg)		{ m_setSlot.push_back(14); Keep(pMsg); return true; }
		virtual ~CActorProbe()										{}

		static std::string Format(const char* fmt, va_list args)	{ char buf[1024]; vsprintf(buf, fmt, args); return buf; }
		void Keep(PLUGIN_MSG* pMsg)		{ m_setMsg.push_back(std::string((const char*)&pMsg->unMsgSize, pMsg->unMsgSize)); }

		int							m_nBigFails, m_nSmallFails;
		CPrizeProbe*				m_pPrize;
		int							m_nItemType;
		std::vector<int>			m_setSlot;
		std::vector<int>			m_setSet;
		std::vector<std::string>	m_setText;
		std::vector<std::string>	m_setMsg;
	};

	HMODULE			s_hLottery	= NULL;
	CFakeDatabase*	s_pDb		= NULL;
	CRegisterProbe	s_register;
	bool			s_bCreated	= false;
	char			s_szOldDir[MAX_PATH];

	std::vector<std::string> Row(const char* a, const char* b, const char* c, const char* d, const char* e, const char* f,
								const char* g, const char* h, const char* i)
	{
		const char* cols[] = { a, b, c, d, e, f, g, h, i };
		return std::vector<std::string>(cols, cols + 9);
	}

	OBJID IdOf(CLottery* p)			{ return p ? ((ILottery*)p)->GetID() : ID_NONE; }

	TEST_CLASS(LotteryDllTests)
	{
	public:
		TEST_CLASS_INITIALIZE(LoadTheDll)
		{
			::GetCurrentDirectoryA(sizeof(s_szOldDir), s_szOldDir);
			char szDir[MAX_PATH];
			strcpy(szDir, __FILE__);
			strcpy(strrchr(szDir, '\\') + 1, "fixtures\\adapter");
			::SetCurrentDirectoryA(szDir);

			HMODULE hSelf = NULL;
			::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&Vtbl, &hSelf);
			char szPath[MAX_PATH];
			::GetModuleFileNameA(hSelf, szPath, sizeof(szPath));
			strcpy(strrchr(szPath, '\\') + 1, "Lottery.dll");
			s_hLottery = ::LoadLibraryA(szPath);
			if (!s_hLottery)
				return;

			s_pDb = new CFakeDatabase;
			s_pDb->m_rowsConfig.push_back(Row("1", "10000", "1", "4", "200", "0", "0", "big", ""));
			s_pDb->m_rowsConfig.push_back(Row("2", "10001", "1", "6", "30", "0", "0", "small", ""));
			s_pDb->m_rowsConfig.push_back(Row("3", "11001", "1", "100", "0", "0", "0", "chance", ""));
			s_pDb->m_rowsConfig.push_back(Row("4", "10000", "2", "4", "200", "0", "0", "other", ""));
			s_pDb->m_rowsLottery.push_back(Row("101", "1", "4", "10", "Alpha", "410001", "3", "1", "2"));
			s_pDb->m_rowsLottery.push_back(Row("102", "1", "4", "20", "Beta", "410002", "4", "0", "0"));
			s_pDb->m_rowsLottery.push_back(Row("103", "1", "6", "5", "Gamma", "410003", "0", "2", "1"));
			s_pDb->m_rowsLottery.push_back(Row("201", "2", "4", "50", "Delta", "410004", "0", "0", "0"));

			MODULE_CREATE funcModuleCreate = (MODULE_CREATE)::GetProcAddress(s_hLottery, "ModuleCreate");
			if (funcModuleCreate)
				s_bCreated = funcModuleCreate(&s_register, (IDatabase*)s_pDb);
		}

		TEST_CLASS_CLEANUP(FreeTheDll)
		{
			if (s_hLottery)
				::FreeLibrary(s_hLottery);
			delete s_pDb;
			::SetCurrentDirectoryA(s_szOldDir);
		}

		void* Mgr()
		{
			Assert::IsNotNull(s_hLottery, L"Lottery.dll is not beside the test DLL (built by the solution)");
			Assert::IsTrue(s_bCreated, L"ModuleCreate failed");
			return s_register.m_pModule;
		}

		TEST_METHOD(ModuleCreateLoadsBothTablesAndRegisters)
		{
			Mgr();
			Assert::AreEqual((size_t)2, s_pDb->m_setSQL.size());
			Assert::AreEqual(std::string("SELECT * FROM cq_config where type between 10000 and 11008"), s_pDb->m_setSQL[0]);
			Assert::AreEqual(std::string("SELECT * FROM cq_lottery"), s_pDb->m_setSQL[1]);
			Assert::IsTrue(s_pDb->m_setNoEmpty[0] && s_pDb->m_setNoEmpty[1]);
			Assert::AreEqual(2, s_pDb->m_nReleased);
			Assert::AreEqual(ADAPTER_MODULE_LOTTERY, s_register.m_idModule);
			Assert::IsNotNull(s_register.m_pModule);
		}

		TEST_METHOD(SumChanceAndGetLotteryByRollSlots)
		{
			void* p = Mgr();
			void** v = Vtbl(p);
			Assert::AreEqual(30, ((PFN_SUM)v[6])(p, NULL, 1, 4));
			Assert::AreEqual(5, ((PFN_SUM)v[6])(p, NULL, 1, 6));
			Assert::AreEqual(0, ((PFN_SUM)v[6])(p, NULL, 1, 5));
			Assert::AreEqual((OBJID)101, IdOf(((PFN_BYROLL)v[7])(p, NULL, 1, 4, 9)));
			Assert::AreEqual((OBJID)102, IdOf(((PFN_BYROLL)v[7])(p, NULL, 1, 4, 10)));
			Assert::AreEqual((OBJID)102, IdOf(((PFN_BYROLL)v[7])(p, NULL, 1, 4, 29)));
			Assert::IsNull(((PFN_BYROLL)v[7])(p, NULL, 1, 4, 30));
			Assert::AreEqual((OBJID)201, IdOf(((PFN_BYROLL)v[7])(p, NULL, 2, 4, 0)));
		}

		TEST_METHOD(GetLotterySlot)
		{
			void* p = Mgr();
			void** v = Vtbl(p);
			Assert::AreEqual((OBJID)103, IdOf(((PFN_GETLOTTERY)v[3])(p, NULL, 1, 6, 0)));
			Assert::IsNull(((PFN_GETLOTTERY)v[3])(p, NULL, 1, 5, 0));
		}

		TEST_METHOD(GetLotteryRankSlot)
		{
			void* p = Mgr();
			CActorProbe actor;
			actor.m_nBigFails = 200;
			int nRank = -1, nBigRank = -1;
			Assert::IsTrue(((PFN_RANK)Vtbl(p)[2])(p, NULL, 1, &actor, &nRank, &nBigRank));
			Assert::AreEqual(4, nRank);
			Assert::AreEqual(6, nBigRank);
			int nSlots[] = { 6, 8, 7, 9 };
			Assert::AreEqual((size_t)4, actor.m_setSlot.size());
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(nSlots[i], actor.m_setSlot[i]);
			int nSet[] = { 0, 0, 0, 1 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(nSet[i], actor.m_setSet[i]);
		}

		TEST_METHOD(AwardPrizeSlot)
		{
			void* p = Mgr();
			void** v = Vtbl(p);
			CLottery* pAlpha = ((PFN_BYROLL)v[7])(p, NULL, 1, 4, 0);
			Assert::AreEqual((OBJID)101, IdOf(pAlpha));

			CActorProbe actor;
			CPrizeProbe prize;
			actor.m_pPrize = &prize;
			Assert::IsTrue(((PFN_AWARD)v[4])(p, NULL, pAlpha, &actor));
			Assert::AreEqual(410001, actor.m_nItemType);
			int nPrizeSlots[] = { 1, 2, 2, 3 };
			Assert::AreEqual((size_t)4, prize.m_setSlot.size());
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(nPrizeSlots[i], prize.m_setSlot[i]);
			int nInt[] = { 0, 1, 1, 2 };
			for (int i = 0; i < 4; i++)
				Assert::AreEqual(nInt[i], prize.m_setInt[i]);
			Assert::AreEqual((size_t)2, actor.m_setSlot.size());
			Assert::AreEqual(10, actor.m_setSlot[0]);
			Assert::AreEqual(12, actor.m_setSlot[1]);
			const std::string& msg = actor.m_setMsg[0];
			Assert::AreEqual((size_t)0x20, msg.size());
			Assert::AreEqual((USHORT)1008, *(USHORT*)&msg[2]);
			Assert::AreEqual((OBJID)9001, *(OBJID*)&msg[4]);
			Assert::AreEqual((DWORD)410001, *(DWORD*)&msg[8]);
			Assert::AreEqual(3, (int)(UCHAR)msg[0x10]);

			CActorProbe noItem;
			Assert::IsFalse(((PFN_AWARD)v[4])(p, NULL, pAlpha, &noItem));
			Assert::AreEqual((size_t)1, noItem.m_setSlot.size());
		}

		TEST_METHOD(LotterySlot)
		{
			void* p = Mgr();
			PFN_LOTTERY pfn = (PFN_LOTTERY)Vtbl(p)[0];

			CActorProbe test;
			test.m_nBigFails = 200;
			test.m_pPrize = NULL;
			Assert::IsTrue(pfn(p, NULL, 1, 0, &test, true));
			Assert::AreEqual((size_t)4, test.m_setSlot.size(), L"a test draw called past the rank");

			CActorProbe actor;
			CPrizeProbe prize;
			actor.m_nBigFails = 200;
			actor.m_pPrize = &prize;
			Assert::IsTrue(pfn(p, NULL, 1, 0, &actor, false));
			Assert::IsTrue(actor.m_nItemType == 410001 || actor.m_nItemType == 410002);
			int nSlots[] = { 6, 8, 7, 9, 10, 12, 2, 13, 11, 1, 14, 2, 1 };
			Assert::AreEqual((size_t)13, actor.m_setSlot.size());
			for (int i = 0; i < 13; i++)
				Assert::AreEqual(nSlots[i], actor.m_setSlot[i]);
			Assert::AreEqual((size_t)2, actor.m_setText.size());
			const std::string& msg = actor.m_setMsg[1];
			Assert::AreEqual((size_t)0x18, msg.size());
			Assert::AreEqual((USHORT)1010, *(USHORT*)&msg[2]);
			Assert::AreEqual((OBJID)1000001, *(OBJID*)&msg[8]);
			Assert::AreEqual((DWORD)4, *(DWORD*)&msg[0x0C]);
			Assert::AreEqual((USHORT)1, *(USHORT*)&msg[0x14]);
			Assert::AreEqual((USHORT)242, *(USHORT*)&msg[0x16]);
		}
	};
}
