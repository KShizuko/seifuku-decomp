#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgInteract.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgInteractProbe : CMsgInteract
{
    typedef CMsgInteract::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgInteractProbe, m_pInfo); }
};

static_assert(sizeof(CMsgInteract) == 0x408, "");
static_assert(sizeof(MsgInteractProbe::Info) == 0x1C, "");
static_assert(offsetof(MsgInteractProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgInteractProbe::Info, dwTimeStamp) == 0x4, "");
static_assert(offsetof(MsgInteractProbe::Info, idSender) == 0x8, "");
static_assert(offsetof(MsgInteractProbe::Info, idTarget) == 0xC, "");
static_assert(offsetof(MsgInteractProbe::Info, unPosX) == 0x10 && offsetof(MsgInteractProbe::Info, unPosY) == 0x12, "");
static_assert(offsetof(MsgInteractProbe::Info, idBullet) == 0x10, "");
static_assert(offsetof(MsgInteractProbe::Info, unType) == 0x14, "");
static_assert(offsetof(MsgInteractProbe::Info, dwData) == 0x18, "");
static_assert(offsetof(MsgInteractProbe::Info, usData0) == 0x18 && offsetof(MsgInteractProbe::Info, usData1) == 0x1A, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, OBJID)>(&CMsgInteract::Create)), BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, OBJID)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, USHORT, USHORT, DWORD)>(&CMsgInteract::Create)), BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, USHORT, USHORT, DWORD)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, USHORT, USHORT, USHORT, USHORT, OBJID)>(&CMsgInteract::Create)), BOOL (CMsgInteract::*)(USHORT, OBJID, OBJID, USHORT, USHORT, USHORT, USHORT, OBJID)>::value, "");
static_assert(std::is_same<decltype(&CMsgInteract::Send), void (CMsgInteract::*)(void)>::value, "");
static_assert(INTERACT_ATTACK == 2 && INTERACT_COURT == 8 && INTERACT_MARRY == 9 && INTERACT_DIVORCE == 10, "");
static_assert(INTERACT_RUSHATK == 20 && INTERACT_MAGICATTACK == 21 && INTERACT_SHOOT == 25, "");
static_assert(INTERACT_SENDFLOWERS == 13 && INTERACT_KILL == 14 && INTERACT_REFLECTWEAPON == 23 && INTERACT_BUMP == 24 && INTERACT_REFLECTMAGIC == 26, "");
static_assert(INTERACT_SOUL == 32, "");

TEST_CLASS(ClientMsgInteractHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgInteractProbe::InfoPtr() == 0x404);
    }
};
