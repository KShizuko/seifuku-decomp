#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <stddef.h>
#include "SynWorldShell.h"
#include "SynWorldManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CreateSyndicateInfo) == 0x150, "");
static_assert(offsetof(CreateSyndicateInfo, szName) == 0x04, "");
static_assert(offsetof(CreateSyndicateInfo, szTenet) == 0x14, "");
static_assert(offsetof(CreateSyndicateInfo, szLeaderTitle) == 0x114, "");
static_assert(offsetof(CreateSyndicateInfo, szMemberTitle) == 0x124, "");
static_assert(offsetof(CreateSyndicateInfo, idLeader) == 0x134, "");
static_assert(offsetof(CreateSyndicateInfo, szLeader) == 0x138, "");
static_assert(offsetof(CreateSyndicateInfo, nMoney) == 0x148, "");
static_assert(offsetof(CreateSyndicateInfo, idFealty) == 0x14C, "");
static_assert(offsetof(SynFuncInfo0, nSize) == 0x04, "");
static_assert(offsetof(SynFuncInfo0, ucFuncType) == 0x06, "");
static_assert(offsetof(SynFuncInfo0, ucSaveFlag) == 0x07, "");
static_assert(offsetof(SynFuncInfo0, IntParam) == 0x08, "");
static_assert(offsetof(SynFuncInfo0, StrParam) == 0x08, "");

namespace
{
	struct SynProbe : public CSyndicateWorld
	{
		static size_t	OffMember()		{ return offsetof(SynProbe, m_setMember); }
		static size_t	OffMsgBd()		{ return offsetof(SynProbe, m_ptrMsgBd); }
		static size_t	SizeSet()		{ return sizeof(((SynProbe*)0)->m_setMember); }
	};
	struct MngProbe : public CSynWorldManager
	{
		static size_t	OffSet()		{ return offsetof(MngProbe, m_setSyn); }
		static size_t	OffDefault()	{ return offsetof(MngProbe, m_pDefault); }
		static size_t	OffObj()		{ return offsetof(MngProbe, m_obj); }
	};
}

TEST_CLASS(SyndicateTests)
{
public:
	TEST_METHOD(Layouts)
	{
		Assert::AreEqual((size_t)0x04, SynProbe::OffMember());
		Assert::AreEqual((size_t)0x04, SynProbe::OffMsgBd() - SynProbe::OffMember() - SynProbe::SizeSet());
		Assert::AreEqual(sizeof(CSyndicateWorld), SynProbe::OffMsgBd() + sizeof(void*));
		Assert::AreEqual((size_t)0x14, sizeof(CSynWorldManager));
		Assert::AreEqual((size_t)0x04, MngProbe::OffSet());
		Assert::AreEqual((size_t)0x08, MngProbe::OffDefault());
		Assert::AreEqual((size_t)0x0C, MngProbe::OffObj());
	}

	TEST_METHOD(Enums)
	{
		Assert::AreEqual(9, (int)SYNDATA_DEL_FLAG);
		Assert::AreEqual(10, (int)SYNDATA_AMOUNT);
		Assert::AreEqual(7, (int)SYNDATA_MONEY);
		Assert::AreEqual(20, (int)SYNDATA_ALLY4);
		Assert::AreEqual(10, (int)SYNFUNC_UP_LEVEL);
	}
};
