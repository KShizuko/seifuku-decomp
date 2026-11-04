#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgDice.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgDiceProbe : CMsgDice
{
    typedef CMsgDice::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgDiceProbe, m_pInfo); }
};

static_assert(sizeof(CMsgDice) == 0x408, "");
static_assert(sizeof(MsgDiceProbe::Info) == 0x14, "");
static_assert(offsetof(MsgDiceProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgDiceProbe::Info, ucAction) == 0x4, "");
static_assert(offsetof(MsgDiceProbe::Info, ucAmount) == 0x5 && offsetof(MsgDiceProbe::Info, ucTime) == 0x5, "");
static_assert(offsetof(MsgDiceProbe::Info, idDice) == 0x8, "");
static_assert(offsetof(MsgDiceProbe::Info, Data) == 0xC && sizeof(MsgDiceProbe::Info().Data[0]) == 0x8, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgDice::*)(OBJID, int, DWORD, int)>(&CMsgDice::Create)), BOOL (CMsgDice::*)(OBJID, int, DWORD, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgDice::*)(char*, DWORD)>(&CMsgDice::Create)), BOOL (CMsgDice::*)(char*, DWORD)>::value, "");
static_assert(_ACTION_CHIPIN == 0 && _ACTION_CANCELCHIP == 2 && _ACTION_DICE == 6, "");

TEST_CLASS(ClientMsgDiceHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgDiceProbe::InfoPtr() == 0x404);
    }
};
