#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
class CGameMap;
#include "User.h"
#include "UserManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct UserLayout : public CUser
{
	static size_t	OffMap()		{ return offsetof(UserLayout, m_pMap); }
	static size_t	OffInfo()		{ return offsetof(UserLayout, m_Info); }
	static size_t	OffActive()		{ return offsetof(UserLayout, m_tActive); }
	static size_t	OffLink()		{ return offsetof(UserLayout, m_lnkThis); }
};
static_assert(sizeof(CUser) == 0x6C, "");
static_assert(sizeof(ST_USERINFO) == 0x48, "");

TEST_CLASS(UserLayoutTests)
{
public:
	TEST_METHOD(UserMembersAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x0C, UserLayout::OffMap());
		Assert::AreEqual((size_t)0x10, UserLayout::OffInfo());
		Assert::AreEqual((size_t)0x58, UserLayout::OffActive());
		Assert::AreEqual((size_t)0x60, UserLayout::OffLink());
	}

	TEST_METHOD(UserInfoAtTheBinarysOffsets)
	{
		Assert::AreEqual((size_t)0x04, offsetof(ST_USERINFO, szName));
		Assert::AreEqual((size_t)0x14, offsetof(ST_USERINFO, dwLookFace));
		Assert::AreEqual((size_t)0x18, offsetof(ST_USERINFO, usHair));
		Assert::AreEqual((size_t)0x1C, offsetof(ST_USERINFO, i64Effect));
		Assert::AreEqual((size_t)0x20, offsetof(ST_USERINFO, dwHelmetType));
		Assert::AreEqual((size_t)0x28, offsetof(ST_USERINFO, dwWeaponRType));
		Assert::AreEqual((size_t)0x2C, offsetof(ST_USERINFO, dwWeaponLType));
		Assert::AreEqual((size_t)0x30, offsetof(ST_USERINFO, dwMountType));
		Assert::AreEqual((size_t)0x34, offsetof(ST_USERINFO, dwMedalSelect));
		Assert::AreEqual((size_t)0x38, offsetof(ST_USERINFO, idSyn));
		Assert::AreEqual((size_t)0x3C, offsetof(ST_USERINFO, nRank));
		Assert::AreEqual((size_t)0x3E, offsetof(ST_USERINFO, usPosX));
		Assert::AreEqual((size_t)0x40, offsetof(ST_USERINFO, usPosY));
		Assert::AreEqual((size_t)0x42, offsetof(ST_USERINFO, ucDir));
		Assert::AreEqual((size_t)0x43, offsetof(ST_USERINFO, ucPose));
		Assert::AreEqual((size_t)0x44, offsetof(ST_USERINFO, ucSizeAdd));
	}

	TEST_METHOD(StatusBitsUserTests)
	{
		Assert::AreEqual((DWORD)0x8001, (DWORD)(KEEPEFFECT_DEEPRED|KEEPEFFECT_CRIME));
		Assert::AreEqual((DWORD)0x00008000, KEEPEFFECT_DEEPRED);
		Assert::AreEqual((DWORD)0x00400000, KEEPEFFECT_LURKER);
		Assert::AreEqual((DWORD)0x08000000, KEEPEFFECT_WING);
		Assert::AreEqual((DWORD)0x00000040, KEEPEFFECT_TEAMLEADER);
		Assert::AreEqual((DWORD)0x08000400, KEEPEFFECT_NOT_BLOCKNPC);
	}
};

struct UserManagerLayout : public CUserManager
{
	static size_t	OffSet()		{ return offsetof(UserManagerLayout, m_pUserSet); }
};

TEST_CLASS(UserManagerLayoutTests)
{
public:
	TEST_METHOD(UserManagerIsItsVtableAndTheSet)
	{
		Assert::AreEqual((size_t)8, sizeof(CUserManager));
		Assert::AreEqual((size_t)4, UserManagerLayout::OffSet());
	}
};
