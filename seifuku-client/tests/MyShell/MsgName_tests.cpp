#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgName.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgNameProbe : CMsgName
{
    typedef CMsgName::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgNameProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgNameProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgName) == 0x414, "");
static_assert(sizeof(MsgNameProbe::Info) == 0xC, "");
static_assert(offsetof(MsgNameProbe::Info, dwData) == 0x4 && offsetof(MsgNameProbe::Info, idTarget) == 0x4, "");
static_assert(offsetof(MsgNameProbe::Info, usPosX) == 0x4 && offsetof(MsgNameProbe::Info, usPosY) == 0x6, "");
static_assert(offsetof(MsgNameProbe::Info, ucType) == 0x8, "");
static_assert(offsetof(MsgNameProbe::Info, szBuf) == 0x9, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgName::*)(int, const char*, DWORD)>(&CMsgName::Create)), BOOL (CMsgName::*)(int, const char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgName::*)(char*, DWORD)>(&CMsgName::Create)), BOOL (CMsgName::*)(char*, DWORD)>::value, "");
static_assert(NAMEACT_MEMBERLIST == 11 && NAMEACT_QUERY_WANTED == 13 && NAMEACT_QUERY_POLICEWANTED == 14 && NAMEACT_QUERY_MATE == 16, "");
static_assert(NAMEACT_ADDDICE_PLAYER == 17 && NAMEACT_DELDICE_PLAYER == 18, "");
static_assert(NAMEACT_FIREWORKS == 1 && NAMEACT_CHANGE_SYN == 3 && NAMEACT_MATE == 6 && NAMEACT_WANTED == 8 && NAMEACT_MAPEFFECT == 9 && NAMEACT_ROLEEFFECT == 10 && NAMEACT_POLICEWANTED == 15, "");
static_assert(NAMEACT_DICE_BONUS == 19 && NAMEACT_PLAYER_WAVE == 20 && NAMEACT_SYN_FRIEND == 21 && NAMEACT_SYN_HOSTILE == 22 && NAMEACT_CHECK_WINDOW == 23, "");
static_assert(NAMEACT_DELROLEEFFECT == 24 && NAMEACT_PATTERNROSE == 25, "");

TEST_CLASS(ClientMsgNameHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgNameProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgNameProbe::Packer() == 0x408);
    }
};
