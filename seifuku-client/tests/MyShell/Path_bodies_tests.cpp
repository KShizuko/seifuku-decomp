#include "stdafx.h"
#include "Path.h"
#include "CppUnitTest.h"
#include <cstddef>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(DEQUE_STEP::value_type) == 1, "");

TEST_CLASS(PathBodies)
{
public:
    TEST_METHOD(ConstructedPathIsEmptyWithTheDefaults)
    {
        CPath path;
        Assert::AreEqual(0, path.GetStepAmount());
        Assert::AreEqual(-1, path.GetNextStep());
        Assert::AreEqual((int) _ASTAR_CLOSE, path.GetSearchMethod());
        Assert::AreEqual(15, path.GetData());
    }

    TEST_METHOD(StepsComeOutInOrderThenMinusOne)
    {
        CPath path;
        path.AddStep(3);
        path.AddStep(5);
        path.AddStep(-2);
        Assert::AreEqual(3, path.GetStepAmount());
        Assert::AreEqual(3, path.GetNextStep());
        Assert::AreEqual(2, path.GetStepAmount());
        Assert::AreEqual(5, path.GetNextStep());
        Assert::AreEqual(-2, path.GetNextStep());
        Assert::AreEqual(-1, path.GetNextStep());
        Assert::AreEqual(0, path.GetStepAmount());
    }

    TEST_METHOD(ClearStepAndTheFields)
    {
        CPath path;
        path.AddStep(1); path.AddStep(2);
        path.ClearStep();
        Assert::AreEqual(0, path.GetStepAmount());
        Assert::AreEqual(-1, path.GetNextStep());
        path.SetSearchMethod(_ASTAR_LEAVE);
        Assert::AreEqual((int) _ASTAR_LEAVE, path.GetSearchMethod());
        path.SetReachable(TRUE);
        Assert::IsTrue(path.IsReachable() != 0);
        path.SetPlayer(NULL);
        path.SetPathData(7);
        Assert::AreEqual(7, path.GetData());
    }
};
