#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "protocol.h"
#include "NetworkDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(PROTOCOL_NPCMSG_HEADSIZE == 8, "");
static_assert(offsetof(CProtocolNpcMsgStruct, nMsgSize) == 0, "");
static_assert(offsetof(CProtocolNpcMsgStruct, idPacket) == 2, "");
static_assert(offsetof(CProtocolNpcMsgStruct, idNpc) == 4, "");
static_assert(offsetof(CProtocolNpcMsgStruct, pMsg) == 8, "");

TEST_CLASS(NpcKernelHeaderTests)
{
public:
	TEST_METHOD(NpcPacketRoundTrip)
	{
		const char body[5] = { 1, 2, 3, 4, 5 };
		char buf[MAX_PACKETSIZE];
		int nSize = UniteNpcPacket(buf, sizeof(buf), _MSG_WALK, body, sizeof(body), 400123);
		Assert::AreEqual(13, nSize);
		const BYTE head[8] = { 13, 0, 0xED, 0x03, 0xFB, 0x1A, 0x06, 0x00 };
		Assert::AreEqual(0, memcmp(buf, head, sizeof(head)));

		OBJID idPacket = 0, idNpc = 0; char* pMsg = NULL; int nMsgSize = 0;
		Assert::AreEqual(13, SplitNpcPacket(buf, nSize, &idPacket, &pMsg, &nMsgSize, &idNpc));
		Assert::AreEqual((OBJID)_MSG_WALK, idPacket);
		Assert::AreEqual((OBJID)400123, idNpc);
		Assert::AreEqual(5, nMsgSize);
		Assert::IsTrue(pMsg == buf + 8);
	}

	TEST_METHOD(SplitNpcPacketRefusals)
	{
		char buf[16] = { 0 };
		OBJID idPacket = 0, idNpc = 0; char* pMsg = NULL; int nMsgSize = 0;
		*(unsigned short*)buf = 12;
		Assert::AreEqual(0, SplitNpcPacket(buf, 8, &idPacket, &pMsg, &nMsgSize, &idNpc));
		Assert::AreEqual(0, SplitNpcPacket(buf, 11, &idPacket, &pMsg, &nMsgSize, &idNpc));
		Assert::AreEqual(12, SplitNpcPacket(buf, 16, &idPacket, &pMsg, &nMsgSize, &idNpc));
		*(unsigned short*)buf = 3;
		Assert::AreEqual(0, SplitNpcPacket(buf, 16, &idPacket, &pMsg, &nMsgSize, &idNpc));
		char big[MAX_PACKETSIZE + 8] = { 0 };
		*(unsigned short*)big = MAX_PACKETSIZE + 1;
		Assert::AreEqual(0, SplitNpcPacket(big, sizeof(big), &idPacket, &pMsg, &nMsgSize, &idNpc));
	}

	TEST_METHOD(DirectionTables)
	{
		const int x[9] = { 0,-1,-1,-1, 0, 1, 1, 1, 0 };
		const int y[9] = { 1, 1, 0,-1,-1,-1, 0, 1, 0 };
		Assert::AreEqual(0, memcmp(x, _DELTA_X, sizeof(x)));
		Assert::AreEqual(0, memcmp(y, _DELTA_Y, sizeof(y)));
	}

	TEST_METHOD(ConstantsTheBinaryUses)
	{
		Assert::AreEqual(987654321, NPCSERVER_VERSION);
		Assert::AreEqual(987654322, NPCSERVER_VERSION_RELOGIN);
		Assert::AreEqual((OBJID)400001, MONSTERID_FIRST);
		Assert::AreEqual((OBJID)499999, MONSTERID_LAST);
		Assert::AreEqual((OBJID)299999, SCENEID_LAST);
		Assert::AreEqual((OBJID)1000000, PLAYER_ID_FIRST);
		Assert::AreEqual(1235, (int)OBJ_USER);
		Assert::AreEqual(1239, (int)OBJ_NPC);
		Assert::AreEqual(1242, (int)OBJ_AGENT);
		const unsigned short types[16] = { _MSG_LOGIN, _MSG_TALK, _MSG_WALK, _MSG_USERINFO, _MSG_ITEMINFO, _MSG_ITEM,
			_MSG_ACTION, _MSG_PLAYER, _MSG_USERATTRIB, _MSG_INTERACT, _MSG_TEAM, _MSG_TEAMMEMBER, _MSG_MAGICEFFECT,
			_MSG_SYNDICATE, _MSG_NPC, _MSG_AINPCINFO };
		const unsigned short factory[16] = { 1002, 1004, 1005, 1006, 1008, 1009, 1010, 1014, 1017, 1022, 1023, 1026,
			1105, 1107, 2031, 20001 };
		Assert::AreEqual(0, memcmp(types, factory, sizeof(types)));
		Assert::AreEqual((int)_TXTATR_PRIVATE, 2001);
		Assert::AreEqual((int)_TXTATR_GLOBAL, 2010);
		Assert::AreEqual((int)_TXTATR_ENTRANCE, 2101);
		Assert::AreEqual((int)_TXTATR_WEBPAGE, 2105);
	}
};
