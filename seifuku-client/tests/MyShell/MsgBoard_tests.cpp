#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgBoard.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgBoardProbe : CMsgMessageBoard
{
    typedef CMsgMessageBoard::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgBoardProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgBoardProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgMessageBoard) == 0x414, "");
static_assert(sizeof(MsgBoardProbe::Info) == 0xA, "");
static_assert(offsetof(MsgBoardProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgBoardProbe::Info, usIndex) == 0x4, "");
static_assert(offsetof(MsgBoardProbe::Info, usChannel) == 0x6, "");
static_assert(offsetof(MsgBoardProbe::Info, ucAction) == 0x8, "");
static_assert(offsetof(MsgBoardProbe::Info, szBuf) == 0x9, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMessageBoard::*)(unsigned short, unsigned char, unsigned short, char*)>(&CMsgMessageBoard::Create)), BOOL (CMsgMessageBoard::*)(unsigned short, unsigned char, unsigned short, char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgMessageBoard::*)(char*, DWORD)>(&CMsgMessageBoard::Create)), BOOL (CMsgMessageBoard::*)(char*, DWORD)>::value, "");
static_assert(MESSAGEBOARD_DEL == 1 && MESSAGEBOARD_GETLIST == 2 && MESSAGEBOARD_LIST == 3 && MESSAGEBOARD_GETWORDS == 4, "");

TEST_CLASS(ClientMsgBoardHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgBoardProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgBoardProbe::Packer() == 0x408);
    }
};
