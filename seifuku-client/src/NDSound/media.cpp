 

#include <windows.h>
#include <dshow.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <tchar.h>
#include "media.h" 
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

void CMedia::SetVideoWindow(int nTop,int nLeft)
{
    LONG lHeight, lWidth;    
    RECT TarRect;

    if (!pBV)
        return  ;

    pBV->GetVideoSize(&lWidth, &lHeight);

	TarRect.top		= nTop;
	TarRect.left	= nLeft;
	TarRect.right	= TarRect.left  + lWidth;
	TarRect.bottom	= TarRect.top  + lHeight;
   
    pVW->SetWindowPosition(TarRect.left, TarRect.top, TarRect.right, 

TarRect.bottom);

    return  ;
}
 

 


void CMedia::CheckVisibility(void)
{
    long lVisible;
    HRESULT hr;

    m_bAudioOnly = FALSE;

    if ((!pVW) || (!pBV))
    {
        m_bAudioOnly = TRUE;
        return;
    }

    hr = pVW->get_Visible(&lVisible);
    if (FAILED(hr))
    {
        if (hr == E_NOINTERFACE)
        {
            m_bAudioOnly = TRUE;
        }
       
    }
}


void CMedia::Pause(void)
{
    HRESULT hr;
    
    if (!pMC)
        return;

    if((m_psCurrent == Paused) || (m_psCurrent == Stopped))
    {
        hr = pMC->Run();
        m_psCurrent = Running;
    }
    else
    {
        hr = pMC->Pause();
        m_psCurrent = Paused;
    }

    
}


void CMedia::Stop(void)
{
    HRESULT hr;
    
    if ((!pMC) || (!pMS))
        return;

    if((m_psCurrent == Paused) || (m_psCurrent == Running))
    {
        LONGLONG pos = 0;
        hr = pMC->Stop();
        m_psCurrent = Stopped;

        hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
            NULL, AM_SEEKING_NoPositioning);

        hr = pMC->Pause();
    }   
}

BOOL CMedia::PlayMedia(HWND hwnd,char * szFileName,int nloop,WORD wStartSec)
{
    HRESULT hr = 0;
    WCHAR wFile[MAX_PATH];
    int nRenderFailed;
	m_hApp = hwnd;
	m_iLoopTime = nloop;
    lstrcpy(m_szFileName, szFileName);
    m_psCurrent = Stopped;

#ifndef UNICODE
    MultiByteToWideChar(CP_ACP, 0, szFileName, -1, wFile, MAX_PATH);
#else
    lstrcpy(wFile, szFileName);
#endif

    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, IID_IGraphBuilder, (void **)&pGB);

    if(SUCCEEDED(hr))
    {
        hr = pGB->RenderFile(wFile, NULL);
        if (FAILED(hr))
            nRenderFailed = 0;

        hr = pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
        if (FAILED(hr))
            return 0;
        hr = pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS);
        if (FAILED(hr))
            return 0;

        hr = pGB->QueryInterface(IID_IBasicVideo, (void **)&pBV);
        if (FAILED(hr))
            return 0;
        hr = pGB->QueryInterface(IID_IVideoWindow, (void **)&pVW);
        if (FAILED(hr))
            return 0;
        hr = pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
        if (FAILED(hr))
            return 0;

        pGB->QueryInterface(IID_IBasicAudio, (void **)&pBA);

        CheckVisibility();

        if (!m_bAudioOnly)
        {
            pVW->put_Owner((OAHWND)m_hApp);
            pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        }

        UpdateWindow(m_hApp);
        SetForegroundWindow(m_hApp);
        SetFocus(m_hApp);

        pMC->Run();
        m_psCurrent=Running;
    }

    if (!GetDuration())
        return 0;
    if (wStartSec != 0)
        SeekTo(wStartSec);
	return 1;
}
 


void CMedia::Close()
{
    HRESULT hr;

    if(pMC)
        hr = pMC->Stop();

    m_psCurrent = Stopped;
    m_bAudioOnly = TRUE;

    m_szFileName[0] = L'\0';
    m_psCurrent = Init;

    if(pVW)
    {
        hr = pVW->put_Visible(OAFALSE);
        hr = pVW->put_Owner(NULL);
    }

#ifdef REGISTER_FILTERGRAPH
    if (g_dwGraphRegister)
    {
        RemoveGraphFromRot(g_dwGraphRegister);
        g_dwGraphRegister = 0;
    }
#endif

    SAFE_RELEASE(pME);
    SAFE_RELEASE(pMS);
    SAFE_RELEASE(pMC);
    SAFE_RELEASE(pBA);
    SAFE_RELEASE(pBV);
    SAFE_RELEASE(pVW);
    SAFE_RELEASE(pGB);
}

int  CMedia::SetVolume(int nVolume)
{
    HRESULT hr=S_OK;

    if ((!pGB) || (!pBA))
        return 0;

    hr = pBA->get_Volume(&m_lVolume);
    if (hr == E_NOTIMPL)
    {
        return -1;
    }
    else if (FAILED(hr))
    {       
        return -1;
    }    
    int nVol = (nVolume -100) * 30 ;  
	m_lVolume = nVolume;
    pBA->put_Volume(nVol);  

    return hr;
}

int CMedia::GetVolume()
{
	return m_lVolume;
}

 
 

 

BOOL CMedia::IsPlaying(void)
{
    LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    while(SUCCEEDED(pME->GetEvent(&evCode, &evParam1, &evParam2, 0)))
    {
        hr = pME->FreeEventParams(evCode, evParam1, evParam2);

        if(EC_COMPLETE == evCode)
        {
			if(m_iLoopTime !=0)
			{
				LONGLONG pos=0;
				hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning , NULL, AM_SEEKING_NoPositioning);

				if (FAILED(hr))
				{
 
					if (FAILED(hr = pMC->Stop()))
					{                   
						return 0;
					}

					if (FAILED(hr = pMC->Run()))
					{                    
						return 1;
					}
				}
				if(m_iLoopTime != -1)
					m_iLoopTime--;
			}
			else
				return 0;

        }
    }

    return 1;
}
 
BOOL CMedia::GetDuration(void)
{
    GUID    guidOld;
    HRESULT hr;

    if (!pMS)
        return 0;
    if (pMS->IsFormatSupported(&TIME_FORMAT_MEDIA_TIME) != S_OK)
        return 0;
    hr = pMS->GetTimeFormat(&guidOld);
    if (FAILED(hr))
        return 0;
    hr = pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
    if (FAILED(hr))
        return 0;
    hr = pMS->GetDuration(&m_llDuration);
    if (FAILED(hr))
        return 0;
    if (guidOld != TIME_FORMAT_MEDIA_TIME)
    {
        hr = pMS->SetTimeFormat(&guidOld);
        if (FAILED(hr))
            return 0;
    }
    return 1;
}

void CMedia::SeekTo(WORD wSec)
{
    LONGLONG llPos;

    llPos = (int)(wSec * 1000 * 10000);
    llPos = llPos % (m_llDuration + 1);
    pMS->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
}

void CMedia::GetPosition(WORD* pwSec)
{
    LONGLONG llPos;
    HRESULT  hr;

    pMS->GetCurrentPosition(&llPos);
    if (llPos == 0)
    {
        hr = pMS->GetCurrentPosition(&llPos);
        if (FAILED(hr))
            *pwSec = 0;
        return;
    }
    *pwSec = (WORD)((int)(WORD)llPos / 10000 / 1000);
}

