#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "weather.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

#define ROUND8(n) (((n) + 7) & ~(size_t)7)

struct WeatherProbe : CWeather
{
    static size_t Snow() { return offsetof(WeatherProbe, m_objSnow); }
    static size_t Rain() { return offsetof(WeatherProbe, m_objRain); }
    static size_t Leaf() { return offsetof(WeatherProbe, m_objLeaf); }
    static size_t Sand() { return offsetof(WeatherProbe, m_objSand); }
    static size_t Flying() { return offsetof(WeatherProbe, m_objFlying); }
    static size_t Bugs() { return offsetof(WeatherProbe, m_objFlyBug); }
    static size_t Cloud() { return offsetof(WeatherProbe, m_objCloud); }
};

static_assert(sizeof(CLeaf) == 0xD0, "");
static_assert(sizeof(CRain) == 0x11C, "");
static_assert(sizeof(CSnow) == 0xA8, "");
static_assert(sizeof(CSand) == 0x48, "");
static_assert(sizeof(CYinFa) == 0xB0, "");
static_assert(sizeof(CFlyFlower) == 0xA0, "");
static_assert(sizeof(CFireworkLetter) == 0x94, "");
static_assert(!std::is_polymorphic<CLeaf>::value && !std::is_polymorphic<CRain>::value && !std::is_polymorphic<CWeather>::value, "");
static_assert(sizeof(CLeafSys) == 0x6C + sizeof(CLEAF) + 0xC, "");
static_assert(sizeof(CRainSys) == 0xD8 + sizeof(CRAIN) + 0x8, "");
static_assert(sizeof(CSnowSys) == 0x68 + sizeof(CSNOW) + 0x10, "");
static_assert(sizeof(CSandSys) == 0x8 + sizeof(CSAND) + 0x10, "");
static_assert(sizeof(CSysFlyFlower) == 0x84 + sizeof(DequeCFlyFlower) + 0x4, "");
static_assert(sizeof(CYinFa2) == 0xA8 + sizeof(DequePoint) + 0x8, "");
static_assert(sizeof(CYinFaSys) == ROUND8(0x80 + sizeof(DequeYinFa) + 0x18 + 20 * sizeof(POINT) + 0x4), "");
static_assert(sizeof(CYinFaSys2) == ROUND8(0x80 + sizeof(DequeYinFa2) + 0x1C), "");
static_assert(sizeof(CFireWork) == sizeof(DequeFireWork) && sizeof(CFireWork2) == sizeof(DequeFireWork2), "");
static_assert(sizeof(Firework) == sizeof(CFireWork2) + sizeof(CFireWork) + 0x4, "");
static_assert(sizeof(CFireworkLetterSys) == ROUND8(0x8C + sizeof(DequeFireworkLetter) + 0x8 + sizeof(DequePoint) + 0xC), "");
static_assert(sizeof(CFireWorkChar) == sizeof(DequeFireWork2) + sizeof(DequeFireworkChar), "");
static_assert(sizeof(CWeather) == 0x4 + sizeof(CSnowSys) + sizeof(CRainSys) + sizeof(CLeafSys) + sizeof(CSandSys) + sizeof(CSysFlyFlower) + sizeof(CSysFireBug) + sizeof(CCloudSys), "");
static_assert(std::is_same<decltype(&CWeather::CreateEnv), BOOL (CWeather::*)(int, int, int, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CWeather::SetEnv), void (CWeather::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CWeather::StopEnv), BOOL (CWeather::*)()>::value, "");

TEST_CLASS(ClientWeatherHeader)
{
public:
    TEST_METHOD(TheSystemsFollowTheTypeInOrder)
    {
        Assert::IsTrue(WeatherProbe::Snow() == 0x4);
        Assert::IsTrue(WeatherProbe::Rain() == WeatherProbe::Snow() + sizeof(CSnowSys));
        Assert::IsTrue(WeatherProbe::Leaf() == WeatherProbe::Rain() + sizeof(CRainSys));
        Assert::IsTrue(WeatherProbe::Sand() == WeatherProbe::Leaf() + sizeof(CLeafSys));
        Assert::IsTrue(WeatherProbe::Flying() == WeatherProbe::Sand() + sizeof(CSandSys));
        Assert::IsTrue(WeatherProbe::Bugs() == WeatherProbe::Flying() + sizeof(CSysFlyFlower));
        Assert::IsTrue(WeatherProbe::Cloud() == WeatherProbe::Bugs() + sizeof(CSysFireBug));
    }
};
