#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgUserAttrib.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgUserAttribProbe : CMsgUserAttrib
{
    typedef CMsgUserAttrib::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgUserAttribProbe, m_pInfo); }
};

static_assert(sizeof(CMsgUserAttrib) == 0x408, "");
static_assert(sizeof(UserAttribStruct) == 0x8, "");
static_assert(offsetof(UserAttribStruct, ucAttributeType) == 0x0 && offsetof(UserAttribStruct, dwAttributeData) == 0x4, "");
static_assert(sizeof(MsgUserAttribProbe::Info) == 0xC + 35 * 8, "");
static_assert(offsetof(MsgUserAttribProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgUserAttribProbe::Info, idUser) == 0x4, "");
static_assert(offsetof(MsgUserAttribProbe::Info, dwAttributeNum) == 0x8, "");
static_assert(offsetof(MsgUserAttribProbe::Info, aUserAttrib) == 0xC, "");
static_assert(_MAX_MSGATRRIB == 35, "");
static_assert(_USERATTRIB_LIFE == 0 && _USERATTRIB_MAXLIFE == 1 && _USERATTRIB_MANA == 2 && _USERATTRIB_MONEY == 4 && _USERATTRIB_EXP == 5 && _USERATTRIB_PK == 6 && _USERATTRIB_PORFESSION == 7 && _USERATTRIB_PP == 9 && _USERATTRIB_ADDPOINT == 11 && _USERATTRIB_LOOK == 12 && _USERATTRIB_LEV == 13, "");
static_assert(_USERATTRIB_SOUL == 14 && _USERATTRIB_HEALTH == 15 && _USERATTRIB_FORCE == 16 && _USERATTRIB_SPEED == 17 && _USERATTRIB_PROFFER == 20 && _USERATTRIB_KO_ORDER == 22 && _USERATTRIB_EXPLOIT == 24 && _USERATTRIB_USERSTATUS == 26 && _USERATTRIB_HAIR == 27 && _USERATTRIB_XP == 28, "");
static_assert(_USERATTRIB_MONEYSAVED == 10 && _USERATTRIB_METEMPSYCHOSIS == 23 && _USERATTRIB_LUCKYTIME == 29, "");
static_assert(_USERATTRIB_SIZEADD == 8 && _USERATTRIB_BLESSTIME == 18 && _USERATTRIB_DBLEXPTIME == 19 && _USERATTRIB_CURSETIME == 21 && _USERATTRIB_EMONEY == 30 && _USERATTRIB_SUPERSOLDIERTAIL == 31 && _USERATTRIB_DAYBONUS == 128, "");
static_assert(std::is_same<decltype(&CMsgUserAttrib::Create), BOOL (CMsgUserAttrib::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgUserAttribHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgUserAttribProbe::InfoPtr() == 0x404);
    }
};
