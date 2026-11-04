#include "CppUnitTest.h"
#include <winsock2.h>
#include <windows.h>
#include <stddef.h>
#include <vector>
#include "MapGroup.h"
#include "Adapter.h"
#include "LotteryActor.h"
#include "NetMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace adapter_init_tests
{
	struct MAPGROUP_BYTES { alignas(alignof(CMapGroup)) char buf[sizeof(CMapGroup)]; };

	struct CMapGroupOffsets : public CMapGroup
	{
		static size_t	OffDb()		{ return offsetof(CMapGroupOffsets, m_pDb); }
	};
}

template<> std::vector<CMapGroup, std::allocator<CMapGroup> >::~vector() noexcept {}
namespace adapter_init_tests
{
	std::vector<MAPGROUP_BYTES>*	s_pSet = NULL;
	int		KeepSet(void* pSet)		{ s_pSet = (std::vector<MAPGROUP_BYTES>*)pSet; return 0; }
}
CMapGroup::MAPGROUP_SET CMapGroup::m_setMapGroup((adapter_init_tests::KeepSet(&m_setMapGroup), MAPGROUP_SET()));

ILotteryActor* LotteryActorCreate(CUser* pUser, PROCESS_ID idProcess)	{ return NULL; }
CNetMsg* CNetMsg::CreateClientMsg(PROCESS_ID idProcess, SOCKET_ID idSocket, OBJID idMsg, const char* pbufMsg, DWORD dwMsgSize, int nTrans)	{ return NULL; }

namespace adapter_init_tests
{
	struct STUB_MODULE
	{
		int			nCalls;
		IRegister*	pRegister;
		IDatabase*	pDb;
		bool		bFail;
		bool		bRegistered;
	};
	STUB_MODULE	g_stub;
	int			g_nModule;
}

extern "C" __declspec(dllexport) bool ModuleCreate(IRegister* pRegister, IDatabase* pDb)
{
	using namespace adapter_init_tests;
	g_stub.nCalls++;
	g_stub.pRegister	= pRegister;
	g_stub.pDb			= pDb;
	if (g_stub.bFail)
		return false;
	g_stub.bRegistered	= pRegister->Register(ADAPTER_MODULE_LOTTERY, &g_nModule);
	return g_stub.bRegistered;
}

namespace adapter_init_tests
{
	const PROCESS_ID	ID_GROUP	= MSGPORT_MAPGROUP_FIRST;
	IDatabase* const	FAKE_DB		= (IDatabase*)0x5A5A1234;

	class CAdapterUnderTest : public CAdapter
	{
	public:
		CAdapterUnderTest() : CAdapter(ID_GROUP) {}
		IRegister*	Register()	{ return this; }
	};

	TEST_CLASS(AdapterInitTests)
	{
		char	m_szOldDir[MAX_PATH];

		void InFixture(const char* pszCase)
		{
			char szDir[MAX_PATH];
			strcpy(szDir, __FILE__);
			strcpy(strrchr(szDir, '\\') + 1, "fixtures\\adapter\\");
			strcat(szDir, pszCase);
			Assert::IsTrue(::SetCurrentDirectoryA(szDir) != FALSE);
		}

	public:
		TEST_METHOD_INITIALIZE(Setup)
		{
			s_pSet->resize(ID_GROUP + 1);
			memset(&(*s_pSet)[0], 0, sizeof(MAPGROUP_BYTES) * s_pSet->size());
			*(IDatabase**)((*s_pSet)[ID_GROUP].buf + CMapGroupOffsets::OffDb()) = FAKE_DB;

			memset(&g_stub, 0, sizeof(g_stub));
			::GetCurrentDirectoryA(sizeof(m_szOldDir), m_szOldDir);

			HMODULE hSelf = NULL;
			::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&ModuleCreate, &hSelf);
			char szPath[MAX_PATH];
			::GetModuleFileNameA(hSelf, szPath, sizeof(szPath));
			Assert::AreEqual("msgserver_tests.dll", strrchr(szPath, '\\') + 1, true);
		}

		TEST_METHOD_CLEANUP(Teardown)
		{
			::SetCurrentDirectoryA(m_szOldDir);
		}

		TEST_METHOD(OneModuleIsCreatedWithTheRegisterAndTheDatabase)
		{
			InFixture("one");
			CAdapterUnderTest adapter;
			Assert::IsTrue(adapter.Init());
			Assert::AreEqual(1, g_stub.nCalls);
			Assert::IsTrue(g_stub.pRegister == adapter.Register());
			Assert::AreEqual(4, (int)((char*)g_stub.pRegister - (char*)&adapter), L"not the register at +4");
			Assert::IsTrue(g_stub.pDb == FAKE_DB, L"not the map group's database");
			Assert::IsTrue(adapter.Find(ADAPTER_MODULE_LOTTERY, 0) == &g_nModule);
			Assert::IsNull(adapter.Find(ADAPTER_MODULE_LOTTERY + 1, 0));
		}

		TEST_METHOD(NoModulesIsTrue)
		{
			InFixture("");
			CAdapterUnderTest noFile;
			Assert::IsTrue(noFile.Init());

			InFixture("nosection");
			CAdapterUnderTest noSection;
			Assert::IsTrue(noSection.Init());

			InFixture("negative");
			CAdapterUnderTest negative;
			Assert::IsTrue(negative.Init());
			Assert::AreEqual(0, g_stub.nCalls);
			Assert::IsNull(negative.Find(ADAPTER_MODULE_LOTTERY, 0));
		}

		TEST_METHOD(OnlyAmountModulesAreRead)
		{
			InFixture("amount1");
			CAdapterUnderTest adapter;
			Assert::IsTrue(adapter.Init());
			Assert::AreEqual(1, g_stub.nCalls);
		}

		TEST_METHOD(AMissingModuleOrExportFails)
		{
			InFixture("missing");
			CAdapterUnderTest missing;
			Assert::IsFalse(missing.Init());
			Assert::AreEqual(0, g_stub.nCalls);

			InFixture("noexport");
			CAdapterUnderTest noExport;
			Assert::IsFalse(noExport.Init());
			Assert::AreEqual(0, g_stub.nCalls);
		}

		TEST_METHOD(AFailedModuleCreateStopsTheRest)
		{
			InFixture("two");
			g_stub.bFail = true;
			CAdapterUnderTest adapter;
			Assert::IsFalse(adapter.Init());
			Assert::AreEqual(1, g_stub.nCalls);
		}

		TEST_METHOD(ATakenIdFailsTheSecondModule)
		{
			InFixture("two");
			CAdapterUnderTest adapter;
			Assert::IsFalse(adapter.Init());
			Assert::AreEqual(2, g_stub.nCalls);
			Assert::IsFalse(g_stub.bRegistered);
			Assert::IsTrue(adapter.Find(ADAPTER_MODULE_LOTTERY, 0) == &g_nModule, L"the first module's registration lost");
		}

		TEST_METHOD(TheModuleNameIsCutToFifteen)
		{
			InFixture("cut");
			CAdapterUnderTest adapter;
			Assert::IsTrue(adapter.Init(), L"the name was not cut at 15 characters");
			Assert::AreEqual(1, g_stub.nCalls);
		}
	};
}
