#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include "dsutil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(MMCKINFO) == 20, "");
static_assert(sizeof(MMIOINFO) == 72, "");

static_assert(sizeof(CSoundManager) == 4, "");
static_assert(sizeof(CSound)        == 0xBC, "");
static_assert(sizeof(CWaveFile)     == 0x90, "");

static_assert(offsetof(CSound, m_nWorldX)         == 0x14, "");
static_assert(offsetof(CSound, m_nWorldY)         == 0x18, "");
static_assert(offsetof(CSound, m_nRange)          == 0x1C, "");
static_assert(offsetof(CSound, m_nPan)            == 0x20, "");
static_assert(offsetof(CSound, m_n3DSoundActived) == 0x24, "");
static_assert(offsetof(CSound, m_dwSleepTime)     == 0x28, "");
static_assert(offsetof(CSound, m_szFileName)      == 0x2C, "");
static_assert(offsetof(CSound, m_dwControl)       == 0xAC, "");
static_assert(offsetof(CSound, m_nLoop)           == 0xB0, "");
static_assert(offsetof(CSound, m_nVolume)         == 0xB4, "");

static_assert(offsetof(CWaveFile, m_pwfx)                 == 0x00, "");
static_assert(offsetof(CWaveFile, m_hmmio)                == 0x04, "");
static_assert(offsetof(CWaveFile, m_ck)                   == 0x08, "");
static_assert(offsetof(CWaveFile, m_ckRiff)               == 0x1C, "");
static_assert(offsetof(CWaveFile, m_dwSize)               == 0x30, "");
static_assert(offsetof(CWaveFile, m_mmioinfoOut)          == 0x34, "");
static_assert(offsetof(CWaveFile, m_dwFlags)              == 0x7C, "");
static_assert(offsetof(CWaveFile, m_bIsReadingFromMemory) == 0x80, "");
static_assert(offsetof(CWaveFile, m_pbData)               == 0x84, "");
static_assert(offsetof(CWaveFile, m_pbDataCur)            == 0x88, "");
static_assert(offsetof(CWaveFile, m_ulDataSize)           == 0x8C, "");

struct CSoundLayoutProbe : CSound
{
    static void Check()
    {
        static_assert(offsetof(CSoundLayoutProbe, m_apDSBuffer)     == 0x04, "");
        static_assert(offsetof(CSoundLayoutProbe, m_dwDSBufferSize) == 0x08, "");
        static_assert(offsetof(CSoundLayoutProbe, m_pWaveFile)      == 0x0C, "");
        static_assert(offsetof(CSoundLayoutProbe, m_dwNumBuffers)   == 0x10, "");
    }
};

static std::string FixturePath(const char* name)
{
    std::string dir(__FILE__);
    dir.erase(dir.find_last_of("\\/") + 1);
    return dir + "fixtures\\" + name;
}

static const DWORD RAMP_SAMPLES = 64;

TEST_CLASS(DsutilHeader)
{
public:
    TEST_METHOD(ObjectSizesMatchBinary)
    {
        Assert::AreEqual<size_t>(4,    sizeof(CSoundManager));
        Assert::AreEqual<size_t>(0xBC, sizeof(CSound));
        Assert::AreEqual<size_t>(0x90, sizeof(CWaveFile));
    }

    TEST_METHOD(CSoundMemberOffsetsMatchBinary)
    {
        Assert::AreEqual<size_t>(0x14, offsetof(CSound, m_nWorldX));
        Assert::AreEqual<size_t>(0x2C, offsetof(CSound, m_szFileName));
        Assert::AreEqual<size_t>(0xAC, offsetof(CSound, m_dwControl));
        Assert::AreEqual<size_t>(0xB0, offsetof(CSound, m_nLoop));
        Assert::AreEqual<size_t>(0xB4, offsetof(CSound, m_nVolume));
    }

    TEST_METHOD(CWaveFileMemberOffsetsMatchBinary)
    {
        Assert::AreEqual<size_t>(0x34, offsetof(CWaveFile, m_mmioinfoOut));
        Assert::AreEqual<size_t>(0x7C, offsetof(CWaveFile, m_dwFlags));
        Assert::AreEqual<size_t>(0x8C, offsetof(CWaveFile, m_ulDataSize));
    }

    TEST_METHOD(VolumeAndPanConstants)
    {
        Assert::AreEqual(-5000, VOL_MIN);
        Assert::AreEqual( 4000, PAN_MAX);
        Assert::AreEqual(-4000, PAN_MIN);
    }
};

TEST_CLASS(WaveFileRead)
{
public:
    TEST_METHOD(ConstructorZeroesState)
    {
        CWaveFile wav;
        Assert::IsNull(wav.m_pwfx);
        Assert::IsNull((void*)wav.m_hmmio);
        Assert::AreEqual<DWORD>(0, wav.m_dwSize);
        Assert::AreEqual<BOOL>(FALSE, wav.m_bIsReadingFromMemory);
    }

    TEST_METHOD(OpenParsesRiffHeader)
    {
        CWaveFile wav;
        std::string path = FixturePath("ramp8k.wav");
        Assert::IsTrue(SUCCEEDED(wav.Open(&path[0], NULL, WAVEFILE_READ)), L"Open failed");
        Assert::AreEqual<DWORD>(RAMP_SAMPLES, wav.GetSize());
        Assert::IsNotNull(wav.GetFormat());
        Assert::AreEqual<int>(WAVE_FORMAT_PCM, wav.GetFormat()->wFormatTag);
        Assert::AreEqual<int>(1, wav.GetFormat()->nChannels);
        Assert::AreEqual<DWORD>(8000, wav.GetFormat()->nSamplesPerSec);
        Assert::AreEqual<int>(8, wav.GetFormat()->wBitsPerSample);
        Assert::AreEqual<int>(0, wav.GetFormat()->cbSize);
        Assert::IsTrue(SUCCEEDED(wav.Close()));
    }

    TEST_METHOD(ReadReturnsSamplesAndStopsAtEnd)
    {
        CWaveFile wav;
        std::string path = FixturePath("ramp8k.wav");
        Assert::IsTrue(SUCCEEDED(wav.Open(&path[0], NULL, WAVEFILE_READ)));

        BYTE buf[128];
        DWORD got = 0;
        Assert::IsTrue(SUCCEEDED(wav.Read(buf, 40, &got)));
        Assert::AreEqual<DWORD>(40, got);
        for (DWORD i = 0; i < 40; i++) Assert::AreEqual<int>(128 + i, buf[i]);

        Assert::IsTrue(SUCCEEDED(wav.Read(buf, sizeof(buf), &got)));
        Assert::AreEqual<DWORD>(24, got);
        for (DWORD i = 0; i < 24; i++) Assert::AreEqual<int>(128 + 40 + i, buf[i]);

        Assert::IsTrue(SUCCEEDED(wav.Read(buf, sizeof(buf), &got)));
        Assert::AreEqual<DWORD>(0, got);
        wav.Close();
    }

    TEST_METHOD(ResetFileRewindsToDataChunk)
    {
        CWaveFile wav;
        std::string path = FixturePath("ramp8k.wav");
        Assert::IsTrue(SUCCEEDED(wav.Open(&path[0], NULL, WAVEFILE_READ)));
        BYTE buf[64];
        DWORD got = 0;
        wav.Read(buf, 64, &got);
        Assert::AreEqual<DWORD>(64, got);
        Assert::IsTrue(SUCCEEDED(wav.ResetFile()));
        Assert::IsTrue(SUCCEEDED(wav.Read(buf, 8, &got)));
        Assert::AreEqual<DWORD>(8, got);
        Assert::AreEqual<int>(128, buf[0]);
        Assert::AreEqual<int>(135, buf[7]);
        wav.Close();
    }

    TEST_METHOD(ReadRejectsNullArguments)
    {
        CWaveFile wav;
        std::string path = FixturePath("ramp8k.wav");
        Assert::IsTrue(SUCCEEDED(wav.Open(&path[0], NULL, WAVEFILE_READ)));
        BYTE buf[8];
        DWORD got = 0;
        Assert::AreEqual<long>(E_INVALIDARG, wav.Read(NULL, 8, &got));
        Assert::AreEqual<long>(E_INVALIDARG, wav.Read(buf, 8, NULL));
        wav.Close();
    }

    TEST_METHOD(OpenFailsForMissingFile)
    {
        CWaveFile wav;
        std::string path = FixturePath("does-not-exist.wav");
        Assert::IsTrue(FAILED(wav.Open(&path[0], NULL, WAVEFILE_READ)));
        Assert::AreEqual<long>(E_INVALIDARG, wav.Open(NULL, NULL, WAVEFILE_READ));
    }

    TEST_METHOD(UnopenedFileReportsNotInitialized)
    {
        CWaveFile wav;
        BYTE buf[8];
        DWORD got = 0;
        Assert::AreEqual<long>(CO_E_NOTINITIALIZED, wav.ResetFile());
        Assert::AreEqual<long>(CO_E_NOTINITIALIZED, wav.Read(buf, 8, &got));
    }
};

TEST_CLASS(WaveFileMemory)
{
public:
    TEST_METHOD(OpenFromMemoryReadsAndClips)
    {
        BYTE data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
        CWaveFile wav;
        Assert::AreEqual<long>(S_OK, wav.OpenFromMemory(data, sizeof(data), &wfx, WAVEFILE_READ));
        Assert::AreEqual<BOOL>(TRUE, wav.m_bIsReadingFromMemory);
        Assert::IsTrue(wav.GetFormat() == &wfx);

        BYTE buf[16];
        DWORD got = 0;
        Assert::AreEqual<long>(S_OK, wav.Read(buf, 6, &got));
        Assert::AreEqual<DWORD>(6, got);
        Assert::AreEqual<int>(5, buf[5]);

        Assert::AreEqual<long>(S_OK, wav.Read(buf, 16, &got));
        Assert::AreEqual<DWORD>(10, got);
        Assert::AreEqual<int>(9, buf[9]);

        Assert::AreEqual<long>(S_OK, wav.ResetFile());
        Assert::AreEqual<long>(E_NOTIMPL, wav.OpenFromMemory(data, sizeof(data), &wfx, WAVEFILE_WRITE));
    }

    TEST_METHOD(WriteIsNotImplementedForMemory)
    {
        BYTE data[4] = { 1, 2, 3, 4 };
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
        CWaveFile wav;
        wav.OpenFromMemory(data, sizeof(data), &wfx, WAVEFILE_READ);
        UINT wrote = 0;
        Assert::AreEqual<long>(E_NOTIMPL, wav.Write(4, data, &wrote));
    }
};

TEST_CLASS(WaveFileWrite)
{
public:
    TEST_METHOD(WriteThenReadRoundTrip)
    {
        std::string path = FixturePath("_roundtrip.tmp.wav");
        DeleteFileA(path.c_str());

        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
        BYTE data[32];
        for (int i = 0; i < 32; i++) data[i] = (BYTE)(200 + i);
        {
            CWaveFile out;
            Assert::IsTrue(SUCCEEDED(out.Open(&path[0], &wfx, WAVEFILE_WRITE)), L"Open for write failed");
            UINT wrote = 0;
            Assert::AreEqual<long>(S_OK, out.Write(32, data, &wrote), L"Write");
            Assert::AreEqual<UINT>(32, wrote);

            Assert::AreEqual<long>(-1, out.Close(), L"Close");
            Assert::IsNotNull((void*)out.m_hmmio, L"Close leaves the handle open");
            Assert::AreEqual<UINT>(0, mmioClose(out.m_hmmio, 0));
            out.m_hmmio = NULL;
        }
        {
            CWaveFile in;
            Assert::IsTrue(SUCCEEDED(in.Open(&path[0], NULL, WAVEFILE_READ)), L"Re-open failed");
            Assert::AreEqual<DWORD>(32, in.GetSize());
            Assert::AreEqual<DWORD>(8000, in.GetFormat()->nSamplesPerSec);
            BYTE buf[32];
            DWORD got = 0;
            Assert::AreEqual<long>(S_OK, in.Read(buf, 32, &got), L"Read back");
            Assert::AreEqual<DWORD>(32, got);
            Assert::AreEqual(0, memcmp(buf, data, 32));
            in.Close();
        }
        Assert::IsTrue(DeleteFileA(path.c_str()) != 0);
    }

    TEST_METHOD(WriteRejectsNullArguments)
    {
        std::string path = FixturePath("_nullargs.tmp.wav");
        DeleteFileA(path.c_str());
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
        {
            CWaveFile out;
            Assert::IsTrue(SUCCEEDED(out.Open(&path[0], &wfx, WAVEFILE_WRITE)));
            BYTE b = 0;
            UINT wrote = 0;
            Assert::AreEqual<long>(E_INVALIDARG, out.Write(1, NULL, &wrote));
            Assert::AreEqual<long>(E_INVALIDARG, out.Write(1, &b, NULL));
            out.Close();
            if (out.m_hmmio) { mmioClose(out.m_hmmio, 0); out.m_hmmio = NULL; }
        }
        Assert::IsTrue(DeleteFileA(path.c_str()) != 0);
    }
};
