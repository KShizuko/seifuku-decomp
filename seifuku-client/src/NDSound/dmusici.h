#ifndef SFK_DMUSICI_H
#define SFK_DMUSICI_H

#include <windows.h>
#include <objbase.h>
#include <dsound.h>
#include <dmusicc.h>

typedef long MUSIC_TIME;

typedef struct _DMUS_VERSION
{
    DWORD dwVersionMS;
    DWORD dwVersionLS;
} DMUS_VERSION, *LPDMUS_VERSION;

#define DMUS_MAX_NAME      64
#define DMUS_MAX_CATEGORY  64
#define DMUS_MAX_FILENAME  MAX_PATH

typedef struct _DMUS_OBJECTDESC
{
    DWORD          dwSize;
    DWORD          dwValidData;
    GUID           guidObject;
    GUID           guidClass;
    FILETIME       ftDate;
    DMUS_VERSION   vVersion;
    WCHAR          wszName[DMUS_MAX_NAME];
    WCHAR          wszCategory[DMUS_MAX_CATEGORY];
    WCHAR          wszFileName[DMUS_MAX_FILENAME];
    LONGLONG       llMemLength;
    LPBYTE         pbMemData;
    IStream *      pStream;
} DMUS_OBJECTDESC, *LPDMUS_OBJECTDESC;

typedef struct _DMUS_AUDIOPARAMS
{
    DWORD   dwSize;
    BOOL    fInitNow;
    DWORD   dwValidData;
    DWORD   dwFeatures;
    DWORD   dwVoices;
    DWORD   dwSampleRate;
    CLSID   clsidDefaultSynth;
} DMUS_AUDIOPARAMS, *LPDMUS_AUDIOPARAMS;

typedef struct _DMUS_PMSG              DMUS_PMSG;
typedef struct _DMUS_NOTIFICATION_PMSG DMUS_NOTIFICATION_PMSG;
typedef struct _DMUS_TIMESIGNATURE     DMUS_TIMESIGNATURE;
typedef struct _DMUS_CHORD_KEY         DMUS_CHORD_KEY;

#define DMUS_OBJ_OBJECT         (1 << 0)
#define DMUS_OBJ_CLASS          (1 << 1)
#define DMUS_OBJ_NAME           (1 << 2)
#define DMUS_OBJ_CATEGORY       (1 << 3)
#define DMUS_OBJ_FILENAME       (1 << 4)
#define DMUS_OBJ_FULLPATH       (1 << 5)
#define DMUS_OBJ_URL            (1 << 6)
#define DMUS_OBJ_VERSION        (1 << 7)
#define DMUS_OBJ_DATE           (1 << 8)
#define DMUS_OBJ_LOADED         (1 << 9)
#define DMUS_OBJ_MEMORY         (1 << 10)
#define DMUS_OBJ_STREAM         (1 << 11)

#define DMUS_SEG_REPEAT_INFINITE    0xFFFFFFFF

#define DMUS_SEGF_REFTIME               (1 << 6)
#define DMUS_SEGF_SECONDARY             (1 << 7)
#define DMUS_SEGF_QUEUE                 (1 << 8)
#define DMUS_SEGF_CONTROL               (1 << 9)
#define DMUS_SEGF_AFTERPREPARETIME      (1 << 10)
#define DMUS_SEGF_GRID                  (1 << 11)
#define DMUS_SEGF_BEAT                  (1 << 12)
#define DMUS_SEGF_MEASURE               (1 << 13)
#define DMUS_SEGF_DEFAULT               (1 << 14)
#define DMUS_SEGF_NOINVALIDATE          (1 << 15)
#define DMUS_SEGF_ALIGN                 (1 << 16)
#define DMUS_SEGF_VALID_START_BEAT      (1 << 17)
#define DMUS_SEGF_VALID_START_GRID      (1 << 18)
#define DMUS_SEGF_VALID_START_TICK      (1 << 19)
#define DMUS_SEGF_AUTOTRANSITION        (1 << 20)
#define DMUS_SEGF_AFTERQUEUETIME        (1 << 21)
#define DMUS_SEGF_AFTERLATENCYTIME      (1 << 22)
#define DMUS_SEGF_SEGMENTEND            (1 << 23)
#define DMUS_SEGF_MARKER                (1 << 24)
#define DMUS_SEGF_TIMESIG_ALWAYS        (1 << 25)
#define DMUS_SEGF_USE_AUDIOPATH         (1 << 26)
#define DMUS_SEGF_VALID_START_MEASURE   (1 << 27)
#define DMUS_SEGF_INVALIDATE_PRI        (1 << 28)

#define DMUS_APATH_SHARED_STEREOPLUSREVERB  1
#define DMUS_APATH_DYNAMIC_3D               6
#define DMUS_APATH_DYNAMIC_MONO             7
#define DMUS_APATH_DYNAMIC_STEREO           8

#define DMUS_AUDIOF_3D          0x1
#define DMUS_AUDIOF_ENVIRON     0x2
#define DMUS_AUDIOF_EAX         0x4
#define DMUS_AUDIOF_DMOS        0x8
#define DMUS_AUDIOF_STREAMING   0x10
#define DMUS_AUDIOF_BUFFERS     0x20
#define DMUS_AUDIOF_ALL         0x3F

#define DMUS_PATH_SEGMENT           0x1000
#define DMUS_PATH_SEGMENT_TRACK     0x1100
#define DMUS_PATH_SEGMENT_GRAPH     0x1200
#define DMUS_PATH_SEGMENT_TOOL      0x1300
#define DMUS_PATH_AUDIOPATH         0x2000
#define DMUS_PATH_AUDIOPATH_GRAPH   0x2100
#define DMUS_PATH_AUDIOPATH_TOOL    0x2200
#define DMUS_PATH_PERFORMANCE       0x3000
#define DMUS_PATH_PERFORMANCE_GRAPH 0x3100
#define DMUS_PATH_PERFORMANCE_TOOL  0x3200
#define DMUS_PATH_PORT              0x4000
#define DMUS_PATH_BUFFER            0x6000
#define DMUS_PATH_BUFFER_DMO        0x6100
#define DMUS_PATH_MIXIN_BUFFER      0x7000
#define DMUS_PATH_MIXIN_BUFFER_DMO  0x7100
#define DMUS_PATH_PRIMARY_BUFFER    0x8000

#ifndef FACILITY_DIRECTMUSIC
#define FACILITY_DIRECTMUSIC        0x878
#endif
#define DMUS_ERRBASE                0x1000
#define MAKE_DMHRESULTSUCCESS(code) MAKE_HRESULT(0, FACILITY_DIRECTMUSIC, (DMUS_ERRBASE + (code)))
#define MAKE_DMHRESULTERROR(code)   MAKE_HRESULT(1, FACILITY_DIRECTMUSIC, (DMUS_ERRBASE + (code)))
#define DMUS_E_LOADER_FAILEDOPEN    MAKE_DMHRESULTERROR(0x0182)

interface IDirectMusicTrack;
interface IDirectMusicSegmentState;
interface IDirectMusicGraph;
interface IDirectMusicBand;
interface IDirectMusicObject;
interface IDirectMusicChordMap;
interface IDirectMusicStyle;
interface IDirectMusicSegment;
interface IDirectMusicAudioPath;
interface IDirectMusicPerformance;

#undef  INTERFACE
#define INTERFACE  IDirectMusicLoader
DECLARE_INTERFACE_(IDirectMusicLoader, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetObject)            (THIS_ LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR *ppv) PURE;
    STDMETHOD(SetObject)            (THIS_ LPDMUS_OBJECTDESC pDesc) PURE;
    STDMETHOD(SetSearchDirectory)   (THIS_ REFGUID rguidClass, WCHAR *pwzPath, BOOL fClear) PURE;
    STDMETHOD(ScanDirectory)        (THIS_ REFGUID rguidClass, WCHAR *pwzFileExtension, WCHAR *pwzScanFileName) PURE;
    STDMETHOD(CacheObject)          (THIS_ IDirectMusicObject *pObject) PURE;
    STDMETHOD(ReleaseObject)        (THIS_ IDirectMusicObject *pObject) PURE;
    STDMETHOD(ClearCache)           (THIS_ REFGUID rguidClass) PURE;
    STDMETHOD(EnableCache)          (THIS_ REFGUID rguidClass, BOOL fEnable) PURE;
    STDMETHOD(EnumObject)           (THIS_ REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicLoader8
DECLARE_INTERFACE_(IDirectMusicLoader8, IDirectMusicLoader)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetObject)            (THIS_ LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR *ppv) PURE;
    STDMETHOD(SetObject)            (THIS_ LPDMUS_OBJECTDESC pDesc) PURE;
    STDMETHOD(SetSearchDirectory)   (THIS_ REFGUID rguidClass, WCHAR *pwzPath, BOOL fClear) PURE;
    STDMETHOD(ScanDirectory)        (THIS_ REFGUID rguidClass, WCHAR *pwzFileExtension, WCHAR *pwzScanFileName) PURE;
    STDMETHOD(CacheObject)          (THIS_ IDirectMusicObject *pObject) PURE;
    STDMETHOD(ReleaseObject)        (THIS_ IDirectMusicObject *pObject) PURE;
    STDMETHOD(ClearCache)           (THIS_ REFGUID rguidClass) PURE;
    STDMETHOD(EnableCache)          (THIS_ REFGUID rguidClass, BOOL fEnable) PURE;
    STDMETHOD(EnumObject)           (THIS_ REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc) PURE;
    STDMETHOD_(void,CollectGarbage) (THIS) PURE;
    STDMETHOD(ReleaseObjectByUnknown)(THIS_ IUnknown *pObject) PURE;
    STDMETHOD(LoadObjectFromFile)   (THIS_ REFGUID rguidClassID, REFIID iidInterfaceID, WCHAR *pwzFilePath, void **ppObject) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicSegment
DECLARE_INTERFACE_(IDirectMusicSegment, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetLength)            (THIS_ MUSIC_TIME *pmtLength) PURE;
    STDMETHOD(SetLength)            (THIS_ MUSIC_TIME mtLength) PURE;
    STDMETHOD(GetRepeats)           (THIS_ DWORD *pdwRepeats) PURE;
    STDMETHOD(SetRepeats)           (THIS_ DWORD dwRepeats) PURE;
    STDMETHOD(GetDefaultResolution) (THIS_ DWORD *pdwResolution) PURE;
    STDMETHOD(SetDefaultResolution) (THIS_ DWORD dwResolution) PURE;
    STDMETHOD(GetTrack)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, IDirectMusicTrack **ppTrack) PURE;
    STDMETHOD(GetTrackGroup)        (THIS_ IDirectMusicTrack *pTrack, DWORD *pdwGroupBits) PURE;
    STDMETHOD(InsertTrack)          (THIS_ IDirectMusicTrack *pTrack, DWORD dwGroupBits) PURE;
    STDMETHOD(RemoveTrack)          (THIS_ IDirectMusicTrack *pTrack) PURE;
    STDMETHOD(InitPlay)             (THIS_ IDirectMusicSegmentState **ppSegState, IDirectMusicPerformance *pPerformance, DWORD dwFlags) PURE;
    STDMETHOD(GetGraph)             (THIS_ IDirectMusicGraph **ppGraph) PURE;
    STDMETHOD(SetGraph)             (THIS_ IDirectMusicGraph *pGraph) PURE;
    STDMETHOD(AddNotificationType)  (THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(RemoveNotificationType)(THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(GetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pParam) PURE;
    STDMETHOD(SetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, void *pParam) PURE;
    STDMETHOD(Clone)                (THIS_ MUSIC_TIME mtStart, MUSIC_TIME mtEnd, IDirectMusicSegment **ppSegment) PURE;
    STDMETHOD(SetStartPoint)        (THIS_ MUSIC_TIME mtStart) PURE;
    STDMETHOD(GetStartPoint)        (THIS_ MUSIC_TIME *pmtStart) PURE;
    STDMETHOD(SetLoopPoints)        (THIS_ MUSIC_TIME mtStart, MUSIC_TIME mtEnd) PURE;
    STDMETHOD(GetLoopPoints)        (THIS_ MUSIC_TIME *pmtStart, MUSIC_TIME *pmtEnd) PURE;
    STDMETHOD(SetPChannelsUsed)     (THIS_ DWORD dwNumPChannels, DWORD *paPChannels) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicSegment8
DECLARE_INTERFACE_(IDirectMusicSegment8, IDirectMusicSegment)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetLength)            (THIS_ MUSIC_TIME *pmtLength) PURE;
    STDMETHOD(SetLength)            (THIS_ MUSIC_TIME mtLength) PURE;
    STDMETHOD(GetRepeats)           (THIS_ DWORD *pdwRepeats) PURE;
    STDMETHOD(SetRepeats)           (THIS_ DWORD dwRepeats) PURE;
    STDMETHOD(GetDefaultResolution) (THIS_ DWORD *pdwResolution) PURE;
    STDMETHOD(SetDefaultResolution) (THIS_ DWORD dwResolution) PURE;
    STDMETHOD(GetTrack)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, IDirectMusicTrack **ppTrack) PURE;
    STDMETHOD(GetTrackGroup)        (THIS_ IDirectMusicTrack *pTrack, DWORD *pdwGroupBits) PURE;
    STDMETHOD(InsertTrack)          (THIS_ IDirectMusicTrack *pTrack, DWORD dwGroupBits) PURE;
    STDMETHOD(RemoveTrack)          (THIS_ IDirectMusicTrack *pTrack) PURE;
    STDMETHOD(InitPlay)             (THIS_ IDirectMusicSegmentState **ppSegState, IDirectMusicPerformance *pPerformance, DWORD dwFlags) PURE;
    STDMETHOD(GetGraph)             (THIS_ IDirectMusicGraph **ppGraph) PURE;
    STDMETHOD(SetGraph)             (THIS_ IDirectMusicGraph *pGraph) PURE;
    STDMETHOD(AddNotificationType)  (THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(RemoveNotificationType)(THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(GetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pParam) PURE;
    STDMETHOD(SetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, void *pParam) PURE;
    STDMETHOD(Clone)                (THIS_ MUSIC_TIME mtStart, MUSIC_TIME mtEnd, IDirectMusicSegment **ppSegment) PURE;
    STDMETHOD(SetStartPoint)        (THIS_ MUSIC_TIME mtStart) PURE;
    STDMETHOD(GetStartPoint)        (THIS_ MUSIC_TIME *pmtStart) PURE;
    STDMETHOD(SetLoopPoints)        (THIS_ MUSIC_TIME mtStart, MUSIC_TIME mtEnd) PURE;
    STDMETHOD(GetLoopPoints)        (THIS_ MUSIC_TIME *pmtStart, MUSIC_TIME *pmtEnd) PURE;
    STDMETHOD(SetPChannelsUsed)     (THIS_ DWORD dwNumPChannels, DWORD *paPChannels) PURE;
    STDMETHOD(SetTrackConfig)       (THIS_ REFGUID rguidTrackClassID, DWORD dwGroupBits, DWORD dwIndex, DWORD dwFlagsOn, DWORD dwFlagsOff) PURE;
    STDMETHOD(GetAudioPathConfig)   (THIS_ IUnknown **ppAudioPathConfig) PURE;
    STDMETHOD(Compose)              (THIS_ MUSIC_TIME mtTime, IDirectMusicSegment *pFromSegment, IDirectMusicSegment *pToSegment, IDirectMusicSegment **ppComposedSegment) PURE;
    STDMETHOD(Download)             (THIS_ IUnknown *pAudioPath) PURE;
    STDMETHOD(Unload)               (THIS_ IUnknown *pAudioPath) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicAudioPath
DECLARE_INTERFACE_(IDirectMusicAudioPath, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetObjectInPath)      (THIS_ DWORD dwPChannel, DWORD dwStage, DWORD dwBuffer, REFGUID guidObject, DWORD dwIndex, REFGUID iidInterface, void **ppObject) PURE;
    STDMETHOD(Activate)             (THIS_ BOOL fActivate) PURE;
    STDMETHOD(SetVolume)            (THIS_ long lVolume, DWORD dwDuration) PURE;
    STDMETHOD(ConvertPChannel)      (THIS_ DWORD dwPChannelIn, DWORD *pdwPChannelOut) PURE;
};
typedef IDirectMusicAudioPath IDirectMusicAudioPath8;

#undef  INTERFACE
#define INTERFACE  IDirectMusicPerformance
DECLARE_INTERFACE_(IDirectMusicPerformance, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(Init)                 (THIS_ IDirectMusic **ppDirectMusic, LPDIRECTSOUND pDirectSound, HWND hWnd) PURE;
    STDMETHOD(PlaySegment)          (THIS_ IDirectMusicSegment *pSegment, DWORD dwFlags, __int64 i64StartTime, IDirectMusicSegmentState **ppSegmentState) PURE;
    STDMETHOD(Stop)                 (THIS_ IDirectMusicSegment *pSegment, IDirectMusicSegmentState *pSegmentState, MUSIC_TIME mtTime, DWORD dwFlags) PURE;
    STDMETHOD(GetSegmentState)      (THIS_ IDirectMusicSegmentState **ppSegmentState, MUSIC_TIME mtTime) PURE;
    STDMETHOD(SetPrepareTime)       (THIS_ DWORD dwMilliSeconds) PURE;
    STDMETHOD(GetPrepareTime)       (THIS_ DWORD *pdwMilliSeconds) PURE;
    STDMETHOD(SetBumperLength)      (THIS_ DWORD dwMilliSeconds) PURE;
    STDMETHOD(GetBumperLength)      (THIS_ DWORD *pdwMilliSeconds) PURE;
    STDMETHOD(SendPMsg)             (THIS_ DMUS_PMSG *pPMSG) PURE;
    STDMETHOD(MusicToReferenceTime) (THIS_ MUSIC_TIME mtTime, REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(ReferenceToMusicTime) (THIS_ REFERENCE_TIME rtTime, MUSIC_TIME *pmtTime) PURE;
    STDMETHOD(IsPlaying)            (THIS_ IDirectMusicSegment *pSegment, IDirectMusicSegmentState *pSegState) PURE;
    STDMETHOD(GetTime)              (THIS_ REFERENCE_TIME *prtNow, MUSIC_TIME *pmtNow) PURE;
    STDMETHOD(AllocPMsg)            (THIS_ ULONG cb, DMUS_PMSG **ppPMSG) PURE;
    STDMETHOD(FreePMsg)             (THIS_ DMUS_PMSG *pPMSG) PURE;
    STDMETHOD(GetGraph)             (THIS_ IDirectMusicGraph **ppGraph) PURE;
    STDMETHOD(SetGraph)             (THIS_ IDirectMusicGraph *pGraph) PURE;
    STDMETHOD(SetNotificationHandle)(THIS_ HANDLE hNotification, REFERENCE_TIME rtMinimum) PURE;
    STDMETHOD(GetNotificationPMsg)  (THIS_ DMUS_NOTIFICATION_PMSG **ppNotificationPMsg) PURE;
    STDMETHOD(AddNotificationType)  (THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(RemoveNotificationType)(THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(AddPort)              (THIS_ IDirectMusicPort *pPort) PURE;
    STDMETHOD(RemovePort)           (THIS_ IDirectMusicPort *pPort) PURE;
    STDMETHOD(AssignPChannelBlock)  (THIS_ DWORD dwBlockNum, IDirectMusicPort *pPort, DWORD dwGroup) PURE;
    STDMETHOD(AssignPChannel)       (THIS_ DWORD dwPChannel, IDirectMusicPort *pPort, DWORD dwGroup, DWORD dwMChannel) PURE;
    STDMETHOD(PChannelInfo)         (THIS_ DWORD dwPChannel, IDirectMusicPort **ppPort, DWORD *pdwGroup, DWORD *pdwMChannel) PURE;
    STDMETHOD(DownloadInstrument)   (THIS_ IDirectMusicInstrument *pInst, DWORD dwPChannel, IDirectMusicDownloadedInstrument **ppDownInst, DMUS_NOTERANGE *pNoteRanges, DWORD dwNumNoteRanges, IDirectMusicPort **ppPort, DWORD *pdwGroup, DWORD *pdwMChannel) PURE;
    STDMETHOD(Invalidate)           (THIS_ MUSIC_TIME mtTime, DWORD dwFlags) PURE;
    STDMETHOD(GetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pParam) PURE;
    STDMETHOD(SetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, void *pParam) PURE;
    STDMETHOD(GetGlobalParam)       (THIS_ REFGUID rguidType, void *pParam, DWORD dwSize) PURE;
    STDMETHOD(SetGlobalParam)       (THIS_ REFGUID rguidType, void *pParam, DWORD dwSize) PURE;
    STDMETHOD(GetLatencyTime)       (THIS_ REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(GetQueueTime)         (THIS_ REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(AdjustTime)           (THIS_ REFERENCE_TIME rtAmount) PURE;
    STDMETHOD(CloseDown)            (THIS) PURE;
    STDMETHOD(GetResolvedTime)      (THIS_ REFERENCE_TIME rtTime, REFERENCE_TIME *prtResolved, DWORD dwTimeResolveFlags) PURE;
    STDMETHOD(MIDIToMusic)          (THIS_ BYTE bMIDIValue, DMUS_CHORD_KEY *pChord, BYTE bPlayMode, BYTE bChordLevel, WORD *pwMusicValue) PURE;
    STDMETHOD(MusicToMIDI)          (THIS_ WORD wMusicValue, DMUS_CHORD_KEY *pChord, BYTE bPlayMode, BYTE bChordLevel, BYTE *pbMIDIValue) PURE;
    STDMETHOD(TimeToRhythm)         (THIS_ MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig, WORD *pwMeasure, BYTE *pbBeat, BYTE *pbGrid, short *pnOffset) PURE;
    STDMETHOD(RhythmToTime)         (THIS_ WORD wMeasure, BYTE bBeat, BYTE bGrid, short nOffset, DMUS_TIMESIGNATURE *pTimeSig, MUSIC_TIME *pmtTime) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicPerformance8
DECLARE_INTERFACE_(IDirectMusicPerformance8, IDirectMusicPerformance)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(Init)                 (THIS_ IDirectMusic **ppDirectMusic, LPDIRECTSOUND pDirectSound, HWND hWnd) PURE;
    STDMETHOD(PlaySegment)          (THIS_ IDirectMusicSegment *pSegment, DWORD dwFlags, __int64 i64StartTime, IDirectMusicSegmentState **ppSegmentState) PURE;
    STDMETHOD(Stop)                 (THIS_ IDirectMusicSegment *pSegment, IDirectMusicSegmentState *pSegmentState, MUSIC_TIME mtTime, DWORD dwFlags) PURE;
    STDMETHOD(GetSegmentState)      (THIS_ IDirectMusicSegmentState **ppSegmentState, MUSIC_TIME mtTime) PURE;
    STDMETHOD(SetPrepareTime)       (THIS_ DWORD dwMilliSeconds) PURE;
    STDMETHOD(GetPrepareTime)       (THIS_ DWORD *pdwMilliSeconds) PURE;
    STDMETHOD(SetBumperLength)      (THIS_ DWORD dwMilliSeconds) PURE;
    STDMETHOD(GetBumperLength)      (THIS_ DWORD *pdwMilliSeconds) PURE;
    STDMETHOD(SendPMsg)             (THIS_ DMUS_PMSG *pPMSG) PURE;
    STDMETHOD(MusicToReferenceTime) (THIS_ MUSIC_TIME mtTime, REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(ReferenceToMusicTime) (THIS_ REFERENCE_TIME rtTime, MUSIC_TIME *pmtTime) PURE;
    STDMETHOD(IsPlaying)            (THIS_ IDirectMusicSegment *pSegment, IDirectMusicSegmentState *pSegState) PURE;
    STDMETHOD(GetTime)              (THIS_ REFERENCE_TIME *prtNow, MUSIC_TIME *pmtNow) PURE;
    STDMETHOD(AllocPMsg)            (THIS_ ULONG cb, DMUS_PMSG **ppPMSG) PURE;
    STDMETHOD(FreePMsg)             (THIS_ DMUS_PMSG *pPMSG) PURE;
    STDMETHOD(GetGraph)             (THIS_ IDirectMusicGraph **ppGraph) PURE;
    STDMETHOD(SetGraph)             (THIS_ IDirectMusicGraph *pGraph) PURE;
    STDMETHOD(SetNotificationHandle)(THIS_ HANDLE hNotification, REFERENCE_TIME rtMinimum) PURE;
    STDMETHOD(GetNotificationPMsg)  (THIS_ DMUS_NOTIFICATION_PMSG **ppNotificationPMsg) PURE;
    STDMETHOD(AddNotificationType)  (THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(RemoveNotificationType)(THIS_ REFGUID rguidNotificationType) PURE;
    STDMETHOD(AddPort)              (THIS_ IDirectMusicPort *pPort) PURE;
    STDMETHOD(RemovePort)           (THIS_ IDirectMusicPort *pPort) PURE;
    STDMETHOD(AssignPChannelBlock)  (THIS_ DWORD dwBlockNum, IDirectMusicPort *pPort, DWORD dwGroup) PURE;
    STDMETHOD(AssignPChannel)       (THIS_ DWORD dwPChannel, IDirectMusicPort *pPort, DWORD dwGroup, DWORD dwMChannel) PURE;
    STDMETHOD(PChannelInfo)         (THIS_ DWORD dwPChannel, IDirectMusicPort **ppPort, DWORD *pdwGroup, DWORD *pdwMChannel) PURE;
    STDMETHOD(DownloadInstrument)   (THIS_ IDirectMusicInstrument *pInst, DWORD dwPChannel, IDirectMusicDownloadedInstrument **ppDownInst, DMUS_NOTERANGE *pNoteRanges, DWORD dwNumNoteRanges, IDirectMusicPort **ppPort, DWORD *pdwGroup, DWORD *pdwMChannel) PURE;
    STDMETHOD(Invalidate)           (THIS_ MUSIC_TIME mtTime, DWORD dwFlags) PURE;
    STDMETHOD(GetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pParam) PURE;
    STDMETHOD(SetParam)             (THIS_ REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, void *pParam) PURE;
    STDMETHOD(GetGlobalParam)       (THIS_ REFGUID rguidType, void *pParam, DWORD dwSize) PURE;
    STDMETHOD(SetGlobalParam)       (THIS_ REFGUID rguidType, void *pParam, DWORD dwSize) PURE;
    STDMETHOD(GetLatencyTime)       (THIS_ REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(GetQueueTime)         (THIS_ REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(AdjustTime)           (THIS_ REFERENCE_TIME rtAmount) PURE;
    STDMETHOD(CloseDown)            (THIS) PURE;
    STDMETHOD(GetResolvedTime)      (THIS_ REFERENCE_TIME rtTime, REFERENCE_TIME *prtResolved, DWORD dwTimeResolveFlags) PURE;
    STDMETHOD(MIDIToMusic)          (THIS_ BYTE bMIDIValue, DMUS_CHORD_KEY *pChord, BYTE bPlayMode, BYTE bChordLevel, WORD *pwMusicValue) PURE;
    STDMETHOD(MusicToMIDI)          (THIS_ WORD wMusicValue, DMUS_CHORD_KEY *pChord, BYTE bPlayMode, BYTE bChordLevel, BYTE *pbMIDIValue) PURE;
    STDMETHOD(TimeToRhythm)         (THIS_ MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig, WORD *pwMeasure, BYTE *pbBeat, BYTE *pbGrid, short *pnOffset) PURE;
    STDMETHOD(RhythmToTime)         (THIS_ WORD wMeasure, BYTE bBeat, BYTE bGrid, short nOffset, DMUS_TIMESIGNATURE *pTimeSig, MUSIC_TIME *pmtTime) PURE;
    STDMETHOD(InitAudio)            (THIS_ IDirectMusic **ppDirectMusic, IDirectSound **ppDirectSound, HWND hWnd, DWORD dwDefaultPathType, DWORD dwPChannelCount, DWORD dwFlags, DMUS_AUDIOPARAMS *pParams) PURE;
    STDMETHOD(PlaySegmentEx)        (THIS_ IUnknown *pSource, WCHAR *pwzSegmentName, IUnknown *pTransition, DWORD dwFlags, __int64 i64StartTime, IDirectMusicSegmentState **ppSegmentState, IUnknown *pFrom, IUnknown *pAudioPath) PURE;
    STDMETHOD(StopEx)               (THIS_ IUnknown *pObjectToStop, __int64 i64StopTime, DWORD dwFlags) PURE;
    STDMETHOD(ClonePMsg)            (THIS_ DMUS_PMSG *pSourcePMSG, DMUS_PMSG **ppCopyPMSG) PURE;
    STDMETHOD(CreateAudioPath)      (THIS_ IUnknown *pSourceConfig, BOOL fActivate, IDirectMusicAudioPath **ppNewPath) PURE;
    STDMETHOD(CreateStandardAudioPath)(THIS_ DWORD dwType, DWORD dwPChannelCount, BOOL fActivate, IDirectMusicAudioPath **ppNewPath) PURE;
    STDMETHOD(SetDefaultAudioPath)  (THIS_ IDirectMusicAudioPath *pAudioPath) PURE;
    STDMETHOD(GetDefaultAudioPath)  (THIS_ IDirectMusicAudioPath **ppAudioPath) PURE;
    STDMETHOD(GetParamEx)           (THIS_ REFGUID rguidType, DWORD dwTrackID, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pParam) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicStyle
DECLARE_INTERFACE_(IDirectMusicStyle, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetBand)              (THIS_ WCHAR *pwszName, IDirectMusicBand **ppBand) PURE;
    STDMETHOD(EnumBand)             (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetDefaultBand)       (THIS_ IDirectMusicBand **ppBand) PURE;
    STDMETHOD(EnumMotif)            (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetMotif)             (THIS_ WCHAR *pwszName, IDirectMusicSegment **ppSegment) PURE;
    STDMETHOD(GetDefaultChordMap)   (THIS_ IDirectMusicChordMap **ppChordMap) PURE;
    STDMETHOD(EnumChordMap)         (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetChordMap)          (THIS_ WCHAR *pwszName, IDirectMusicChordMap **ppChordMap) PURE;
    STDMETHOD(GetTimeSignature)     (THIS_ DMUS_TIMESIGNATURE *pTimeSig) PURE;
    STDMETHOD(GetEmbellishmentLength)(THIS_ DWORD dwType, DWORD dwLevel, DWORD *pdwMin, DWORD *pdwMax) PURE;
    STDMETHOD(GetTempo)             (THIS_ double *pTempo) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicStyle8
DECLARE_INTERFACE_(IDirectMusicStyle8, IDirectMusicStyle)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetBand)              (THIS_ WCHAR *pwszName, IDirectMusicBand **ppBand) PURE;
    STDMETHOD(EnumBand)             (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetDefaultBand)       (THIS_ IDirectMusicBand **ppBand) PURE;
    STDMETHOD(EnumMotif)            (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetMotif)             (THIS_ WCHAR *pwszName, IDirectMusicSegment **ppSegment) PURE;
    STDMETHOD(GetDefaultChordMap)   (THIS_ IDirectMusicChordMap **ppChordMap) PURE;
    STDMETHOD(EnumChordMap)         (THIS_ DWORD dwIndex, WCHAR *pwszName) PURE;
    STDMETHOD(GetChordMap)          (THIS_ WCHAR *pwszName, IDirectMusicChordMap **ppChordMap) PURE;
    STDMETHOD(GetTimeSignature)     (THIS_ DMUS_TIMESIGNATURE *pTimeSig) PURE;
    STDMETHOD(GetEmbellishmentLength)(THIS_ DWORD dwType, DWORD dwLevel, DWORD *pdwMin, DWORD *pdwMax) PURE;
    STDMETHOD(GetTempo)             (THIS_ double *pTempo) PURE;
    STDMETHOD(EnumPattern)          (THIS_ DWORD dwIndex, DWORD dwPatternType, WCHAR *pwszName) PURE;
};

#undef  INTERFACE
#define INTERFACE  IDirectMusicChordMap
DECLARE_INTERFACE_(IDirectMusicChordMap, IUnknown)
{
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;
    STDMETHOD(GetScale)             (THIS_ DWORD *pdwScale) PURE;
};
typedef IDirectMusicChordMap IDirectMusicChordMap8;

#undef  INTERFACE

typedef IDirectMusicLoader8      *LPDIRECTMUSICLOADER8;
typedef IDirectMusicPerformance8 *LPDIRECTMUSICPERFORMANCE8;
typedef IDirectMusicSegment8     *LPDIRECTMUSICSEGMENT8;
typedef IDirectMusicAudioPath    *LPDIRECTMUSICAUDIOPATH;

#define SFK_DMUS_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

SFK_DMUS_GUID(CLSID_DirectMusicPerformance,   0xD2AC2881, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(CLSID_DirectMusicSegment,       0xD2AC2882, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(CLSID_DirectMusicStyle,         0xD2AC288A, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(CLSID_DirectMusicChordMap,      0xD2AC288F, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(CLSID_DirectMusicLoader,        0xD2AC2892, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);

SFK_DMUS_GUID(IID_IDirectMusicLoader8,        0x19E7C08C, 0x0A44, 0x4E6A, 0xA1,0x16,0x59,0x5A,0x7C,0xD5,0xDE,0x8C);
SFK_DMUS_GUID(IID_IDirectMusicPerformance8,   0x679C4137, 0xC62E, 0x4147, 0xB2,0xB4,0x9D,0x56,0x9A,0xCB,0x25,0x4C);
SFK_DMUS_GUID(IID_IDirectMusicSegment8,       0xC6784488, 0x41A3, 0x418F, 0xAA,0x15,0xB3,0x50,0x93,0xBA,0x42,0xD4);
SFK_DMUS_GUID(IID_IDirectMusicStyle8,         0xFD24AD8A, 0xA260, 0x453D, 0xBF,0x50,0x6F,0x93,0x84,0xF7,0x09,0x85);
SFK_DMUS_GUID(IID_IDirectMusicChordMap,       0xD2AC28BE, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
#define IID_IDirectMusicChordMap8 IID_IDirectMusicChordMap
SFK_DMUS_GUID(IID_IDirectMusicAudioPath,      0xC87631F5, 0x23BE, 0x4986, 0x88,0x36,0x05,0x83,0x2F,0xCC,0x8F,0x4F);

SFK_DMUS_GUID(GUID_DirectMusicAllTypes,       0xD2AC2893, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(GUID_IDirectMusicStyle,         0xD2AC28A1, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(GUID_StandardMIDIFile,          0x06621075, 0xE92E, 0x11D1, 0xA8,0xC5,0x00,0xC0,0x4F,0xA3,0x72,0x6E);
SFK_DMUS_GUID(GUID_PerfMasterVolume,          0xD2AC28B1, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);
SFK_DMUS_GUID(GUID_PerfMasterTempo,           0xD2AC28B0, 0xB39B, 0x11D1, 0x87,0x04,0x00,0x60,0x08,0x93,0xB1,0xBD);

#endif
