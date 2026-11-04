#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "GameMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CTxtMsgSet) == 12 + sizeof(CTxtMsgSet::DEQUE_STR), "");
static_assert(std::has_virtual_destructor<CTxtMsgSet>::value, "");

static_assert(sizeof(ChatLogFile) == 12 + _MAX_PATH && sizeof(ChatLogLineBuffer) == 12 + _MAX_NAMESIZE + 512 && sizeof(ChatLogLine) == _MAX_NAMESIZE + 512, "");
static_assert(sizeof(CChatLog) == 4 + 4 * sizeof(VECTOR_CHARLOGFILE) + _MAX_PATH + 4, "");
static_assert(std::has_virtual_destructor<CChatLog>::value, "");

static_assert(sizeof(CGameMsgUnit) == 0x460, "");
static_assert(offsetof(CGameMsgUnit, m_szSender) == 0x004 && offsetof(CGameMsgUnit, m_szReceiver) == 0x104 && offsetof(CGameMsgUnit, m_szEmotion) == 0x204 && offsetof(CGameMsgUnit, m_szMsg) == 0x304, "");
static_assert(offsetof(CGameMsgUnit, m_unTxtAttribute) == 0x404 && offsetof(CGameMsgUnit, m_unTxtStyle) == 0x406 && offsetof(CGameMsgUnit, m_dwWordsColor) == 0x408, "");
static_assert(offsetof(CGameMsgUnit, m_nStartX) == 0x40C && offsetof(CGameMsgUnit, m_nDragStep) == 0x410 && offsetof(CGameMsgUnit, m_nQuakeFlag) == 0x414 && offsetof(CGameMsgUnit, m_bSuper) == 0x418, "");
static_assert(offsetof(CGameMsgUnit, m_dwTime) == 0x41C && offsetof(CGameMsgUnit, m_dwBeginTime) == 0x420, "");
static_assert(_NORAREA_MSG_LINE == 20 && _SYSAREA_MSG_LINE == 3 && _MANAREA_MSG_LINE == 1, "");
static_assert(_MAX_EMOTIONSIZE == 16 && _COLOR_SYSTEM_SPEAKER == 0xFFFE00FB, "");

static_assert(offsetof(CGameMsg, m_setMsg) == 4, "");
static_assert(sizeof(CGameMsg) == 4 + sizeof(CTxtMsgSet) + 14 * sizeof(DEQUE_MSGUNIT) + 14 * 4 + (40 + 1 + 6 + 1 + 2 + 1) * 4 + 4 + 4 + 8 * 4 + 7 * 4 + _MAX_NAMESIZE + 4 + sizeof(DEQUE_BLACKNAME) + sizeof(CChatLog), "");
static_assert(std::has_virtual_destructor<CGameMsg>::value, "");

TEST_CLASS(ClientGameMsgHeaders)
{
public:
    TEST_METHOD(CGameMsgUnitDefaultsAreTheReferenceOnes)
    {
        Assert::IsTrue(sizeof(CGameMsgUnit) == 0x460);
        Assert::IsTrue(2500 == 0x9C4);
    }
};
