#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "airbubble.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct AirBubbleProbe : CAirBubble
{
    static size_t Status() { return offsetof(AirBubbleProbe, m_iStatus); }
    static size_t World() { return offsetof(AirBubbleProbe, m_iWorldX); }
    static size_t File() { return offsetof(AirBubbleProbe, m_szFileName); }
    static size_t Show() { return offsetof(AirBubbleProbe, m_nShowWorldPoint); }
    static size_t Orig() { return offsetof(AirBubbleProbe, m_nOrigWorldPoint); }
    static size_t Delay() { return offsetof(AirBubbleProbe, m_dwDelayTime); }
};
struct AirBubbleSysProbe : CAirBubbleSys
{
    static size_t Bubbles() { return offsetof(AirBubbleSysProbe, m_dequeBubble); }
    static size_t Files() { return offsetof(AirBubbleSysProbe, m_szFileName); }
    static size_t Status() { return offsetof(AirBubbleSysProbe, m_nStatus); }
    static size_t Created() { return offsetof(AirBubbleSysProbe, m_dwCreateTime); }
};

static_assert(sizeof(CAirBubble) == 0x9C, "");
static_assert(std::is_base_of<CMapObj, CAirBubble>::value, "");
static_assert(sizeof(CAirBubbleSys) == 0xC + sizeof(DequeBubble) + BUBBLE_TYPE * 100 + 0xC, "");
static_assert(std::is_base_of<CMapObj, CAirBubbleSys>::value, "");
static_assert(BUBBLE_TYPE == 7 && BUBBLE_FRAME == 1, "");
static_assert(std::is_same<decltype(&CAirBubble::Create), BOOL (CAirBubble::*)(DWORD, int, int)>::value, "");
static_assert(std::is_same<decltype(&CAirBubbleSys::Show), void (CAirBubbleSys::*)()>::value, "");
static_assert(std::is_same<decltype(&CAirBubbleSys::Destroy), void (CAirBubbleSys::*)()>::value, "");
static_assert(std::is_same<decltype(&CAirBubble::GetPos), void (CAirBubble::*)(int&, int&)>::value, "");
static_assert(std::is_same<decltype(&CAirBubbleSys::AddBubble), void (CAirBubbleSys::*)(int, int)>::value && std::is_same<decltype(&CAirBubbleSys::Create), BOOL (CAirBubbleSys::*)(int, int)>::value, "");

TEST_CLASS(ClientAirBubbleHeader)
{
public:
    TEST_METHOD(BubbleMembersFollowTheMapObject)
    {
        Assert::IsTrue(AirBubbleProbe::Status() == 0xC);
        Assert::IsTrue(AirBubbleProbe::World() == 0x10);
        Assert::IsTrue(AirBubbleProbe::File() == 0x18);
        Assert::IsTrue(AirBubbleProbe::Show() == 0x7C);
        Assert::IsTrue(AirBubbleProbe::Orig() == 0x84);
        Assert::IsTrue(AirBubbleProbe::Delay() == 0x98);
    }
    TEST_METHOD(SystemMembersFollowTheMapObject)
    {
        const size_t d = sizeof(DequeBubble);
        Assert::IsTrue(AirBubbleSysProbe::Bubbles() == 0xC);
        Assert::IsTrue(AirBubbleSysProbe::Files() == 0xC + d);
        Assert::IsTrue(AirBubbleSysProbe::Status() == 0xC + d + BUBBLE_TYPE * 100);
        Assert::IsTrue(AirBubbleSysProbe::Created() == 0xC + d + BUBBLE_TYPE * 100 + 8);
    }
};
