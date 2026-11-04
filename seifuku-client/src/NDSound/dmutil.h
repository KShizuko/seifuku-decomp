//-----------------------------------------------------------------------------
// File: DMUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef DMUTIL_H
#define DMUTIL_H

#include <dmusicc.h>
#include <dmusici.h>
#include <dsound.h>

class CMusicManager;
class CMusicSegment;
class CMusicScript;

class CMusicManager
{
protected:
    BOOL                      m_bCleanupCOM;
    IDirectMusicLoader8*      m_pLoader;
    IDirectMusicPerformance8* m_pPerformance;

public:
    CMusicManager();
    ~CMusicManager();

    inline IDirectMusicLoader8*      GetLoader()      { return m_pLoader; }
    inline IDirectMusicPerformance8* GetPerformance() { return m_pPerformance; }
    IDirectMusicAudioPath8* GetDefaultAudioPath();

    HRESULT Initialize( HWND hWnd, DWORD dwPChannels = 128, DWORD dwDefaultPathType = DMUS_APATH_DYNAMIC_STEREO );

    HRESULT SetSearchDirectory( const TCHAR* strMediaPath );
    VOID    CollectGarbage();
    BOOL    MusicToReferenceTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime );
    BOOL    ReferenceToMusicTime( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime );

    HRESULT CreateSegmentFromFile( CMusicSegment** ppSegment, TCHAR* strFileName, 
		                           BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE );

    HRESULT CreateChordMapFromFile( IDirectMusicChordMap8** ppChordMap, TCHAR* strFileName );
    HRESULT CreateStyleFromFile( IDirectMusicStyle8** ppStyle, TCHAR* strFileName );
    HRESULT GetMotifFromStyle( IDirectMusicSegment8** ppMotif, TCHAR* strStyle, TCHAR* wstrMotif );

    HRESULT CreateSegmentFromResource( CMusicSegment** ppSegment, TCHAR* strResource, TCHAR* strResourceType, 
		                           BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE );
};

class CMusicSegment
{
protected:
    IDirectMusicSegment8*     m_pSegment;
    IDirectMusicLoader8*      m_pLoader;
    IDirectMusicPerformance8* m_pPerformance;
    IDirectMusicAudioPath8*   m_pEmbeddedAudioPath;
    BOOL                      m_bDownloaded;

public:
    CMusicSegment( IDirectMusicPerformance8* pPerformance, 
                   IDirectMusicLoader8* pLoader,
                   IDirectMusicSegment8* pSegment );
    virtual ~CMusicSegment();

    inline  IDirectMusicSegment8* GetSegment() { return m_pSegment; }
    HRESULT GetStyle( IDirectMusicStyle8** ppStyle, DWORD dwStyleIndex = 0 );

    HRESULT SetRepeats( DWORD dwRepeats );
    HRESULT GetLength( MUSIC_TIME* pmtLength )
    {
        if( m_pSegment == NULL )
            return CO_E_NOTINITIALIZED;
        return m_pSegment->GetLength( pmtLength );
    }
    HRESULT SetStartPoint( MUSIC_TIME mtStart )
    {
        if( m_pSegment == NULL )
            return CO_E_NOTINITIALIZED;
        return m_pSegment->SetStartPoint( mtStart );
    }
    HRESULT Play( DWORD dwFlags = DMUS_SEGF_SECONDARY, IDirectMusicAudioPath8* pAudioPath = NULL );
    HRESULT Stop( DWORD dwFlags = 0 );
    HRESULT Download( IDirectMusicAudioPath8* pAudioPath = NULL );
    HRESULT Unload( IDirectMusicAudioPath8* pAudioPath = NULL );

    BOOL    IsPlaying();
};

#endif
