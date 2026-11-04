#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgTaskDialog.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgTaskDialogProbe : CMsgTaskDialog
{
    typedef CMsgTaskDialog::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgTaskDialogProbe, m_pInfo); }
    static size_t Packer() { return offsetof(MsgTaskDialogProbe, m_StrPacker); }
};

static_assert(sizeof(CMsgTaskDialog) == 0x414, "");
static_assert(sizeof(MsgTaskDialogProbe::Info) == 0x10, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, idTask) == 0x4 && offsetof(MsgTaskDialogProbe::Info, usPosX) == 0x4 && offsetof(MsgTaskDialogProbe::Info, usPosY) == 0x6, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, usData) == 0x8, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, idxTask) == 0xA, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, ucAction) == 0xB, "");
static_assert(offsetof(MsgTaskDialogProbe::Info, szBuf) == 0xC, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTaskDialog::*)(int, char*, OBJID, int)>(&CMsgTaskDialog::Create)), BOOL (CMsgTaskDialog::*)(int, char*, OBJID, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgTaskDialog::*)(char*, DWORD)>(&CMsgTaskDialog::Create)), BOOL (CMsgTaskDialog::*)(char*, DWORD)>::value, "");
static_assert(MSGDIALOG_ANSWER == 101 && MSGDIALOG_TASKID == 102, "");

TEST_CLASS(ClientMsgTaskDialogHeader)
{
public:
    TEST_METHOD(MembersFollowTheBuffer)
    {
        Assert::IsTrue(MsgTaskDialogProbe::InfoPtr() == 0x404);
        Assert::IsTrue(MsgTaskDialogProbe::Packer() == 0x408);
    }
};
