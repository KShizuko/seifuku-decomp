#include "CppUnitTest.h"
#include <cstddef>
#include "NDsound.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CSoundManager) == 4, "");
static_assert(offsetof(CNDsound, m_hWnd) == 4, "");

static_assert(sizeof(CNDsound) - 3 * sizeof(dequeSound) == 0x34, "");
static_assert(sizeof(dequeSound) == sizeof(dequeMedia) && sizeof(dequeMedia) == sizeof(deque3DSound), "");

TEST_CLASS(NDsoundHeader)
{
public:
    TEST_METHOD(NonDequePartMatchesBinary)
    {
        Assert::AreEqual<size_t>(0x34, sizeof(CNDsound) - 3 * sizeof(dequeSound));
        Assert::AreEqual<size_t>(4, offsetof(CNDsound, m_hWnd));
    }

    TEST_METHOD(InitResultCodes)
    {
        Assert::AreEqual(-3, SETPRIMARYBUFFERFORMATERR);
        Assert::AreEqual(-2, SETCOOPERACTIVELEVELERR);
        Assert::AreEqual(-1, DIRECTSOUNDCREATEERR);
        Assert::AreEqual( 0, INITOK);
    }

};

extern CNDsound sound;

TEST_CLASS(NDsoundBeforeInit)
{
public:
    TEST_METHOD(ConstructorDefaults)
    {
        Assert::IsNull((void*)sound.m_hWnd);
        Assert::AreEqual(80, sound.GetMusicVolume());
        Assert::AreEqual(80, sound.GetSoundVolume());
    }

    TEST_METHOD(VolumeSettersAreGuardedByInit)
    {
        sound.SetSoundVolume(150);
        sound.SetMusicVolume(10);
        Assert::AreEqual(80, sound.GetSoundVolume());
        Assert::AreEqual(80, sound.GetMusicVolume());
    }

    TEST_METHOD(PlayAndLookupReturnNullBeforeInit)
    {
        char name[] = "Sound/Eat.wav";
        Assert::IsNull((void*)sound.PlaySound(name, 0, 0, 0, 0, 100));
        Assert::IsNull((void*)sound.PlayMedia(NULL, name, 0, 0));
        Assert::IsNull((void*)sound.GetSound(name, 0, 0));
        Assert::IsNull((void*)sound.Get3DSound(name, 0, 0));
        Assert::AreEqual<BOOL>(0, sound.CloseSound(name, 0, 0));
        Assert::AreEqual<BOOL>(0, sound.CloseMedia(name));
    }

    TEST_METHOD(PauseAndContinueAreGuardedByInit)
    {
        sound.Pause(TRUE);
        sound.Continue(TRUE);
        sound.ProcessSound(1, 2, 0);
        Assert::AreEqual(80, sound.GetMusicVolume());
    }
};
