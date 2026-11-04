#if !defined(AFX_MAGICEFFECTMANAGER_H__A3654A44_8041_4EFE_9713_9F29222D4BFC__INCLUDED_)
#define AFX_MAGICEFFECTMANAGER_H__A3654A44_8041_4EFE_9713_9F29222D4BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "MagicEffect.h"
#include <deque>
using namespace std;
typedef deque<CMagicEffect*> DEQUE_MAGICEFFECT;


class CMagicEffectManager  
{
public:
	CMagicEffectManager();
	virtual ~CMagicEffectManager();
private:
	DEQUE_MAGICEFFECT m_setMagicEffect;
	friend struct MagicEffectManagerProbe;

public:
	void Destroy();
	void Process();
	void AddMagicEffect(CMagicEffect* pEffect);
};

#endif
