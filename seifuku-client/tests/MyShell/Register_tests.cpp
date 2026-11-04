#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Register.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct RegisterProbe : CRegister
{
    static size_t Msgs() { return offsetof(RegisterProbe, m_TxtMsgSet); }
};

static_assert(sizeof(CRegister) == 0x4 + sizeof(CTxtMsgSet), "");
static_assert(std::is_polymorphic<CRegister>::value, "");
static_assert(std::is_same<decltype(&CRegister::GetLastMsg), const char* (CRegister::*)()>::value, "");

TEST_CLASS(ClientRegisterHeader)
{
public:
    TEST_METHOD(TheMessageSetFollowsTheVtable)
    {
        Assert::IsTrue(RegisterProbe::Msgs() == 0x4);
    }
};
