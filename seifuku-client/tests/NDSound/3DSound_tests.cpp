#include "CppUnitTest.h"
#include <cstddef>
#include "3DSound.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(DS3DBUFFER) == 64 && sizeof(DS3DLISTENER) == 64, "");

static_assert(sizeof(C3DSound) == 0x1BC, "");

static_assert(offsetof(C3DSound, m_szFileName)  == 0xB0, "");
static_assert(offsetof(C3DSound, m_dwSleepTime) == 0x1B4, "");
static_assert(offsetof(C3DSound, m_dwEndTime)   == 0x1B8, "");

TEST_CLASS(Sound3DHeader)
{
public:
    TEST_METHOD(ObjectSizeMatchesBinary)
    {
        Assert::AreEqual<size_t>(0x1BC, sizeof(C3DSound));
        Assert::AreEqual<size_t>(0x1B8, offsetof(C3DSound, m_dwEndTime));
    }
};

TEST_CLASS(Sound3DWithoutRuntime)
{
public:

    TEST_METHOD(ResetOrigPosStoresPositions)
    {
        C3DSound s;
        s.ResetOrigPos(3, -4, 5);
        Assert::AreEqual(3,  s.GetWorldX());
        Assert::AreEqual(-4, s.GetWorldY());
        Assert::AreEqual(5,  s.GetWorldZ());
    }

    TEST_METHOD(SetVolumeWithoutManagerOnlyRecords)
    {
        C3DSound s;
        s.SetVolume(37);
        Assert::AreEqual(37, s.GetVolume());
    }

    TEST_METHOD(IsBeginningCountsTenCalls)
    {
        C3DSound s;
        int nTrue = 0;
        for (int i = 0; i < 20; i++)
            if (s.IsBeginning()) nTrue++;
        Assert::AreEqual(9, nTrue);
        Assert::AreEqual<BOOL>(0, s.IsBeginning());
    }
};
