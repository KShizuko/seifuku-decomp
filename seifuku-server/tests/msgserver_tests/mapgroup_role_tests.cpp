#include "CppUnitTest.h"
#include <windows.h>
#include "../../src/MsgServer/MapGroupKernel/I_Role.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	int	g_nHit = 0;

	struct RoleProbe : public IRole
	{
		virtual DWORD	GetEffect(int nIndex)							{ g_nHit = 8; return nIndex; }
		virtual int		GetDistance(int x, int y)						{ g_nHit = 33; return x + y; }
		virtual int		GetDistance(IMapThing* pTarget)					{ g_nHit = 32; return 0; }
		virtual bool	IsAlive()										{ g_nHit = 45; return true; }
		virtual void	ProcessOnMove(int nMoveMode)					{ g_nHit = 65; }
		virtual void	BroadcastRoomMsg(CNetMsg* pMsg, bool bSendSelf)	{ g_nHit = 80; }
		virtual void	BroadcastRoomMsg(LPCTSTR szMsg, bool bSendSelf)	{ g_nHit = 79; }
		virtual bool	MoveToward(int nDir)							{ g_nHit = 84; return true; }
		virtual void	SendDamageMsg(OBJID idTarget, int nDamage)		{ g_nHit = 101; }
		virtual void	AfterTheInterface()								{ g_nHit = 102; }
	};

	struct MapThingProbe : public IMapThing
	{
		virtual bool	QueryRole(void** ppv)		{ g_nHit = 7; return true; }
		virtual void	AfterTheInterface()			{ g_nHit = 8; }
	};

	void** Vtable(void* p)	{ return *reinterpret_cast<void***>(p); }
}

TEST_CLASS(MapGroupRoleTests)
{
public:
	TEST_METHOD(InterfacesAreOnePointer)
	{
		Assert::AreEqual(4u, (unsigned)sizeof(IRole));
		Assert::AreEqual(4u, (unsigned)sizeof(IMapThing));
	}

	TEST_METHOD(IRoleSlotsInTheOrderOf2008)
	{
		RoleProbe probe;
		void** vt = Vtable(&probe);
		typedef DWORD	(__thiscall *FnIntArg)(void*, int);
		typedef int		(__thiscall *FnTwoInts)(void*, int, int);
		typedef bool	(__thiscall *FnNoArg)(void*);
		typedef void	(__thiscall *FnVoidArg)(void*, int);
		typedef void	(__thiscall *FnVoidTwo)(void*, void*, bool);
		typedef void	(__thiscall *FnVoid)(void*);

		g_nHit = 0; Assert::AreEqual(1ul, ((FnIntArg)vt[8])(&probe, 1));			Assert::AreEqual(8, g_nHit);
		g_nHit = 0; Assert::AreEqual(5, ((FnTwoInts)vt[33])(&probe, 2, 3));		Assert::AreEqual(33, g_nHit);
		g_nHit = 0; ((FnIntArg)vt[32])(&probe, 0);									Assert::AreEqual(32, g_nHit);
		g_nHit = 0; Assert::IsTrue(((FnNoArg)vt[45])(&probe));						Assert::AreEqual(45, g_nHit);
		g_nHit = 0; ((FnVoidArg)vt[0x104 / 4])(&probe, 0);							Assert::AreEqual(65, g_nHit);
		g_nHit = 0; ((FnVoidTwo)vt[0x140 / 4])(&probe, NULL, false);				Assert::AreEqual(80, g_nHit);
		g_nHit = 0; ((FnVoidTwo)vt[79])(&probe, NULL, false);						Assert::AreEqual(79, g_nHit);
		g_nHit = 0; ((FnIntArg)vt[0x150 / 4])(&probe, 3);							Assert::AreEqual(84, g_nHit);
		g_nHit = 0; ((FnTwoInts)vt[101])(&probe, 1, 2);								Assert::AreEqual(101, g_nHit);
		g_nHit = 0; ((FnVoid)vt[102])(&probe);										Assert::AreEqual(102, g_nHit);
	}

	TEST_METHOD(IMapThingHasEightSlots)
	{
		MapThingProbe probe;
		void** vt = Vtable(&probe);
		typedef bool	(__thiscall *FnPtrArg)(void*, void**);
		typedef void	(__thiscall *FnVoid)(void*);
		g_nHit = 0; Assert::IsTrue(((FnPtrArg)vt[7])(&probe, NULL));	Assert::AreEqual(7, g_nHit);
		g_nHit = 0; ((FnVoid)vt[8])(&probe);							Assert::AreEqual(8, g_nHit);
	}
};
