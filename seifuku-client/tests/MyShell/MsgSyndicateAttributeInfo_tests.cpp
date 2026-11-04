#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgSyndicateAttributeInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgSynAttrProbe : CMsgSyndicateAttributeInfo
{
    typedef CMsgSyndicateAttributeInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgSynAttrProbe, m_pInfo); }
};

static_assert(sizeof(CMsgSyndicateAttributeInfo) == 0x408, "");
static_assert(sizeof(MsgSynAttrProbe::Info) == 0x28, "");
static_assert(offsetof(MsgSynAttrProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgSynAttrProbe::Info, idSyndicate) == 0x4, "");
static_assert(offsetof(MsgSynAttrProbe::Info, nProffer) == 0x8, "");
static_assert(offsetof(MsgSynAttrProbe::Info, dwSyndicateFund) == 0xC, "");
static_assert(offsetof(MsgSynAttrProbe::Info, dwSyndicatePopulation) == 0x10, "");
static_assert(offsetof(MsgSynAttrProbe::Info, ucRank) == 0x14, "");
static_assert(offsetof(MsgSynAttrProbe::Info, szLeader) == 0x15, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgSyndicateAttributeInfo::*)(char*, DWORD)>(&CMsgSyndicateAttributeInfo::Create)), BOOL (CMsgSyndicateAttributeInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgSyndicateAttributeInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgSynAttrProbe::InfoPtr() == 0x404);
    }
};
