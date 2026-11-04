#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgPetInfo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgPetInfoProbe : CMsgPetInfo
{
    typedef CMsgPetInfo::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgPetInfoProbe, m_pInfo); }
};

static_assert(sizeof(CMsgPetInfo) == 0x408, "");
static_assert(sizeof(MsgPetInfoProbe::Info) == 0x24, "");
static_assert(offsetof(MsgPetInfoProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgPetInfoProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgPetInfoProbe::Info, dwLookFace) == 0x8, "");
static_assert(offsetof(MsgPetInfoProbe::Info, nAIType) == 0xC, "");
static_assert(offsetof(MsgPetInfoProbe::Info, usPosX) == 0x10 && offsetof(MsgPetInfoProbe::Info, usPosY) == 0x12, "");
static_assert(offsetof(MsgPetInfoProbe::Info, szName) == 0x14 && sizeof(MsgPetInfoProbe::Info().szName) == 16, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgPetInfo::*)(char*, DWORD)>(&CMsgPetInfo::Create)), BOOL (CMsgPetInfo::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgPetInfoHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgPetInfoProbe::InfoPtr() == 0x404);
    }
};
