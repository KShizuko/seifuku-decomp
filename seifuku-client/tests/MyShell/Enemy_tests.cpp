#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Enemy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct EnemyProbe : CEnemy
{
    static size_t Id() { return offsetof(EnemyProbe, m_idEnemy); }
    static size_t Name() { return offsetof(EnemyProbe, m_szName); }
    static size_t Online() { return offsetof(EnemyProbe, m_bFlagOnline); }
    static size_t Face() { return offsetof(EnemyProbe, m_dwFace); }
    static size_t Level() { return offsetof(EnemyProbe, m_dwLevel); }
    static size_t Professional() { return offsetof(EnemyProbe, m_dwProfessional); }
    static size_t PK() { return offsetof(EnemyProbe, m_nPK); }
    static size_t Syndicate() { return offsetof(EnemyProbe, m_IDSyndicate); }
    static size_t Rank() { return offsetof(EnemyProbe, m_dwRank); }
    static size_t Mate() { return offsetof(EnemyProbe, m_szMate); }
};

static_assert(sizeof(CEnemy) == 0x44, "");
static_assert(std::has_virtual_destructor<CEnemy>::value, "");
static_assert(std::is_same<decltype(&CEnemy::CreateNew), CEnemy* (*)(OBJID, char*, BOOL)>::value, "");
static_assert(std::is_same<decltype(((CEnemy*) 0)->GetProfessional()), DWORD>::value, "");

TEST_CLASS(ClientEnemyHeader)
{
public:
    TEST_METHOD(MemberOffsetsMatchTheMessageWrites)
    {
        Assert::IsTrue(EnemyProbe::Id() == 0x4);
        Assert::IsTrue(EnemyProbe::Name() == 0x8);
        Assert::IsTrue(EnemyProbe::Online() == 0x18);
        Assert::IsTrue(EnemyProbe::Face() == 0x1C);
        Assert::IsTrue(EnemyProbe::Level() == 0x20);
        Assert::IsTrue(EnemyProbe::Professional() == 0x24);
        Assert::IsTrue(EnemyProbe::PK() == 0x28);
        Assert::IsTrue(EnemyProbe::Syndicate() == 0x2C);
        Assert::IsTrue(EnemyProbe::Rank() == 0x30);
        Assert::IsTrue(EnemyProbe::Mate() == 0x34);
    }
};
