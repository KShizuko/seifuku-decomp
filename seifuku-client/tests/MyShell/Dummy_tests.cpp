#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Dummy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct DummyProbe : CDummy
{
    static size_t Action() { return offsetof(DummyProbe, m_nCurrentDummyAction); }
    static size_t Dir() { return offsetof(DummyProbe, m_nCurrentDummyDir); }
    static size_t Frame() { return offsetof(DummyProbe, m_nDummyFrameIndex); }
    static size_t Play() { return offsetof(DummyProbe, m_bDummyPlay); }
    static size_t Lock() { return offsetof(DummyProbe, m_bLock); }
    static size_t Timer() { return offsetof(DummyProbe, m_dwDummyTimer); }
    static size_t Player() { return offsetof(DummyProbe, m_objDummy); }
    static size_t Template() { return offsetof(DummyProbe, m_idDummyTemplate); }
    static size_t Equipment() { return offsetof(DummyProbe, m_pEquipment); }
    static size_t Name() { return offsetof(DummyProbe, m_szName); }
    static size_t Mate() { return offsetof(DummyProbe, m_szMate); }
};

static_assert(sizeof(CDummy) == 0x1C + sizeof(CPlayer) + 0x4 + 9 * 4 + 0x20, "");
static_assert(std::has_virtual_destructor<CDummy>::value, "");
static_assert(std::is_same<decltype(&CDummy::ShowDummy), void (CDummy::*)(CMyPos, int, int)>::value, "");
static_assert(std::is_same<decltype(&CDummy::LockDummy), BOOL (CDummy::*)(OBJID)>::value, "");
static_assert(_MAX_EQUIPMENT == 9, "");

TEST_CLASS(ClientDummyHeader)
{
public:
    TEST_METHOD(SixWordsThenThePlayerThenTheEquipment)
    {
        Assert::IsTrue(DummyProbe::Action() == 0x4);
        Assert::IsTrue(DummyProbe::Dir() == 0x8);
        Assert::IsTrue(DummyProbe::Frame() == 0xC);
        Assert::IsTrue(DummyProbe::Play() == 0x10);
        Assert::IsTrue(DummyProbe::Lock() == 0x14);
        Assert::IsTrue(DummyProbe::Timer() == 0x18);
        Assert::IsTrue(DummyProbe::Player() == 0x1C);
        const size_t p = 0x1C + sizeof(CPlayer);
        Assert::IsTrue(DummyProbe::Template() == p);
        Assert::IsTrue(DummyProbe::Equipment() == p + 4);
        Assert::IsTrue(DummyProbe::Name() == p + 4 + 9 * 4);
        Assert::IsTrue(DummyProbe::Mate() == p + 4 + 9 * 4 + 0x10);
    }
};
