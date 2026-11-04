#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "InfoViewManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct InfoViewManagerProbe : CInfoViewManager
{
    static size_t Views() { return offsetof(InfoViewManagerProbe, m_setView); }
};

static_assert(sizeof(CInfoViewManager) == 0x4 + sizeof(vector<IInfoView*>), "");
static_assert(std::is_polymorphic<CInfoViewManager>::value, "");
static_assert(std::is_same<decltype(&CInfoViewManager::AddStringView), void (CInfoViewManager::*)(char*, CMyPos, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CInfoViewManager::AddAniView), void (CInfoViewManager::*)(char*, char*, CMyPos, int)>::value, "");
static_assert(std::is_same<decltype(&CInfoViewManager::Clear), void (CInfoViewManager::*)()>::value, "");
static_assert(std::is_same<decltype(&CInfoViewManager::Show), void (CInfoViewManager::*)()>::value, "");

TEST_CLASS(ClientInfoViewManagerHeader)
{
public:
    TEST_METHOD(TheVectorFollowsTheVtable)
    {
        Assert::IsTrue(InfoViewManagerProbe::Views() == 0x4);
    }
};
