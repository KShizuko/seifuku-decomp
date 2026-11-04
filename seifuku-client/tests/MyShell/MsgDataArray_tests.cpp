#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgDataArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgDataArrayProbe : CMsgDataArray
{
    typedef CMsgDataArray::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgDataArrayProbe, m_pInfo); }
};

static_assert(sizeof(CMsgDataArray) == 0x408, "");
static_assert(sizeof(MsgDataArrayProbe::Info) == 0xC, "");
static_assert(offsetof(MsgDataArrayProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgDataArrayProbe::Info, ucAction) == 0x4, "");
static_assert(offsetof(MsgDataArrayProbe::Info, ucAmount) == 0x5, "");
static_assert(offsetof(MsgDataArrayProbe::Info, dwLong) == 0x8 && offsetof(MsgDataArrayProbe::Info, ucChar) == 0x8 && offsetof(MsgDataArrayProbe::Info, usShort) == 0x8, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgDataArray::*)(DWORD, DWORD, DWORD, DWORD, DWORD)>(&CMsgDataArray::Create)), BOOL (CMsgDataArray::*)(DWORD, DWORD, DWORD, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgDataArray::*)(char*, DWORD)>(&CMsgDataArray::Create)), BOOL (CMsgDataArray::*)(char*, DWORD)>::value, "");
static_assert(_DATAARRAY_ACTION_MEDAL == 0, "");
static_assert(_DATAARRAY_ACTION_TABLE == 75, "");

TEST_CLASS(ClientMsgDataArrayHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgDataArrayProbe::InfoPtr() == 0x404);
    }
};
