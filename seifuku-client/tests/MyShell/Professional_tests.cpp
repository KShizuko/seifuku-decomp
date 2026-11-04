#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Professional.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct ProfessionalProbe : CProfessional
{
    static size_t List() { return offsetof(ProfessionalProbe, m_setProfessional); }
    static size_t Error() { return offsetof(ProfessionalProbe, m_szError); }
};

static_assert(sizeof(ProfessionalInfo) == 0x14 && offsetof(ProfessionalInfo, dwIndex) == 0x10, "");
static_assert(sizeof(CProfessional) == 4 + sizeof(DEQUE_PROFESSIONAL) + 0x10, "");
static_assert(std::has_virtual_destructor<CProfessional>::value, "");
static_assert(std::is_same<decltype(&CProfessional::GetProfessionalName), char* (CProfessional::*)(DWORD)>::value, "");

TEST_CLASS(ClientProfessionalHeader)
{
public:
    TEST_METHOD(ErrorStringFollowsTheDeque)
    {
        Assert::IsTrue(ProfessionalProbe::List() == 0x4);
        Assert::IsTrue(ProfessionalProbe::Error() == 0x4 + sizeof(DEQUE_PROFESSIONAL));
    }
};
