#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Bulletin.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct BulletinProbe : CBulletin
{
    static size_t Titles() { return offsetof(BulletinProbe, m_setTitle); }
    static size_t Msg() { return offsetof(BulletinProbe, m_szMsg); }
    static size_t ListIndex() { return offsetof(BulletinProbe, m_nCurrentListIndex); }
    static size_t Channel() { return offsetof(BulletinProbe, m_nCurrentChannel); }
};

static_assert(sizeof(BulletinTitleInfo) == 0x70, "");
static_assert(offsetof(BulletinTitleInfo, szTitle) == 0x10 && offsetof(BulletinTitleInfo, szTime) == 0x50, "");
static_assert(sizeof(CBulletin) == 4 + sizeof(STRING_SET) + 0x208, "");
static_assert(std::has_virtual_destructor<CBulletin>::value, "");
static_assert(std::is_same<decltype(&CBulletin::SetList), void (CBulletin::*)(unsigned short, unsigned short, CNetStringPacker&)>::value, "");
static_assert(std::is_same<decltype(&CBulletin::SendBulletin), void (CBulletin::*)(char*, unsigned short)>::value, "");

TEST_CLASS(ClientBulletinHeader)
{
public:
    TEST_METHOD(NoteAndIndicesFollowTheVector)
    {
        const size_t v = sizeof(STRING_SET);
        Assert::IsTrue(BulletinProbe::Titles() == 0x4);
        Assert::IsTrue(BulletinProbe::Msg() == 0x4 + v);
        Assert::IsTrue(BulletinProbe::ListIndex() == 0x4 + v + 0x200);
        Assert::IsTrue(BulletinProbe::Channel() == 0x4 + v + 0x204);
    }
};
