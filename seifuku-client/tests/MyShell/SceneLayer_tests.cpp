#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "SceneLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CMapLayer) == 0xC, "");
static_assert(offsetof(CSceneLayer, m_setMapObj) == 0xC, "");
static_assert(sizeof(CSceneLayer) == 0xC + sizeof(DEQUE_MAPOBJ), "");
static_assert(std::is_base_of<CMapLayer, CSceneLayer>::value, "");
static_assert(std::is_polymorphic<CSceneLayer>::value, "");
static_assert(CMapLayer::LAYER_SCENE == 4, "");
static_assert(std::is_same<decltype(&CSceneLayer::LoadDataLayer), void (CSceneLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::AddObj), void (CSceneLayer::*)(CMapObj*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::CreateNew), CSceneLayer* (*)()>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::SaveDataLayer), void (CSceneLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::LoadTextLayer), void (CSceneLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::SaveTextLayer), void (CSceneLayer::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::DelObj), void (CSceneLayer::*)(CMapObj*)>::value, "");
static_assert(std::is_same<decltype(&CSceneLayer::GetFocusObj), CMapObj* (CSceneLayer::*)()>::value, "");

TEST_CLASS(ClientSceneLayerHeader)
{
public:
    TEST_METHOD(TheDequeFollowsTheBase)
    {
        Assert::IsTrue(offsetof(CSceneLayer, m_setMapObj) == 0xC);
    }
};
