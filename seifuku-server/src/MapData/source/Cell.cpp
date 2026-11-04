#pragma warning(disable:4786)
#include "windows.h"
#include "typedef.h"
#include "cell.h"

MYHEAP_IMPLEMENTATION(CCell,s_heap);

bool CCell::Create(int nAlt, DWORD dwMask)
{
	m_nAlt2Mask		= ((nAlt*2) & MASK_ALTITUDE) | (dwMask ? MASK_MASK : 0);
	m_nCountFlag	= 0;
	m_idxLayer		= IDXLAYER_NONE;
	return true;
}

void CCell::DecRole(int nVal)
{
	CHECK((m_nCountFlag & MASK_ROLECOUNT) >= nVal);
	m_nCountFlag = (m_nCountFlag&MASK_ROLECOUNT) - nVal;
}

void  CCell::AddLayer(DEQUE2_LAYER& set2Layer, shared_ptr<CLayer> pLayer)
{
	if(!pLayer)
		return;

	if(m_idxLayer == IDXLAYER_NONE)
	{
		for(int i = 0; i < set2Layer.size(); i++)
		{
			if(set2Layer[i].size() == 0)
			{
				m_idxLayer	= i;
				set2Layer[i].push_back(pLayer);
				return ;
			}
		}
		m_idxLayer	= set2Layer.size();
		set2Layer.resize(m_idxLayer + 1);
		set2Layer[m_idxLayer].push_back(pLayer);
	}
	else
		set2Layer[m_idxLayer].push_back(pLayer);
}

bool  CCell::DelLayer(DEQUE2_LAYER& set2Layer, int nIndex )
{
	CHECKF(nIndex == LAYER_TOP || nIndex > 0);
	CHECKF(m_idxLayer != IDXLAYER_NONE && m_idxLayer < set2Layer.size());

	if(!( nIndex == LAYER_TOP || nIndex >= 0 && nIndex < set2Layer[m_idxLayer].size() ))
		return false;

	int	idx;
	if(nIndex == LAYER_TOP)
		idx	= set2Layer[m_idxLayer].size() - 1;
	else
		idx = nIndex - 1;

	set2Layer[m_idxLayer].erase(set2Layer[m_idxLayer].begin() + idx);
	if(set2Layer[m_idxLayer].size() == 0)
		m_idxLayer = IDXLAYER_NONE;

	return true;
}

DWORD CCell::GetFloorMask(const DEQUE2_LAYER& set2Layer) const
{
	if(m_idxLayer == IDXLAYER_NONE)
		return (m_nAlt2Mask&MASK_MASK) != 0;
	else
	{
		int nAmount = set2Layer[m_idxLayer].size();
		if(nAmount > 0)
		{
			CLayer* pLayer = set2Layer[m_idxLayer][nAmount-1].get();
			if(pLayer)
			{
				return pLayer->dwMask;
			}
		}
		MYASSERT(!"CCell::GetFloorMask");
		return 1;
	}
}

int CCell::GetFloorAttr(const DEQUE2_LAYER& set2Layer) const
{
	if(m_idxLayer == IDXLAYER_NONE)
		return DEFAULT_FLOORATTR;
	else
	{
		int nAmount = set2Layer[m_idxLayer].size();
		if(nAmount > 0)
		{
			CLayer* pLayer = set2Layer[m_idxLayer][nAmount-1].get();
			if(pLayer)
			{
				return pLayer->nTerrian;
			}
		}
		MYASSERT(!"CCell::GetFloorAttr");
		return DEFAULT_FLOORATTR;
	}
}

int CCell::GetFloorAlt(const DEQUE2_LAYER& set2Layer) const
{
	if(m_idxLayer == IDXLAYER_NONE)
		return short(m_nAlt2Mask&MASK_ALTITUDE) / 2;
	else
	{
		int nAmount = set2Layer[m_idxLayer].size();
		if(nAmount > 0)
		{
			CLayer* pLayer = set2Layer[m_idxLayer][nAmount-1].get();
			if(pLayer)
			{
				return pLayer->nAltitude;
			}
		}
		MYASSERT(!"CCell::GetFloorAlt");
		return ALTITUDE_LIMIT;
	}
}

int CCell::GetSurfaceAlt() const
{
	return (m_nAlt2Mask&MASK_ALTITUDE) / 2;
}

void CCell::IncRole(int nVal)
{
	CHECK((m_nCountFlag & MASK_ROLECOUNT) + nVal <= MASK_ROLECOUNT);
	m_nCountFlag = (m_nCountFlag&MASK_ROLECOUNT) + nVal;
}
