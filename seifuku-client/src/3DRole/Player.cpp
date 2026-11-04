#include "BaseFunc.h"
#include "Player.h"
#include "Hero.h"
#include "gamedataset.h"
#include "GamePlayerSet.h"
#include "../NDSound/NDsound.h"
#include "3DGameMap.h"
#include "3DRoleData.h"
#include "Pet.h"
#include "GameDataSetProvisional.h"

BOOL	CPlayer::s_bShowName	=false;
HANDLE	CPlayer::s_hHeap		=NULL;
UINT	CPlayer::s_uNumAllocsInHeap	=0;

void * CPlayer::operator new(size_t size)
{
	if (!s_hHeap)
	{
		s_hHeap	=::HeapCreate(0, 0, 0);
		if (!s_hHeap)
			return NULL;
	}
	void* p	=::HeapAlloc(s_hHeap, HEAP_ZERO_MEMORY, size);
	if (p)
	{
		s_uNumAllocsInHeap++;
	}
	return p;
}

void CPlayer::operator delete(void* p)
{
	if (!s_hHeap || s_uNumAllocsInHeap <= 0)
		return;

	if (::HeapFree(s_hHeap, 0, p))
	{
		s_uNumAllocsInHeap--;
	}

	if (s_uNumAllocsInHeap <= 0)
	{
		if (::HeapDestroy(s_hHeap))
			s_hHeap	=NULL;
	}
}

BOOL CPlayer::IsValidPt(void* p)
{
	if (!s_hHeap || !p)
		return false;

	if ((DWORD)p < 0x00010000 || (DWORD)p >= 0x7FFEFFFF)
		return false;

	if (::IsBadCodePtr((FARPROC)p))
		return false;

	BOOL bOk = ::HeapValidate(s_hHeap, 0, p);
	if(bOk == false)
		int kkk = 1;
	return bOk;
}

CPlayer::CPlayer()
{
	memset(&m_infoPlayer, 0L, sizeof(m_infoPlayer));
	m_nType = MAP_PLAYER;
	const DWORD MaxPhysicalForce = 100;
	m_infoPlayer.dwMaxPhysicalForce = MaxPhysicalForce;
	strcpy(m_szCryOut, "");
	m_infoPlayer.dwKoAmount = 0;
	m_dwKoScale = 0;
	m_dwLastDamage = 0;
	m_pFD = CFDword::CreateNew(::GetDate(), 16);
}
CPlayer::~CPlayer()
{
	SAFE_DELETE(m_pFD);
}

void CPlayer::Show(void* pInfo)
{
	if (!this->IsVisible())
		return;
	if(this->TestStatus(0x00400000))
		this->SetARGB(0x66000000 | (this->GetARGB() & 0x00ffffff));
	CRole::Show();
	if (this->IsMouseFocus())
	{
		this->ShowInfo();
	}
	else
	{
		if (CPlayer::s_bShowName || g_objGameMap.GetShowMapObjInfoFlag())
		{
			if((this->GetID() >= MONSTERID_FIRST  && this->GetID() <= MONSTERID_LAST)
				&& (this->TestStatus(0x00000800) || this->TestStatus(USERSTATUS_DIE)))
			{
			}
			else
			{
				this->ShowName();
				this->ShowSyndicate();
				this->ShowKoAmount(false);
			}
		}
	}
	CMyPos posNum;
	posNum.x = m_Info.posScr.x;
	posNum.y = m_Info.posScr.y-100;
	m_objZoomNum.Show(posNum);
}

void CPlayer::ShowLife()
{
	CMySize sizeFont;
	sizeFont.iHeight	=CMyBitmap::GetFontSize();
	sizeFont.iWidth		=sizeFont.iHeight/2;
	if(this->GetRoleType() == _ROLE_SYNFLAG_NPC &&
		this->GetData(CPlayer::_PLAYER_LIFE)>0 && this->GetMaxLife()>0)
	{
		DWORD dwBloodColor;
		DWORD dwLife = this->GetData(CPlayer::_PLAYER_LIFE);
		DWORD dwMaxLife = this->GetMaxLife();
		if(dwLife > 20000000)
			dwBloodColor = _COLOR_GREEN;
		else if(dwLife > 10000000)
			dwBloodColor = _COLOR_YELLOW;
		else
			dwBloodColor = _COLOR_RED;
		int nBarAmount = dwMaxLife/2000000;
		if(nBarAmount * 2000000 < dwMaxLife)
			nBarAmount ++;
		if(nBarAmount > 5)
			nBarAmount = 5;
		int nBarWidth = 50, nBarHeight = 4;
		int nNameShowOffset = this->GetZoomPercent()*OFFSET_NAMESHOW/100;
		int nShowX	=m_Info.posScr.x-nBarWidth/2;
		int nShowY	=m_Info.posScr.y-nNameShowOffset-50;
		dwLife = dwLife%(10000000+1);
		for(int i = 0; i < nBarAmount; i ++)
		{
			CMyPos posBegin, posEnd;
			posBegin.x = nShowX;
			posBegin.y = nShowY-nBarHeight*i*2;
			posEnd.x   = nShowX+nBarWidth;
			posEnd.y   = nShowY+nBarHeight-nBarHeight*i*2;
			g_objGameMap.MapScaleShowPos(posBegin.x, posBegin.y);
			g_objGameMap.MapScaleShowPos(posEnd.x, posEnd.y);
			CMyBitmap::ShowBlock(posBegin.x, posBegin.y, posEnd.x, posEnd.y, 0x55000000+_COLOR_GRAY);
			DWORD dwCurrentMaxLife = 2000000;
			DWORD dwCurrentLife;
			if(dwLife > dwCurrentMaxLife*i)
				dwCurrentLife = dwLife - dwCurrentMaxLife*i;
			else
				dwCurrentLife = 0;
			if(dwCurrentLife > dwCurrentMaxLife)
				dwCurrentLife = dwCurrentMaxLife;
			int nLifeBarLen;
			if(dwCurrentMaxLife == 0)
				nLifeBarLen = 0;
			else
				nLifeBarLen = nBarWidth*dwCurrentLife/dwCurrentMaxLife;
			posBegin.x = nShowX;
			posBegin.y = nShowY-nBarHeight*i*2;
			posEnd.x   = nShowX+nLifeBarLen;
			posEnd.y   = nShowY+nBarHeight-nBarHeight*i*2;
			g_objGameMap.MapScaleShowPos(posBegin.x, posBegin.y);
			g_objGameMap.MapScaleShowPos(posEnd.x, posEnd.y);
			CMyBitmap::ShowBlock(posBegin.x, posBegin.y, posEnd.x, posEnd.y, 0xff000000+dwBloodColor);
		}
	}
	else if (this->IsNpc() &&
			this->GetData(CPlayer::_PLAYER_LIFE)>0 && this->GetMaxLife()>0)
	{
		int nBarWidth = 50, nBarHeight = 4;
		int nNameShowOffset = this->GetZoomPercent()*OFFSET_NAMESHOW/100;
		int nShowX	=m_Info.posScr.x-nBarWidth/2;
		int nShowY	=m_Info.posScr.y-nNameShowOffset+1;
		CMyPos posBegin, posEnd;
		posBegin.x = nShowX;
		posBegin.y = nShowY;
		posEnd.x   = nShowX+nBarWidth;
		posEnd.y   = nShowY+nBarHeight;
		g_objGameMap.MapScaleShowPos(posBegin.x, posBegin.y);
		g_objGameMap.MapScaleShowPos(posEnd.x, posEnd.y);
		CMyBitmap::ShowBlock(posBegin.x, posBegin.y, posEnd.x, posEnd.y, 0x55000000+_COLOR_GRAY);
		int nLifeBarLen = nBarWidth*this->GetData(CPlayer::_PLAYER_LIFE)/this->GetMaxLife();
		posBegin.x = nShowX;
		posBegin.y = nShowY;
		posEnd.x   = nShowX+nLifeBarLen;
		posEnd.y   = nShowY+nBarHeight;
		g_objGameMap.MapScaleShowPos(posBegin.x, posBegin.y);
		g_objGameMap.MapScaleShowPos(posEnd.x, posEnd.y);
		CMyBitmap::ShowBlock(posBegin.x, posBegin.y, posEnd.x, posEnd.y, 0xffB80303);
	}
}

void CPlayer::ShowName(void)
{
	this->ShowLife();
	CMySize sizeFont;
	sizeFont.iHeight	=CMyBitmap::GetFontSize();
	sizeFont.iWidth		=sizeFont.iHeight/2;
	const char* pszString	=this->GetName();
#ifdef _DEBUG
	char szID[256] = "";
	if (this->IsNpc())
	{
		sprintf(szID, "%s[%u]", pszString, this->GetID());
		pszString = szID;
	}
	if (this->IsMyPet())
	{
		CPet* pPet = (CPet*)this;
		CMyPos posC;
		pPet->GetPos(posC);
		pszString = szID;
	}
#endif
	int iInfoLen	=strlen(pszString);
	if (iInfoLen <= 0)
		return;
	static DWORD dwNameFlashTimer = ::TimeGet();
	DWORD dwColor = _COLOR_WHITE;
	if (this->IsNpc())
	{
		DWORD dwHeroLev = g_objHero.GetLev();
		if(this->GetLev() >= dwHeroLev+5)
			dwColor = _COLOR_BLACK;
		else if(this->GetLev() > dwHeroLev)
			dwColor = _COLOR_RED;
		else if(this->GetLev()+5 > dwHeroLev)
			dwColor = _COLOR_WHITE;
		else
			dwColor = _COLOR_GREEN;
	}
	else
	{
		if(this->TestStatus(USERSTATUS_DARKRED))
			dwColor = _COLOR_BLACK;
		else if (this->TestStatus(USERSTATUS_RED))
			dwColor = _COLOR_RED;
		if(this->TestStatus(0x00000001))
		{
			int nFrame = (::TimeGet() - dwNameFlashTimer)/200;
			if(nFrame > nFrame/2*2)
				dwColor = _COLOR_BLUE;
		}
	}
	int nNameShowOffset = this->GetZoomPercent()*OFFSET_NAMESHOW/100;
	if (this->GetActionType() == _ACTION_JUMP)
		nNameShowOffset = this->GetZoomPercent()*(OFFSET_NAMESHOW+40)/100;
	int nShowX	=m_Info.posScr.x-sizeFont.iWidth*iInfoLen/2;
	int nShowY	=m_Info.posScr.y- nNameShowOffset - sizeFont.iHeight;
	if(this->GetRoleType() == _ROLE_BOOTH_NPC)
	{
		char szTemp[64];
		const OBJID idStr = 100124;
		sprintf(szTemp, "%s%s", pszString, GameDataSetQuery()->GetStr(idStr));
		g_objGameMap.MapScaleShowPos(nShowX, nShowY);
		{
			DWORD dwBkColor = 0;
			if (((dwColor&0x00ff0000)>>16) < 0x20
					&& ((dwColor&0x00ff00)>>8) < 0x20
					&& (dwColor&0x00ff) < 0x20)
				dwBkColor = 0xaaaaaa;
			CMyPos posShow;
			posShow.x = nShowX+1;
			posShow.y = nShowY+1-this->GetOffsetY();
			g_objGameMap.m_InfoViewManager.AddStringView(szTemp, posShow, dwBkColor);
			posShow.x = nShowX;
			posShow.y = nShowY-this->GetOffsetY();
			g_objGameMap.m_InfoViewManager.AddStringView(szTemp, posShow, dwColor);
		}
	}
	else
	{
		g_objGameMap.MapScaleShowPos(nShowX, nShowY);
		{
			DWORD dwBkColor = 0;
			if (((dwColor&0x00ff0000)>>16) < 0x20
					&& ((dwColor&0x00ff00)>>8) < 0x20
					&& (dwColor&0x00ff) < 0x20)
				dwBkColor = 0xaaaaaa;
			CMyPos posShow;
			posShow.x = nShowX+1;
			posShow.y = nShowY+1-this->GetOffsetY();
			g_objGameMap.m_InfoViewManager.AddStringView((char*)pszString, posShow, dwBkColor);
			posShow.x = nShowX;
			posShow.y = nShowY-this->GetOffsetY();
			g_objGameMap.m_InfoViewManager.AddStringView((char*)pszString, posShow, dwColor);
		}
	}
}

void CPlayer::ShowSyndicate (void)
{
	if(this->GetSydicateID() == 0)
		return;
	CMySize sizeFont;
	sizeFont.iHeight	=CMyBitmap::GetFontSize();
	sizeFont.iWidth		=sizeFont.iHeight/2;
	int iInfoLen	=strlen(this->GetSyndicate());
	if (iInfoLen <= 0)
		return;
	char szSyndicate[256] = "";
	sprintf(szSyndicate, "%s", this->GetSyndicate());
	char szSyndicateInfo[256] = "";
	switch (this->GetSyndicateRank())
	{
	case RANK_ASSISTANT:
	case RANK_LEADER:
		sprintf(szSyndicateInfo, "%s %s", this->GetSyndicate(), this->GetSyndicateTitle());
		break;
	case RANK_TEAMLEADER:
	case RANK_SUBASSISTANT:
	case RANK_SUBLEADER:
		sprintf(szSyndicateInfo, "%s %s %s", this->GetSyndicate(), this->GetSubSyndicate(), this->GetSyndicateTitle());
		break;
	case RANK_SAN:
		if(strlen(this->GetSubSyndicate()) == 0)
			strcpy(szSyndicateInfo, this->GetSyndicate());
		else
			sprintf(szSyndicateInfo, "%s %s", this->GetSyndicate(), this->GetSubSyndicate());
		break;
	}
	iInfoLen = strlen(szSyndicateInfo);
	DWORD dwColor = _COLOR_WHITE;
	DWORD dwSyndicateColor;
	switch(g_objPlayerSet.GetSyndicateStatus(this->GetSydicateID()) )
	{
	case SYNDICATE_ALLY:
		dwSyndicateColor = _COLOR_GREEN;
		break;
	case SYNDICATE_ANTAGONIZE:
		dwSyndicateColor = _COLOR_RED;
		break;
	case SYNDICATE_SELF:
		dwSyndicateColor = _COLOR_YELLOW;
		break;
	case _COLOR_WHITE:
		dwSyndicateColor = SYNDICATE_NORMAL;
		break;
	default:
		dwSyndicateColor = _COLOR_WHITE;
		break;
	}
	int nNameShowOffset = this->GetZoomPercent()*(OFFSET_NAMESHOW+20)/100;
	if (this->GetActionType() == _ACTION_JUMP)
		nNameShowOffset = this->GetZoomPercent()*(OFFSET_NAMESHOW+60)/100;
	int nShowX	=m_Info.posScr.x-sizeFont.iWidth*iInfoLen/2;
	int nShowY	=m_Info.posScr.y-nNameShowOffset - sizeFont.iHeight;
	g_objGameMap.MapScaleShowPos(nShowX, nShowY);
	CMyPos posShow;
	posShow.x = nShowX+1;
	posShow.y = nShowY+1-this->GetOffsetY();
	g_objGameMap.m_InfoViewManager.AddStringView(szSyndicate, posShow, 0x0);
	posShow.x = nShowX;
	posShow.y = nShowY-this->GetOffsetY();
	g_objGameMap.m_InfoViewManager.AddStringView(szSyndicate, posShow, dwSyndicateColor);
	if(strlen(this->GetSubSyndicate()) == 0 && this->GetSyndicateRank() != RANK_SAN)
	{
		nShowX += (strlen(szSyndicate)) * sizeFont.iWidth;
		posShow.x = nShowX+1;
		posShow.y = nShowY+1-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSyndicateTitle(), posShow, 0x0);
		posShow.x = nShowX;
		posShow.y = nShowY-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSyndicateTitle(), posShow, 0x6699ff);
	}
	if(strlen(this->GetSubSyndicate()) != 0)
	{
		posShow.x = nShowX + strlen(szSyndicate) * sizeFont.iWidth;
		posShow.y = nShowY+1-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddAniView("ani/common.ani", "Syndicate", posShow, 0);
		nShowX += (2+strlen(szSyndicate)) * sizeFont.iWidth;
		posShow.x = nShowX+1;
		posShow.y = nShowY+1-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSubSyndicate(), posShow, 0x0);
		posShow.x = nShowX;
		posShow.y = nShowY-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSubSyndicate(), posShow, dwSyndicateColor);
	}
	if(strlen(this->GetSubSyndicate()) != 0 && this->GetSyndicateRank() != RANK_SAN)
	{
		nShowX += (strlen(this->GetSubSyndicate())) * sizeFont.iWidth;
		posShow.x = nShowX+1;
		posShow.y = nShowY+1-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSyndicateTitle(), posShow, 0x0);
		posShow.x = nShowX;
		posShow.y = nShowY-this->GetOffsetY();
		g_objGameMap.m_InfoViewManager.AddStringView(this->GetSyndicateTitle(), posShow, 0x6699ff);
	}
}

void CPlayer::ShowInfo(void)
{
	if (this->IsNpc())
	{
		if (this->IsDead())
			return;
		this->ShowName();
		if(this->GetRoleType() == _ROLE_STATUARY_NPC)
			this->ShowSyndicate();
		return;
	}
	if(this->GetRoleType() == _ROLE_MOUSE_NPC)
		return;
	IAni* pIcon = RoleDataQuery()->GetPlayerFaceIcon (this->GetFace(), 0);
	if(!pIcon)
		return;
	CMySize sizeFont;
	sizeFont.iHeight	=CMyBitmap::GetFontSize();
	sizeFont.iWidth		=sizeFont.iHeight/2;
	int nShowX	=m_Info.posScr.x-50;
	int nShowY	=m_Info.posScr.y-sizeFont.iHeight-(OFFSET_NAMESHOW+20)-this->GetOffsetY();
	g_objGameMap.MapScaleShowPos(nShowX, nShowY);
	DWORD dwIconSize = 32*g_objGameMap.GetScale()/_NORMAL_SCALE;
	pIcon->ShowEx(0, nShowX, nShowY, NULL, dwIconSize, dwIconSize);
	CMyPos posWorld, posBg;
	this->GetWorldPos(posWorld.x, posWorld.y);
	g_objGameMap.World2Bg(posWorld.x, posWorld.y, posBg.x, posBg.y);
	CMyPos posShadow;
	posShadow.x = 1;
	posShadow.y = 1;
	int nTextX = nShowX+dwIconSize+2;
	const char* pszString	=this->GetName();
	if (strlen(pszString) > 0)
	{
		static DWORD dwNameFlashTimer = ::TimeGet();
		DWORD dwColor = _COLOR_WHITE;
		if(this->TestStatus(USERSTATUS_DARKRED))
			dwColor = _COLOR_BLACK;
		else if (this->TestStatus(USERSTATUS_RED))
			dwColor = _COLOR_RED;
		if(this->TestStatus(0x00000001))
		{
			int nFrame = (::TimeGet() - dwNameFlashTimer)/200;
			if(nFrame > nFrame/2*2)
				dwColor = _COLOR_BLUE;
		}
		int nTextY = nShowY+sizeFont.iHeight-this->GetOffsetY();
		CMyBitmap::ShowString(nTextX, nTextY+5, dwColor, pszString, NULL, 0, false, RENDER_TEXT_SHADOW, 0xff000000 | ~dwColor, posShadow);
	}
	if (strlen(this->GetSyndicate()) > 0 && this->GetSydicateID() != 0)
	{
		DWORD dwSyndicateColor;
		switch(g_objPlayerSet.GetSyndicateStatus(this->GetSydicateID()) )
		{
		case SYNDICATE_ALLY:
			dwSyndicateColor = _COLOR_GREEN;
			break;
		case SYNDICATE_ANTAGONIZE:
			dwSyndicateColor = _COLOR_RED;
			break;
		case SYNDICATE_SELF:
			dwSyndicateColor = _COLOR_YELLOW;
			break;
		case _COLOR_WHITE:
			dwSyndicateColor = SYNDICATE_NORMAL;
			break;
		default:
			dwSyndicateColor = _COLOR_WHITE;
			break;
		}
		int nSynX = nShowX+dwIconSize+2;
		int nSynY = nShowY;
		CMyBitmap::ShowString(nSynX+1, nSynY+1-this->GetOffsetY(), 0x0, this->GetSyndicate());
		CMyBitmap::ShowString(nSynX, nSynY-this->GetOffsetY(), dwSyndicateColor, this->GetSyndicate());
		if(strlen(this->GetSubSyndicate()) == 0 && this->GetSyndicateRank() != RANK_SAN)
		{
			nSynX += strlen(this->GetSyndicate()) * sizeFont.iWidth;
			CMyBitmap::ShowString(nSynX, nSynY-this->GetOffsetY(), 0x6699ff, this->GetSyndicateTitle(), NULL, 0, true, RENDER_TEXT_SHADOW, 0x0, posShadow);
		}
		if(strlen(this->GetSubSyndicate()) != 0)
		{
			IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "Syndicate");
			if(pAni)
				pAni->Show(0, nSynX + strlen(this->GetSyndicate()) * sizeFont.iWidth, nSynY+1-this->GetOffsetY());
			nSynX += (2+strlen(this->GetSyndicate())) * sizeFont.iWidth;
			CMyBitmap::ShowString(nSynX, nSynY-this->GetOffsetY(), dwSyndicateColor, this->GetSubSyndicate(), NULL, 0, true, RENDER_TEXT_SHADOW, 0x0, posShadow);
		}
		if(strlen(this->GetSubSyndicate()) != 0 && this->GetSyndicateRank() != RANK_SAN)
		{
			nSynX += strlen(this->GetSubSyndicate()) * sizeFont.iWidth;
			CMyBitmap::ShowString(nSynX, nSynY-this->GetOffsetY(), 0x6699ff, this->GetSyndicateTitle(), NULL, 0, true, RENDER_TEXT_SHADOW, 0x0, posShadow);
		}
	}
	this->ShowKoAmount(true);
}

DWORD CPlayer::SetCommand (CCommand* pCommand)
{
	if (!pCommand)
		return false;
	switch(pCommand->iType)
	{
	case _COMMAND_DIE:
		{
			m_setCmdList.clear();
			this->ResetActionData();
			int nCmds = m_infoPlayer.setCmd.size();
			for (int i = 0; i < nCmds; i++)
			{
				CCommand& cmd = m_infoPlayer.setCmd[i];
				CRole::SetCommand(&cmd);
				CRole::ProcessCommand();
				this->ResetActionData();
			}
			m_infoPlayer.setCmd.clear();
			CRole::SetCommand(pCommand);
		}
		break;
	case _COMMAND_INTONE:
	case _COMMAND_ATTACK:
	case _COMMAND_SHOOT:
	case _COMMAND_SPATTACK:
	case _COMMAND_RUSHATK:
	case _COMMAND_LOCKATK:
	case _COMMAND_BUMP:
	case _COMMAND_SHITHAPPEN:
		{
			m_setCmdList.clear();
			int nCmds = m_infoPlayer.setCmd.size();
			for (int i = 0; i < nCmds; i++)
			{
				CCommand& cmd = m_infoPlayer.setCmd[i];
				CRole::SetCommand(&cmd);
				CRole::ProcessCommand();
				this->ResetActionData();
			}
			m_infoPlayer.setCmd.clear();
			this->ResetActionData();
			CRole::SetCommand(pCommand);
		}
		break;
	case _COMMAND_TAKEOFF:
	case _COMMAND_LANDING:
	case _COMMAND_WALK:
	case _COMMAND_RUN:
	case _COMMAND_JUMP:
	case _COMMAND_WALKFORWARD:
	case _COMMAND_RUNFORWARD:
		{
			m_setCmdList.clear();
			int nCmds = m_infoPlayer.setCmd.size();
			for (int i = nCmds - 1; i >= 0; i--)
			{
				CONST CCommand& cmd = m_infoPlayer.setCmd[i];
				switch (cmd.iType)
				{
				case _COMMAND_POSE:
				case _COMMAND_STANDBY:
				case _COMMAND_EMOTION:
				case _COMMAND_ACTION:
				case _COMMAND_MINE:
				case _COMMAND_WOUND:
				case _COMMAND_DEFEND:
					m_infoPlayer.setCmd.erase(m_infoPlayer.setCmd.begin() + i);
					break;
				default:
					break;
				}
			}
			if (this->IsIdle())
				this->ResetActionData();
			m_infoPlayer.setCmd.push_back(*pCommand);
		}
		break;
	case _COMMAND_ACTION:
		if (pCommand->nData == _ACTION_DISAPPEAR)
		{
			m_infoPlayer.setCmd.clear();
			this->ResetActionData();
			CRole::SetCommand(pCommand);
			break;
		}
		m_infoPlayer.setCmd.push_back(*pCommand);
		break;
	default:
		m_infoPlayer.setCmd.push_back(*pCommand);
		break;
	}
	return TRUE;
}
void CPlayer::Process(void* pInfo)
{
	m_infoPlayer.bShowIcon = false;
	if(!m_pIRoleView)
		return;
	if(!this->TestStatus(USERSTATUS_FLY))
		this->SetFlyOffsetGoal(0);
	else
		this->SetFlyOffsetGoal(100);
	this->ProcessAniEffect();
	if (this->ProcessAction())
	{
		if (m_infoPlayer.setCmd.size() > 0)
		{
			CCommand cmd = m_infoPlayer.setCmd.front();
			CRole::SetCommand(&cmd);
			m_infoPlayer.setCmd.pop_front();
		}
		this->ProcessCommand();
		this->SetActionData();
	}
	else if (this->IsIdle() && this->ProcessTimerCommand())
	{
		this->ProcessCommand();
		this->SetActionData();
	}
	if (this->GetActionType() == _ACTION_WALKL ||
			this->GetActionType() == _ACTION_RUNL ||
				this->GetActionType() == _ACTION_RUNR)
	{
		CMyPos posCurrent = m_Info.posMap;
		CMyPos posPreStep;
		int nDir = this->GetDir();
		if(nDir > 3)
			g_objGameMap.World2Cell(m_Info.posActionEnd.x, m_Info.posActionEnd.y, posPreStep.x, posPreStep.y);
		else
			g_objGameMap.World2Cell(m_Info.posActionBegin.x, m_Info.posActionBegin.y, posPreStep.x, posPreStep.y);
		m_Info.posMap = posPreStep;
		if (g_objGameMap.IsPosVisible(m_Info.posMap, 2))
			g_objGameMap.ApplyObjShow(this);
		m_Info.posMap = posCurrent;
	}
	else
	{
		if (g_objGameMap.IsPosVisible(m_Info.posMap, 2))
			g_objGameMap.ApplyObjShow(this);
	}
	if (strlen(m_szCryOut) > 0)
	{
		if (RandGet(100) == 1)
		{
			this->Say(m_szCryOut);
		}
	}
	m_objFStr.Process();
	m_objZoomNum.Process();
	const int nStepUp = 6;
	const int nStepDn = 6;
	if(m_Info.nFlyOffset < m_Info.nFlyOffsetGoal)
	{
		m_Info.nFlyOffset += nStepUp;
		if(m_Info.nFlyOffset > m_Info.nFlyOffsetGoal)
			m_Info.nFlyOffset = m_Info.nFlyOffsetGoal;
	}
	if(m_Info.nFlyOffset > m_Info.nFlyOffsetGoal)
	{
		m_Info.nFlyOffset -= nStepDn;
		if(m_Info.nFlyOffset < m_Info.nFlyOffsetGoal)
			m_Info.nFlyOffset = m_Info.nFlyOffsetGoal;
	}
}

void CPlayer::SetMate(const char* pszMate)
{
	if(!pszMate || _MAX_NAMESIZE < strlen(pszMate))
		return;

	strcpy(m_infoPlayer.szMate, pszMate);
}

void CPlayer::DoLevelUp()
{
	m_infoPlayer.nExp = 0;
	if (m_objEffect)
		this->m_objEffect->Add("LevelUp");
	if (this->IsVisible())
	{
		CMyPos posWorld = { 0, 0 };
		this->GetWorldPos(posWorld);
		::DXPlaySound("sound/levelup.wav", posWorld.x, posWorld.y, 2000);
	}
}
void CPlayer::ReplaceStatus	(DWORD dwStatus)
{
	this->ProcessStatusEffect(dwStatus);
	if (dwStatus&USERSTATUS_SPSOLDIER)
	{
		if (!this->TestStatus(USERSTATUS_SPSOLDIER))
		{
			m_dwKoScale = 100;
		}
	}
	else
	{
		if (this->TestStatus(USERSTATUS_SPSOLDIER))
		{
			m_infoPlayer.dwKoAmount = 0;
		}
	}
	if (dwStatus&USERSTATUS_TORNADO)
	{
		if (!this->TestStatus(USERSTATUS_TORNADO))
		{
			m_dwKoScale = 100;
		}
	}
	else
	{
		if (this->TestStatus(USERSTATUS_TORNADO))
		{
			m_infoPlayer.dwKoAmount = 0;
		}
	}
	if (dwStatus&0x00080000)
	{
		if (!this->TestStatus(0x00080000))
		{
			this->ResetStepDir();
			this->SetActionCmd(_ACTION_BODYSHIELD);
		}
	}
	if (dwStatus&0x00100000)
	{
		if (!this->TestStatus(0x00100000))
		{
			this->ResetStepDir();
			this->SetActionCmd(_ACTION_GODBELIEVE);
		}
	}
	if (dwStatus&USERSTATUS_DIE)
	{
	}
	else
	{
		if (TestStatus(USERSTATUS_DIE))
		{
			this->SetActionCmd(_ACTION_EXCITEMENT);
			if (this->IsBlessed() && m_objEffect)
				m_objEffect->Add("bless");
		}
	}
	if (dwStatus&USERSTATUS_PRAY)
	{
		if (!this->TestStatus(USERSTATUS_PRAY))
			if (m_objEffect)
				m_objEffect->Add("LuckDiffuse");
	}
	else
	{
		if (this->TestStatus(USERSTATUS_PRAY))
			if (m_objEffect)
				m_objEffect->DeleteByName("LuckDiffuse");
	}
	if (dwStatus&USERSTATUS_PRAYFOLLOW)
	{
		if (!this->TestStatus(USERSTATUS_PRAYFOLLOW))
			if (m_objEffect)
				m_objEffect->Add("LuckAbsorb");
	}
	else
	{
		if (this->TestStatus(USERSTATUS_PRAYFOLLOW))
			if (m_objEffect)
				m_objEffect->DeleteByName("LuckAbsorb");
	}
	if (dwStatus&0x00000800)
	{
		m_tmDisappear = ::TimeGet();
	}
	if(!this->TestStatus(USERSTATUS_FLY))
	{
		if(dwStatus&USERSTATUS_FLY)
		{
			CMyPos posCell;
			this->GetPos(posCell);
			CCommand cmd;
			memset(&cmd, 0, sizeof(CCommand));
			cmd.iType		=_COMMAND_TAKEOFF;
			cmd.iStatus		=_CMDSTATUS_BEGIN;
			cmd.posTarget	=posCell;
			this->SetCommand(&cmd);
		}
	}
	else
	{
		if(!(dwStatus&USERSTATUS_FLY))
		{
			CMyPos posCell;
			this->GetPos(posCell);
			CCommand cmd;
			memset(&cmd, 0, sizeof(CCommand));
			cmd.iType		=_COMMAND_LANDING;
			cmd.iStatus		=_CMDSTATUS_BEGIN;
			cmd.posTarget	=posCell;
			this->SetCommand(&cmd);
		}
	}
	this->SetStatus(dwStatus, 0);
}
void CPlayer::ReplaceStatus2	(DWORD dwStatus)
{
	if (dwStatus&USERSTATUS2_CURSE)
	{
		if (!this->TestStatus(USERSTATUS2_CURSE, 1))
		{
			if (!(m_objEffect && m_objEffect->TestEffect("curse")))
				if (m_objEffect)
					m_objEffect->Add("curse");
		}
	}
	else
	{
		if (m_objEffect)
			m_objEffect->DeleteByName("curse");
	}
	if (dwStatus&USERSTATUS2_BLESS)
	{
		if (!this->TestStatus(USERSTATUS2_BLESS, 1))
		{
			if (!(m_objEffect && m_objEffect->TestEffect("bless")))
				if (m_objEffect)
					m_objEffect->Add("bless");
		}
	}
	else
	{
		if (m_objEffect)
			m_objEffect->DeleteByName("bless");
	}
	this->SetStatus(dwStatus, 1);
}
OBJID CPlayer::GetSydicateID()
{
	return m_infoPlayer.idSyndicate;
}
void CPlayer::SetSyndicateID(OBJID idSyndicate)
{
	m_infoPlayer.idSyndicate = idSyndicate;	
}
DWORD CPlayer::GetSyndicateRank()
{
	return m_infoPlayer.dwSyndicateRank;
}
void CPlayer::SetSyndicateRank(DWORD dwRank)
{
	m_infoPlayer.dwSyndicateRank = dwRank;
}
int CPlayer::GetSyndicateProffer()
{
	return m_infoPlayer.nSyndicateProffer;
}
void CPlayer::SetSyndicateProffer(int nSyndicateProffer)
{
	m_infoPlayer.nSyndicateProffer = nSyndicateProffer;
}
char* CPlayer::GetSyndicate()
{
	return g_objPlayerSet.GetSyndicateName(m_infoPlayer.idSyndicate);
}
char* CPlayer::GetSubSyndicate()
{
	return g_objPlayerSet.GetSyndicateLeaderName(m_infoPlayer.idSyndicate);
}
char* CPlayer::GetSyndicateTitle()
{
	int i;
	for(i = 0; i < RANKAMOUNT; i++)
	{
		if(SYNDICATERANKINFOSET[i].dwRank == m_infoPlayer.dwSyndicateRank)
		{
			return (char*)GameDataSetQuery()->GetStr(SYNDICATERANKINFOSET[i].idTitle);
		}
	}
	for(i = 0; i < RANKAMOUNT; i++)
	{
		if(SYNDICATERANKINFOSET[i].dwRank == RANK_NONE)
		{
			return (char*)GameDataSetQuery()->GetStr(SYNDICATERANKINFOSET[i].idTitle);
		}
	}
	return NULL;
}
char*	CPlayer::GetSyndicateLeaderName()
{
	return m_infoPlayer.szSyndicateLeader;
}
void	CPlayer::SetSyndicateLeaderName(char* pszLeader)
{
	if(pszLeader)
		strcpy(m_infoPlayer.szSyndicateLeader, pszLeader);
}
DWORD	CPlayer::GetSyndicateFund()
{
	return m_infoPlayer.dwSyndicateFund;
}
void	CPlayer::SetSyndicateFund(DWORD dwFund)
{
	m_infoPlayer.dwSyndicateFund = dwFund;
}
DWORD	CPlayer::GetSyndicatePopulation()
{
	return m_infoPlayer.dwSyndicatePopulation;
}
void	CPlayer::SetSyndicatePopulation(DWORD dwPopulation)
{
	m_infoPlayer.dwSyndicatePopulation = dwPopulation;
}

void CPlayer::SetMaxLife(DWORD dwMaxLife)
{
	if(m_pIRoleView)
		m_pIRoleView->SetVariable(_VAR_MAX_LIFE, dwMaxLife);
	m_infoPlayer.dwMaxLife = dwMaxLife;
}

DWORD CPlayer::GetLife()
{
	return m_infoPlayer.dataLife.GetValue();
}

void CPlayer::SetLife(DWORD dwLife)
{
	if(m_pIRoleView)
		m_pIRoleView->SetVariable(_VAR_LIFE, dwLife);

	m_infoPlayer.dataLife.SetValue(dwLife);
}

DWORD CPlayer::GetMana()
{
	return m_infoPlayer.dataMana.GetValue();
}

void CPlayer::SetMana(DWORD dwMana)
{
	int nMana = m_infoPlayer.dataMana.GetValue();
	if(nMana > (int)dwMana && g_objHero.GetID() == this->GetID())
		g_objHero.m_objCheck.DecMana();
	m_infoPlayer.dataMana.SetValue(dwMana);
}

char* CPlayer::GetPkTitle()
{
	return RoleDataQuery()->GetPkTitle(m_infoPlayer.nPk);
}
void CPlayer::AddProfessionalCoolposEffect(DWORD dwProfessional, BOOL bAllNosuch )
{
	switch(dwProfessional)
	{
		case 1:
			if (m_objEffect)
			{
				if (bAllNosuch)
					this->m_objEffect->Add("warrior");
				else
					this->m_objEffect->Add("warrior-s");
			}
			break;
		case 2:
			if (m_objEffect)
			{
				if (bAllNosuch)
					this->m_objEffect->Add("fighter");
				else
					this->m_objEffect->Add("fighter-s");
			}
			break;
		case 4:
			if (m_objEffect)
			{
				if (bAllNosuch)
					this->m_objEffect->Add("archer");
				else
					this->m_objEffect->Add("archer-s");
			}
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if (m_objEffect)
			{
				if (bAllNosuch)
					this->m_objEffect->Add("taoist");
				else
					this->m_objEffect->Add("taoist-s");
			}
			break;
	}
}
void CPlayer::AddMoneyEffect(DWORD dwMoney)
{
	char szStr[64];
	if (m_objEffect)
		this->m_objEffect->Add("money");
	sprintf(szStr, "$ %u", dwMoney);
	this->AddFStr(szStr, 64,0x00ffffff,3);
}
BOOL CPlayer::IsBaddy()
{
	if(this->TestStatus(0x00000001) || this->TestStatus(USERSTATUS_DARKRED))
		return true;
	return false;
}
DWORD CPlayer::GetCurLife()
{
	return m_infoPlayer.dataCurLife.GetValue();
}
void CPlayer::SetCurLife(int nLife)
{
	m_infoPlayer.dataCurLife.SetValue(nLife);
}
DWORD CPlayer::GetCurMana()
{
	return m_infoPlayer.dataCurMana.GetValue();
}
void CPlayer::SetCurMana(int nMana)
{
	m_infoPlayer.dataCurMana.SetValue(nMana);
}
void CPlayer::ShowKoAmount(BOOL bImmedia)
{
	if(!this->TestStatus(USERSTATUS_SPSOLDIER) && !this->TestStatus(USERSTATUS_TORNADO))
		return;
	int nNameShowOffset = this->GetZoomPercent()*(OFFSET_NAMESHOW+50)/100 + this->GetOffsetY();
	int nShowX	=m_Info.posScr.x;
	int nShowY	=m_Info.posScr.y-nNameShowOffset;
	IAni* pAni = GameDataSetQuery()->GetDataAni("ani/common.ani", "Ko", EXIGENCE_IMMEDIATE);
	if (pAni)
	{
		CMyBitmap* pBmp = pAni->GetFrame(0);
		if(pBmp)
		{
			CMySize sizeKO;
			pBmp->GetSize(sizeKO.iWidth, sizeKO.iHeight);
			sizeKO.iWidth = sizeKO.iWidth*2/3;
			sizeKO.iHeight = sizeKO.iHeight*2/3;
			CMyPos posShow = {CMyBitmap::GetScreenWidth()-60, CMyBitmap::GetScreenHeight()-220};
			posShow.x = nShowX-sizeKO.iWidth*4*this->GetZoomPercent()/3/100;
			posShow.y = nShowY-sizeKO.iHeight*this->GetZoomPercent()/2/100;
			pBmp->ShowEx(posShow.x, posShow.y, NULL, sizeKO.iWidth, sizeKO.iHeight);
			CMyPos posKo = {posShow.x+sizeKO.iWidth*this->GetZoomPercent()*2/100, posShow.y+sizeKO.iHeight*this->GetZoomPercent()/2/100};
			m_objImgNumber.SetShowPos(posKo);
			char szInfo[256] = "";
			sprintf(szInfo, "%02u", this->GetKoAmount());
			m_objImgNumber.SetString(szInfo);
			if(m_dwKoScale > 50)
				m_dwKoScale -= 10;
			m_objImgNumber.SetScalePercent(100);
			if(this->TestStatus(USERSTATUS_SPSOLDIER))
				m_objImgNumber.SetColor(_COLOR_RED, 0xff);
			else
				m_objImgNumber.SetColor(_COLOR_GREEN, 0xff);
			m_objImgNumber.Show();
		}
	}
}
void	CPlayer::ProcessStatusEffect(DWORD dwStatus)
{
	int nAmount = RoleDataQuery()->GetStatusEffectInfoAmount();
	for(int i = 0; i < nAmount; i ++)
	{
		StatusEffectInfo* pInfo = RoleDataQuery()->GetStatusEffectInfoByIndex(i);
		if(!pInfo)
			continue;
		if (dwStatus&pInfo->dwStatus)
		{
			if (!this->TestStatus(pInfo->dwStatus))
			{
				if (strcmp(pInfo->sz2DEffect, "NULL") != 0 && (!this->TestAniEffect(pInfo->sz2DEffect)))
					this->AddAniEffect(pInfo->sz2DEffect);
				if(strcmp(pInfo->sz3DEffect, "NULL") != 0)
					if (m_objEffect)
						m_objEffect->Add(pInfo->sz3DEffect);
			}
		}
		else
		{
			if (this->TestStatus(pInfo->dwStatus))
			{
				if(strcmp(pInfo->sz2DEffect, "NULL") != 0)
					this->ClrAniEffect(pInfo->sz2DEffect);
				if(strcmp(pInfo->sz3DEffect, "NULL") != 0)
					if (m_objEffect)
						m_objEffect->DeleteByName(pInfo->sz3DEffect);
			}
		}
	}
}
DWORD	CPlayer::GetData(DWORD dwDataType)
{
	if(m_pFD)
		return m_pFD->GetValue(dwDataType);
	return 0;
}
void	CPlayer::SetData(DWORD dwDataType, DWORD dwData)
{

	if(g_objHero.GetID() == this->GetID() && _PLAYER_LIFE == dwDataType)
	{
		int nLife = this->GetData(_PLAYER_LIFE);
		if(nLife > (int)dwData)
			g_objHero.m_objCheck.DecLife();
	}
	if(m_pFD)
		m_pFD->SetValue(dwData, dwDataType);
}

void CPlayer::SetTimerCommand(CCommand& cmd, DWORD tmExecute, const char* pszEffect, const char* pszSound)
{
	TimerCmdInfo info;
	info.cmd = cmd;
	info.tmExecute = tmExecute;

	if (pszEffect)
		info.strEffect = pszEffect;

	if (pszSound)
		info.strSound = pszSound;

	m_setCmdList.push_back(info);
}

void CPlayer::ClrTimerCommand()
{
	m_setCmdList.clear();
}

bool CPlayer::ProcessTimerCommand(void)
{
	if (m_setCmdList.size() <= 0)
		return false;

	std::list< TimerCmdInfo >::iterator iter = m_setCmdList.begin();
	for ( ; iter != m_setCmdList.end(); iter++)
	{
		TimerCmdInfo& info = (*iter);
		if (::TimeGet() >= info.tmExecute)
		{
			CRole::SetCommand(&info.cmd);

			if (info.strEffect.size())
				if (m_objEffect)
					m_objEffect->Add((char*)info.strEffect.c_str());

			if (info.strSound.size())
			{
				CMyPos pos;
				this->GetWorldPos(pos);
				::DXPlaySound((char*)info.strSound.c_str(), pos.x, pos.y, 2000);
			}

			m_setCmdList.erase(iter);

			return true;
		}
	}

	return false;
}

void CPlayer::SetMetempsychosis(int nMetempsychosis)
{
	m_infoPlayer.nMetempsychosis = nMetempsychosis;
	if (nMetempsychosis == 2)
	{
		if (!m_objEffect->TestEffect("2NDMetempsychosis"))
			if (m_objEffect)
				m_objEffect->Add("2NDMetempsychosis");
	}
}
