#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgRegister.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgRegisterProbe : CMsgRegister
{
    typedef CMsgRegister::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgRegisterProbe, m_pInfo); }
};

static_assert(sizeof(CMsgRegister) == 0x408, "");
static_assert(sizeof(MsgRegisterProbe::Info) == 0x3C, "");
static_assert(offsetof(MsgRegisterProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgRegisterProbe::Info, szAccount) == 0x4, "");
static_assert(offsetof(MsgRegisterProbe::Info, szName) == 0x14, "");
static_assert(offsetof(MsgRegisterProbe::Info, szPassword) == 0x24, "");
static_assert(offsetof(MsgRegisterProbe::Info, unLook) == 0x34 && sizeof(MsgRegisterProbe::Info::unLook) == 2, "");
static_assert(offsetof(MsgRegisterProbe::Info, unData) == 0x36 && sizeof(MsgRegisterProbe::Info::unData) == 2, "");
static_assert(offsetof(MsgRegisterProbe::Info, idAccount) == 0x38, "");
static_assert(_MSG_REGISTER == 1001, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgRegister::*)(const char*, const char*, const char*, USHORT, USHORT, OBJID)>(&CMsgRegister::Create)), BOOL (CMsgRegister::*)(const char*, const char*, const char*, USHORT, USHORT, OBJID)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgRegister::*)(char*, DWORD)>(&CMsgRegister::Create)), BOOL (CMsgRegister::*)(char*, DWORD)>::value, "");

TEST_CLASS(ClientMsgRegisterHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgRegisterProbe::InfoPtr() == 0x404);
    }
};
