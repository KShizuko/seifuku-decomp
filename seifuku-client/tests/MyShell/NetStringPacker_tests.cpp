#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "NetStringPacker.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct NetStringPackerProbe : CNetStringPacker
{
    static size_t Buf() { return offsetof(NetStringPackerProbe, m_pBuf); }
    static size_t BufSize() { return offsetof(NetStringPackerProbe, m_uBufSize); }
};

static_assert(sizeof(CNetStringPacker) == 0xC, "");
static_assert(std::has_virtual_destructor<CNetStringPacker>::value, "");
static_assert(std::is_same<decltype(&CNetStringPacker::GetString), BOOL (CNetStringPacker::*)(UINT, char*, UINT)>::value, "");
static_assert(std::is_same<decltype(&CNetStringPacker::SetBuf), void (CNetStringPacker::*)(char*, UINT)>::value, "");

TEST_CLASS(ClientNetStringPackerHeader)
{
public:
    TEST_METHOD(BufferPointerThenSize)
    {
        Assert::IsTrue(NetStringPackerProbe::Buf() == 0x4);
        Assert::IsTrue(NetStringPackerProbe::BufSize() == 0x8);
    }
};
