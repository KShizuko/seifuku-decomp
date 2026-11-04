#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgPackage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgPackageProbe : CMsgPackage
{
    typedef CMsgPackage::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgPackageProbe, m_pInfo); }
};

static_assert(sizeof(CMsgPackage) == 0x408, "");
static_assert(sizeof(MsgPackageItemInfo) == 0x14, "");
static_assert(offsetof(MsgPackageItemInfo, ucBless) == 0xE && offsetof(MsgPackageItemInfo, usEnchant) == 0x10 && offsetof(MsgPackageItemInfo, usRestrain) == 0x12, "");
static_assert(offsetof(MsgPackageItemInfo, idItemType) == 0x4 && offsetof(MsgPackageItemInfo, ucStatus) == 0x8 && offsetof(MsgPackageItemInfo, ucMagic3) == 0xD, "");
static_assert(sizeof(MsgPackageProbe::Info) == 0x24, "");
static_assert(offsetof(MsgPackageProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgPackageProbe::Info, id) == 0x4, "");
static_assert(offsetof(MsgPackageProbe::Info, ucAction) == 0x8, "");
static_assert(offsetof(MsgPackageProbe::Info, ucType) == 0x9, "");
static_assert(offsetof(MsgPackageProbe::Info, idItem) == 0xC, "");
static_assert(offsetof(MsgPackageProbe::Info, usItemAmount) == 0xC && offsetof(MsgPackageProbe::Info, setItemInfo) == 0x10, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgPackage::*)(int, int, OBJID, OBJID)>(&CMsgPackage::Create)), BOOL (CMsgPackage::*)(int, int, OBJID, OBJID)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgPackage::*)(char*, DWORD)>(&CMsgPackage::Create)), BOOL (CMsgPackage::*)(char*, DWORD)>::value, "");
static_assert(MSGPACKAGE_QUERYLIST == 0 && MSGPACKAGE_CHECKIN == 1 && MSGPACKAGE_CHECKOUT == 2, "");

TEST_CLASS(ClientMsgPackageHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgPackageProbe::InfoPtr() == 0x404);
    }
};
