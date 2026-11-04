#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "define.h"
class CNetMsg;
class CUser;
#include "../../src/MsgServer/MapGroupKernel/Weather.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_weather_tests
{
	struct WeatherProbe : public CWeather
	{
		WeatherProbe() : CWeather(NULL) {}
		static size_t Owner()		{ return offsetof(WeatherProbe, m_pOwner); }
		static size_t DefType()		{ return offsetof(WeatherProbe, m_nDefaultType); }
		static size_t DefColor()	{ return offsetof(WeatherProbe, m_nDefaultColor); }
		static size_t CurType()		{ return offsetof(WeatherProbe, m_nCurrType); }
		static size_t CurParticle()	{ return offsetof(WeatherProbe, m_nCurrParticle); }
		static size_t TargType()	{ return offsetof(WeatherProbe, m_nTargType); }
		static size_t Incr()		{ return offsetof(WeatherProbe, m_nIncrIntensity); }
		static size_t Keep()		{ return offsetof(WeatherProbe, m_nKeepSecs); }
		static size_t Speed()		{ return offsetof(WeatherProbe, m_nSpeedSecs); }
		static size_t Loop()		{ return offsetof(WeatherProbe, m_tLoop); }
	};

	TEST_CLASS(MapGroupWeatherTests)
	{
	public:
		TEST_METHOD(WeatherLayout)
		{
			Assert::AreEqual((size_t)0x54, sizeof(CWeather));
			Assert::AreEqual((size_t)0x04, WeatherProbe::Owner());
			Assert::AreEqual((size_t)0x08, WeatherProbe::DefType());
			Assert::AreEqual((size_t)0x14, WeatherProbe::DefColor());
			Assert::AreEqual((size_t)0x1C, WeatherProbe::CurType());
			Assert::AreEqual((size_t)0x2C, WeatherProbe::CurParticle());
			Assert::AreEqual((size_t)0x30, WeatherProbe::TargType());
			Assert::AreEqual((size_t)0x40, WeatherProbe::Incr());
			Assert::AreEqual((size_t)0x44, WeatherProbe::Keep());
			Assert::AreEqual((size_t)0x48, WeatherProbe::Speed());
			Assert::AreEqual((size_t)0x4C, WeatherProbe::Loop());
		}
		TEST_METHOD(WeatherConstants)
		{
			Assert::AreEqual(12, (int)WEATHER_ALL);
			Assert::AreEqual(11, (int)WEATHER_CLOUDY);
			Assert::AreEqual(1000, MAX_WEATHER_INTENSITY);
			Assert::AreEqual(360, MAX_WEATHER_DIR);
			Assert::AreEqual(10, WEATHER_NORMAL_SPEED);
			Assert::AreEqual(5, WEATHER_FAST_SPEED);
			Assert::AreEqual(701, MAX_KEEP_SECS - MIN_KEEP_SECS + 1);
			Assert::AreEqual(500, MIN_KEEP_SECS);
			Assert::AreEqual(60, WERTHER_FINE_PERCENT);
			Assert::AreEqual(5, WERTHER_CHANGE_DIR_PERCENT);
		}
	};
}
