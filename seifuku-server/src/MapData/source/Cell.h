#ifndef _CELL_H
#define _CELL_H

#include "BaseFunc.h"
#include "Layer.h"
#include "MyHeap.h"

const int	LAYER_TOP			= -1;

const int	MASK_SEARCHED		= 0x80;
const int	MASK_ROLECOUNT		= 0x7F;
const int	MASK_MASK			= 0x0001;
const int	MASK_ALTITUDE		= 0xFFFE;
const int	MAX_IDXLAYER		= 0xFFFF;
const int	IDXLAYER_NONE		= USHORT(-1);
const int	DEFAULT_FLOORATTR	= 0;
const int	ALTITUDE_LIMIT		= 32767;

class CCell
{
private:
	UCHAR		m_nCountFlag;
	USHORT		m_idxLayer;
	SHORT		m_nAlt2Mask;
public:
	bool	Create(int nAlt, DWORD dwMask);

public:
public:
	int		GetFloorAttr(const DEQUE2_LAYER& set2Layer) const;
	DWORD	GetFloorMask(const DEQUE2_LAYER& set2Layer) const;
	int		GetFloorAlt(const DEQUE2_LAYER& set2Layer) const;
	int		GetSurfaceAlt() const;
	void	FullMask()							{ m_nAlt2Mask |= MASK_MASK; }
	void	IncRole(int nVal = 1);
	void	DecRole(int nVal = 1);
	int		GetRoleAmount() const				{ return (m_nCountFlag&MASK_ROLECOUNT); }

public:

	void            AddLayer(DEQUE2_LAYER& set2Layer, shared_ptr<CLayer> pLayer);
	bool            DelLayer(DEQUE2_LAYER& set2Layer, int nIndex = LAYER_TOP);

	BOOL            BeSearched(){return (m_nCountFlag&MASK_SEARCHED) != 0;}
	void            SetSearchFlag(bool bSearched){if(bSearched) m_nCountFlag |= MASK_SEARCHED; else m_nCountFlag &= (~MASK_SEARCHED);}

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
