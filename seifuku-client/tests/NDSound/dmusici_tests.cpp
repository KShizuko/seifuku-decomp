#include "CppUnitTest.h"
#include <cstddef>
#include <dmusici.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(DMUS_OBJECTDESC) == 0x350, "");
static_assert(offsetof(DMUS_OBJECTDESC, dwValidData) == 0x04, "");
static_assert(offsetof(DMUS_OBJECTDESC, guidClass)   == 0x18, "");
static_assert(offsetof(DMUS_OBJECTDESC, wszName)     == 0x38, "");
static_assert(offsetof(DMUS_OBJECTDESC, wszFileName) == 0x138, "");
static_assert(offsetof(DMUS_OBJECTDESC, llMemLength) == 0x340, "");
static_assert(offsetof(DMUS_OBJECTDESC, pbMemData)   == 0x348, "");
static_assert(offsetof(DMUS_OBJECTDESC, pStream)     == 0x34C, "");
static_assert(sizeof(DMUS_AUDIOPARAMS) == 40, "");
static_assert(sizeof(MUSIC_TIME) == 4 && sizeof(REFERENCE_TIME) == 8, "");

static_assert(DMUS_SEGF_BEAT == 0x1000 && DMUS_SEGF_SECONDARY == 0x80 && DMUS_SEGF_DEFAULT == 0x4000, "");
static_assert(DMUS_PATH_BUFFER == 0x6000 && DMUS_PATH_PRIMARY_BUFFER == 0x8000, "");
static_assert(DMUS_APATH_DYNAMIC_3D == 6 && DMUS_APATH_DYNAMIC_STEREO == 8, "");
static_assert(DMUS_AUDIOF_ALL == 0x3F, "");
static_assert((DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY) == 0x402, "");
static_assert(DMUS_SEG_REPEAT_INFINITE == 0xFFFFFFFF, "");
static_assert(DMUS_E_LOADER_FAILEDOPEN == (HRESULT)0x88781182, "");

#define SLOT(n) { return (HRESULT)(n); }

struct LoaderProbe : IDirectMusicLoader8
{
    STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *)                                  SLOT(0)
    STDMETHOD_(ULONG,AddRef)()                                                       { return 1; }
    STDMETHOD_(ULONG,Release)()                                                      { return 2; }
    STDMETHOD(GetObject)(LPDMUS_OBJECTDESC, REFIID, LPVOID FAR *)                    SLOT(3)
    STDMETHOD(SetObject)(LPDMUS_OBJECTDESC)                                          SLOT(4)
    STDMETHOD(SetSearchDirectory)(REFGUID, WCHAR *, BOOL)                            SLOT(5)
    STDMETHOD(ScanDirectory)(REFGUID, WCHAR *, WCHAR *)                              SLOT(6)
    STDMETHOD(CacheObject)(IDirectMusicObject *)                                     SLOT(7)
    STDMETHOD(ReleaseObject)(IDirectMusicObject *)                                   SLOT(8)
    STDMETHOD(ClearCache)(REFGUID)                                                   SLOT(9)
    STDMETHOD(EnableCache)(REFGUID, BOOL)                                            SLOT(10)
    STDMETHOD(EnumObject)(REFGUID, DWORD, LPDMUS_OBJECTDESC)                         SLOT(11)
    STDMETHOD_(void,CollectGarbage)()                                                { s_collect = 12; }
    STDMETHOD(ReleaseObjectByUnknown)(IUnknown *)                                    SLOT(13)
    STDMETHOD(LoadObjectFromFile)(REFGUID, REFIID, WCHAR *, void **)                 SLOT(14)
    static int s_collect;
};
int LoaderProbe::s_collect = 0;

struct SegmentProbe : IDirectMusicSegment8
{
    STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *)                                  SLOT(0)
    STDMETHOD_(ULONG,AddRef)()                                                       { return 1; }
    STDMETHOD_(ULONG,Release)()                                                      { return 2; }
    STDMETHOD(GetLength)(MUSIC_TIME *)                                               SLOT(3)
    STDMETHOD(SetLength)(MUSIC_TIME)                                                 SLOT(4)
    STDMETHOD(GetRepeats)(DWORD *)                                                   SLOT(5)
    STDMETHOD(SetRepeats)(DWORD)                                                     SLOT(6)
    STDMETHOD(GetDefaultResolution)(DWORD *)                                         SLOT(7)
    STDMETHOD(SetDefaultResolution)(DWORD)                                           SLOT(8)
    STDMETHOD(GetTrack)(REFGUID, DWORD, DWORD, IDirectMusicTrack **)                 SLOT(9)
    STDMETHOD(GetTrackGroup)(IDirectMusicTrack *, DWORD *)                           SLOT(10)
    STDMETHOD(InsertTrack)(IDirectMusicTrack *, DWORD)                               SLOT(11)
    STDMETHOD(RemoveTrack)(IDirectMusicTrack *)                                      SLOT(12)
    STDMETHOD(InitPlay)(IDirectMusicSegmentState **, IDirectMusicPerformance *, DWORD) SLOT(13)
    STDMETHOD(GetGraph)(IDirectMusicGraph **)                                        SLOT(14)
    STDMETHOD(SetGraph)(IDirectMusicGraph *)                                         SLOT(15)
    STDMETHOD(AddNotificationType)(REFGUID)                                          SLOT(16)
    STDMETHOD(RemoveNotificationType)(REFGUID)                                       SLOT(17)
    STDMETHOD(GetParam)(REFGUID, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME *, void *)     SLOT(18)
    STDMETHOD(SetParam)(REFGUID, DWORD, DWORD, MUSIC_TIME, void *)                   SLOT(19)
    STDMETHOD(Clone)(MUSIC_TIME, MUSIC_TIME, IDirectMusicSegment **)                 SLOT(20)
    STDMETHOD(SetStartPoint)(MUSIC_TIME)                                             SLOT(21)
    STDMETHOD(GetStartPoint)(MUSIC_TIME *)                                           SLOT(22)
    STDMETHOD(SetLoopPoints)(MUSIC_TIME, MUSIC_TIME)                                 SLOT(23)
    STDMETHOD(GetLoopPoints)(MUSIC_TIME *, MUSIC_TIME *)                             SLOT(24)
    STDMETHOD(SetPChannelsUsed)(DWORD, DWORD *)                                      SLOT(25)
    STDMETHOD(SetTrackConfig)(REFGUID, DWORD, DWORD, DWORD, DWORD)                   SLOT(26)
    STDMETHOD(GetAudioPathConfig)(IUnknown **)                                       SLOT(27)
    STDMETHOD(Compose)(MUSIC_TIME, IDirectMusicSegment *, IDirectMusicSegment *, IDirectMusicSegment **) SLOT(28)
    STDMETHOD(Download)(IUnknown *)                                                  SLOT(29)
    STDMETHOD(Unload)(IUnknown *)                                                    SLOT(30)
};

struct AudioPathProbe : IDirectMusicAudioPath
{
    STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *)                                  SLOT(0)
    STDMETHOD_(ULONG,AddRef)()                                                       { return 1; }
    STDMETHOD_(ULONG,Release)()                                                      { return 2; }
    STDMETHOD(GetObjectInPath)(DWORD, DWORD, DWORD, REFGUID, DWORD, REFGUID, void **) SLOT(3)
    STDMETHOD(Activate)(BOOL)                                                        SLOT(4)
    STDMETHOD(SetVolume)(long, DWORD)                                                SLOT(5)
    STDMETHOD(ConvertPChannel)(DWORD, DWORD *)                                       SLOT(6)
};

struct StyleProbe : IDirectMusicStyle8
{
    STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *)                                  SLOT(0)
    STDMETHOD_(ULONG,AddRef)()                                                       { return 1; }
    STDMETHOD_(ULONG,Release)()                                                      { return 2; }
    STDMETHOD(GetBand)(WCHAR *, IDirectMusicBand **)                                 SLOT(3)
    STDMETHOD(EnumBand)(DWORD, WCHAR *)                                              SLOT(4)
    STDMETHOD(GetDefaultBand)(IDirectMusicBand **)                                   SLOT(5)
    STDMETHOD(EnumMotif)(DWORD, WCHAR *)                                             SLOT(6)
    STDMETHOD(GetMotif)(WCHAR *, IDirectMusicSegment **)                             SLOT(7)
    STDMETHOD(GetDefaultChordMap)(IDirectMusicChordMap **)                           SLOT(8)
    STDMETHOD(EnumChordMap)(DWORD, WCHAR *)                                          SLOT(9)
    STDMETHOD(GetChordMap)(WCHAR *, IDirectMusicChordMap **)                         SLOT(10)
    STDMETHOD(GetTimeSignature)(DMUS_TIMESIGNATURE *)                                SLOT(11)
    STDMETHOD(GetEmbellishmentLength)(DWORD, DWORD, DWORD *, DWORD *)                SLOT(12)
    STDMETHOD(GetTempo)(double *)                                                    SLOT(13)
    STDMETHOD(EnumPattern)(DWORD, DWORD, WCHAR *)                                    SLOT(14)
};

struct PerformanceProbe : IDirectMusicPerformance8
{
    STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *)                                  SLOT(0)
    STDMETHOD_(ULONG,AddRef)()                                                       { return 1; }
    STDMETHOD_(ULONG,Release)()                                                      { return 2; }
    STDMETHOD(Init)(IDirectMusic **, LPDIRECTSOUND, HWND)                            SLOT(3)
    STDMETHOD(PlaySegment)(IDirectMusicSegment *, DWORD, __int64, IDirectMusicSegmentState **) SLOT(4)
    STDMETHOD(Stop)(IDirectMusicSegment *, IDirectMusicSegmentState *, MUSIC_TIME, DWORD) SLOT(5)
    STDMETHOD(GetSegmentState)(IDirectMusicSegmentState **, MUSIC_TIME)              SLOT(6)
    STDMETHOD(SetPrepareTime)(DWORD)                                                 SLOT(7)
    STDMETHOD(GetPrepareTime)(DWORD *)                                               SLOT(8)
    STDMETHOD(SetBumperLength)(DWORD)                                                SLOT(9)
    STDMETHOD(GetBumperLength)(DWORD *)                                              SLOT(10)
    STDMETHOD(SendPMsg)(DMUS_PMSG *)                                                 SLOT(11)
    STDMETHOD(MusicToReferenceTime)(MUSIC_TIME, REFERENCE_TIME *)                    SLOT(12)
    STDMETHOD(ReferenceToMusicTime)(REFERENCE_TIME, MUSIC_TIME *)                    SLOT(13)
    STDMETHOD(IsPlaying)(IDirectMusicSegment *, IDirectMusicSegmentState *)          SLOT(14)
    STDMETHOD(GetTime)(REFERENCE_TIME *, MUSIC_TIME *)                               SLOT(15)
    STDMETHOD(AllocPMsg)(ULONG, DMUS_PMSG **)                                        SLOT(16)
    STDMETHOD(FreePMsg)(DMUS_PMSG *)                                                 SLOT(17)
    STDMETHOD(GetGraph)(IDirectMusicGraph **)                                        SLOT(18)
    STDMETHOD(SetGraph)(IDirectMusicGraph *)                                         SLOT(19)
    STDMETHOD(SetNotificationHandle)(HANDLE, REFERENCE_TIME)                         SLOT(20)
    STDMETHOD(GetNotificationPMsg)(DMUS_NOTIFICATION_PMSG **)                        SLOT(21)
    STDMETHOD(AddNotificationType)(REFGUID)                                          SLOT(22)
    STDMETHOD(RemoveNotificationType)(REFGUID)                                       SLOT(23)
    STDMETHOD(AddPort)(IDirectMusicPort *)                                           SLOT(24)
    STDMETHOD(RemovePort)(IDirectMusicPort *)                                        SLOT(25)
    STDMETHOD(AssignPChannelBlock)(DWORD, IDirectMusicPort *, DWORD)                 SLOT(26)
    STDMETHOD(AssignPChannel)(DWORD, IDirectMusicPort *, DWORD, DWORD)               SLOT(27)
    STDMETHOD(PChannelInfo)(DWORD, IDirectMusicPort **, DWORD *, DWORD *)            SLOT(28)
    STDMETHOD(DownloadInstrument)(IDirectMusicInstrument *, DWORD, IDirectMusicDownloadedInstrument **, DMUS_NOTERANGE *, DWORD, IDirectMusicPort **, DWORD *, DWORD *) SLOT(29)
    STDMETHOD(Invalidate)(MUSIC_TIME, DWORD)                                         SLOT(30)
    STDMETHOD(GetParam)(REFGUID, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME *, void *)     SLOT(31)
    STDMETHOD(SetParam)(REFGUID, DWORD, DWORD, MUSIC_TIME, void *)                   SLOT(32)
    STDMETHOD(GetGlobalParam)(REFGUID, void *, DWORD)                                SLOT(33)
    STDMETHOD(SetGlobalParam)(REFGUID, void *, DWORD)                                SLOT(34)
    STDMETHOD(GetLatencyTime)(REFERENCE_TIME *)                                      SLOT(35)
    STDMETHOD(GetQueueTime)(REFERENCE_TIME *)                                        SLOT(36)
    STDMETHOD(AdjustTime)(REFERENCE_TIME)                                            SLOT(37)
    STDMETHOD(CloseDown)()                                                           SLOT(38)
    STDMETHOD(GetResolvedTime)(REFERENCE_TIME, REFERENCE_TIME *, DWORD)              SLOT(39)
    STDMETHOD(MIDIToMusic)(BYTE, DMUS_CHORD_KEY *, BYTE, BYTE, WORD *)               SLOT(40)
    STDMETHOD(MusicToMIDI)(WORD, DMUS_CHORD_KEY *, BYTE, BYTE, BYTE *)               SLOT(41)
    STDMETHOD(TimeToRhythm)(MUSIC_TIME, DMUS_TIMESIGNATURE *, WORD *, BYTE *, BYTE *, short *) SLOT(42)
    STDMETHOD(RhythmToTime)(WORD, BYTE, BYTE, short, DMUS_TIMESIGNATURE *, MUSIC_TIME *) SLOT(43)
    STDMETHOD(InitAudio)(IDirectMusic **, IDirectSound **, HWND, DWORD, DWORD, DWORD, DMUS_AUDIOPARAMS *) SLOT(44)
    STDMETHOD(PlaySegmentEx)(IUnknown *, WCHAR *, IUnknown *, DWORD, __int64, IDirectMusicSegmentState **, IUnknown *, IUnknown *) SLOT(45)
    STDMETHOD(StopEx)(IUnknown *, __int64, DWORD)                                    SLOT(46)
    STDMETHOD(ClonePMsg)(DMUS_PMSG *, DMUS_PMSG **)                                  SLOT(47)
    STDMETHOD(CreateAudioPath)(IUnknown *, BOOL, IDirectMusicAudioPath **)           SLOT(48)
    STDMETHOD(CreateStandardAudioPath)(DWORD, DWORD, BOOL, IDirectMusicAudioPath **) SLOT(49)
    STDMETHOD(SetDefaultAudioPath)(IDirectMusicAudioPath *)                          SLOT(50)
    STDMETHOD(GetDefaultAudioPath)(IDirectMusicAudioPath **)                         SLOT(51)
    STDMETHOD(GetParamEx)(REFGUID, DWORD, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME *, void *) SLOT(52)
};
#undef SLOT

template <typename Fn> static Fn Entry(void* obj, unsigned offset)
{
    void** vtbl = *reinterpret_cast<void***>(obj);
    return reinterpret_cast<Fn>(vtbl[offset / 4]);
}

TEST_CLASS(DmusiciShim)
{
public:
    TEST_METHOD(LoaderSlotsMatchBinary)
    {
        LoaderProbe p;
        typedef HRESULT (__stdcall *GetObjectFn)(void*, LPDMUS_OBJECTDESC, REFIID, LPVOID FAR *);
        typedef HRESULT (__stdcall *SetSearchDirFn)(void*, REFGUID, WCHAR*, BOOL);
        typedef void    (__stdcall *CollectFn)(void*);
        typedef HRESULT (__stdcall *ReleaseByUnkFn)(void*, IUnknown*);
        typedef HRESULT (__stdcall *LoadFromFileFn)(void*, REFGUID, REFIID, WCHAR*, void**);
        Assert::AreEqual<long>(3,  Entry<GetObjectFn>(&p, 0x0C)(&p, NULL, GUID_NULL, NULL),          L"");
        Assert::AreEqual<long>(5,  Entry<SetSearchDirFn>(&p, 0x14)(&p, GUID_NULL, NULL, FALSE),      L"SetSearchDirectory at +0x14");
        Entry<CollectFn>(&p, 0x30)(&p);
        Assert::AreEqual(12, LoaderProbe::s_collect,                                                 L"CollectGarbage at +0x30");
        Assert::AreEqual<long>(13, Entry<ReleaseByUnkFn>(&p, 0x34)(&p, NULL),                        L"ReleaseObjectByUnknown at +0x34");
        Assert::AreEqual<long>(14, Entry<LoadFromFileFn>(&p, 0x38)(&p, GUID_NULL, GUID_NULL, NULL, NULL), L"");
    }

    TEST_METHOD(PerformanceSlotsMatchBinary)
    {
        PerformanceProbe p;
        typedef HRESULT (__stdcall *StopFn)(void*, IDirectMusicSegment*, IDirectMusicSegmentState*, MUSIC_TIME, DWORD);
        typedef HRESULT (__stdcall *IsPlayingFn)(void*, IDirectMusicSegment*, IDirectMusicSegmentState*);
        typedef HRESULT (__stdcall *SetGlobalFn)(void*, REFGUID, void*, DWORD);
        typedef HRESULT (__stdcall *CloseDownFn)(void*);
        typedef HRESULT (__stdcall *InitAudioFn)(void*, IDirectMusic**, IDirectSound**, HWND, DWORD, DWORD, DWORD, DMUS_AUDIOPARAMS*);
        typedef HRESULT (__stdcall *PlaySegExFn)(void*, IUnknown*, WCHAR*, IUnknown*, DWORD, __int64, IDirectMusicSegmentState**, IUnknown*, IUnknown*);
        typedef HRESULT (__stdcall *CreatePathFn)(void*, IUnknown*, BOOL, IDirectMusicAudioPath**);
        typedef HRESULT (__stdcall *CreateStdPathFn)(void*, DWORD, DWORD, BOOL, IDirectMusicAudioPath**);
        typedef HRESULT (__stdcall *GetDefPathFn)(void*, IDirectMusicAudioPath**);
        Assert::AreEqual<long>(5,  Entry<StopFn>(&p, 0x14)(&p, NULL, NULL, 0, 0),                    L"");
        Assert::AreEqual<long>(14, Entry<IsPlayingFn>(&p, 0x38)(&p, NULL, NULL),                     L"");
        Assert::AreEqual<long>(34, Entry<SetGlobalFn>(&p, 0x88)(&p, GUID_NULL, NULL, 0),             L"");
        Assert::AreEqual<long>(38, Entry<CloseDownFn>(&p, 0x98)(&p),                                 L"CloseDown at +0x98");
        Assert::AreEqual<long>(44, Entry<InitAudioFn>(&p, 0xB0)(&p, NULL, NULL, NULL, 0, 0, 0, NULL), L"InitAudio at +0xB0");
        Assert::AreEqual<long>(45, Entry<PlaySegExFn>(&p, 0xB4)(&p, NULL, NULL, NULL, 0, 0, NULL, NULL, NULL), L"PlaySegmentEx at +0xB4");
        Assert::AreEqual<long>(48, Entry<CreatePathFn>(&p, 0xC0)(&p, NULL, FALSE, NULL),             L"");
        Assert::AreEqual<long>(49, Entry<CreateStdPathFn>(&p, 0xC4)(&p, 0, 0, FALSE, NULL),          L"");
        Assert::AreEqual<long>(51, Entry<GetDefPathFn>(&p, 0xCC)(&p, NULL),                          L"GetDefaultAudioPath at +0xCC");
    }

    TEST_METHOD(SegmentSlotsMatchBinary)
    {
        SegmentProbe p;
        typedef HRESULT (__stdcall *SetRepeatsFn)(void*, DWORD);
        typedef HRESULT (__stdcall *GetParamFn)(void*, REFGUID, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME*, void*);
        typedef HRESULT (__stdcall *SetParamFn)(void*, REFGUID, DWORD, DWORD, MUSIC_TIME, void*);
        typedef HRESULT (__stdcall *UnkOutFn)(void*, IUnknown**);
        typedef HRESULT (__stdcall *UnkInFn)(void*, IUnknown*);
        Assert::AreEqual<long>(6,  Entry<SetRepeatsFn>(&p, 0x18)(&p, 0),                            L"");
        Assert::AreEqual<long>(18, Entry<GetParamFn>(&p, 0x48)(&p, GUID_NULL, 0, 0, 0, NULL, NULL),  L"");
        Assert::AreEqual<long>(19, Entry<SetParamFn>(&p, 0x4C)(&p, GUID_NULL, 0, 0, 0, NULL),        L"SetParam at +0x4C");
        Assert::AreEqual<long>(27, Entry<UnkOutFn>(&p, 0x6C)(&p, NULL),                              L"GetAudioPathConfig at +0x6C");
        Assert::AreEqual<long>(29, Entry<UnkInFn>(&p, 0x74)(&p, NULL),                               L"Download at +0x74");
        Assert::AreEqual<long>(30, Entry<UnkInFn>(&p, 0x78)(&p, NULL),                               L"");
    }

    TEST_METHOD(AudioPathAndStyleSlotsMatchBinary)
    {
        AudioPathProbe a;
        typedef HRESULT (__stdcall *GetObjInPathFn)(void*, DWORD, DWORD, DWORD, REFGUID, DWORD, REFGUID, void**);
        typedef HRESULT (__stdcall *ActivateFn)(void*, BOOL);
        typedef HRESULT (__stdcall *SetVolumeFn)(void*, long, DWORD);
        Assert::AreEqual<long>(3, Entry<GetObjInPathFn>(&a, 0x0C)(&a, 0, 0, 0, GUID_NULL, 0, GUID_NULL, NULL), L"GetObjectInPath at +0x0C");
        Assert::AreEqual<long>(4, Entry<ActivateFn>(&a, 0x10)(&a, FALSE),                            L"Activate at +0x10");
        Assert::AreEqual<long>(5, Entry<SetVolumeFn>(&a, 0x14)(&a, 0, 0),                            L"SetVolume at +0x14");

        StyleProbe s;
        typedef HRESULT (__stdcall *GetMotifFn)(void*, WCHAR*, IDirectMusicSegment**);
        Assert::AreEqual<long>(7, Entry<GetMotifFn>(&s, 0x1C)(&s, NULL, NULL),                       L"GetMotif at +0x1C");
    }

    TEST_METHOD(GuidsMatchBinaryTable)
    {
        Assert::AreEqual<unsigned long>(0xD2AC2892, CLSID_DirectMusicLoader.Data1);
        Assert::AreEqual<unsigned long>(0xD2AC2881, CLSID_DirectMusicPerformance.Data1);
        Assert::AreEqual<unsigned long>(0xC6784488, IID_IDirectMusicSegment8.Data1);
        Assert::AreEqual<int>(0x418F, IID_IDirectMusicSegment8.Data3);
        Assert::AreEqual<int>(0xD4,   IID_IDirectMusicSegment8.Data4[7]);
        Assert::AreEqual<unsigned long>(0x19E7C08C, IID_IDirectMusicLoader8.Data1);
        Assert::AreEqual<unsigned long>(0x679C4137, IID_IDirectMusicPerformance8.Data1);
        Assert::AreEqual<unsigned long>(0x06621075, GUID_StandardMIDIFile.Data1);
        Assert::IsTrue(IID_IDirectMusicChordMap8 == IID_IDirectMusicChordMap);
    }
};
