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
#include "define.h"
#include "protocol.h"
#include "EncryptServer.h"
#include "SSocket.h"
#include "I_Shell.h"
#include "I_MessagePort.h"
#include "SSocket.h"
#include "ServerSocket.h"

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

static_assert(offsetof(CServerSocket, m_sock) == 0x4, "");
static_assert(offsetof(CServerSocket, m_bufMsg) == 0x8, "");
static_assert(offsetof(CServerSocket, m_nLen) == 0x1008, "");
static_assert(offsetof(CServerSocket, m_bLocal) == 0x100C, "");
static_assert(sizeof(CServerSocket) == 0x1420, "");

static_assert(offsetof(CSocketKernel, m_setNetBreakFlag) == 0x34, "");
static_assert(offsetof(CSocketKernel, m_pMsgPort) == 0x48 - (STLP_VECBOOL - sizeof(CSocketKernel::m_setNetBreakFlag)), "");
static_assert(offsetof(CSocketKernel, m_idNpcSocket) == 0x4C - (STLP_VECBOOL - sizeof(CSocketKernel::m_setNetBreakFlag)), "");
static_assert(offsetof(CSocketKernel, m_setNpcProcessID) == 0x50 - (STLP_VECBOOL - sizeof(CSocketKernel::m_setNetBreakFlag)), "");
static_assert(sizeof(CSocketKernel) == 0x5C - (STLP_VECBOOL - sizeof(CSocketKernel::m_setNetBreakFlag)) - (STLP_MAP - sizeof(CSocketKernel::m_setNpcProcessID)), "");

static_assert(sizeof(OSVERSIONINFOEX) == 0x9C, "");
static_assert(sizeof(STAT_STRUCT) == 0xB4, "");
static_assert(sizeof(ST_SOCKETIP) == 0x8, "");
static_assert(sizeof(SYSTEM_INFO) == 0x24, "");
static_assert(sizeof(sockaddr_in) == 0x10, "");

#endif
