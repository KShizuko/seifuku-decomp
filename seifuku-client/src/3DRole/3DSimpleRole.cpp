#include "3DSimpleRole.h"
#include "GameDataSet.h"
#include "3DGameMap.h"
#include "Role.h"

C3DSimpleRole::C3DSimpleRole()
{
	m_nIndex			= -1;
	m_nLook				= -1;
	m_dwBegin			= ::TimeGet();
	m_idSimpleObj		= ID_NONE;
	m_idMotionStandBy	= ID_NONE;
	m_idMotionBlaze		= ID_NONE;
	m_bBlaze			= false;
	m_obj3DSimpleObj	= NULL;
	m_pSound			= NULL;
	m_objEffectB		= ::Game3DEffectCreate(NULL);
	m_objEffectF		= ::Game3DEffectCreate(NULL);
	m_obj3DRole			= ::RoleViewCreate(_ROLE_VIEW_ROLE);
}

C3DSimpleRole::~C3DSimpleRole()
{
	if(m_obj3DSimpleObj)
	{
		m_obj3DSimpleObj->Release();
		m_obj3DSimpleObj = NULL;
	}
	if(m_obj3DRole)
	{
		m_obj3DRole->Release();
		m_obj3DRole = NULL;
	}
	if(m_objEffectF)
	{
		m_objEffectF->Release();
		m_objEffectF = NULL;
	}
	if(m_objEffectB)
	{
		m_objEffectB->Release();
		m_objEffectB = NULL;
	}
}
void C3DSimpleRole::Blaze()
{
	m_dwBegin			= ::TimeGet();
	m_bBlaze			= true;

	const char szIniFile[]="ini/3DSimpleRole.ini";
	char szTitle[64], szSubtitle[64];
	sprintf(szTitle, "Role%d", m_nIndex);

	char szSoundFile[_MAX_PATH];
	strcpy(szSubtitle, "BlazeSound");
	if(m_pSound)
	{
		DXClose2DSound(m_pSound);
		m_pSound = NULL;
	}
	if(::IniStrGet(szIniFile, szTitle, szSubtitle, szSoundFile))
	{
		m_pSound = (CSound*)::DXPlaySound(szSoundFile);
	}
}
void C3DSimpleRole::Show(int nIndex, CMyPos posScr, int nZoomPercent, int nRotateX, int nStatus)
{
	if(nIndex < 0)
		return;

	if(nIndex != m_nIndex)
	{
		m_bBlaze = true;
		m_dwBegin			= ::TimeGet();
		m_nIndex			= nIndex;

		const char szIniFile[]="ini/3DSimpleRole.ini";
		char szTitle[64], szSubtitle[64];
		sprintf(szTitle, "Role%d", m_nIndex);

		strcpy(szSubtitle, "3DSimpleObjID");
		int nData = 0;
		MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, nData));
		m_idSimpleObj = nData;

		strcpy(szSubtitle, "3DStandByMotion");
		MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, nData));
		m_idMotionStandBy = nData;

		strcpy(szSubtitle, "3DBlazeMotion");
		MYASSERT(::IniDataGet(szIniFile, szTitle, szSubtitle, nData));
		m_idMotionBlaze = nData;

		char szSoundFile[_MAX_PATH] = "";
		strcpy(szSubtitle, "BlazeSound");
		if(m_pSound)
		{
			DXClose2DSound(m_pSound);
			m_pSound = NULL;
		}
		if(::IniStrGet(szIniFile, szTitle, szSubtitle, szSoundFile))
		{
			m_pSound = (CSound*)::DXPlaySound(szSoundFile);
		}

		m_objEffectF->Clear();
		char szIndex[_MAX_STRING];
		strcpy(szSubtitle, "F3DEffect");
		if(::IniStrGet(szIniFile, szTitle, szSubtitle, szIndex))
			m_objEffectF->Add(szIndex);

		m_objEffectB->Clear();
		strcpy(szSubtitle, "B3DEffect");
		if(::IniStrGet(szIniFile, szTitle, szSubtitle, szIndex))
			m_objEffectB->Add(szIndex);

		m_obj3DSimpleObj = ::Simple3DObjCreate(m_idSimpleObj);
		if(m_obj3DSimpleObj)
		{
			m_obj3DSimpleObj->SetOblique(false);
			m_obj3DSimpleObj->SetShadowType(CRole::SHADOW_NONE);
		}
	}
	if(!m_obj3DSimpleObj)
		return;

	CMySize sizeCamera = {CMyBitmap::GetScreenWidth(), CMyBitmap::GetScreenHeight()};
	CMyBitmap::GameCameraBuild(sizeCamera);
	CMyBitmap::GameCameraSet(CMyBitmap::GetScreenWidth()/2, CMyBitmap::GetScreenHeight()/2, -1000.0f, 3);

	const DWORD dwFrameInterval = 33;
	if(m_bBlaze)
	{
		I3DMotion* p3DMotion = GameDataSetQuery()->Get3DMotion("body", m_idMotionBlaze);
		m_obj3DSimpleObj->SetMotion(p3DMotion);
		int nFrameAmount = p3DMotion->GetFrameAmount();
		int nCurrentFrame = (::TimeGet() - m_dwBegin)/dwFrameInterval;
		if(nCurrentFrame >= nFrameAmount)
		{
			nCurrentFrame = nFrameAmount-1;
			m_bBlaze = false;
			m_dwBegin = ::TimeGet();
		}
		m_obj3DSimpleObj->SetFrame(nCurrentFrame);
	}
	else
	{
		I3DMotion* p3DMotion = GameDataSetQuery()->Get3DMotion("body", m_idMotionStandBy);
		m_obj3DSimpleObj->SetMotion(p3DMotion);
		int nFrameAmount = p3DMotion->GetFrameAmount();
		int nCurrentFrame = (::TimeGet() - m_dwBegin)/dwFrameInterval;
		m_obj3DSimpleObj->SetFrame(nCurrentFrame);
	}

	m_objEffectB->Process();
	m_objEffectB->Show(posScr);

	m_obj3DSimpleObj->SetPos(posScr.x, posScr.y, 0, nRotateX, nZoomPercent/100.0f);
	m_obj3DSimpleObj->Draw2BG(0, 0);

	m_objEffectF->Process();
	posScr.y += 10;
	m_objEffectF->Show(posScr);
}
void C3DSimpleRole::ShowRole(int nIndex, CMyPos posScr, int nZoomPercent, int nRotateX, int nStatus)
{
	if(nIndex < 0)
		return;

	if(!m_obj3DRole)
		return;

	if(m_nIndex != nIndex)
	{
		m_nIndex = nIndex;
		m_bBlaze = true;

		const char szIniFile[]="ini/3DSimpleRole.ini";
		char szTitle[64], szSubtitle[64];
		sprintf(szTitle, "Role%d", m_nIndex);

		strcpy(szSubtitle, "Look");
		int nData = 0;
		if(::IniDataGet(szIniFile, szTitle, szSubtitle, nData, true))
			m_obj3DRole->SetVariable(_VAR_LOOK, nData);

		strcpy(szSubtitle, "Hair");
		if(::IniDataGet(szIniFile, szTitle, szSubtitle, nData, true))
			m_obj3DRole->SetVariable(_VAR_ARMET, nData);

		strcpy(szSubtitle, "Armor");
		if(::IniDataGet(szIniFile, szTitle, szSubtitle, nData, true))
			m_obj3DRole->SetVariable(_VAR_ARMOR, nData);
		m_dwFrameAmount = m_obj3DRole->SetAction(_ACTION_STANDBY);

		m_objEffectF->Clear();
		char szIndex[_MAX_STRING];
		strcpy(szSubtitle, "F3DEffect");
		if(::IniStrGet(szIniFile, szTitle, szSubtitle, szIndex))
			m_objEffectF->Add(szIndex);

		m_objEffectB->Clear();
		strcpy(szSubtitle, "B3DEffect");
		if(::IniStrGet(szIniFile, szTitle, szSubtitle, szIndex))
			m_objEffectB->Add(szIndex);

		m_bPose = false;
		m_bResetPose = true;
	}

	CMySize sizeCamera = {CMyBitmap::GetScreenWidth(), CMyBitmap::GetScreenHeight()};
	CMyBitmap::GameCameraBuild(sizeCamera);
	CMyBitmap::GameCameraSet(CMyBitmap::GetScreenWidth()/2, CMyBitmap::GetScreenHeight()/2, -1000.0f, 3);

	const DWORD dwFrameInterval = 33;
	DWORD dwFrame = (::TimeGet() - m_dwBegin)/dwFrameInterval;
	if(dwFrame > m_dwFrameAmount)
	{
		m_dwBegin = ::TimeGet();
		dwFrame = 0;
		m_dwFrameAmount = m_obj3DRole->SetAction(_ACTION_STANDBY);
		m_bPose = false;
	}
	m_obj3DRole->SetVariable(_VAR_FRAME, dwFrame);
	m_obj3DRole->SetPos((float)posScr.x, (float)posScr.y, 0.0f, nRotateX, nZoomPercent*1.0/100.0);
	m_obj3DRole->SetLightOffset(CRole::SHADOW_NONE, 300, -300, -1000, 0.8f, 0.1f, 0.1f, 0.1f);

	if(nStatus == 0)
	{
		if(!m_bPose)
			m_bResetPose = true;
	}
	else
	{
		if(!m_bPose && m_bResetPose)
		{
			m_bResetPose = false;
			m_dwBegin = ::TimeGet();
			m_bPose = true;
			m_dwFrameAmount = m_obj3DRole->SetAction(_ACTION_LIE);
		}
		m_objEffectB->Process();
		m_objEffectB->Show(posScr);
	}

	m_obj3DRole->Draw(0, 0);
	posScr.y += 10;
	if(nStatus != 0)
	{
		m_objEffectF->Process();
		m_objEffectF->Show(posScr);
	}
}
