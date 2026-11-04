#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "FloatingStr.h"
#include "ZoomNum.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(FStrInfo) == 0x20, "");
static_assert(offsetof(FStrInfo, szStr) == 0 && offsetof(FStrInfo, nMaxHeight) == 0x10 && offsetof(FStrInfo, nHeight) == 0x14, "");
static_assert(offsetof(FStrInfo, nStepHeight) == 0x18 && offsetof(FStrInfo, dwColor) == 0x1C, "");
static_assert(_MAX_NAMESIZE == 16, "");
static_assert(std::is_same<decltype(((CFloatingStr*) 0)->AddStr((char*) 0, 0, 0, 0)), void>::value, "");
static_assert(sizeof(CFloatingStr) == sizeof(void*) + sizeof(DEQUE_FSTRINFO), "");

static_assert(sizeof(ZoomNumInfo) == 0x18, "");
static_assert(offsetof(ZoomNumInfo, nNum) == 0 && offsetof(ZoomNumInfo, dwTimeBegin) == 4 && offsetof(ZoomNumInfo, dwColor) == 8, "");
static_assert(offsetof(ZoomNumInfo, dwDelay) == 0xC && offsetof(ZoomNumInfo, nOffsetX) == 0x10 && offsetof(ZoomNumInfo, pImgString) == 0x14, "");
static_assert(std::is_same<decltype(((CZoomNum*) 0)->Add(0, 0, 0, 0, 0)), void>::value, "");
static_assert(std::is_same<decltype(&CZoomNum::Process), void (CZoomNum::*)()>::value, "");
static_assert(std::is_same<decltype(&CZoomNum::Show), void (CZoomNum::*)(CMyPos)>::value, "");
static_assert(sizeof(CZoomNum) == sizeof(void*) + sizeof(DEQUE_ZOOMNUMINFO), "");

TEST_CLASS(ClientFloatingHeaders)
{
public:
    TEST_METHOD(FloatingStringHoldsAtMostFifteenCharacters)
    {
        FStrInfo info;
        Assert::IsTrue(sizeof(info.szStr) == _MAX_NAMESIZE);
        Assert::IsTrue(sizeof(FStrInfo) == sizeof(info.szStr) + 4 * sizeof(DWORD));
    }
};
