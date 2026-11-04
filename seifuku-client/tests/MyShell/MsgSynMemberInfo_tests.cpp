#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgSynMemberInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgSynMemberInfoProbe : CMsgSynMemberInfo
{
    typedef CMsgSynMemberInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgSynMemberInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgSynMemberInfo) == 0x408, "");
static_assert(sizeof(MsgSynMemberInfoProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgSynMemberInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgSynMemberInfoProbe::Info, nProffer) == 0x4, "");
static_assert(offsetof(MsgSynMemberInfoProbe::Info, ucRank) == 0x8, "");
static_assert(offsetof(MsgSynMemberInfoProbe::Info, szName) == 0x9 && sizeof(MsgSynMemberInfoProbe::Info().szName) == 16, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgSynMemberInfo::*)(char*)>(&CMsgSynMemberInfo::Create)), BOOL (CMsgSynMemberInfo::*)(char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgSynMemberInfo::*)(char*, DWORD)>(&CMsgSynMemberInfo::Create)), BOOL (CMsgSynMemberInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgSynMemberInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgSynMemberInfoProbe::InfoPtr() == 0x404);
    }
};
