#include "CppUnitTest.h"
#include <type_traits>
#include "SurfaceLayer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CSurfaceLayer) == 0xC, "");
static_assert(sizeof(CSurfaceLayer) == sizeof(CMapLayer), "");
static_assert(std::is_base_of<CMapLayer, CSurfaceLayer>::value, "");
static_assert(std::is_polymorphic<CSurfaceLayer>::value, "");
static_assert(CMapLayer::LAYER_SURFACE == 6, "");
static_assert(std::is_same<decltype(&CSurfaceLayer::CreateNew), CSurfaceLayer* (*)()>::value, "");

TEST_CLASS(ClientSurfaceLayerHeader)
{
public:
    TEST_METHOD(TheLayerIsTheBaseAlone)
    {
        Assert::IsTrue(sizeof(CSurfaceLayer) == 0xC);
    }
};
