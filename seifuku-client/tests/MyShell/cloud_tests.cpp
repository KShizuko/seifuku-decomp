#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "cloud.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct CloudProbe : CCloud
{
    static size_t Res() { return offsetof(CloudProbe, m_nCloudResource); }
    static size_t Info() { return offsetof(CloudProbe, m_infoCloud); }
    static size_t Count() { return offsetof(CloudProbe, m_bCount); }
};
struct CloudSysProbe : CCloudSys
{
    static size_t Clouds() { return offsetof(CloudSysProbe, m_dequeCloud); }
    static size_t Status() { return offsetof(CloudSysProbe, m_bStatus); }
    static size_t Target() { return offsetof(CloudSysProbe, m_nTargetAmound); }
};

static_assert(sizeof(CLOUDOFF) == 0x18, "");
static_assert(sizeof(CCloud) == 0x74, "");
static_assert(!std::is_polymorphic<CCloud>::value, "");
static_assert(sizeof(CCloudSys) == sizeof(CLOUDDEQUE) + 0x4, "");
static_assert(MAXCLOUD == 30 && CLOUD_PAUSE == 4, "");
static_assert(std::is_same<decltype(&CCloudSys::Create), BOOL (CCloudSys::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CCloudSys::Process), void (CCloudSys::*)(int&)>::value, "");
static_assert(std::is_same<decltype(&CCloudSys::ReSet), void (CCloudSys::*)(int)>::value, "");
static_assert(std::is_same<decltype(&CCloudSys::Pause), void (CCloudSys::*)()>::value && std::is_same<decltype(&CCloudSys::Continue), void (CCloudSys::*)()>::value, "");

TEST_CLASS(ClientCloudHeader)
{
public:
    TEST_METHOD(CloudMembersInOrder)
    {
        Assert::IsTrue(CloudProbe::Res() == 0x0);
        Assert::IsTrue(CloudProbe::Info() == 0x10);
        Assert::IsTrue(CloudProbe::Count() == 0x70);
    }
    TEST_METHOD(SystemMembersInOrder)
    {
        Assert::IsTrue(CloudSysProbe::Clouds() == 0x0);
        Assert::IsTrue(CloudSysProbe::Status() == sizeof(CLOUDDEQUE));
        Assert::IsTrue(CloudSysProbe::Target() == sizeof(CLOUDDEQUE) + 0x2);
    }
};
