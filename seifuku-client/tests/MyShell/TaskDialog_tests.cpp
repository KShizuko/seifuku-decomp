#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "TaskDialog.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct TaskDialogProbe : CTaskDialog
{
    static size_t Create() { return offsetof(TaskDialogProbe, m_bCreate); }
    static size_t Lines() { return offsetof(TaskDialogProbe, m_setTaskDialogLine); }
};

static_assert(sizeof(TaskDialogLine) == 0x114, "");
static_assert(offsetof(TaskDialogLine, nType) == 0x0 && offsetof(TaskDialogLine, dwData) == 0x4 && offsetof(TaskDialogLine, nIndex) == 0x8, "");
static_assert(offsetof(TaskDialogLine, szStr) == 0xC, "");
static_assert(offsetof(TaskDialogLine, posLine) == 0x10C, "");
static_assert(sizeof(CTaskDialog) == 4 + 4 + sizeof(DEQUE_TASKDIALOGLINE), "");
static_assert(std::has_virtual_destructor<CTaskDialog>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CTaskDialog::*)(TaskDialogLine*)>(&CTaskDialog::AddLine)), void (CTaskDialog::*)(TaskDialogLine*)>::value, "");
static_assert(std::is_same<decltype(static_cast<void (CTaskDialog::*)(int, DWORD, int, char*, CMyPos)>(&CTaskDialog::AddLine)), void (CTaskDialog::*)(int, DWORD, int, char*, CMyPos)>::value, "");

TEST_CLASS(ClientTaskDialogHeader)
{
public:
    TEST_METHOD(CreateFlagPrecedesTheDeque)
    {
        Assert::IsTrue(TaskDialogProbe::Create() == 0x4);
        Assert::IsTrue(TaskDialogProbe::Lines() == 0x8);
    }
};
