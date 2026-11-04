#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
#include "Item.h"
#include "NetMsg.h"
#include "MsgAction.h"
#include "MsgItem.h"
#define private public
#include "MsgTalk.h"
#include "MsgTeam.h"
#undef private

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

struct ActionLayout : public CMsgAction
{
	typedef MSG_Info	Info;
	static size_t	OffPtr()	{ return offsetof(ActionLayout, m_pInfo); }
};
struct TalkLayout : public CMsgTalk
{
	typedef MSG_Info	Info;
};
struct ItemLayout : public CMsgItem
{
	typedef MSG_Info	Info;
};
struct TeamLayout : public CMsgTeam
{
	typedef MSG_Info	Info;
};

static_assert(sizeof(ActionLayout::Info) == 0x14, "");
static_assert(sizeof(ItemLayout::Info) == 0x10, "");
static_assert(sizeof(TeamLayout::Info) == 8, "");
static_assert(sizeof(TalkLayout::Info) == 0x10, "");
static_assert(sizeof(CMsgAction) == 0x414 && sizeof(CMsgItem) == 0x414 && sizeof(CMsgTeam) == 0x414, "");
static_assert(sizeof(CMsgTalk) == 0x420, "");

TEST_CLASS(MsgLayoutTests)
{
public:
	TEST_METHOD(ActionFieldsInConquersOrder)
	{
		Assert::AreEqual((size_t)0x00, offsetof(ActionLayout::Info, dwTimeStamp));
		Assert::AreEqual((size_t)0x04, offsetof(ActionLayout::Info, idUser));
		Assert::AreEqual((size_t)0x08, offsetof(ActionLayout::Info, dwData));
		Assert::AreEqual((size_t)0x08, offsetof(ActionLayout::Info, usTargetPosX));
		Assert::AreEqual((size_t)0x0A, offsetof(ActionLayout::Info, usTargetPosY));
		Assert::AreEqual((size_t)0x0C, offsetof(ActionLayout::Info, unPosX));
		Assert::AreEqual((size_t)0x0E, offsetof(ActionLayout::Info, unPosY));
		Assert::AreEqual((size_t)0x10, offsetof(ActionLayout::Info, unDir));
		Assert::AreEqual((size_t)0x12, offsetof(ActionLayout::Info, usAction));
		Assert::AreEqual((size_t)0x410, ActionLayout::OffPtr());
	}
	TEST_METHOD(TalkStringsAfterTheTime)
	{
		Assert::AreEqual((size_t)0x04, offsetof(TalkLayout::Info, unTxtAttribute));
		Assert::AreEqual((size_t)0x08, offsetof(TalkLayout::Info, dwTime));
		Assert::AreEqual((size_t)0x0C, offsetof(TalkLayout::Info, szBuf));
	}
	TEST_METHOD(ItemAndTeamFields)
	{
		Assert::AreEqual((size_t)0x04, offsetof(ItemLayout::Info, dwData));
		Assert::AreEqual((size_t)0x08, offsetof(ItemLayout::Info, usAction));
		Assert::AreEqual((size_t)0x00, offsetof(TeamLayout::Info, unAction));
		Assert::AreEqual((size_t)0x04, offsetof(TeamLayout::Info, idPlayer));
	}
	TEST_METHOD(ActionValuesTheImageUses)
	{
		Assert::AreEqual(74, (int)actionEnterMap);
		Assert::AreEqual(97, (int)actionGetSynAttr);
		Assert::AreEqual(94, (int)actionReborn);
		Assert::AreEqual(138, (int)actionSynchro);
		Assert::AreEqual(200, (int)actionSuperChgMap);
		Assert::AreEqual(25, (int)ITEMACT_SYNCHRO_AMOUNT);
	}
};
