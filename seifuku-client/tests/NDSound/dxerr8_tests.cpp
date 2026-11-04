#include "CppUnitTest.h"
#include <windows.h>
#include <dxerr8.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static int s_messageEvaluations = 0;
static const TCHAR* Message() { ++s_messageEvaluations; return TEXT("CreateSoundBuffer"); }

TEST_CLASS(Dxerr8Shim)
{
public:
    TEST_METHOD(ReturnsTheHresultUnchanged)
    {
        HRESULT hr = DXTRACE_ERR(TEXT("Initialize"), E_FAIL);
        Assert::AreEqual<long>(E_FAIL, hr);
        Assert::AreEqual<long>(S_OK, DXTRACE_ERR(TEXT("x"), S_OK));
    }

    TEST_METHOD(NeverEvaluatesTheMessage)
    {
        s_messageEvaluations = 0;
        HRESULT hr = DXTRACE_ERR(Message(), E_OUTOFMEMORY);
        Assert::AreEqual<long>(E_OUTOFMEMORY, hr);
        Assert::AreEqual(0, s_messageEvaluations);
    }
};
