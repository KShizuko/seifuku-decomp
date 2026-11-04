#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
class CGameMap;
#include "Npc.h"
#include "MsgWalk.h"
#include "NpcManager.h"
#include "SynManager.h"
#include "MapManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct NpcLayout : public CNpc
{
	static size_t	OffId()			{ return offsetof(NpcLayout, m_id); }
	static size_t	OffType()		{ return offsetof(NpcLayout, m_pType); }
	static size_t	OffLife()		{ return offsetof(NpcLayout, m_nCurrLife); }
	static size_t	OffPower()		{ return offsetof(NpcLayout, m_nCurrPower); }
	static size_t	OffLink()		{ return offsetof(NpcLayout, m_lnkThis); }
	static size_t	OffActMode()	{ return offsetof(NpcLayout, m_nActMode); }
	static size_t	OffActTarget()	{ return offsetof(NpcLayout, m_idActTarget); }
	static size_t	OffAction()		{ return offsetof(NpcLayout, m_tAction); }
	static size_t	OffPosX()		{ return offsetof(NpcLayout, m_nPosX); }
	static size_t	OffPosY()		{ return offsetof(NpcLayout, m_nPosY); }
	static size_t	OffEffect()		{ return offsetof(NpcLayout, m_i64Effect); }
	static size_t	OffDir()		{ return offsetof(NpcLayout, m_nDir); }
	static size_t	OffPose()		{ return offsetof(NpcLayout, m_nPose); }
	static size_t	OffMap()		{ return offsetof(NpcLayout, m_pMap); }
	static size_t	OffDormancy()	{ return offsetof(NpcLayout, m_tDormancy); }
	static size_t	OffMove()		{ return offsetof(NpcLayout, m_tMove); }
	static size_t	OffAtkMe()		{ return offsetof(NpcLayout, m_idAtkMe); }
	static size_t	OffAtkFirst()	{ return offsetof(NpcLayout, m_bAtkFirst); }
	static size_t	OffAttackFlag()	{ return offsetof(NpcLayout, m_bAttackFlag); }
	static size_t	OffOwnerType()	{ return offsetof(NpcLayout, m_nOwnerType); }
	static size_t	OffOwner()		{ return offsetof(NpcLayout, m_idOwner); }
	static size_t	OffGenerator()	{ return offsetof(NpcLayout, m_pGenerator); }
	static size_t	OffDie()		{ return offsetof(NpcLayout, m_bDie); }
	static size_t	OffPosTarget()	{ return offsetof(NpcLayout, m_posTarget); }
	static size_t	OffAheadPath()	{ return offsetof(NpcLayout, m_bAheadPath); }
	static size_t	OffPathLen()	{ return offsetof(NpcLayout, m_nPathLen); }
	static size_t	OffBufPath()	{ return offsetof(NpcLayout, m_bufPath); }
};
static_assert(sizeof(CNpc) == 0xC8, "");

struct MsgWalkLayout : public CMsgWalk
{
	static size_t	Size()			{ return sizeof(MSG_Info); }
	static size_t	OffUser()		{ return offsetof(MSG_Info, idUser); }
	static size_t	OffDir()		{ return offsetof(MSG_Info, ucDir); }
	static size_t	OffMode()		{ return offsetof(MSG_Info, ucMode); }
};

TEST_CLASS(NpcLayoutTests)
{
public:
	TEST_METHOD(NpcMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x0C, NpcLayout::OffId());
		Assert::AreEqual((size_t)0x10, NpcLayout::OffType());
		Assert::AreEqual((size_t)0x14, NpcLayout::OffLife());
		Assert::AreEqual((size_t)0x18, NpcLayout::OffPower());
		Assert::AreEqual((size_t)0x1C, NpcLayout::OffLink());
		Assert::AreEqual((size_t)0x28, NpcLayout::OffActMode());
		Assert::AreEqual((size_t)0x2C, NpcLayout::OffActTarget());
		Assert::AreEqual((size_t)0x30, NpcLayout::OffAction());
		Assert::AreEqual((size_t)0x38, NpcLayout::OffPosX());
		Assert::AreEqual((size_t)0x3C, NpcLayout::OffPosY());
		Assert::AreEqual((size_t)0x40, NpcLayout::OffEffect());
		Assert::AreEqual((size_t)0x44, NpcLayout::OffDir());
		Assert::AreEqual((size_t)0x48, NpcLayout::OffPose());
		Assert::AreEqual((size_t)0x4C, NpcLayout::OffMap());
		Assert::AreEqual((size_t)0x50, NpcLayout::OffDormancy());
		Assert::AreEqual((size_t)0x58, NpcLayout::OffMove());
		Assert::AreEqual((size_t)0x60, NpcLayout::OffAtkMe());
		Assert::AreEqual((size_t)0x64, NpcLayout::OffAtkFirst());
		Assert::AreEqual((size_t)0x65, NpcLayout::OffAttackFlag());
		Assert::AreEqual((size_t)0x68, NpcLayout::OffOwnerType());
		Assert::AreEqual((size_t)0x6C, NpcLayout::OffOwner());
		Assert::AreEqual((size_t)0x70, NpcLayout::OffGenerator());
		Assert::AreEqual((size_t)0x74, NpcLayout::OffDie());
		Assert::AreEqual((size_t)0x78, NpcLayout::OffPosTarget());
		Assert::AreEqual((size_t)0x80, NpcLayout::OffAheadPath());
		Assert::AreEqual((size_t)0x84, NpcLayout::OffPathLen());
		Assert::AreEqual((size_t)0x88, NpcLayout::OffBufPath());
	}

	TEST_METHOD(MsgWalkBodyIsEightBytes)
	{
		Assert::AreEqual((size_t)8, MsgWalkLayout::Size());
		Assert::AreEqual((size_t)0, MsgWalkLayout::OffUser());
		Assert::AreEqual((size_t)4, MsgWalkLayout::OffDir());
		Assert::AreEqual((size_t)5, MsgWalkLayout::OffMode());
	}

	TEST_METHOD(StatusBitsCNpcTests)
	{
		Assert::AreEqual((DWORD)0x28, KEEPEFFECT_NOT_ACTIVE);
		Assert::AreEqual((DWORD)0x20, KEEPEFFECT_DIE);
		Assert::AreEqual((DWORD)0x01000000, KEEPEFFECT_SYNCRIME);
		Assert::AreEqual((DWORD)0x20000000, KEEPEFFECT_INSTEAD);
		Assert::AreEqual((DWORD)0x08000400, KEEPEFFECT_NOT_BLOCKNPC);
		Assert::AreEqual((DWORD)0x0000C001, KEEPEFFECT_NOT_VIRTUOUS);
	}

	TEST_METHOD(AtkUserBitsCNpcTests)
	{
		Assert::AreEqual(0x0001, (int)ATKUSER_PASSIVE);
		Assert::AreEqual(0x0002, (int)ATKUSER_ACTIVE);
		Assert::AreEqual(0x0004, (int)ATKUSER_RIGHTEOUS);
		Assert::AreEqual(0x2004, (int)(ATKUSER_RIGHTEOUS|ATKUSER_NEUTRAL));
		Assert::AreEqual(0x0008, (int)ATKUSER_GUARD);
		Assert::AreEqual(0x0010, (int)ATKUSER_PPKER);
		Assert::AreEqual(0x0020, (int)ATKUSER_JUMP);
		Assert::AreEqual(0x0040, (int)ATKUSER_FIXED);
		Assert::AreEqual(0x0080, (int)ATKUSER_FASTBACK);
		Assert::AreEqual(0x0100, (int)ATKUSER_LOCKUSER);
		Assert::AreEqual(0x0200, (int)ATKUSER_LOCKONE);
		Assert::AreEqual(0x0800, (int)ATKUSER_EVIL_KILLER);
		Assert::AreEqual(0x1000, (int)ATKUSER_WING);
		Assert::AreEqual(0x8000, (int)ATKUSER_NOESCAPE);
		Assert::AreEqual((int)ATKUSER_LOCKUSER, ATKUSER_NO_DORMANCY);
		Assert::AreEqual(21, (int)NPCTYPEDATA_ATKUSER);
		Assert::AreEqual(24, (int)NPCTYPEDATA_SIZEADD);
		Assert::AreEqual(26, (int)NPCTYPEDATA_RUNSPEED);
		Assert::AreEqual(36, (int)NPCTYPEDATA_MAGIC_TYPE);
	}
};

struct NpcManagerLayout : public CNpcManager
{
	static size_t	OffNpcSet()		{ return offsetof(NpcManagerLayout, m_pNpcSet); }
	static size_t	OffGenSet()		{ return offsetof(NpcManagerLayout, m_pNpcGenSet); }
	static size_t	OffTypeSet()	{ return offsetof(NpcManagerLayout, m_pNpcTypeSet); }
	static size_t	OffAgentGen()	{ return offsetof(NpcManagerLayout, m_setAgentGen); }
};

TEST_CLASS(NpcManagerLayoutTests)
{
public:
	TEST_METHOD(NpcManagerSetsAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, NpcManagerLayout::OffNpcSet());
		Assert::AreEqual((size_t)0x08, NpcManagerLayout::OffGenSet());
		Assert::AreEqual((size_t)0x0C, NpcManagerLayout::OffTypeSet());
		Assert::AreEqual((size_t)0x10, NpcManagerLayout::OffAgentGen());
	}
};

struct SyndicateLayout : public CSyndicate
{
	static size_t	OffId()			{ return offsetof(SyndicateLayout, m_id); }
	static size_t	OffFealty()		{ return offsetof(SyndicateLayout, m_idFealty); }
	static size_t	OffAlly()		{ return offsetof(SyndicateLayout, m_setAlly); }
};

TEST_CLASS(SyndicateLayoutTests)
{
public:
	TEST_METHOD(SyndicateMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0, SyndicateLayout::OffId());
		Assert::AreEqual((size_t)4, SyndicateLayout::OffFealty());
		Assert::AreEqual((size_t)8, SyndicateLayout::OffAlly());
	}
};

struct GameMapLayout : public CGameMap
{
	static size_t	OffMapData()	{ return offsetof(GameMapLayout, m_pMapData); }
	static size_t	OffBlockSet()	{ return offsetof(GameMapLayout, m_setBlock); }
};
struct GameBlockLayout : public CGameBlock
{
	static size_t	OffUserSet()	{ return offsetof(GameBlockLayout, m_pUserSet); }
	static size_t	OffNpcSet()		{ return offsetof(GameBlockLayout, m_pNpcSet); }
	static size_t	OffDormancy()	{ return offsetof(GameBlockLayout, m_bDormancy); }
};
struct MapManagerLayout : public CMapManager
{
	static size_t	OffMapSet()		{ return offsetof(MapManagerLayout, m_pMapSet); }
};

TEST_CLASS(MapLayoutTests)
{
public:
	TEST_METHOD(GameMapMembersBeforeTheBlockSet)
	{
		Assert::AreEqual((size_t)0x08, GameMapLayout::OffMapData());
		Assert::AreEqual((size_t)0x0C, GameMapLayout::OffBlockSet());
	}

	TEST_METHOD(GameBlockIsItsVtableTwoSetsAndTheFlag)
	{
		Assert::AreEqual((size_t)0x10, sizeof(CGameBlock));
		Assert::AreEqual((size_t)0x04, GameBlockLayout::OffUserSet());
		Assert::AreEqual((size_t)0x08, GameBlockLayout::OffNpcSet());
		Assert::AreEqual((size_t)0x0C, GameBlockLayout::OffDormancy());
	}

	TEST_METHOD(MapManagerIsItsVtableAndTheSet)
	{
		Assert::AreEqual((size_t)8, sizeof(CMapManager));
		Assert::AreEqual((size_t)4, MapManagerLayout::OffMapSet());
	}
};