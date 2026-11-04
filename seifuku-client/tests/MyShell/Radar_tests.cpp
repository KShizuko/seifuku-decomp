#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Radar.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct RadarProbe : CRadar
{
    static size_t PosFrame() { return offsetof(RadarProbe, m_posFrame); }
    static size_t ScaleX() { return offsetof(RadarProbe, m_fScaleX); }
    static size_t ScaleY() { return offsetof(RadarProbe, m_fScaleY); }
    static size_t Mode() { return offsetof(RadarProbe, m_nMode); }
    static size_t RcShow() { return offsetof(RadarProbe, m_rcShow); }
    static size_t RcShowBg() { return offsetof(RadarProbe, m_rcShowBg); }
    static size_t PosHide() { return offsetof(RadarProbe, m_posHide); }
    static size_t ShowState() { return offsetof(RadarProbe, m_nShowState); }
};

static_assert(sizeof(CRadar) == 0x44, "");
static_assert(sizeof(CMyPos) == 8, "");
static_assert(_MODE_128 == 0 && _MODE_256 == 1 && _MODE_512 == 2, "");
static_assert(std::is_same<decltype(&CRadar::Show), void (CRadar::*)(OBJID)>::value, "");
static_assert(std::is_same<decltype(&CRadar::CellPos2RaderPos), void (CRadar::*)(IAni*, CMyPos, CMyPos&)>::value, "");
static_assert(std::is_polymorphic<CRadar>::value, "");
static_assert(std::is_same<decltype(&CRadar::SetShowState), void (CRadar::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CRadar::SetHidePos), void (CRadar::*)(CMyPos)>::value, "");

TEST_CLASS(ClientRadarHeader)
{
public:
    TEST_METHOD(MembersFollowTheVtable)
    {
        Assert::IsTrue(RadarProbe::PosFrame() == 0x4);
        Assert::IsTrue(RadarProbe::ScaleX() == 0xC);
        Assert::IsTrue(RadarProbe::ScaleY() == 0x10);
        Assert::IsTrue(RadarProbe::Mode() == 0x14);
        Assert::IsTrue(RadarProbe::RcShow() == 0x18);
        Assert::IsTrue(RadarProbe::RcShowBg() == 0x28);
        Assert::IsTrue(RadarProbe::PosHide() == 0x38);
        Assert::IsTrue(RadarProbe::ShowState() == 0x40);
    }
};
