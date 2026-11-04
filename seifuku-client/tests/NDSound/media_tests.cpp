#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include "media.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CMedia) == 0x148, "");
static_assert(offsetof(CMedia, m_szFileName) == 0x44, "");
static_assert(sizeof(LONGLONG) == 8, "");

static_assert(Stopped == 0 && Paused == 1 && Running == 2 && Init == 3, "");

TEST_CLASS(MediaHeader)
{
public:
    TEST_METHOD(ObjectSizeMatchesBinary)
    {
        Assert::AreEqual<size_t>(0x148, sizeof(CMedia));
        Assert::AreEqual<size_t>(0x44, offsetof(CMedia, m_szFileName));
    }

    TEST_METHOD(PlayStateValues)
    {
        Assert::AreEqual(0, (int)Stopped);
        Assert::AreEqual(1, (int)Paused);
        Assert::AreEqual(2, (int)Running);
        Assert::AreEqual(3, (int)Init);
    }
};

#include <new>
struct ZeroedMedia
{
    unsigned char buf[sizeof(CMedia)];
    CMedia* p;
    ZeroedMedia()  { memset(buf, 0, sizeof(buf)); p = new (buf) CMedia(); }
    ~ZeroedMedia() { p->~CMedia(); }
};

TEST_CLASS(MediaWithoutGraph)
{
public:
    TEST_METHOD(ConstructorDefaults)
    {
        ZeroedMedia z;
        Assert::AreEqual(80, z.p->GetVolume());
    }

    TEST_METHOD(SetVolumeWithoutGraphReturnsZero)
    {
        ZeroedMedia z;
        Assert::AreEqual(0, z.p->SetVolume(50));
        Assert::AreEqual(80, z.p->GetVolume());
    }

    TEST_METHOD(GetDurationWithoutSeekingInterfaceFails)
    {
        ZeroedMedia z;
        Assert::AreEqual<BOOL>(0, z.p->GetDuration());
    }

    TEST_METHOD(ControlCallsAreNoOpsWithoutInterfaces)
    {
        ZeroedMedia z;
        z.p->Pause();
        z.p->Stop();
        z.p->SetVideoWindow(10, 20);
        Assert::AreEqual(80, z.p->GetVolume());
    }

    TEST_METHOD(CloseClearsFileName)
    {
        ZeroedMedia z;
        z.p->m_szFileName[0] = 'x';
        z.p->Close();
        Assert::AreEqual<int>(0, z.p->m_szFileName[0]);
    }
};
