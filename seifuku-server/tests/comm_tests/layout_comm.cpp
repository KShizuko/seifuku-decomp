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
#include "InternetPort.h"
#include "MessagePort.h"

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

static_assert(sizeof(CInternetPort) == 0x148 + (sizeof(CInternetPort::m_setMsg) - STLP_LIST), "");
static_assert(offsetof(CMessagePort, m_setMsg) == 0x0C, "");
static_assert(offsetof(CMessagePort, m_xCtrl) == 0x10 + (sizeof(CMessagePort::m_setMsg) - STLP_LIST), "");
static_assert(offsetof(CMessagePort, m_hHaveMsg) == 0x30 + (sizeof(CMessagePort::m_setMsg) - STLP_LIST), "");
static_assert(offsetof(CMessagePort, m_debugPackets) == 0x34 + (sizeof(CMessagePort::m_setMsg) - STLP_LIST), "");
static_assert(offsetof(CMessagePort, m_setRecycle) == 0x38 + (sizeof(CMessagePort::m_setMsg) - STLP_LIST), "");
static_assert(sizeof(CMessagePort) == 0x60 + (sizeof(CMessagePort::m_setMsg) - STLP_LIST) - (STLP_DEQUE - sizeof(CMessagePort::m_setRecycle)), "");

static_assert(sizeof(SOCKADDR_IN) == 0x10, "");
static_assert(sizeof(sockaddr_in) == 0x10, "");

#endif
