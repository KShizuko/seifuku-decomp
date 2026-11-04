#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
class CGameMap;
#include "Agent.h"
#include "MsgUserInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(UserInfoStruct) == 0x88, "");

struct AgentLayout : public CAgent
{
	static size_t	OffInfo()		{ return offsetof(AgentLayout, m_info); }
	static size_t	OffDir()		{ return offsetof(AgentLayout, m_nDir); }
	static size_t	OffTimeOfLife()	{ return offsetof(AgentLayout, m_nTimeOfLife); }
	static size_t	OffLink()		{ return offsetof(AgentLayout, m_lnkThis); }
	static size_t	OffSizeAdd()	{ return offsetof(AgentLayout, m_nSizeAdd); }
	static size_t	OffAiCenter()	{ return offsetof(AgentLayout, m_pAiCenter); }
	static size_t	OffBody()		{ return offsetof(AgentLayout, m_pBody); }
	static size_t	OffSense()		{ return offsetof(AgentLayout, m_pSense); }
	static size_t	OffTeam()		{ return offsetof(AgentLayout, m_pTeam); }
	static size_t	OffItemPack()	{ return offsetof(AgentLayout, m_pItemPack); }
	static size_t	OffMove()		{ return offsetof(AgentLayout, m_pMove); }
	static size_t	OffFight()		{ return offsetof(AgentLayout, m_pFight); }
	static size_t	OffStep()		{ return offsetof(AgentLayout, m_nStep); }
	static size_t	OffAction()		{ return offsetof(AgentLayout, m_tAction); }
};
static_assert(sizeof(CAgent) == 0xD0, "");

struct MsgUserInfoLayout : public CMsgUserInfo
{
	static size_t	Size()			{ return sizeof(MSG_Info); }
	static size_t	OffMoney()		{ return offsetof(MSG_Info, dwMoney); }
	static size_t	OffExp()		{ return offsetof(MSG_Info, nExp); }
	static size_t	OffPotential()	{ return offsetof(MSG_Info, nPotential); }
	static size_t	OffVirtue()		{ return offsetof(MSG_Info, dwVirtue); }
	static size_t	OffForce()		{ return offsetof(MSG_Info, usForce); }
	static size_t	OffPk()			{ return offsetof(MSG_Info, sPk); }
	static size_t	OffLevel()		{ return offsetof(MSG_Info, ucLevel); }
	static size_t	OffAutoAllot()	{ return offsetof(MSG_Info, ucAutoAllot); }
	static size_t	OffBuf()		{ return offsetof(MSG_Info, szBuf); }
};

TEST_CLASS(AgentLayoutTests)
{
public:
	TEST_METHOD(UserInfoStructAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, offsetof(UserInfoStruct, szName));
		Assert::AreEqual((size_t)0x14, offsetof(UserInfoStruct, szMate));
		Assert::AreEqual((size_t)0x24, offsetof(UserInfoStruct, dwLookface));
		Assert::AreEqual((size_t)0x28, offsetof(UserInfoStruct, dwHair));
		Assert::AreEqual((size_t)0x2C, offsetof(UserInfoStruct, dwMoney));
		Assert::AreEqual((size_t)0x30, offsetof(UserInfoStruct, ucLevel));
		Assert::AreEqual((size_t)0x38, offsetof(UserInfoStruct, nPotential));
		Assert::AreEqual((size_t)0x3C, offsetof(UserInfoStruct, dwReserved3C));
		Assert::AreEqual((size_t)0x40, offsetof(UserInfoStruct, usForce));
		Assert::AreEqual((size_t)0x42, offsetof(UserInfoStruct, usDexterity));
		Assert::AreEqual((size_t)0x44, offsetof(UserInfoStruct, usHealth));
		Assert::AreEqual((size_t)0x46, offsetof(UserInfoStruct, usSoul));
		Assert::AreEqual((size_t)0x48, offsetof(UserInfoStruct, usAdditional_point));
		Assert::AreEqual((size_t)0x4A, offsetof(UserInfoStruct, ucAuto_allot));
		Assert::AreEqual((size_t)0x4C, offsetof(UserInfoStruct, usLife));
		Assert::AreEqual((size_t)0x4E, offsetof(UserInfoStruct, usMana));
		Assert::AreEqual((size_t)0x50, offsetof(UserInfoStruct, ucProfession));
		Assert::AreEqual((size_t)0x54, offsetof(UserInfoStruct, nDeed));
		Assert::AreEqual((size_t)0x58, offsetof(UserInfoStruct, sPk));
		Assert::AreEqual((size_t)0x5A, offsetof(UserInfoStruct, ucNobility));
		Assert::AreEqual((size_t)0x5C, offsetof(UserInfoStruct, dwMedal));
		Assert::AreEqual((size_t)0x60, offsetof(UserInfoStruct, dwMedal_select));
		Assert::AreEqual((size_t)0x64, offsetof(UserInfoStruct, ucMetempsychosis));
		Assert::AreEqual((size_t)0x70, offsetof(UserInfoStruct, dwVirtue));
		Assert::AreEqual((size_t)0x74, offsetof(UserInfoStruct, dwEffect));
	}
	TEST_METHOD(AgentMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x08, AgentLayout::OffInfo());
		Assert::AreEqual((size_t)0x90, AgentLayout::OffDir());
		Assert::AreEqual((size_t)0x94, AgentLayout::OffTimeOfLife());
		Assert::AreEqual((size_t)0x98, AgentLayout::OffLink());
		Assert::AreEqual((size_t)0xA4, AgentLayout::OffSizeAdd());
		Assert::AreEqual((size_t)0xA8, AgentLayout::OffAiCenter());
		Assert::AreEqual((size_t)0xAC, AgentLayout::OffBody());
		Assert::AreEqual((size_t)0xB0, AgentLayout::OffSense());
		Assert::AreEqual((size_t)0xB4, AgentLayout::OffTeam());
		Assert::AreEqual((size_t)0xB8, AgentLayout::OffItemPack());
		Assert::AreEqual((size_t)0xBC, AgentLayout::OffMove());
		Assert::AreEqual((size_t)0xC0, AgentLayout::OffFight());
		Assert::AreEqual((size_t)0xC4, AgentLayout::OffStep());
		Assert::AreEqual((size_t)0xC8, AgentLayout::OffAction());
	}
	TEST_METHOD(MsgUserInfoBodyAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x40, MsgUserInfoLayout::Size());
		Assert::AreEqual((size_t)0x0A, MsgUserInfoLayout::OffMoney());
		Assert::AreEqual((size_t)0x0E, MsgUserInfoLayout::OffExp());
		Assert::AreEqual((size_t)0x12, MsgUserInfoLayout::OffPotential());
		Assert::AreEqual((size_t)0x26, MsgUserInfoLayout::OffVirtue());
		Assert::AreEqual((size_t)0x2A, MsgUserInfoLayout::OffForce());
		Assert::AreEqual((size_t)0x38, MsgUserInfoLayout::OffPk());
		Assert::AreEqual((size_t)0x3A, MsgUserInfoLayout::OffLevel());
		Assert::AreEqual((size_t)0x3E, MsgUserInfoLayout::OffAutoAllot());
		Assert::AreEqual((size_t)0x3F, MsgUserInfoLayout::OffBuf());
	}
	TEST_METHOD(StatusConstantsAreTheBinarysBits)
	{
		Assert::AreEqual((DWORD)0x20, KEEPEFFECT_DIE);
		Assert::AreEqual((DWORD)0x8001, KEEPEFFECT_DEEPRED|KEEPEFFECT_CRIME);
		Assert::AreEqual((DWORD)0xC001, KEEPEFFECT_NOT_VIRTUOUS);
		Assert::AreEqual((DWORD)0x400000, KEEPEFFECT_LURKER);
		Assert::AreEqual((DWORD)0x8000000, KEEPEFFECT_WING);
		Assert::AreEqual((DWORD)0x8000400, KEEPEFFECT_NOT_BLOCKNPC);
	}
};
