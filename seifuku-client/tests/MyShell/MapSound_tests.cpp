#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MapSound.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MapSoundProbe : CMapSound
{
    static size_t World() { return offsetof(MapSoundProbe, m_posWorld); }
    static size_t Cell() { return offsetof(MapSoundProbe, m_posCell); }
    static size_t Focus() { return offsetof(MapSoundProbe, m_bFocus); }
    static size_t Sound() { return offsetof(MapSoundProbe, m_pSound); }
    static size_t Range() { return offsetof(MapSoundProbe, m_nRange); }
    static size_t Volume() { return offsetof(MapSoundProbe, m_nVolume); }
    static size_t File() { return offsetof(MapSoundProbe, m_szFile); }
};

static_assert(sizeof(CMapSound) == 0x130, "");
static_assert(_MAX_PATH == 260, "");
static_assert(std::is_base_of<CMapObj, CMapSound>::value, "");
static_assert(MAP_SOUND == 15, "");
static_assert(std::is_same<decltype(&CMapSound::SetWorldPos), void (CMapSound::*)(CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CMapSound::LoadDataObj), void (CMapSound::*)(FILE*)>::value, "");
static_assert(std::is_same<decltype(&CMapSound::Play), void (CMapSound::*)()>::value, "");

TEST_CLASS(ClientMapSoundHeader)
{
public:
    TEST_METHOD(MembersFollowTheBase)
    {
        Assert::IsTrue(MapSoundProbe::World() == 0xC);
        Assert::IsTrue(MapSoundProbe::Cell() == 0x14);
        Assert::IsTrue(MapSoundProbe::Focus() == 0x1C);
        Assert::IsTrue(MapSoundProbe::Sound() == 0x20);
        Assert::IsTrue(MapSoundProbe::Range() == 0x24);
        Assert::IsTrue(MapSoundProbe::Volume() == 0x28);
        Assert::IsTrue(MapSoundProbe::File() == 0x2C);
    }
};
