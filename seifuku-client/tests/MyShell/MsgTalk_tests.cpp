#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTalk.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTalkProbe : CMsgTalk
{
    typedef CMsgTalk::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTalkProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgTalkProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgTalk) == 0x414, "");
static_assert(sizeof(MsgTalkProbe::Info) == 0x14, "");
static_assert(offsetof(MsgTalkProbe::Info, dwWordsColor) == 0x4, "");
static_assert(offsetof(MsgTalkProbe::Info, unTxtAttribute) == 0x8, "");
static_assert(offsetof(MsgTalkProbe::Info, unTxtStyle) == 0xA, "");
static_assert(offsetof(MsgTalkProbe::Info, dwTime) == 0xC, "");
static_assert(offsetof(MsgTalkProbe::Info, szBuf) == 0x10, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTalk::*)(char*, char*, char*, char*, DWORD, USHORT, USHORT)>(&CMsgTalk::Create)), BOOL (CMsgTalk::*)(char*, char*, char*, char*, DWORD, USHORT, USHORT)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTalk::*)(char*, DWORD)>(&CMsgTalk::Create)), BOOL (CMsgTalk::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgTalkHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgTalkProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgTalkProbe::Packer() == 0x408);
    }
};
