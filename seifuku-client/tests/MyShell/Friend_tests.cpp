#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Friend.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct FriendProbe : CFriend
{
    static size_t Id() { return offsetof(FriendProbe, m_idFriend); }
    static size_t Name() { return offsetof(FriendProbe, m_szName); }
    static size_t Online() { return offsetof(FriendProbe, m_bFlagOnline); }
    static size_t NewMsg() { return offsetof(FriendProbe, m_bFlagNewMsg); }
    static size_t Face() { return offsetof(FriendProbe, m_dwFace); }
    static size_t Level() { return offsetof(FriendProbe, m_dwLevel); }
    static size_t Professional() { return offsetof(FriendProbe, m_dwProfessional); }
    static size_t PK() { return offsetof(FriendProbe, m_nPK); }
    static size_t Syndicate() { return offsetof(FriendProbe, m_IDSyndicate); }
    static size_t Rank() { return offsetof(FriendProbe, m_dwRank); }
    static size_t Mate() { return offsetof(FriendProbe, m_szMate); }
};

static_assert(sizeof(CFriend) == 0x48, "");
static_assert(std::has_virtual_destructor<CFriend>::value, "");
static_assert(std::is_same<decltype(&CFriend::CreateNew), CFriend* (*)(OBJID, char*, BOOL)>::value, "");
static_assert(std::is_same<decltype(((CFriend*) 0)->GetProfessional()), DWORD>::value, "");

TEST_CLASS(ClientFriendHeader)
{
public:
    TEST_METHOD(MemberOffsetsMatchTheMessageWrites)
    {
        Assert::IsTrue(FriendProbe::Id() == 0x4);
        Assert::IsTrue(FriendProbe::Name() == 0x8);
        Assert::IsTrue(FriendProbe::Online() == 0x18);
        Assert::IsTrue(FriendProbe::NewMsg() == 0x1C);
        Assert::IsTrue(FriendProbe::Face() == 0x20);
        Assert::IsTrue(FriendProbe::Level() == 0x24);
        Assert::IsTrue(FriendProbe::Professional() == 0x28);
        Assert::IsTrue(FriendProbe::PK() == 0x2C);
        Assert::IsTrue(FriendProbe::Syndicate() == 0x30);
        Assert::IsTrue(FriendProbe::Rank() == 0x34);
        Assert::IsTrue(FriendProbe::Mate() == 0x38);
    }
};
