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
#include "mydb.h"
#include "MyThread.h"
#include "myConstrecord.h"
#include "mydatabase.h"
#include "myrecord.h"
#include "myrecordset.h"

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

static_assert(sizeof(CMyConstRecord) == 0x10, "");
static_assert(sizeof(CMyRecord) == 0x78, "");
static_assert(sizeof(CMyRecordset) == 0xCF8, "");
static_assert(sizeof(CMyThread) == 0x20, "");

static_assert(offsetof(CMyDatabase, m_eSupportMode) == 0x8, "");
static_assert(offsetof(CMyDatabase, m_hdbc) == 0xC, "");
static_assert(offsetof(CMyDatabase, m_nRefCount) == 0x10, "");
static_assert(offsetof(CMyDatabase, m_hDbMutex) == 0x14, "");
static_assert(offsetof(CMyDatabase, m_bIsOpen) == 0x18, "");
static_assert(offsetof(CMyDatabase, m_szDBName) == 0x19, "");
static_assert(offsetof(CMyDatabase, m_pThreadAsyncSql) == 0x3C, "");
static_assert(offsetof(CMyDatabase, m_hdbcAsync) == 0x40, "");
static_assert(offsetof(CMyDatabase, m_xAsyncCtrl) == 0x44, "");
static_assert(offsetof(CMyDatabase, m_setAsyncSql) == 0x60, "");

static_assert(offsetof(CMyRecord, m_pMyDatabase) == 0x4, "");
static_assert(offsetof(CMyRecord, m_Fields) == 0x8, "");
static_assert(offsetof(CMyRecord, m_uiFieldsCount) == 0xC, "");
static_assert(offsetof(CMyRecord, m_szKey) == 0x10, "");
static_assert(offsetof(CMyRecord, m_uiKeyIndex) == 0x20, "");
static_assert(offsetof(CMyRecord, m_szKeyData) == 0x24, "");
static_assert(offsetof(CMyRecord, m_pbDirty) == 0x44, "");
static_assert(offsetof(CMyRecord, m_bEdit) == 0x48, "");
static_assert(offsetof(CMyRecord, m_bIsOpen) == 0x49, "");
static_assert(offsetof(CMyRecord, m_nEditMode) == 0x4C, "");
static_assert(offsetof(CMyRecord, m_objFields) == 0x50, "");
static_assert(offsetof(CMyRecord, m_szTableName) == 0x54, "");
static_assert(offsetof(CMyRecord, m_bCanUpdate) == 0x74, "");

static_assert(offsetof(CMyThread, m_dwWorkInterval) == 0x4, "");
static_assert(offsetof(CMyThread, m_pEvent) == 0x8, "");
static_assert(offsetof(CMyThread, m_hThread) == 0xC, "");
static_assert(offsetof(CMyThread, m_hCloseEvent) == 0x10, "");
static_assert(offsetof(CMyThread, m_hWorkEvent) == 0x14, "");
static_assert(offsetof(CMyThread, m_id) == 0x18, "");
static_assert(offsetof(CMyThread, m_nStatus) == 0x1C, "");

#endif
