#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "3DMagicMapItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MagicMapItemProbe
{
    static size_t Bg() { return offsetof(C3DMagicMapItem, m_posBg); }
    static size_t Start() { return offsetof(C3DMagicMapItem, m_szStart); }
    static size_t Last() { return offsetof(C3DMagicMapItem, m_szLast); }
    static size_t End() { return offsetof(C3DMagicMapItem, m_szEnd); }
    static size_t Id() { return offsetof(C3DMagicMapItem, m_id); }
    static size_t Unit() { return offsetof(C3DMagicMapItem, m_pUnit); }
    static size_t Moment() { return offsetof(C3DMagicMapItem, m_nMoment); }
};

static_assert(sizeof(C3DMapObj) == 0x14, "");
static_assert(sizeof(C3DMagicMapItem) == 0x334, "");
static_assert(std::is_base_of<C3DMapObj, C3DMagicMapItem>::value, "");
static_assert(std::is_polymorphic<C3DMagicMapItem>::value, "");
static_assert(MAP_3DMAGICMAPITEM == 17, "");
static_assert(std::is_same<decltype(&C3DMagicMapItem::CreateNew), C3DMagicMapItem* (*)(OBJID, OBJID)>::value, "");

TEST_CLASS(ClientMagicMapItemHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheBinaryReadsThem)
    {
        Assert::IsTrue(MagicMapItemProbe::Bg() == 0x14);
        Assert::IsTrue(MagicMapItemProbe::Start() == 0x1C);
        Assert::IsTrue(MagicMapItemProbe::Last() == 0x120);
        Assert::IsTrue(MagicMapItemProbe::End() == 0x224);
        Assert::IsTrue(MagicMapItemProbe::Id() == 0x328);
        Assert::IsTrue(MagicMapItemProbe::Unit() == 0x32C);
        Assert::IsTrue(MagicMapItemProbe::Moment() == 0x330);
    }
};
