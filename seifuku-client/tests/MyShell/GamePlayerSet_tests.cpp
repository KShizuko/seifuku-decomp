#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "GamePlayerSet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct GamePlayerSetProbe : CGamePlayerSet
{
    static size_t Deque() { return offsetof(GamePlayerSetProbe, m_dequePlayer); }
    static size_t Focus() { return offsetof(GamePlayerSetProbe, m_pFocusPlayer); }
    static size_t Time() { return offsetof(GamePlayerSetProbe, m_dwTimeFor3DRole); }
    static size_t Syndicates() { return offsetof(GamePlayerSetProbe, m_setSyndicateInfo); }
    static size_t White() { return offsetof(GamePlayerSetProbe, m_idWhiteSyn); }
    static size_t Black() { return offsetof(GamePlayerSetProbe, m_idBlackSyn); }
};

static_assert(sizeof(InfoSyndicate) == 0x28, "");
static_assert(offsetof(InfoSyndicate, idSyndicate) == 0x0, "");
static_assert(offsetof(InfoSyndicate, szSyndicateName) == 0x4, "");
static_assert(offsetof(InfoSyndicate, szSyndicateLeaderName) == 0x14, "");
static_assert(offsetof(InfoSyndicate, dwStatus) == 0x24, "");
static_assert(sizeof(CGamePlayerSet) == 4 + sizeof(DEQUE_PLAYER) + 4 + 4 + sizeof(VECTOR_SYNDICATEINFO) + 4 + 4, "");
static_assert(std::has_virtual_destructor<CGamePlayerSet>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::AddPlayer), BOOL (CGamePlayerSet::*)(CPlayer*)>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::DelPlayer), BOOL (CGamePlayerSet::*)(OBJID, BOOL)>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::DelAllPlayer), void (CGamePlayerSet::*)(BOOL)>::value, "");
static_assert(std::is_same<decltype(static_cast<CPlayer* (CGamePlayerSet::*)(OBJID, BOOL) const>(&CGamePlayerSet::GetPlayer)), CPlayer* (CGamePlayerSet::*)(OBJID, BOOL) const>::value, "");
static_assert(std::is_same<decltype(static_cast<CPlayer* (CGamePlayerSet::*)(int, int, int) const>(&CGamePlayerSet::GetPlayer)), CPlayer* (CGamePlayerSet::*)(int, int, int) const>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::GetFocusPlayer), CPlayer* (CGamePlayerSet::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::Process), void (CGamePlayerSet::*)(int, int)>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::SetSyndicateName), void (CGamePlayerSet::*)(OBJID, char*, char*)>::value, "");
static_assert(std::is_same<decltype(&CGamePlayerSet::DestroySyndicate), void (CGamePlayerSet::*)(OBJID)>::value, "");
static_assert(SYNDICATE_ALLY == 0 && SYNDICATE_ANTAGONIZE == 1 && SYNDICATE_NORMAL == 3, "");
static_assert(PLAYER_INCLUDESELF == 1 && PLAYER_INCLUDEDEAD == 2 && PLAYER_MONSTERPRI == 4 && PLAYER_PLAYERPRI == 8, "");

TEST_CLASS(ClientGamePlayerSetHeader)
{
public:
    TEST_METHOD(MembersFollowTheConstructorsOrder)
    {
        Assert::IsTrue(GamePlayerSetProbe::Deque() == 0x4);
        Assert::IsTrue(GamePlayerSetProbe::Focus() == GamePlayerSetProbe::Deque() + sizeof(DEQUE_PLAYER));
        Assert::IsTrue(GamePlayerSetProbe::Time() == GamePlayerSetProbe::Focus() + 4);
        Assert::IsTrue(GamePlayerSetProbe::Syndicates() == GamePlayerSetProbe::Time() + 4);
        Assert::IsTrue(GamePlayerSetProbe::White() == GamePlayerSetProbe::Syndicates() + sizeof(VECTOR_SYNDICATEINFO));
        Assert::IsTrue(GamePlayerSetProbe::Black() == GamePlayerSetProbe::White() + 4);
        Assert::IsTrue(sizeof(CGamePlayerSet) == GamePlayerSetProbe::Black() + 4);
    }
};
