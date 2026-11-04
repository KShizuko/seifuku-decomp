#include "Radar.h"
#include "3DGameMap.h"
#include "GameDataSet.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "nd_bitmap.h"

CRadar::CRadar()
{
	m_nMode = _MODE_256;
}

CRadar::~CRadar()
{
}

void CRadar::Show(OBJID idMap)
{
	if(!ProcessShowRect())
		return;
	char szAniFile[16] = "ani/MiniMap.ani";
	char szAniIndex[256] = "";
	sprintf(szAniIndex, "%u", idMap);
	IAni* pAni = GameDataSetQuery()->GetDataAni(szAniFile, szAniIndex);
	if(!pAni)
		return;
	if(pAni->GetFrameAmount() == 0)
		return;

	int nMode = m_nMode;
	if(pAni->GetFrameAmount() < 4)
		m_nMode = 4;

	switch(m_nMode)
	{
	case _MODE_128:
		this->ShowMap128(pAni);
		break;
	case _MODE_256:
		ShowMap256(pAni);
		break;
	case _MODE_512:
		ShowMap512(pAni);
		break;
	case 4:
		ShowMapSmall(pAni);
		break;
	default:
		return;
	}
	this->ShowHero(pAni);
	this->ShowMarks(pAni);
	m_nMode = nMode;
}

void CRadar::ShowMapSmall(IAni* pAni)
{
	if(!pAni)
		return;
	if(pAni->GetFrameAmount() <= 0)
		return;
	CMySize sizeFrame;
	sizeFrame.iWidth = pAni->GetSize(0).iWidth;
	sizeFrame.iHeight = pAni->GetSize(0).iHeight;
	CMySize sizeBg;
	g_objGameMap.GetBgSize(sizeBg);
	CMyBitmap* pBmp = pAni->GetFrame(0);
	if(pBmp)
		pBmp->ShowEx(CMyBitmap::GetScreenWidth() - sizeFrame.iWidth, 0, NULL, sizeFrame.iWidth, sizeFrame.iHeight, 0xAA, 0);
}

void CRadar::ShowMap128(IAni* pAni)
{
	if(!pAni)
		return;
	if(pAni->GetFrameAmount() != 4)
		return;

	CMySize sizeFrame3 = pAni->GetSize(3);
	CMySize sizeFrame0 = pAni->GetSize(0);
	int nTotalWidth = sizeFrame0.iWidth + sizeFrame3.iWidth;
	int nTotalHeight = pAni->GetSize(3).iHeight + pAni->GetSize(0).iHeight;

	CMySize sizeBg;
	g_objGameMap.GetBgSize(sizeBg);
	int nWorldX, nWorldY;
	g_objGameMap.Screen2World(CMyBitmap::GetScreenWidth() / 2, CMyBitmap::GetScreenHeight() / 2, nWorldX, nWorldY);
	int nBgX, nBgY;
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);

	m_fScaleX = (float)nTotalWidth / sizeBg.iWidth;
	m_fScaleY = (float)nTotalHeight / sizeBg.iHeight;
	int nWidth = m_rcShow.right - m_rcShow.left;
	int nHeight = m_rcShow.bottom - m_rcShow.top;
	int nX = (int)(nBgX * m_fScaleX) - nWidth / 2;
	int nY = (int)(nBgY * m_fScaleY) - nHeight / 2;
	if(nX < 0)
		nX = 0;
	if(nY < 0)
		nY = 0;
	if(nX > nTotalWidth - nWidth)
		nX = nTotalWidth - nWidth;
	if(nY > nTotalHeight - nHeight)
		nY = nTotalHeight - nHeight;

	m_posFrame.x = m_rcShow.left - nX;
	m_posFrame.y = m_rcShow.top - nY;

	ShowFrame(pAni->GetFrame(0), m_posFrame.x, m_posFrame.y);
	ShowFrame(pAni->GetFrame(1), m_posFrame.x + pAni->GetSize(0).iWidth, m_posFrame.y);
	ShowFrame(pAni->GetFrame(2), m_posFrame.x, m_posFrame.y + pAni->GetSize(0).iHeight);
	ShowFrame(pAni->GetFrame(3), m_posFrame.x + pAni->GetSize(0).iWidth, m_posFrame.y + pAni->GetSize(0).iHeight);
}

void CRadar::ShowMap256(IAni* pAni)
{
	if(!pAni)
		return;
	if(pAni->GetFrameAmount() != 4)
		return;

	CMySize sizeFrame3 = pAni->GetSize(3);
	CMySize sizeFrame0 = pAni->GetSize(0);
	int nTotalWidth = sizeFrame0.iWidth + sizeFrame3.iWidth;
	int nTotalHeight = pAni->GetSize(3).iHeight + pAni->GetSize(0).iHeight;
	int nWidth = m_rcShow.right - m_rcShow.left;
	int nHeight = m_rcShow.bottom - m_rcShow.top;
	int nWidth0 = pAni->GetSize(0).iWidth * nWidth / nTotalWidth;
	int nHeight0 = pAni->GetSize(0).iHeight * nHeight / nTotalHeight;
	int nX = m_rcShow.left;
	int nY = m_rcShow.top;

	CMyBitmap* pBmp = pAni->GetFrame(0);
	if(!pBmp)
		return;
	pBmp->ShowEx(nX, nY, NULL, nWidth0, nHeight0, 0x99, 0);
	int nHeight1 = pAni->GetSize(1).iHeight * nHeight / nTotalHeight;
	pBmp = pAni->GetFrame(1);
	if(!pBmp)
		return;
	pBmp->ShowEx(nX + nWidth0, nY, NULL, nWidth - nWidth0, nHeight1, 0x99, 0);
	pBmp = pAni->GetFrame(2);
	if(!pBmp)
		return;
	pBmp->ShowEx(nX, nY + nHeight0, NULL, nWidth0, nHeight - nHeight0, 0x99, 0);
	pBmp = pAni->GetFrame(3);
	if(!pBmp)
		return;
	pBmp->ShowEx(nX + nWidth0, nY + nHeight0, NULL, nWidth - nWidth0, nHeight - nHeight1, 0x99, 0);
}

void CRadar::ShowMap512(IAni* pAni)
{
	if(!pAni)
		return;
	if(pAni->GetFrameAmount() != 4)
		return;

	int nTotalWidth = pAni->GetSize(0).iWidth + pAni->GetSize(3).iWidth;
	pAni->GetSize(3);
	pAni->GetSize(0);
	int nX = CMyBitmap::GetScreenWidth() - nTotalWidth;
	pAni->Show(0, nX, 0, 0x99, 0);
	pAni->Show(1, pAni->GetSize(0).iWidth + nX, 0, 0x99, 0);
	pAni->Show(2, nX, pAni->GetSize(0).iHeight, 0x99, 0);
	pAni->Show(3, pAni->GetSize(0).iWidth + nX, pAni->GetSize(1).iHeight, 0x99, 0);
}

void CRadar::ShowFrame(CMyBitmap* pBmp, int nX, int nY)
{
	if(!pBmp)
		return;
	int nWidth, nHeight;
	pBmp->GetSize(nWidth, nHeight);
	int nRight = nX + nWidth;
	int nBottom = nY + nHeight;
	int nRcWidth = m_rcShow.right - m_rcShow.left;
	int nRcHeight = m_rcShow.bottom - m_rcShow.top;
	RECT rc;
	if(nX <= m_rcShow.left && m_rcShow.left <= nRight && nY <= m_rcShow.top && m_rcShow.top <= nBottom)
	{
		nWidth = nRight - m_rcShow.left;
		if(nWidth > nRcWidth)
			nWidth = nRcWidth;
		nHeight = nBottom - m_rcShow.top;
		if(nHeight > nRcHeight)
			nHeight = nRcHeight;
		if(nWidth != 0 && nHeight != 0)
		{
			rc.top = m_rcShow.top - nY;
			rc.left = m_rcShow.left - nX;
			rc.bottom = rc.top + nHeight;
			rc.right = rc.left + nWidth;
			pBmp->ShowEx(m_rcShow.left, m_rcShow.top, &rc, nWidth, nHeight, 0xAA, 0);
		}
	}
	else if(nX <= m_rcShow.right && m_rcShow.right <= nRight && nY <= m_rcShow.top && m_rcShow.top <= nBottom)
	{
		rc.right = m_rcShow.right - nX;
		nWidth = rc.right;
		if(nWidth > nRcWidth)
			nWidth = nRcWidth;
		nHeight = nBottom - m_rcShow.top;
		if(nHeight > nRcHeight)
			nHeight = nRcHeight;
		if(nWidth != 0 && nHeight != 0)
		{
			rc.top = m_rcShow.top - nY;
			rc.left = rc.right - nWidth;
			rc.bottom = rc.top + nHeight;
			pBmp->ShowEx(m_rcShow.right - nWidth, m_rcShow.top, &rc, nWidth, nHeight, 0xAA, 0);
		}
	}
	else if(nX <= m_rcShow.left && m_rcShow.left <= nRight && nY <= m_rcShow.bottom && m_rcShow.bottom <= nBottom)
	{
		nWidth = nRight - m_rcShow.left;
		if(nWidth > nRcWidth)
			nWidth = nRcWidth;
		rc.bottom = m_rcShow.bottom - nY;
		nHeight = rc.bottom;
		if(nHeight > nRcHeight)
			nHeight = nRcHeight;
		if(nWidth != 0 && nHeight != 0)
		{
			rc.left = m_rcShow.left - nX;
			rc.top = rc.bottom - nHeight;
			rc.right = rc.left + nWidth;
			pBmp->ShowEx(m_rcShow.left, m_rcShow.bottom - nHeight, &rc, nWidth, nHeight, 0xAA, 0);
		}
	}
	else if(nX <= m_rcShow.right && m_rcShow.right <= nRight && nY <= m_rcShow.bottom && m_rcShow.bottom <= nBottom)
	{
		rc.right = m_rcShow.right - nX;
		nWidth = rc.right;
		if(nWidth > nRcWidth)
			nWidth = nRcWidth;
		rc.bottom = m_rcShow.bottom - nY;
		nHeight = rc.bottom;
		if(nHeight > nRcHeight)
			nHeight = nRcHeight;
		if(nWidth != 0 && nHeight != 0)
		{
			rc.left = 0;
			rc.top = 0;
			pBmp->ShowEx(nX, nY, &rc, nWidth, nHeight, 0xAA, 0);
		}
	}
}

void CRadar::ShowHero(IAni* pAni)
{
	if(!pAni)
		return;
	CMyPos posHero;
	g_objHero.GetPos(posHero);
	CMyPos posRadar;
	this->CellPos2RaderPos(pAni, posHero, posRadar);
	char szAniFile[16] = "ani/MiniMap.ani";
	char szAniIndex[8] = "hero";
	IAni* pAniHero = GameDataSetQuery()->GetDataAni(szAniFile, szAniIndex);
	if(!pAniHero)
		return;
	static int s_dwTimeBegin = ::TimeGet();
	CMyBitmap* pBmp = pAniHero->GetFrame((s_dwTimeBegin / 30) % pAniHero->GetFrameAmount());
	if(pBmp)
	{
		int nWidth, nHeight;
		pBmp->GetSize(nWidth, nHeight);
		pBmp->Show(posRadar.x - nWidth / 2, posRadar.y - nHeight / 2, 0, 0);
	}
}

void CRadar::ShowMarks(IAni* pAni)
{
	CMyPos posCell, posRadar;
	IAni* pAniMark;

	CPlayer* pPlayer = g_objPlayerSet.GetPlayer(g_objHero.GetMate());
	if(pPlayer)
	{
		pPlayer->GetPos(posCell);
		this->CellPos2RaderPos(pAni, posCell, posRadar);
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "PlayerFlag", 2);
		if(pAniMark)
		{
			CMySize sizeMark = pAniMark->GetSize(2);
			pAniMark->Show(2, posRadar.x - sizeMark.iWidth / 2, posRadar.y - sizeMark.iHeight / 2, 0, 0);
			pPlayer->SetShowIcon(true);
		}
	}

	DWORD dwFlashTime = 8000;
	if(g_objHero.IsTeamMember(g_objHero.GetID()) && !g_objHero.IsTeamLeader() &&
		::TimeGet() - g_objHero.GetTeamLeaderFlashTime() < dwFlashTime)
	{
		CMyPos posLeader = g_objHero.GetTeamLeaderPos();
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "TeamLeader");
		if(pAniMark)
		{
			this->CellPos2RaderPos(pAni, posLeader, posRadar);
			static DWORD s_dwTimeBegin = ::TimeGet();
			CMyBitmap* pBmp = pAniMark->GetFrame(0);
			if(!pBmp)
				return;
			int nWidth, nHeight;
			pBmp->GetSize(nWidth, nHeight);
			pBmp->Show(posRadar.x - nWidth / 2, posRadar.y - nHeight / 2, 0, 0);
		}
	}

	if(::TimeGet() - g_objGameMap.GetFriendBeginShow() < 6001)
	{
		CMyPos posFriend;
		g_objGameMap.GetFriendPos(posFriend);
		this->CellPos2RaderPos(pAni, posFriend, posRadar);
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "Friend", 2);
		if(pAniMark)
		{
			DWORD dwFrame = ::TimeGet() / 30;
			CMySize sizeMark = pAniMark->GetSize(dwFrame);
			pAniMark->Show(dwFrame, posRadar.x - sizeMark.iWidth / 2, posRadar.y - sizeMark.iHeight / 2, 0, 0);
		}
	}

	int nAmount = g_objHero.GetTeamMemberAmount();
	int i;
	for(i = 1; i < nAmount; i ++)
	{
		TeamMemberInfo* pInfo = g_objHero.GetTeamMemberInfo(i);
		if(!pInfo)
			continue;
		pPlayer = g_objPlayerSet.GetPlayer(pInfo->id);
		if(!pPlayer || pPlayer->GetID() == g_objHero.GetID() || pPlayer->GetShowIcon())
			continue;
		pPlayer->GetPos(posCell);
		this->CellPos2RaderPos(pAni, posCell, posRadar);
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "PlayerFlag", 2);
		if(pAniMark)
		{
			CMySize sizeMark = pAniMark->GetSize(3);
			pAniMark->Show(3, posRadar.x - sizeMark.iWidth / 2, posRadar.y - sizeMark.iHeight / 2, 0, 0);
		}
		pPlayer->SetShowIcon(true);
	}

	nAmount = g_objHero.GetFriendAmount();
	for(i = 0; i < nAmount; i ++)
	{
		CFriend* pFriend = g_objHero.GetFriendByIndex(i);
		if(!pFriend)
			continue;
		pPlayer = g_objPlayerSet.GetPlayer(pFriend->GetID());
		if(!pPlayer || pPlayer->GetShowIcon())
			continue;
		pPlayer->GetPos(posCell);
		this->CellPos2RaderPos(pAni, posCell, posRadar);
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "PlayerFlag", 2);
		if(pAniMark)
		{
			CMySize sizeMark = pAniMark->GetSize(0);
			pAniMark->Show(0, posRadar.x - sizeMark.iWidth / 2, posRadar.y - sizeMark.iHeight / 2, 0, 0);
		}
		pPlayer->SetShowIcon(true);
	}

	nAmount = g_objHero.GetEnemyAmount();
	for(i = 0; i < nAmount; i ++)
	{
		CEnemy* pEnemy = g_objHero.GetEnemyByIndex(i);
		if(!pEnemy)
			continue;
		pPlayer = g_objPlayerSet.GetPlayer(pEnemy->GetID());
		if(!pPlayer || pPlayer->GetShowIcon())
			continue;
		pPlayer->GetPos(posCell);
		this->CellPos2RaderPos(pAni, posCell, posRadar);
		pAniMark = GameDataSetQuery()->GetDataAni("ani/common.ani", "PlayerFlag", 2);
		if(pAniMark)
		{
			CMySize sizeMark = pAniMark->GetSize(1);
			pAniMark->Show(1, posRadar.x - sizeMark.iWidth / 2, posRadar.y - sizeMark.iHeight / 2, 0, 0);
		}
		pPlayer->SetShowIcon(true);
	}
}

void CRadar::CellPos2RaderPos(IAni* pAni, CMyPos posCell, CMyPos& posRader)
{
	if(!pAni)
		return;
	int nWorldX, nWorldY;
	g_objGameMap.Cell2World(posCell.x, posCell.y, nWorldX, nWorldY);
	int nBgX, nBgY;
	g_objGameMap.World2Bg(nWorldX, nWorldY, nBgX, nBgY);
	CMySize sizeBg;
	g_objGameMap.GetBgSize(sizeBg);

	int nTotalWidth, nTotalHeight;
	if(m_nMode == 4)
	{
		nTotalWidth = pAni->GetSize(0).iWidth;
		nTotalHeight = pAni->GetSize(0).iHeight;
	}
	else
	{
		nTotalWidth = pAni->GetSize(3).iWidth + pAni->GetSize(0).iWidth;
		nTotalHeight = pAni->GetSize(3).iHeight + pAni->GetSize(0).iHeight;
	}

	switch(m_nMode)
	{
	case _MODE_128:
		posRader.x = (int)(nBgX * m_fScaleX);
		posRader.y = (int)(nBgY * m_fScaleY);
		posRader.x += m_posFrame.x;
		posRader.y += m_posFrame.y;
		break;
	case _MODE_256:
		m_fScaleX = (float)(m_rcShow.right - m_rcShow.left) / sizeBg.iWidth;
		m_fScaleY = (float)(m_rcShow.bottom - m_rcShow.top) / sizeBg.iHeight;
		posRader.x = (int)(nBgX * m_fScaleX) + m_rcShow.left;
		posRader.y = (int)(nBgY * m_fScaleY + m_rcShow.top);
		break;
	case _MODE_512:
	case 4:
		m_fScaleX = (float)nTotalWidth / sizeBg.iWidth;
		m_fScaleY = (float)nTotalHeight / sizeBg.iHeight;
		posRader.x = (int)(nBgX * m_fScaleX);
		posRader.y = (int)(nBgY * m_fScaleY);
		posRader.x = posRader.x - nTotalWidth + CMyBitmap::GetScreenWidth();
		break;
	}
}
BOOL CRadar::ProcessShowRect()
{
	static int s_nStepX = 0;
	static int s_nStepY = 0;
	int nCenterX = (m_rcShowBg.left + m_rcShowBg.right) / 2;
	int nCenterY = (m_rcShowBg.top + m_rcShowBg.bottom) / 2;
	if(m_nShowState == 1)
	{
		s_nStepX += 60;
		s_nStepY = (nCenterY - m_posHide.y) * s_nStepX / (nCenterX - m_posHide.x);
		m_rcShow.left = m_rcShow.right = m_posHide.x;
		m_rcShow.top = m_rcShow.bottom = m_posHide.y;
		InflateRect(&m_rcShow, (m_rcShowBg.right - m_rcShowBg.left) * s_nStepX / (nCenterX - m_posHide.x) / 2,
			(m_rcShowBg.bottom - m_rcShowBg.top) * s_nStepY / (nCenterY - m_posHide.y) / 2);
		OffsetRect(&m_rcShow, s_nStepX, s_nStepY);
		if(m_rcShow.left >= m_rcShowBg.left || m_rcShow.top <= m_rcShowBg.top)
		{
			m_rcShow = m_rcShowBg;
			s_nStepX = s_nStepY = 0;
			m_nShowState = 0;
		}
	}
	if(m_nShowState == 2)
	{
		s_nStepX += 60;
		s_nStepY = (nCenterY - m_posHide.y) * s_nStepX / (nCenterX - m_posHide.x);
		m_rcShow = m_rcShowBg;
		InflateRect(&m_rcShow, -((m_rcShowBg.right - m_rcShowBg.left) * s_nStepX / (nCenterX - m_posHide.x) / 2),
			-((m_rcShowBg.bottom - m_rcShowBg.top) * s_nStepY / (nCenterY - m_posHide.y) / 2));
		OffsetRect(&m_rcShow, -s_nStepX, -s_nStepY);
		if(m_rcShow.left <= m_posHide.x || m_rcShow.top >= m_posHide.y)
		{
			g_objGameMap.m_bShowMiniMap = FALSE;
			m_nShowState = 0;
			s_nStepX = s_nStepY = 0;
			m_rcShow.left = m_rcShow.right = m_posHide.x;
			m_rcShow.top = m_rcShow.bottom = m_posHide.y;
		}
	}
	return g_objGameMap.m_bShowMiniMap;
}

void CRadar::SetShowState(int nState)
{
	if(nState)
		g_objGameMap.m_bShowMiniMap = TRUE;
	m_nShowState = nState;
}
