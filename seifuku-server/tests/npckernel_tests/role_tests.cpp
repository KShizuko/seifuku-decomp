#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
#include "I_Role.h"
#include "AgentData.h"
#include "NetMsg.h"
#include "Body.h"
#include "Sense.h"
class CGameMap;
#include "RoleMove.h"
#include "RoleFight.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct NetMsgLayout : public CNetMsg
{
	static size_t	OffType()		{ return offsetof(NetMsgLayout, m_unMsgType); }
	static size_t	OffSize()		{ return offsetof(NetMsgLayout, m_unMsgSize); }
	static size_t	OffBuf()		{ return offsetof(NetMsgLayout, m_bufMsg); }
	static size_t	OffNpc()		{ return offsetof(NetMsgLayout, m_idNpc); }
};
static_assert(sizeof(CNetMsg) == 0x410, "");

struct BodyLayout : public CBody
{
	static size_t	OffOwner()	{ return offsetof(BodyLayout, m_pOwner); }
};
static_assert(sizeof(CBody) == 8, "");

struct SenseLayout : public CSense
{
	static size_t	OffOwner()	{ return offsetof(SenseLayout, m_pOwner); }
	static size_t	OffLook()	{ return offsetof(SenseLayout, m_tLook); }
};
static_assert(sizeof(CSense) == 0x10, "");

struct RoleMoveLayout : public CRoleMove
{
	static size_t	OffOwner()	{ return offsetof(RoleMoveLayout, m_pOwner); }
	static size_t	OffMap()		{ return offsetof(RoleMoveLayout, m_pMap); }
	static size_t	OffPos()		{ return offsetof(RoleMoveLayout, m_pos); }
	static size_t	OffTarget()	{ return offsetof(RoleMoveLayout, m_posTarget); }
	static size_t	OffPath()	{ return offsetof(RoleMoveLayout, m_setPath); }
};

struct RoleFightLayout : public CRoleFight
{
	static size_t	OffOwner()	{ return offsetof(RoleFightLayout, m_pOwner); }
	static size_t	OffAttackMe()	{ return offsetof(RoleFightLayout, m_idAttackMe); }
	static size_t	OffTarget()	{ return offsetof(RoleFightLayout, m_pTarget); }
	static size_t	OffDelay()	{ return offsetof(RoleFightLayout, m_tAttackDelay); }
	static size_t	OffFight()	{ return offsetof(RoleFightLayout, m_tFight); }
};
static_assert(sizeof(CRoleFight) == 0x28, "");

static_assert(sizeof(((IRole*)0)->GetEffect()) == 4, "");

TEST_CLASS(RoleHeaderTests)
{
public:
	TEST_METHOD(NetMsgMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, NetMsgLayout::OffType());
		Assert::AreEqual((size_t)0x08, NetMsgLayout::OffSize());
		Assert::AreEqual((size_t)0x0C, NetMsgLayout::OffBuf());
		Assert::AreEqual((size_t)0x40C, NetMsgLayout::OffNpc());
	}
	TEST_METHOD(BodyOwnerAtPlus4)
	{
		Assert::AreEqual((size_t)0x04, BodyLayout::OffOwner());
	}
	TEST_METHOD(SenseMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, SenseLayout::OffOwner());
		Assert::AreEqual((size_t)0x08, SenseLayout::OffLook());
	}
	TEST_METHOD(RoleMoveMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, RoleMoveLayout::OffOwner());
		Assert::AreEqual((size_t)0x08, RoleMoveLayout::OffMap());
		Assert::AreEqual((size_t)0x0C, RoleMoveLayout::OffPos());
		Assert::AreEqual((size_t)0x14, RoleMoveLayout::OffTarget());
		Assert::AreEqual((size_t)0x1C, RoleMoveLayout::OffPath());
	}
	TEST_METHOD(RoleFightMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, RoleFightLayout::OffOwner());
		Assert::AreEqual((size_t)0x08, RoleFightLayout::OffAttackMe());
		Assert::AreEqual((size_t)0x0C, RoleFightLayout::OffTarget());
		Assert::AreEqual((size_t)0x18, RoleFightLayout::OffDelay());
		Assert::AreEqual((size_t)0x20, RoleFightLayout::OffFight());
	}
	TEST_METHOD(ActionTableMatchesTheImage)
	{
		const char* const* set = GetActionStringSet();
		int n = 0;
		while (set[n])
			n++;
		Assert::AreEqual(16, n);
		Assert::AreEqual("ChangeMode", set[ACTION_CHANGE_MODE]);
		Assert::AreEqual("LeaveTeam", set[ACTION_LEAVE_TEAM]);
	}
	TEST_METHOD(FactTableMatchesTheImage)
	{
		const char* const* set = GetFactFuncStringSet();
		int n = 0;
		while (set[n])
			n++;
		Assert::AreEqual(15, n);
		Assert::AreEqual("IsLeader", set[FACTFUNC_IS_LEADER]);
	}
	TEST_METHOD(PriorityTableMatchesTheImage)
	{
		Assert::AreEqual(INT_MAX/2, Priority2Durable(0));
		Assert::AreEqual(1200, Priority2Durable(10));
		Assert::AreEqual(300, Priority2Durable(20));
		Assert::AreEqual(8, Priority2Durable(50));
		Assert::AreEqual(3, Priority2Durable(80));
		Assert::AreEqual(1, Priority2Durable(90));
		Assert::AreEqual(0, Priority2Durable(100));
	}
};
