#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "SkyLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct SkyLayerProbe : CSkyLayer
{
    static size_t Weather() { return offsetof(SkyLayerProbe, m_objWeather); }
    static size_t Fire() { return offsetof(SkyLayerProbe, m_objfirework); }
    static size_t Chars() { return offsetof(SkyLayerProbe, m_objCharFirework); }
    static size_t Bubbles() { return offsetof(SkyLayerProbe, m_objAirBubble); }
    static size_t Objs() { return offsetof(SkyLayerProbe, m_dequeMapObj); }
};

static_assert(sizeof(CSkyLayer) == 0xC + sizeof(CWeather) + sizeof(Firework) + sizeof(CFireWorkChar) + sizeof(CAirBubbleSys) + sizeof(DEQUE_MAPOBJ), "");
static_assert(std::is_base_of<CMapLayer, CSkyLayer>::value, "");
static_assert(std::is_same<decltype(&CSkyLayer::CreateNew), CSkyLayer* (*)()>::value, "");
static_assert(std::is_same<decltype(&CSkyLayer::AddObj), void (CSkyLayer::*)(CMapObj*)>::value, "");
static_assert(std::is_same<decltype(&CSkyLayer::CreateWeather), BOOL (CSkyLayer::*)(int, int, int, DWORD)>::value, "");

TEST_CLASS(ClientSkyLayerHeader)
{
public:
    TEST_METHOD(MembersFollowTheLayer)
    {
        Assert::IsTrue(SkyLayerProbe::Weather() == 0xC);
        Assert::IsTrue(SkyLayerProbe::Fire() == SkyLayerProbe::Weather() + sizeof(CWeather));
        Assert::IsTrue(SkyLayerProbe::Chars() == SkyLayerProbe::Fire() + sizeof(Firework));
        Assert::IsTrue(SkyLayerProbe::Bubbles() == SkyLayerProbe::Chars() + sizeof(CFireWorkChar));
        Assert::IsTrue(SkyLayerProbe::Objs() == SkyLayerProbe::Bubbles() + sizeof(CAirBubbleSys));
    }
};
