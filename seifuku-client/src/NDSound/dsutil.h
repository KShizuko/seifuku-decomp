//-----------------------------------------------------------------------------
// File: DSUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2000 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef DSUTIL_H
#define DSUTIL_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

class CSound;
class CWaveFile;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       {  try{if(p)   delete (p);     (p)=NULL; } catch(...){ p=NULL;}   }
#endif
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

const int  VOL_MIN			= -5000;
const int  PAN_MAX			= 4000;
const int  PAN_MIN			= 0 - PAN_MAX;

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }

class CSoundManager
{
protected:
    LPDIRECTSOUND8 m_pDS;
public:
    CSoundManager();
    ~CSoundManager();

    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    inline  LPDIRECTSOUND GetDirectSound() { return m_pDS; }
    int		SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT Create( CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateFromMemory( CSound** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
 };

class CSound
{
protected:
    LPDIRECTSOUNDBUFFER  m_apDSBuffer;
    DWORD                m_dwDSBufferSize;
    CWaveFile*           m_pWaveFile;
    DWORD                m_dwNumBuffers;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
    CSound( LPDIRECTSOUNDBUFFER apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile* pWaveFile );
    virtual ~CSound();
  
    HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
  
    HRESULT Play( DWORD dwPriority, DWORD dwFlags );
    HRESULT Stop();
	HRESULT Continue();    
    BOOL    IsSoundPlaying();
	BOOL	RePlay(int nLoop,int nRange=0,int nX=0,int nY=0);

	void	SetVolume(int nVolume);
	void	SetPan(int nPan);
	void	SetSoundPos(int nX,int nY,int nRange){m_nWorldX = nX;m_nWorldY = nY;m_nRange = nRange;}

	int		m_nWorldX;
	int		m_nWorldY;
	int		m_nRange;
	int		m_nPan;
	int		m_n3DSoundActived;
	DWORD	m_dwSleepTime;
	char	m_szFileName[126];
	DWORD	m_dwControl; 
	int		m_nLoop;
	int		m_nVolume;
	void	SetInfo(DWORD dwControl,char * szFileName,int nLoop){m_nLoop = nLoop;m_dwControl = dwControl;strcpy(m_szFileName,szFileName);}
	void	SetSelfVolume(int nVolume){m_nSelfVolume=nVolume;}

private:
	int		m_nSelfVolume;
	
};

class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;
    HMMIO         m_hmmio;
    MMCKINFO      m_ck;
    MMCKINFO      m_ckRiff;
    DWORD         m_dwSize;
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    BYTE*         m_pbData;
    BYTE*         m_pbDataCur;
    ULONG         m_ulDataSize;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

#endif
