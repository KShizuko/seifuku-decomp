#include "stdafx.h"
#include "TaskDialog.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static TaskDialogLine* NewLine(int nType, int nIndex, const char* psz)
{
    TaskDialogLine* pLine = new TaskDialogLine;
    memset(pLine, 0, sizeof(*pLine));
    pLine->nType = nType;
    pLine->nIndex = nIndex;
    strcpy(pLine->szStr, psz);
    return pLine;
}

TEST_CLASS(TaskDialogBodies)
{
public:
    TEST_METHOD(ConstructedDialogIsEmpty)
    {
        CTaskDialog dlg;
        Assert::AreEqual(0, dlg.GetLineAmount());
        Assert::IsNull(dlg.GetLineByIndex(0));
        Assert::IsNull(dlg.GetLineByIndex(-1));
    }

    TEST_METHOD(AddLineKeepsOrderAndIgnoresNull)
    {
        CTaskDialog dlg;
        TaskDialogLine* pText = NewLine(_TASKDIALOG_TEXT, 0, "Hello");
        TaskDialogLine* pLink = NewLine(_TASKDIALOG_LINK, 1, "Yes");
        dlg.AddLine(pText);
        dlg.AddLine(NULL);
        dlg.AddLine(pLink);
        Assert::AreEqual(2, dlg.GetLineAmount());
        Assert::IsTrue(dlg.GetLineByIndex(0) == pText);
        Assert::IsTrue(dlg.GetLineByIndex(1) == pLink);
        Assert::IsNull(dlg.GetLineByIndex(2));
        Assert::AreEqual("Yes", dlg.GetLineByIndex(1)->szStr);
    }

    TEST_METHOD(AddLineAfterCreateStartsANewDialog)
    {
        CTaskDialog dlg;
        dlg.AddLine(NewLine(_TASKDIALOG_TEXT, 0, "old"));
        dlg.AddLine(NewLine(_TASKDIALOG_LINK, 1, "old link"));
        dlg.Create();
        TaskDialogLine* pNew = NewLine(_TASKDIALOG_TEXT, 0, "new");
        dlg.AddLine(pNew);
        Assert::AreEqual(1, dlg.GetLineAmount());
        Assert::IsTrue(dlg.GetLineByIndex(0) == pNew);
        dlg.AddLine(NewLine(_TASKDIALOG_LINK, 1, "new link"));
        Assert::AreEqual(2, dlg.GetLineAmount());
    }

    TEST_METHOD(DestroyEmptiesTheDialog)
    {
        CTaskDialog dlg;
        dlg.AddLine(NewLine(_TASKDIALOG_TEXT, 0, "a"));
        dlg.AddLine(NewLine(_TASKDIALOG_PIC, 0, ""));
        dlg.Destroy();
        Assert::AreEqual(0, dlg.GetLineAmount());
        Assert::IsNull(dlg.GetLineByIndex(0));
        dlg.Destroy();
        dlg.AddLine(NewLine(_TASKDIALOG_TEXT, 0, "b"));
        Assert::AreEqual(1, dlg.GetLineAmount());
    }
};
