#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
#include "protocol.h"
#include "networkdef.h"
#include "I_Shell.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(PROTOCOL_MSG_HEADSIZE == 4 && PROTOCOL_MSG_SEALSIZE == 8, "");
static_assert(offsetof(CProtocolMsgStruct, idPacket) == 2 && offsetof(CProtocolMsgStruct, pMsg) == 4, "");
static_assert(PROTOCOL_NPCMSG_HEADSIZE == 8, "");
static_assert(offsetof(CProtocolNpcMsgStruct, idNpc) == 4 && offsetof(CProtocolNpcMsgStruct, pMsg) == 8, "");

static_assert(offsetof(CLIENTMSG_PACKET0, idPacket) == 4 && offsetof(CLIENTMSG_PACKET0, nSize) == 8
		&& offsetof(CLIENTMSG_PACKET0, buf) == 0x0C, "");
static_assert(offsetof(NPCMSG_PACKET0, idPacket) == 4 && offsetof(NPCMSG_PACKET0, buf) == 0x0C, "");
static_assert(offsetof(SENDCLIENTMSG_PACKET0, nSize) == 8 && offsetof(SENDCLIENTMSG_PACKET0, buf) == 0x0C, "");
static_assert(offsetof(SENDNPCMSG_PACKET0, nSize) == 8 && offsetof(SENDNPCMSG_PACKET0, buf) == 0x0C, "");
static_assert(offsetof(CHANGE_USERDATA, nData) == 4 && offsetof(CHANGE_NPCPROCESSID, idProcess) == 4, "");
static_assert(sizeof(ST_QUERYIP) == 8 && offsetof(ST_QUERYIP, idSocket) == 4, "");
static_assert(sizeof(ST_SOCKETIP) == 8 && offsetof(ST_SOCKETIP, dwIP) == 4, "");

static_assert(MAX_PACKETSIZE == 0x400 && MAX_MESSAGESIZE == 0x418, "");
static_assert(SERVER_LISTEN_PORT == 5816 && SOCKET_SENDBUFSIZE == 0x10000 && SOCKET_NPCBUFSIZE == 0x20000, "");
static_assert(NPCSERVER_VERSION == 0x3ADE68B1 && NPCSERVER_VERSION_RELOGIN == 0x3ADE68B2, "");
static_assert(KERNEL_CLIENTMSG == 1001 && KERNEL_NPCMSG == 1002 && KERNEL_CLOSEKERNEL == 1003, "");
static_assert(SOCKET_CLOSESOCKET == 30001 && SOCKET_SENDNPCMSG == 30004 && SOCKET_SETNPCPROCESSID == 30006
		&& SOCKET_CHANGECODE == 30007 && SOCKET_BREAKCONNECT == 30008 && SOCKET_BREAKALLCONNECT == 30011
		&& SOCKET_QUERYIP == 30012, "");
static_assert(MAPGROUP_DELALLMONSTER == 10016 && MAPGROUP_SOCKETIP == 10021 && SHELL_PRINTTEXT == 40001, "");
static_assert(_MSG_CONNECT == 1052 && _MSG_AINPCINFO == 20001 && _MSG_ITEMATTRIB == 2045 && _TXTATR_ENTRANCE == 2101, "");

TEST_CLASS(MsgServerHeaderTests)
{
public:
	TEST_METHOD(ClientPacketRoundTrip)
	{
		const char body[6] = { 1, 2, 3, 4, 5, 6 };
		char buf[MAX_PACKETSIZE];
		int nSize = UnitePacket(buf, sizeof(buf), _MSG_CONNECT, body, sizeof(body));
		Assert::AreEqual(10, nSize);
		const BYTE head[4] = { 10, 0, 0x1C, 0x04 };
		Assert::AreEqual(0, memcmp(buf, head, sizeof(head)));

		OBJID idPacket = 0; char* pMsg = NULL; int nMsgSize = 0;
		Assert::AreEqual(10, SplitPacket(buf, nSize, &idPacket, &pMsg, &nMsgSize, false));
		Assert::AreEqual((OBJID)_MSG_CONNECT, idPacket);
		Assert::AreEqual(6, nMsgSize);
		Assert::IsTrue(pMsg == buf + 4);
		Assert::AreEqual(0, SplitPacket(buf, nSize - 1, &idPacket, &pMsg, &nMsgSize, false), L"short buffer");
	}

	TEST_METHOD(ClientPacketSeal)
	{
		char buf[32] = { 6, 0, 0x1C, 0x04, 7, 8 };
		OBJID idPacket = 0; char* pMsg = NULL; int nMsgSize = 0;
		Assert::AreEqual(0, SplitPacket(buf, 6, &idPacket, &pMsg, &nMsgSize, true), L"the seal must fit");
		memcpy(buf + 6, "TQClient", 8);
		Assert::AreEqual(6, SplitPacket(buf, 14, &idPacket, &pMsg, &nMsgSize, true));
		buf[13] = 'x';
		Assert::AreEqual(-1, SplitPacket(buf, 14, &idPacket, &pMsg, &nMsgSize, true));
		buf[0] = 3; buf[1] = 0;
		Assert::AreEqual(-1, SplitPacket(buf, 14, &idPacket, &pMsg, &nMsgSize, false));
		buf[0] = 0x01; buf[1] = 0x04;
		Assert::AreEqual(-1, SplitPacket(buf, 14, &idPacket, &pMsg, &nMsgSize, false));
	}

	TEST_METHOD(NpcPacketRoundTrip)
	{
		const char body[3] = { 9, 8, 7 };
		char buf[MAX_PACKETSIZE];
		int nSize = UniteNpcPacket(buf, sizeof(buf), _MSG_AINPCINFO, body, sizeof(body), 400001);
		Assert::AreEqual(11, nSize);
		OBJID idPacket = 0, idNpc = 0; char* pMsg = NULL; int nMsgSize = 0;
		Assert::AreEqual(11, SplitNpcPacket(buf, nSize, &idPacket, &pMsg, &nMsgSize, &idNpc));
		Assert::AreEqual((OBJID)_MSG_AINPCINFO, idPacket);
		Assert::AreEqual((OBJID)400001, idNpc);
		Assert::AreEqual(3, nMsgSize);
		Assert::AreEqual(0, UniteNpcPacket(buf, sizeof(buf), 1, body, MAX_PACKETSIZE - 7, 0), L"size + 8 > 1024");
	}
};
