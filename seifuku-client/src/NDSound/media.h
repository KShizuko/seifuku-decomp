
#include <dshow.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <tchar.h>
 
 

enum PLAYSTATE {Stopped, Paused, Running, Init};
 


class CMedia{
private:
	
	HWND      m_hApp;
	HINSTANCE m_hInst;
	BOOL      m_bAudioOnly;
	LONG      m_lVolume;
	DWORD     m_dwGraphRegister;
	PLAYSTATE m_psCurrent;
	int		  m_iLoopTime;
	LONGLONG  m_llDuration;

	IGraphBuilder *pGB ;
	IMediaControl *pMC ;
	IMediaEventEx *pME ;
	IVideoWindow  *pVW ;
	IBasicAudio   *pBA ;
	IBasicVideo   *pBV ;
	IMediaSeeking *pMS ;


	void CheckVisibility(void);

public: 
	CMedia() {	m_bAudioOnly=FALSE;
				m_lVolume=80;
				m_dwGraphRegister=0;
				m_psCurrent=Stopped;
				pMS=NULL;
				m_llDuration=0;
		}

	~CMedia(){Close();}
	BOOL	PlayMedia(HWND hwnd ,char * szFileName,int nloop,WORD wStartSec);
	void	Close(void);
	void	Pause(void);
	void	Stop(void); 
	int		GetVolume();
	int		SetVolume(int nVolume);
 
	void	SetVideoWindow(int nTop,int nLeft);
	BOOL	IsPlaying(void);
	BOOL	GetDuration(void);
	void	SeekTo(WORD wSec);
	void	GetPosition(WORD* pwSec);
	TCHAR     m_szFileName[MAX_PATH];
  



};
