#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "KillCounter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct KillCounterProbe
{
    static size_t Numbers() { return offsetof(CKillCounter, m_setNumber); }
    static size_t Prefix() { return offsetof(CKillCounter, m_strEffectPrefix); }
    static size_t BeginTime() { return offsetof(CKillCounter, m_dwBeginTime); }
    static size_t Status() { return offsetof(CKillCounter, m_nStatus); }
    static size_t Number() { return offsetof(CKillCounter, m_szNumber); }
    static size_t LastNumber() { return offsetof(CKillCounter, m_szLastNumber); }
    static size_t Color() { return offsetof(CKillCounter, m_dwColor); }
    static size_t Digits() { return offsetof(CKillCounter, m_objDigits); }
    static size_t Title() { return offsetof(CKillCounter, m_objTitle); }
    static size_t LastDigit() { return offsetof(CKillCounter, m_objLastDigit); }
    static size_t ZoomIn() { return offsetof(CKillCounter, m_objZoomIn); }
    static size_t String() { return offsetof(CKillCounter, m_szString); }
    static size_t StringTime() { return offsetof(CKillCounter, m_dwStringTime); }
    static size_t StringEffect() { return offsetof(CKillCounter, m_objString); }
};

struct KoCounterProbe
{
    static size_t Numbers() { return offsetof(CKoCounter, m_setNumber); }
    static size_t Status() { return offsetof(CKoCounter, m_nStatus); }
    static size_t Color() { return offsetof(CKoCounter, m_dwColor); }
    static size_t BeginTime() { return offsetof(CKoCounter, m_dwBeginTime); }
    static size_t Number() { return offsetof(CKoCounter, m_szNumber); }
    static size_t LastNumber() { return offsetof(CKoCounter, m_szLastNumber); }
    static size_t Digits() { return offsetof(CKoCounter, m_objDigits); }
    static size_t Title() { return offsetof(CKoCounter, m_objTitle); }
    static size_t LastDigit() { return offsetof(CKoCounter, m_objLastDigit); }
};

static_assert(std::has_virtual_destructor<CKillCounter>::value && std::has_virtual_destructor<CKoCounter>::value, "");
static_assert(sizeof(CKillCounter) == 4 + sizeof(deque<DWORD>) + sizeof(std::string) + 4 + 4 + 16 + 16 + 4 + 3 * sizeof(IGame3DEffect*) + sizeof(CZoomInImgString) + 16 + 4 + sizeof(IGame3DEffect*), "");
static_assert(sizeof(CKoCounter) == 4 + sizeof(deque<DWORD>) + 4 + 4 + 4 + 16 + 16 + 3 * sizeof(IGame3DEffect*), "");
static_assert(std::is_same<decltype(((CKillCounter*) 0)->Add(0)), void>::value && std::is_same<decltype(((CKoCounter*) 0)->Add(0)), void>::value, "");
static_assert(4 + 0x30 + 0x10 + 4 + 4 + 16 + 16 + 4 + 3 * 4 + 0x20 + 16 + 4 + 4 == 0xB4, "");
static_assert(4 + 0x30 + 4 + 4 + 4 + 16 + 16 + 3 * 4 == 0x6C, "");

TEST_CLASS(ClientKillCounter)
{
public:
    TEST_METHOD(KillCounterMembersFollowTheConstructor)
    {
        Assert::IsTrue(KillCounterProbe::Numbers() == 0x4);
        Assert::IsTrue(KillCounterProbe::Prefix() == KillCounterProbe::Numbers() + sizeof(deque<DWORD>));
        Assert::IsTrue(KillCounterProbe::BeginTime() == KillCounterProbe::Prefix() + sizeof(std::string));
        Assert::IsTrue(KillCounterProbe::Status() == KillCounterProbe::BeginTime() + 4);
        Assert::IsTrue(KillCounterProbe::Number() == KillCounterProbe::Status() + 4);
        Assert::IsTrue(KillCounterProbe::LastNumber() == KillCounterProbe::Number() + 16);
        Assert::IsTrue(KillCounterProbe::Color() == KillCounterProbe::LastNumber() + 16);
        Assert::IsTrue(KillCounterProbe::Digits() == KillCounterProbe::Color() + 4);
        Assert::IsTrue(KillCounterProbe::Title() == KillCounterProbe::Digits() + sizeof(IGame3DEffect*));
        Assert::IsTrue(KillCounterProbe::LastDigit() == KillCounterProbe::Title() + sizeof(IGame3DEffect*));
        Assert::IsTrue(KillCounterProbe::ZoomIn() == KillCounterProbe::LastDigit() + sizeof(IGame3DEffect*));
        Assert::IsTrue(KillCounterProbe::String() == KillCounterProbe::ZoomIn() + sizeof(CZoomInImgString));
        Assert::IsTrue(KillCounterProbe::StringTime() == KillCounterProbe::String() + 16);
        Assert::IsTrue(KillCounterProbe::StringEffect() == KillCounterProbe::StringTime() + 4);
    }

    TEST_METHOD(KoCounterMembersFollowTheConstructor)
    {
        Assert::IsTrue(KoCounterProbe::Numbers() == 0x4);
        Assert::IsTrue(KoCounterProbe::Status() == KoCounterProbe::Numbers() + sizeof(deque<DWORD>));
        Assert::IsTrue(KoCounterProbe::Color() == KoCounterProbe::Status() + 4);
        Assert::IsTrue(KoCounterProbe::BeginTime() == KoCounterProbe::Color() + 4);
        Assert::IsTrue(KoCounterProbe::Number() == KoCounterProbe::BeginTime() + 4);
        Assert::IsTrue(KoCounterProbe::LastNumber() == KoCounterProbe::Number() + 16);
        Assert::IsTrue(KoCounterProbe::Digits() == KoCounterProbe::LastNumber() + 16);
        Assert::IsTrue(KoCounterProbe::Title() == KoCounterProbe::Digits() + sizeof(IGame3DEffect*));
        Assert::IsTrue(KoCounterProbe::LastDigit() == KoCounterProbe::Title() + sizeof(IGame3DEffect*));
    }
};
