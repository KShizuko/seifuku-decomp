#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgWeather.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgWeatherProbe : CMsgWeather
{
    static size_t InfoPtr() { return offsetof(MsgWeatherProbe, m_pInfo); }
};

static_assert(sizeof(CMsgWeather) == 0x408, "");
static_assert(sizeof(MSG_WeatherInfo) == 0x14, "");
static_assert(offsetof(MSG_WeatherInfo, unMsgType) == 0x2, "");
static_assert(offsetof(MSG_WeatherInfo, nType) == 0x4, "");
static_assert(offsetof(MSG_WeatherInfo, nIntensity) == 0x8, "");
static_assert(offsetof(MSG_WeatherInfo, nDir) == 0xC, "");
static_assert(offsetof(MSG_WeatherInfo, dwColor) == 0x10, "");
static_assert(WEATHER_NONE == 0 && WEATHER_FINE == 1 && WEATHER_NOUSED == 6 && WEATHER_CLOUD == 11 && WEATHER_ALL == 12, "");
static_assert(WEATHER_RESET == 1 && WEATHER_STOP == 2 && WEATHER_CREATE == 3 && WEATHER_DESTROY == 4, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgWeather::*)(int, int, int, DWORD)>(&CMsgWeather::ConvertInfoForm)), BOOL (CMsgWeather::*)(int, int, int, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgWeather::*)(char*, DWORD)>(&CMsgWeather::Create)), BOOL (CMsgWeather::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgWeatherHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgWeatherProbe::InfoPtr() == 0x404);
    }
};
