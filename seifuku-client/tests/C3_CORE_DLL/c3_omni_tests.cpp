#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include "c3_omni.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Omni) == 0x28, "");
static_assert(offsetof(C3Omni, lpName) == 0x00 && offsetof(C3Omni, pos) == 0x04, "");
static_assert(offsetof(C3Omni, color) == 0x10, "");
static_assert(offsetof(C3Omni, fRadius) == 0x20 && offsetof(C3Omni, fAttenuation) == 0x24, "");
static_assert(sizeof(D3DXCOLOR) == 16, "");

static_assert(std::is_same<decltype(&Omni_Load), BOOL (*)(C3Omni**, char*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Omni_Save), BOOL (*)(char*, C3Omni*, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Omni_Unload), void (*)(C3Omni**)>::value && std::is_same<decltype(&Omni_Clear), void (*)(C3Omni*)>::value, "");

TEST_CLASS(C3OmniHeader)
{
public:
    TEST_METHOD(ClearImageMatchesOmniClear)
    {
        C3Omni o;
        memset(&o, 0xCD, sizeof(o));
        o.lpName = 0;
        ZeroMemory(&o.pos, sizeof(D3DXVECTOR3));
        ZeroMemory(&o.color, sizeof(D3DXVECTOR3));
        o.fRadius = 10.0f;
        o.fAttenuation = 1.0f;
        const DWORD* w = reinterpret_cast<const DWORD*>(&o);
        for (int i = 0; i < 7; i++)
            Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, w[7]);
        Assert::AreEqual<DWORD>(0x41200000, w[8]);
        Assert::AreEqual<DWORD>(0x3F800000, w[9]);
    }
};

TEST_CLASS(C3OmniFunctions)
{
public:
    TEST_METHOD(ClearLeavesTheAlphaAlone)
    {
        C3Omni o;
        memset(&o, 0xCD, sizeof(o));
        Omni_Clear(&o);
        const DWORD* w = reinterpret_cast<const DWORD*>(&o);
        for (int i = 0; i < 7; i++) Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, w[7]);
        Assert::AreEqual(10.0f, o.fRadius); Assert::AreEqual(1.0f, o.fAttenuation);
    }

    TEST_METHOD(SaveThenLoadRoundTrip)
    {
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        std::string path = dir + "\\seifuku_omni.c3";
        SeifukuFixtures::WriteBlob(path, "MAXFILE C3 00001", 16);
        C3Omni o;
        Omni_Clear(&o);
        char name[] = "light";
        o.lpName = name;
        o.pos = D3DXVECTOR3(1, 2, 3);
        o.color = D3DXCOLOR(0.25f, 0.5f, 0.75f, 0.125f);
        Assert::IsTrue(Omni_Save((char*) path.c_str(), &o, FALSE));
        C3Omni* l = 0;
        Assert::IsTrue(Omni_Load(&l, (char*) path.c_str(), 0));
        Assert::AreEqual("light", l->lpName);
        Assert::AreEqual(1.0f, l->pos.x); Assert::AreEqual(3.0f, l->pos.z);
        Assert::AreEqual(0.25f, l->color.r); Assert::AreEqual(0.75f, l->color.b);
        Assert::AreEqual(10.0f, l->fRadius);
        Assert::IsFalse(Omni_Load(&l, (char*) path.c_str(), 1));
        Assert::IsFalse(Omni_Load(&l, (char*) (dir + "\\seifuku_missing.c3").c_str(), 0));
        Omni_Unload(&l);
        Assert::IsNull(l);
    }
};
