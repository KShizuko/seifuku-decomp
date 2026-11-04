#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "2DMapItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapItem2DProbe : C2DMapItem
{
    static size_t Begin() { return offsetof(MapItem2DProbe, m_dwBegin); }
    static size_t Frame() { return offsetof(MapItem2DProbe, m_dwFrame); }
    static size_t ItemType() { return offsetof(MapItem2DProbe, m_idItemType); }
    static size_t World() { return offsetof(MapItem2DProbe, m_posWorld); }
    static size_t ShowName() { return offsetof(MapItem2DProbe, m_bShowName); }
    static size_t AniEffect() { return offsetof(MapItem2DProbe, m_objAniEffect); }
    static size_t EffectEx() { return offsetof(MapItem2DProbe, m_pEffectEx); }
};

static_assert(sizeof(C2DMapItem) == 0x214, "");
static_assert(std::is_base_of<C2DMapObj, C2DMapItem>::value, "");
static_assert(std::is_same<decltype(&C2DMapItem::CreateNew), C2DMapItem* (*)(CMyPos, OBJID)>::value, "");
static_assert(std::is_same<decltype(((C2DMapItem*) 0)->GetObjType()), int>::value, "");

TEST_CLASS(Client2DMapItemHeader)
{
public:
    TEST_METHOD(MemberOffsetsMatchTheCtorAndShow)
    {
        Assert::IsTrue(MapItem2DProbe::Begin() == 0x194);
        Assert::IsTrue(MapItem2DProbe::Frame() == 0x198);
        Assert::IsTrue(MapItem2DProbe::ItemType() == 0x19C);
        Assert::IsTrue(MapItem2DProbe::World() == 0x1A0);
        Assert::IsTrue(MapItem2DProbe::ShowName() == 0x1A8);
        Assert::IsTrue(MapItem2DProbe::AniEffect() == 0x1AC);
        Assert::IsTrue(MapItem2DProbe::EffectEx() == 0x210);
    }
};
