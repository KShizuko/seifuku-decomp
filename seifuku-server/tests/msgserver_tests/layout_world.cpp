#include <winsock2.h>
#include <windows.h>
#include <stddef.h>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#define private public
#define protected public
#include "inifile.h"
#include "protocol.h"
#include "BaseFunc.h"
#include "I_MessagePort.h"
#include "AllMsg.h"
#include "NetMsg.h"
#include "ConnectUser.h"
#include "MapList.h"
#include "MessageBoard.h"
#include "NETWORK2/MsgAccount_A.h"
#include "NETWORK2/MsgAiNpcInfo.h"
#include "NETWORK2/MsgConnect.h"
#include "NETWORK2/MsgDialog.h"
#include "NETWORK2/MsgFriend.h"
#include "NETWORK2/MsgLogin.h"
#include "NETWORK2/MsgMessageBoard.h"
#include "NETWORK2/MsgName.h"
#include "NETWORK2/MsgRegister.h"
#include "NETWORK2/MsgTalk.h"
#include "NETWORK2/MsgTime.h"
#include "SupermanList.h"
#include "SynWorldShell.h"
#include "UserList.h"
#include "WorldKernel.h"

#ifdef NDEBUG

namespace
{
	const size_t STLP_MAP		= 0x0C;
	const size_t STLP_LIST		= 0x04;
	const size_t STLP_DEQUE		= 0x28;
	const size_t STLP_STRING	= 0x0C;
	const size_t STLP_VECBOOL	= 0x14;
	const size_t VC6_STRING		= 0x10;
}

static_assert(sizeof(CConnectUser) == 0x28, "");
static_assert(sizeof(CMapList) == 0x10, "");
static_assert(sizeof(CMsgAiNpcInfo) == 0x420, "");
static_assert(sizeof(CMsgConnect) == 0x420, "");
static_assert(sizeof(CMsgDialog) == 0x42C, "");
static_assert(sizeof(CMsgFriend) == 0x420, "");
static_assert(sizeof(CMsgLogin) == 0x420, "");
static_assert(sizeof(CMsgMessageBoard) == 0x42C, "");
static_assert(sizeof(CMsgName) == 0x42C, "");
static_assert(sizeof(CMsgRegister) == 0x420, "");
static_assert(sizeof(CMsgTalk) == 0x42C, "");
static_assert(sizeof(CMsgTime) == 0x420, "");
static_assert(sizeof(CSupermanList) == 0x10, "");
static_assert(sizeof(CSyndicateWorld) == 0x18, "");

static_assert(offsetof(CMsgAccount_A::MSG_Info, nServerPort) == 0x34, "");

static_assert(offsetof(CMsgConnect::MSG_Info, usVersion) == 0x8, "");
static_assert(offsetof(CMsgConnect::MSG_Info, szLanguage) == 0xA, "");
static_assert(offsetof(CMsgConnect::MSG_Info, bufClient) == 0xC, "");
static_assert(offsetof(CMsgConnect::MSG_Info, usReserved12) == 0x12, "");
static_assert(offsetof(CMsgConnect::MSG_Info, dwResData) == 0x14, "");

static_assert(offsetof(CPlayer, m_nReserved) == 0x2C, "");

static_assert(offsetof(CUserList, m_setAccountLock) == 0x30, "");

static_assert(sizeof(CMessageBoard) == 0x34 - (STLP_DEQUE - sizeof(CMessageBoard::m_setMsg)), "");
static_assert(offsetof(CMessageBoard, m_nChannel) == 0x2C - (STLP_DEQUE - sizeof(CMessageBoard::m_setMsg)), "");
static_assert(offsetof(CMessageBoard, m_nSize) == 0x30 - (STLP_DEQUE - sizeof(CMessageBoard::m_setMsg)), "");

static_assert(sizeof(CUserList) == 0x60 - 3 * (STLP_MAP - sizeof(CUserList::m_setMinorFlag)), "");
static_assert(offsetof(CUserList, m_setClientVersion) == 0x3C - 1 * (STLP_MAP - sizeof(CUserList::m_setMinorFlag)), "");
static_assert(offsetof(CUserList, m_setMinorFlag) == 0x48 - 2 * (STLP_MAP - sizeof(CUserList::m_setMinorFlag)), "");
static_assert(offsetof(CUserList, m_setGmIP) == 0x54 - 3 * (STLP_MAP - sizeof(CUserList::m_setMinorFlag)), "");

static_assert(sizeof(CWorldKernel) == 0x1CAC - (STLP_MAP - sizeof(CWorldKernel::m_setSocketIP)), "");

static_assert(sizeof(ST_CREATENEWNPC) == 0x20, "");

#endif
