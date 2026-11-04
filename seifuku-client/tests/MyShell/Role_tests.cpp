#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Role.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CCommand) == 0x108, "");
static_assert(offsetof(CCommand, iStatus) == 4 && offsetof(CCommand, szCmd) == 8, "");
static_assert(offsetof(CCommand, idTarget) == 8 && offsetof(CCommand, posTarget) == 0xC, "");
static_assert(offsetof(CCommand, nDir) == 0x14, "");
static_assert(offsetof(CCommand, dwData) == 0x28 && offsetof(CCommand, nData) == 0x2C && offsetof(CCommand, bData) == 0x30, "");

static_assert(sizeof(CEncryptData) == 0xC4 && sizeof(CMyProtectPos) == 0x188, "");

static_assert(sizeof(CRoleInfo) == 0x94 + sizeof(CPath) + 0x108 + 0x10 + sizeof(CMyProtectPos), "");
static_assert(offsetof(CRoleInfo, id) == 0 && offsetof(CRoleInfo, usLook) == 4 && offsetof(CRoleInfo, usHair) == 6, "");
static_assert(offsetof(CRoleInfo, usHead) == 8 && offsetof(CRoleInfo, usFace) == 0xA && offsetof(CRoleInfo, usPose) == 0xC, "");
static_assert(offsetof(CRoleInfo, nTransformLook) == 0x10, "");
static_assert(offsetof(CRoleInfo, idArmorType) == 0x14 && offsetof(CRoleInfo, idRWeaponType) == 0x18 && offsetof(CRoleInfo, idLWeaponType) == 0x1C, "");
static_assert(offsetof(CRoleInfo, idMountType) == 0x20 && offsetof(CRoleInfo, idArmetType) == 0x24, "");
static_assert(offsetof(CRoleInfo, idMiscType) == 0x28 && offsetof(CRoleInfo, idMantleType) == 0x2C, "");
static_assert(offsetof(CRoleInfo, szName) == 0x30, "");
static_assert(offsetof(CRoleInfo, iSize) == 0x40 && offsetof(CRoleInfo, iAddSize) == 0x44 && offsetof(CRoleInfo, nSex) == 0x48, "");
static_assert(offsetof(CRoleInfo, iDirection) == 0x4C && offsetof(CRoleInfo, iZoomPercent) == 0x54, "");
static_assert(offsetof(CRoleInfo, posRush) == 0x58 && offsetof(CRoleInfo, posWorld) == 0x60 && offsetof(CRoleInfo, posScr) == 0x68, "");
static_assert(offsetof(CRoleInfo, posActionBegin) == 0x70 && offsetof(CRoleInfo, posActionEnd) == 0x78 && offsetof(CRoleInfo, iActType) == 0x80, "");
static_assert(offsetof(CRoleInfo, iActFrameIndex) == 0x84 && offsetof(CRoleInfo, iActFrameAmount) == 0x88 && offsetof(CRoleInfo, timeFirstFrame) == 0x8C && offsetof(CRoleInfo, timeFrameInterval) == 0x90, "");
static_assert(offsetof(CRoleInfo, objPath) == 0x94, "");
static_assert(offsetof(CRoleInfo, cmdProc) == 0x94 + sizeof(CPath), "");
static_assert(offsetof(CRoleInfo, sizeBase) == offsetof(CRoleInfo, cmdProc) + 0x108 && offsetof(CRoleInfo, nFlyOffset) == offsetof(CRoleInfo, sizeBase) + 8 && offsetof(CRoleInfo, nFlyOffsetGoal) == offsetof(CRoleInfo, sizeBase) + 0xC, "");
static_assert(offsetof(CRoleInfo, posMap) == offsetof(CRoleInfo, nFlyOffsetGoal) + 4, "");
static_assert(sizeof(CPath) == 4 + sizeof(DEQUE_STEP) + 4 * 4, "");

namespace
{
    struct RoleProbe : CRole
    {
        static size_t Status() { return offsetof(RoleProbe, m_dwStatus); }
        static size_t StatusSize() { return sizeof(((RoleProbe*) 0)->m_dwStatus); }
        static size_t Info() { return offsetof(RoleProbe, m_Info); }
        static size_t Type() { return offsetof(RoleProbe, m_nType); }
        static size_t ViewType() { return offsetof(RoleProbe, m_nViewType); }
        static size_t LastAtk() { return offsetof(RoleProbe, m_tmLastAtk); }
        static size_t StepDir() { return offsetof(RoleProbe, m_lstStepDir); }
    };
}

static_assert(std::has_virtual_destructor<CRole>::value, "");
static_assert(std::is_same<decltype(((CRole*) 0)->SetVisible(0)), void>::value, "");
static_assert(sizeof(DWORD) == 4 && std::is_same<decltype(((CRole*) 0)->GetStatus()), DWORD>::value, "");
static_assert(std::is_same<decltype(((CRole*) 0)->TestStatus(0, 1)), BOOL>::value && std::is_same<decltype(((CRole*) 0)->SetStatus(0, 1)), void>::value, "");
static_assert(std::is_same<decltype(((CRole*) 0)->SetRGBA(0.0f, 0.0f, 0.0f, 0.0f)), void>::value, "");
static_assert(std::is_same<decltype(((CRole*) 0)->AddFStr((char*) 0, 0, 0, 0)), void>::value, "");
static_assert(_LOOPTIME_MAX == 99999999, "");

TEST_CLASS(ClientRoleHeader)
{
public:
    TEST_METHOD(MemberOrderMatchesTheConstructor)
    {
        Assert::IsTrue(RoleProbe::Status() == 0xC && RoleProbe::StatusSize() == 8);
        Assert::IsTrue(RoleProbe::Info() == 0x14 + sizeof(DEQUE_EFFECT));
        Assert::IsTrue(RoleProbe::Type() == RoleProbe::Info() + sizeof(CRoleInfo));
        Assert::IsTrue(sizeof(CRoleInfo) + (0x30 - sizeof(DEQUE_STEP)) == 0x378);
        Assert::IsTrue(RoleProbe::ViewType() == RoleProbe::Type() + 4);
        Assert::IsTrue(RoleProbe::LastAtk() == RoleProbe::Type() + 0x14);
        Assert::IsTrue(RoleProbe::StepDir() == RoleProbe::Type() + 0x1C);
        Assert::IsTrue(offsetof(CRole, m_pIRoleView) == RoleProbe::StepDir() + sizeof(LIST_DIR) + 8);
        Assert::IsTrue(offsetof(CRole, m_objSayHello) == offsetof(CRole, m_pIRoleView) + 4);
        Assert::IsTrue(offsetof(CRole, m_objFStr) == offsetof(CRole, m_objSayHello) + sizeof(CSayHello));
        Assert::IsTrue(offsetof(CRole, m_objZoomNum) == offsetof(CRole, m_objFStr) + sizeof(CFloatingStr));
        Assert::IsTrue(offsetof(CRole, m_objEffect) == offsetof(CRole, m_objZoomNum) + sizeof(CZoomNum));
        Assert::IsTrue(offsetof(CRole, m_objScrEffect) == offsetof(CRole, m_objEffect) + sizeof(IGame3DEffect*));
        Assert::IsTrue(sizeof(CRole) == offsetof(CRole, m_objScrEffect) + sizeof(IGame3DEffect*));
        size_t nInfo = 0x14 + 0x30;
        Assert::IsTrue(nInfo == 0x44 && nInfo + 0x378 == 0x3BC && 0x3BC + 0x1C == 0x3D8 && 0x3D8 + 0xC == 0x3E4 && 0x3E4 + 8 == 0x3EC);
        Assert::IsTrue(0x3F0 + 0x404 == 0x7F4 && 0x7F4 + 0x34 == 0x828 && 0x828 + 0x34 == 0x85C && 0x85C + 4 == 0x860 && 0x860 + 4 == 0x864);
    }

    TEST_METHOD(IdRangesAreConquersOwn)
    {
        Assert::IsTrue(CALLPETID_FIRST == 700001 && CALLPETID_LAST == 799999);
        Assert::IsTrue(MONSTERID_FIRST == 400001 && MONSTERID_LAST == 499999 && PETID_FIRST == 500001 && PETID_LAST == 599999);
        Assert::IsTrue(SYSNPCID_FIRST == 1 && SYSNPCID_LAST == 99999 && DYNANPCID_FIRST == 100001 && DYNANPCID_LAST == 199999);
        Assert::IsTrue(PLAYER_ID_FIRST == 1000000 && _IDMAX_NPC == 999999);
        Assert::IsTrue(_PLAYER_ID_MONSTER == 1 && _PLAYER_ID_PET == 2 && _PLAYER_ID_SYSNPC == 3 && _PLAYER_ID_DYNNPC == 4 && _PLAYER_ID_CALLPET == 5 && _PLAYER_ID_PLAYER == 6);
    }

    TEST_METHOD(CommandTableMatchesProcessCommand)
    {
        Assert::IsTrue(_COMMAND_PICKUP == 1 && _COMMAND_STANDBY == 5 && _COMMAND_WALK == 14 && _COMMAND_RUN == 15 && _COMMAND_JUMP == 16);
        Assert::IsTrue(_COMMAND_ATTACK == 20 && _COMMAND_LOCKATK == 24 && _COMMAND_DIE == 27 && _COMMAND_TAKEOFF == 12 && _COMMAND_LANDING == 13 && _COMMAND_WOUND == 9 && _COMMAND_INTONE == 19 && _COMMAND_SPATTACK == 22);
        Assert::IsTrue(_COMMAND_STOP >= 28 && _COMMAND_FAINT >= 28 && _COMMAND_FLOAT >= 28, L"the unverified names stay above the table");
        Assert::IsTrue(_COMMAND_SHITHAPPEN == 26 && _COMMAND_INTONE == 19, L"");
        Assert::IsTrue(_CMDSTATUS_BEGIN == 0 && _CMDSTATUS_CONTINUE == 3 && _CMDSTATUS_OVER == 5 && _CMDSTATUS_ACCOMPLISH == 6);
        Assert::IsTrue(_ROLE_BOOTH_NPC == 14 && _ROLE_PLAYER == 11 && _ROLE_MONSTER == 13);
    }
};
