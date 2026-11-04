#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/Adapter/Include/I_Adapter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace adapter_tests
{
	typedef void* (__fastcall* PFN_0)(void* pThis, void* pEdx);
	typedef void* (__fastcall* PFN_1)(void* pThis, void* pEdx, DWORD a);
	typedef void* (__fastcall* PFN_2)(void* pThis, void* pEdx, DWORD a, DWORD b);
	typedef bool (__cdecl* PFN_VA)(void* pThis, const char* fmt, ...);

	void** Vtbl(void* p)	{ return *(void***)p; }

	int	g_nDestroyed = 0;

	class CAdapterProbe : public IAdapter, public IRegister
	{
	public:
		int		m_nSlot;
		DWORD	m_a, m_b;
		CAdapterProbe() : m_nSlot(-1), m_a(0), m_b(0) {}
		virtual void*	CreateActor		(LPCTSTR szName)				{ m_nSlot = 1; m_a = (DWORD)szName; return NULL; }
		virtual void*	Find			(int idModule, int nUnknown)	{ m_nSlot = 2; m_a = idModule; m_b = nUnknown; return NULL; }
		virtual void*	GetActor		(int idModule, void* pBuf)		{ m_nSlot = 3; m_a = idModule; m_b = (DWORD)pBuf; return NULL; }
		virtual bool	Register		(int idModule, void* pModule)	{ m_nSlot = 100; m_a = idModule; m_b = (DWORD)pModule; return true; }
	};

	class CActorProbe : public ILotteryActor
	{
	public:
		int		m_nSlot;
		DWORD	m_a, m_b;
		CActorProbe() : m_nSlot(-1), m_a(0), m_b(0) {}
		virtual ~CActorProbe()									{ g_nDestroyed++; }
		virtual USHORT	Release				()						{ m_nSlot = 0; return 0; }
		virtual OBJID	GetID				()						{ m_nSlot = 1; return 0; }
		virtual LPCTSTR	GetName				()						{ m_nSlot = 2; return NULL; }
		virtual OBJID	GetMapID			()						{ m_nSlot = 3; return 0; }
		virtual DWORD	GetLev				()						{ m_nSlot = 4; return 0; }
		virtual __int64	GetExp				()						{ m_nSlot = 5; return 0; }
		virtual int		GetBigPrizeFails	()						{ m_nSlot = 6; return 0; }
		virtual void	SetBigPrizeFails	(int nData, bool bUpdate)	{ m_nSlot = 7; m_a = nData; m_b = bUpdate; }
		virtual int		GetSmallPrizeFails	()						{ m_nSlot = 8; return 0; }
		virtual void	SetSmallPrizeFails	(int nData, bool bUpdate)	{ m_nSlot = 9; m_a = nData; m_b = bUpdate; }
		virtual ILotteryPrize*	AwardItem	(int nItemType)			{ m_nSlot = 10; m_a = nItemType; return NULL; }
		virtual bool	SendSysMsg			(const char* fmt, ...)	{ m_nSlot = 11; m_a = (DWORD)fmt; return true; }
		virtual bool	SendMsg				(PLUGIN_MSG* pMsg)		{ m_nSlot = 12; m_a = (DWORD)pMsg; return true; }
		virtual bool	BroadcastMsg		(const char* fmt, ...)	{ m_nSlot = 13; m_a = (DWORD)fmt; return true; }
		virtual bool	BroadcastRoomMsg	(PLUGIN_MSG* pMsg)		{ m_nSlot = 14; m_a = (DWORD)pMsg; return true; }
	};

	class CPrizeProbe : public ILotteryPrize
	{
	public:
		int		m_nSlot;
		DWORD	m_a, m_b;
		CPrizeProbe() : m_nSlot(-1), m_a(0), m_b(0) {}
		virtual ~CPrizeProbe()								{ g_nDestroyed++; }
		virtual OBJID	GetID			()						{ m_nSlot = 0; return 0; }
		virtual bool	IsEquip			()						{ m_nSlot = 1; return false; }
		virtual bool	SetInt			(int nIdx, int nData)	{ m_nSlot = 2; m_a = nIdx; m_b = nData; return true; }
		virtual bool	GetInfo			(PLUGIN_ITEMINFO* pInfo)	{ m_nSlot = 3; m_a = (DWORD)pInfo; return true; }
		virtual USHORT	Release			()						{ m_nSlot = 4; return 0; }
	};

	TEST_CLASS(AdapterInterfaceTests)
	{
	public:
		TEST_METHOD(AdapterAndRegisterSlots)
		{
			CAdapterProbe obj;
			void** v = Vtbl(&obj);
			Assert::AreEqual((void*)0, ((PFN_0)v[0])(&obj, NULL));
			((PFN_1)v[1])(&obj, NULL, 0x1234);								Assert::AreEqual(1, obj.m_nSlot);	Assert::AreEqual((DWORD)0x1234, obj.m_a);
			((PFN_2)v[2])(&obj, NULL, ADAPTER_MODULE_LOTTERY, 7);			Assert::AreEqual(2, obj.m_nSlot);	Assert::AreEqual((DWORD)0x20, obj.m_a);	Assert::AreEqual((DWORD)7, obj.m_b);
			((PFN_2)v[3])(&obj, NULL, ADAPTER_MODULE_LOTTERY, 0x5678);		Assert::AreEqual(3, obj.m_nSlot);	Assert::AreEqual((DWORD)0x5678, obj.m_b);

			IRegister* pRegister = &obj;
			Assert::AreEqual(4, (int)((char*)pRegister - (char*)&obj), L"IRegister is not at +4");
			((PFN_2)Vtbl(pRegister)[0])(pRegister, NULL, ADAPTER_MODULE_LOTTERY, 0x9ABC);
			Assert::AreEqual(100, obj.m_nSlot);	Assert::AreEqual((DWORD)0x20, obj.m_a);	Assert::AreEqual((DWORD)0x9ABC, obj.m_b);
		}

		TEST_METHOD(LotteryActorSlots)
		{
			CActorProbe* p = new CActorProbe;
			void** v = Vtbl(p);
			int nNoArg[] = { 0, 1, 2, 3, 4, 5, 6, 8 };
			for (int i = 0; i < 8; i++)
			{
				((PFN_0)v[nNoArg[i]])(p, NULL);
				Assert::AreEqual(nNoArg[i], p->m_nSlot);
			}
			((PFN_2)v[7])(p, NULL, 11, 1);			Assert::AreEqual(7, p->m_nSlot);	Assert::AreEqual((DWORD)11, p->m_a);	Assert::AreEqual((DWORD)1, p->m_b);
			((PFN_2)v[9])(p, NULL, 12, 0);			Assert::AreEqual(9, p->m_nSlot);	Assert::AreEqual((DWORD)12, p->m_a);
			((PFN_1)v[10])(p, NULL, 1000000);		Assert::AreEqual(10, p->m_nSlot);	Assert::AreEqual((DWORD)1000000, p->m_a);
			((PFN_VA)v[11])(p, "%s", "x");			Assert::AreEqual(11, p->m_nSlot);
			((PFN_1)v[12])(p, NULL, 0x40);			Assert::AreEqual(12, p->m_nSlot);	Assert::AreEqual((DWORD)0x40, p->m_a);
			((PFN_VA)v[13])(p, "%s", "y");			Assert::AreEqual(13, p->m_nSlot);
			((PFN_1)v[14])(p, NULL, 0x44);			Assert::AreEqual(14, p->m_nSlot);	Assert::AreEqual((DWORD)0x44, p->m_a);
			int nBefore = g_nDestroyed;
			((PFN_1)v[15])(p, NULL, 1);
			Assert::AreEqual(nBefore + 1, g_nDestroyed);
		}

		TEST_METHOD(LotteryPrizeSlots)
		{
			CPrizeProbe* p = new CPrizeProbe;
			void** v = Vtbl(p);
			((PFN_0)v[0])(p, NULL);					Assert::AreEqual(0, p->m_nSlot);
			((PFN_0)v[1])(p, NULL);					Assert::AreEqual(1, p->m_nSlot);
			((PFN_2)v[2])(p, NULL, 1, 2);			Assert::AreEqual(2, p->m_nSlot);	Assert::AreEqual((DWORD)1, p->m_a);	Assert::AreEqual((DWORD)2, p->m_b);
			((PFN_1)v[3])(p, NULL, 0x48);			Assert::AreEqual(3, p->m_nSlot);	Assert::AreEqual((DWORD)0x48, p->m_a);
			((PFN_0)v[4])(p, NULL);					Assert::AreEqual(4, p->m_nSlot);
			int nBefore = g_nDestroyed;
			((PFN_1)v[5])(p, NULL, 1);
			Assert::AreEqual(nBefore + 1, g_nDestroyed);
		}
	};
}
