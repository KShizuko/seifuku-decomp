#pragma warning(disable:4786)
#include "BaseFunc.h"
#include "3DGameMap.h"
#include "Role.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "GameMsg.h"

#define _USE_ASTAR

const DWORD _TIME_FAINT	=150;

void CRole::StaticAction(int nAction, int nDir )
{
	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

	if (nDir != -1)
		m_Info.iDirection = nDir;

	m_Info.iActType = nAction;

	if (m_Info.iActType >= _ACTION_ATTACK0 && m_Info.iActType < _ACTION_ATTACK4)
		m_tmLastAtk = ::TimeGet();
}

BOOL CRole::StandBy(void)
{
	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

	if (_COMMAND_STANDBY == this->GetLastCommand() &&
			::RandGet(10) == 1)
	{
		switch(::RandGet(3))
		{
		case 0:
			m_Info.iActType	= _ACTION_REST1;
			break;
		case 1:
			m_Info.iActType	= _ACTION_REST2;
			break;
		default:
			m_Info.iActType	= _ACTION_REST3;
			break;
		}
	}
	else
		m_Info.iActType	=_ACTION_STANDBY;
	if (m_Info.nFlyOffset == 0)
	{
		if (this->IsInBattle())
			m_Info.iActType = _ACTION_ALERT;
	}
	else
	{
		m_Info.iActType = _ACTION_FLY_STANDBY;
		if (this->IsInBattle())
			m_Info.iActType = _ACTION_FLY_ALERT;
	}

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

	return true;
}

BOOL CRole::Mine(void)
{
	MYASSERT (_COMMAND_MINE == m_Info.cmdProc.iType);

	m_Info.cmdProc.iStatus	= _CMDSTATUS_ACCOMPLISH;
	m_Info.iActType			= _ACTION_MINE;

	m_Info.posActionBegin.x	= m_Info.posWorld.x;
	m_Info.posActionBegin.y	= m_Info.posWorld.y;
	m_Info.posActionEnd.x	= m_Info.posWorld.x;
	m_Info.posActionEnd.y	= m_Info.posWorld.y;

	return true;
}

BOOL CRole::Bump(void)
{
	MYASSERT (m_Info.cmdProc.iType == _COMMAND_BUMP);

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_BEGIN)
		m_Info.cmdProc.iStatus = _CMDSTATUS_PROGRESS;

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
			m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		m_Info.objPath.ClearStep();

		CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget);
		if (pTarget)
		{
			CMyPos posTarget;
			pTarget->GetPos(posTarget);

			CMyPos posMyself;
			this->GetPos(posMyself);

			int nAtkRange = 1;
			if (abs(posMyself.x-posTarget.x) <= nAtkRange
					&& abs(posMyself.y-posTarget.y) <= nAtkRange)
			{
				if (!(posMyself.x == posTarget.x && posMyself.y == posTarget.y))
				{
					this->SetDir(this->GetDir(posTarget));

					m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
					m_Info.iActType	        =_ACTION_BUMP;

					m_Info.posActionBegin.x	=m_Info.posWorld.x;
					m_Info.posActionBegin.y	=m_Info.posWorld.y;

					CMyPos posDes = { posTarget.x+_DELTA_X[m_Info.iDirection],
						posTarget.y+_DELTA_Y[m_Info.iDirection] };

					if (g_objGameMap.TestPath(pTarget, posDes) && pTarget->IsBumpBackEnable())
					{
						g_objGameMap.Cell2World(posTarget.x,
							posTarget.y,
							m_Info.posActionEnd.x,
							m_Info.posActionEnd.y);
					}
					else
						m_Info.posActionEnd = m_Info.posActionBegin;

					m_tmLastAtk = ::TimeGet();
					return true;
				}
			}
		}
		else
		{
			m_Info.objPath.ClearStep();

			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}

		CMyPos posTarget;
		pTarget->GetPos(posTarget);

		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, posTarget);
		if (m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.iDirection = m_Info.objPath.GetNextStep();
			this->Step();

			if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
			{
				m_Info.iActType	=_ACTION_RUNL;
				m_Info.cmdProc.iStatus =_CMDSTATUS_CONTINUE;
			}
			else if (m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
			{
				m_Info.iActType	=_ACTION_RUNR;
				m_Info.cmdProc.iStatus =_CMDSTATUS_PROGRESS;
			}
		}
		else
		{
			m_Info.objPath.ClearStep();

			this->StandBy();

			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}
	}

	return true;
}

BOOL CRole::Action(void)
{
	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
	m_Info.iActType	        =m_Info.cmdProc.nData;
	if (_ACTION_POPOFF != m_Info.iActType)
		this->SetDir(m_Info.cmdProc.nDir);
	else
		this->SetDir((m_Info.cmdProc.nDir));

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;

	if (m_Info.cmdProc.dwData == 0)
	{
		m_Info.posActionEnd.x	=m_Info.posWorld.x;
		m_Info.posActionEnd.y	=m_Info.posWorld.y;
	}
	else if (m_Info.cmdProc.dwData == 1)
	{
		g_objGameMap.Cell2World(m_Info.posMap.x+_DELTA_X[m_Info.cmdProc.nDir],
								m_Info.posMap.y+_DELTA_Y[m_Info.cmdProc.nDir],
								m_Info.posActionEnd.x,
								m_Info.posActionEnd.y);
	}
	else if (m_Info.cmdProc.dwData == 2)
	{
		g_objGameMap.Cell2World(m_Info.cmdProc.posTarget.x,
								m_Info.cmdProc.posTarget.y,
								m_Info.posActionEnd.x,
								m_Info.posActionEnd.y);
	}

	if (IsAttackAction(m_Info.iActType))
	{
		if (!this->IsInBattle())
			m_nLastAttackAction = -1;
		if (m_nLastAttackAction == -1)
			m_Info.iActType = _ACTION_ATTACK0;
		else
			m_Info.iActType = _ACTION_ATTACK0 + (__max(m_nLastAttackAction - _ACTION_ATTACK0, 0) + 1) % 4;
		m_tmLastAtk = ::TimeGet();
		m_nLastAttackAction = m_Info.iActType;
	}

	return true;
}

BOOL CRole::Pose(void)
{
	this->SetDir(m_Info.cmdProc.nDir);

	m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	m_Info.iActType	        =m_Info.cmdProc.nData;

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

	return false;
}

BOOL CRole::Emotion(void)
{
	this->SetDir(m_Info.cmdProc.nDir);

	BOOL bAccomplished	=false;
    switch(m_Info.cmdProc.nData)
    {
	case 903:
        m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
	    m_Info.iActType	        =m_Info.cmdProc.nData;
        bAccomplished           =true;
        break;

	case _ACTION_DANCE1:
	case _ACTION_DANCE2:
	case _ACTION_DANCE3:
	case _ACTION_DANCE4:
	case _ACTION_DANCE5:
	case _ACTION_DANCE6:
	case _ACTION_DANCE7:
	case _ACTION_DANCE8:
    case _ACTION_SAD:
    case _ACTION_SALUTE:
    case _ACTION_GENUFLECT:
	case _ACTION_SAYHELLO:
	case _ACTION_LIE:
	case 210:
	case _ACTION_SITDOWN:
        m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	    m_Info.iActType	        =m_Info.cmdProc.nData;
        break;
	case _ACTION_COOL:
        {
            if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
            {
                m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
                m_Info.iActType	        =_ACTION_COOL;
            }
            else if(_CMDSTATUS_PROGRESS == m_Info.cmdProc.iStatus)
            {
              	m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
	            m_Info.iActType	        =_ACTION_COOLPOSE;
            }
        }
        break;
	case 270:
		{
            if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
            {
                m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
                m_Info.iActType	        =270;
            }
            else if(_CMDSTATUS_PROGRESS == m_Info.cmdProc.iStatus)
            {
				m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
				m_Info.iActType	        =271;
            }
		}
		break;

	default:
        m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	    m_Info.iActType	        =_ACTION_STANDBY;
        bAccomplished           =true;
        break;
    }

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

    return bAccomplished;
}

BOOL CRole::Attack(void)
{
	if (!this->IsInBattle())
		m_nLastAttackAction = -1;
	if (m_nLastAttackAction == -1)
		m_Info.iActType	= _ACTION_ATTACK0;
	else
		m_Info.iActType	= _ACTION_ATTACK0 + (__max(m_nLastAttackAction - _ACTION_ATTACK0, 0) + 1) % 4;
	m_Info.iDirection		=m_Info.cmdProc.nDir;
	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;
	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

	m_tmLastAtk = ::TimeGet();
	m_nLastAttackAction = m_Info.iActType;
	return true;
}

BOOL CRole::RushAtk(void)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		CPlayer* pTarget	=g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget);
		if (!pTarget)
		{
			if(g_objHero.GetID() == (OBJID)m_Info.cmdProc.idTarget)
				pTarget	=&g_objHero;
			else
			{
				this->StandBy();
				return true;
			}
		}

		m_Info.iDirection	=m_Info.cmdProc.nDir;

		CMyPos posTarget;
		pTarget->GetPos(posTarget);
		posTarget.x	-=_DELTA_X[m_Info.cmdProc.nDir];
		posTarget.y	-=_DELTA_Y[m_Info.cmdProc.nDir];

		m_Info.posActionBegin.x	=m_Info.posWorld.x;
		m_Info.posActionBegin.y	=m_Info.posWorld.y;

		g_objGameMap.Cell2World(posTarget.x, posTarget.y, m_Info.posActionEnd.x, m_Info.posActionEnd.y);

		m_Info.cmdProc.iStatus	=_CMDSTATUS_DEPART;
		m_Info.cmdProc.posTarget=posTarget;

		m_Info.iActType			=_ACTION_JUMP;
		m_Info.iDirection		=this->GetDir(m_Info.cmdProc.posTarget);

		return false;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_DEPART)
	{
		this->Attack();
		return true;
	}

	return true;
}
BOOL CRole::InTone(void)
{
	if(_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		this->StaticAction(900, m_Info.cmdProc.nDir);
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}
	else if (_CMDSTATUS_PROGRESS == m_Info.cmdProc.iStatus)
	{
		this->StaticAction(901, m_Info.cmdProc.nDir);
		m_Info.cmdProc.iStatus = _CMDSTATUS_CONTINUE;
	}

	return false;
}

BOOL CRole::LockBooth(void)
{
	assert(m_Info.cmdProc.iType==_COMMAND_LOCKBOOTH);

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_BEGIN)
		m_Info.cmdProc.iStatus = _CMDSTATUS_PROGRESS;

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
			m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		m_Info.objPath.ClearStep();

		CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget);
		if (!pTarget)
		{
			m_Info.objPath.ClearStep();
			StandBy();
			this->SetDir(m_Info.cmdProc.nDir);
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}

		CMyPos posTarget;
		pTarget->GetPos(posTarget);

		CMyPos posBooth;
		posBooth.x = posTarget.x + _DELTA_X[m_Info.cmdProc.nDir]*2;
		posBooth.y = posTarget.y + _DELTA_Y[m_Info.cmdProc.nDir]*2;
		CPlayer* pPlayer=g_objPlayerSet.GetPlayer(posBooth.x, posBooth.y);
		if(pPlayer)
			return false;
		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, posBooth);
		if(m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.iDirection = m_Info.objPath.GetNextStep();
			Step();

			if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
			{
			m_Info.iActType	=_ACTION_RUNL;
			m_Info.cmdProc.iStatus =_CMDSTATUS_CONTINUE;
			}
			else if (m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
			{
			m_Info.iActType	=_ACTION_RUNR;
			m_Info.cmdProc.iStatus =_CMDSTATUS_PROGRESS;
			}
		}
		else
		{
			m_Info.objPath.ClearStep();
			StandBy();
			this->SetDir(m_Info.cmdProc.nDir);
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}
	}


	return true;
}
BOOL CRole::LockAtk(void)
{
	assert(m_Info.cmdProc.iType==_COMMAND_LOCKATK);

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_BEGIN)
	{
		m_Info.cmdProc.iStatus = _CMDSTATUS_PROGRESS;
	}

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
			m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		m_Info.objPath.ClearStep();

		CPlayer* pTarget = g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget, true);
		if (pTarget)
		{
			CMyPos posTarget;
			pTarget->GetPos(posTarget);

			CMyPos posMyself;
			this->GetPos(posMyself);

			int nAtkRange = (this->GetAddSize()+pTarget->GetAddSize()+1)/2+this->GetAtkRange();
			BOOL bCheck = false;
			if(!this->IsMyPet())
				bCheck = true;
			else
			{
				CPlayer* pStandPlayer = g_objPlayerSet.GetPlayer(posMyself.x, posMyself.y);
				if(!pStandPlayer || pStandPlayer == this)
					bCheck = true;
			}

			if (abs(posMyself.x-posTarget.x) <= nAtkRange &&
					abs(posMyself.y-posTarget.y) <= nAtkRange && bCheck)
			{
				m_Info.cmdProc.nDir = this->GetDir(posTarget);
				if(this->HaveBow())
				{
					this->Attack();
					CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(this->GetID(), pTarget->GetID(), this->GetLWeapon(), 0);
					g_objGameMap.AddMagicEffect(pEffect);
				}
				else
				{
					if(this->IsMyPet())
					{
						CPlayer* pStandPlayer = g_objPlayerSet.GetPlayer(posMyself.x, posMyself.y);
						if(!pStandPlayer || pStandPlayer == this)
							this->Attack();
						else
						{
							StandBy();
							m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
							return false;
						}
					}
					this->Attack();
				}
				return true;
			}
		}
		else
		{
			m_Info.objPath.ClearStep();

			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}

		CMyPos posTarget;
		pTarget->GetPos(posTarget);

		if(this->IsMyPet())
		{
			CMyPos posMyTarget;
			int nDir = ::RandGet(8);
			posMyTarget.x = posTarget.x + _DELTA_X[nDir];
			posMyTarget.y = posTarget.y + _DELTA_Y[nDir];
			CPlayer* pStandPlayer = g_objPlayerSet.GetPlayer(posMyTarget.x, posMyTarget.y);
			if(!pStandPlayer || pStandPlayer == this)
				g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, posMyTarget);
			else
			{
				m_Info.objPath.ClearStep();

				StandBy();
				m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
				return false;
			}
		}
		else
		{
			g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, posTarget);
		}
		if (m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.iDirection = m_Info.objPath.GetNextStep();
			Step();

			if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
			{
				m_Info.iActType	=_ACTION_RUNL;
				m_Info.cmdProc.iStatus =_CMDSTATUS_CONTINUE;
			}
			else if (m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
			{
				m_Info.iActType	=_ACTION_RUNR;
				m_Info.cmdProc.iStatus =_CMDSTATUS_PROGRESS;
			}
		}
		else
		{
			m_Info.objPath.ClearStep();

			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			return false;
		}
	}

	return true;
}

BOOL CRole::TeamFollow	(void)
{
	assert(_COMMAND_TEAMFOLLOW == m_Info.cmdProc.iType);
	CMyPos posOffset = m_Info.cmdProc.posTarget;

	CRole*	pTarget	=g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget);
	if(!pTarget)
	{
		if(g_objHero.GetID() == m_Info.cmdProc.idTarget)
			pTarget	=&g_objHero;
		else
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			StandBy();
			m_Info.cmdProc.posTarget = posOffset;
			return false;
		}
	}
	CMyPos posTarget;
	pTarget->GetPos(posTarget);
	int nDir = pTarget->GetDir(this);

	m_Info.cmdProc.posTarget.x = posTarget.x + _DELTA_X[nDir] * m_Info.cmdProc.dwData;
 	m_Info.cmdProc.posTarget.y = posTarget.y + _DELTA_Y[nDir] * m_Info.cmdProc.dwData;

	if(m_Info.posMap.x != m_Info.cmdProc.posTarget.x ||
			m_Info.posMap.y != m_Info.cmdProc.posTarget.y)
	{
		m_Info.cmdProc.iStatus		=_CMDSTATUS_BEGIN;
	}

	if(m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, m_Info.cmdProc.posTarget);

		BOOL bHavePath = false;
		if(m_Info.objPath.GetStepAmount() > 0)
			bHavePath =true;

		if (bHavePath)
			m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
		else
		{
			m_Info.objPath.ClearStep();
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
			m_Info.cmdProc.posTarget = posOffset;
			return false;
		}
	}

	if(m_Info.cmdProc.iStatus==_CMDSTATUS_PROGRESS)
	{
		if(m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.iDirection = m_Info.objPath.GetNextStep();
			Step();
			if(m_Info.objPath.GetStepAmount() > 2)
			{
				if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
				{
					m_Info.iActType	=_ACTION_RUNL;
					m_Info.cmdProc.iStatus =_CMDSTATUS_CONTINUE;
				}
				else if (m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
				{
					m_Info.iActType	=_ACTION_RUNR;
					m_Info.cmdProc.iStatus =_CMDSTATUS_PROGRESS;
				}
			}
		}
		else
		{
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
		}
	}


	if(m_Info.cmdProc.iStatus==_CMDSTATUS_ACCOMPLISH)
	{
		m_Info.cmdProc.posTarget = posOffset;
		return true;
	}
	else
	{
		m_Info.cmdProc.posTarget = posOffset;
		return false;
	}
}
BOOL CRole::PickUp(void)
{
	MYASSERT (m_Info.cmdProc.iType == _COMMAND_PICKUP);
	CMyPos posMyself;
	this->GetPos(posMyself);
	CMyPos posTarget = m_Info.cmdProc.posTarget;

	int nRange = 0;
	if (m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		m_Info.objPath.ClearStep();

		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, m_Info.cmdProc.posTarget);

		if (abs(posMyself.x-posTarget.x) <= nRange &&
			abs(posMyself.y-posTarget.y) <= nRange)
		{
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			this->StaticAction(_ACTION_PICKUP, this->GetDir());
			return true;
		}
		else if (m_Info.objPath.GetStepAmount() > 0)
			m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
		else
		{
			m_Info.objPath.ClearStep();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

			this->StandBy();
			return false;
		}
	}

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
		m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		if (abs(posMyself.x-posTarget.x) <= nRange &&
			abs(posMyself.y-posTarget.y) <= nRange)
		{
			m_Info.cmdProc.iStatus = _CMDSTATUS_ACCOMPLISH;
			this->StaticAction(_ACTION_PICKUP, this->GetDir());
			return true;
		}
		else
		{
			if(m_Info.objPath.GetStepAmount() > 0)
			{
				m_Info.iDirection = m_Info.objPath.GetNextStep();
				Step();

				if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
				{
				m_Info.iActType	=_ACTION_RUNL;
				m_Info.cmdProc.iStatus =_CMDSTATUS_CONTINUE;
				}
				else if (m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
				{
				m_Info.iActType	=_ACTION_RUNR;
				m_Info.cmdProc.iStatus =_CMDSTATUS_PROGRESS;
				}
			}
			else
			{
				StandBy();
				m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			}
		}
	}

	return false;
}

int CRole::GetStepDir(void)
{
	double dMinDistance		=65535;
	int	iStepDir	=-1;
	for(int i=0;i<_MAX_DIRECTION;i++)
	{
		int iStepX	=m_Info.posMap.x+_DELTA_X[i];
		int iStepY	=m_Info.posMap.y+_DELTA_Y[i];

		if (IsMapAttainable(iStepX,iStepY))
		{
			int iTargetX	=m_Info.cmdProc.posTarget.x;
			int iTargetY	=m_Info.cmdProc.posTarget.y;
			double  dDistance	=CGameMap::GetPreciseDistance(iStepX,iStepY,iTargetX,iTargetY);
			if (dDistance<dMinDistance)
			{
				dMinDistance	=dDistance;
				iStepDir		=i;
			}
		}
	}

	return iStepDir;
}

BOOL CRole::Jump(void)
{
	m_Info.objPath.ClearStep();

	m_lstStepDir.clear();

	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
	m_Info.iActType			=_ACTION_JUMP;
	if(this->TestStatus(USERSTATUS_FLY))
		m_Info.iActType			=_ACTION_FLY_MOVE;
	m_Info.iDirection		=this->GetDir(m_Info.cmdProc.posTarget);

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;

	g_objGameMap.Cell2World(m_Info.cmdProc.posTarget.x, m_Info.cmdProc.posTarget.y, m_Info.posActionEnd.x, m_Info.posActionEnd.y);

	return true;
}
BOOL	CRole::TakeOff(void)
{
	m_Info.objPath.ClearStep();

	m_lstStepDir.clear();

	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
	m_Info.iActType			=_ACTION_FLY_UP;
	m_Info.iDirection		=this->GetDir(m_Info.cmdProc.posTarget);
	this->SetFlyOffsetGoal(100);
	return true;
}
BOOL	CRole::Landing(void)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		m_Info.objPath.ClearStep();

		m_lstStepDir.clear();

		m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
		m_Info.iActType			=_ACTION_FLY_DOWN;
		m_Info.iDirection		=this->GetDir(m_Info.cmdProc.posTarget);
		this->SetFlyOffsetGoal(0);
	}
	return true;
}
BOOL CRole::Die(void)
{
	enum
	{
			DIE_NORMAL			= 1,
			DIE_FLY4			= 2,
			DIE_FLY6			= 3,
			DIE_FLY8			= 4,
			DIE_MAGIC			= 5,
			DIE_FLOAT			= 6,
			DIE_ARROWSHOOT		= 7,
	};

	BOOL bReturn = false;
	if(m_Info.nFlyOffset != 0)
		m_Info.cmdProc.dwData = DIE_FLOAT;
	switch(m_Info.cmdProc.dwData)
	{
	case DIE_FLY4:
		bReturn = this->DieFlying(4);
		break;
	case DIE_FLY6:
		bReturn = this->DieFlying(6);
		break;
	case DIE_FLY8:
		bReturn = this->DieFlying(8);
		break;
	case DIE_MAGIC:
		bReturn = this->DieDelay();
		break;
	case DIE_FLOAT:
		bReturn = this->DieFloat();
		break;
	case DIE_ARROWSHOOT:
		bReturn = this->DieDelay();
		break;
	default:
		bReturn = this->DieNormal();
		break;
	}

	return bReturn;
}

BOOL CRole::DieNormal(void)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		switch (m_Info.cmdProc.dwData)
		{
		case 2:
			m_Info.iActType		=_ACTION_DIE3;
			break;
		case 3:
			m_Info.iActType		=_ACTION_DIE1;
			break;
		case 4:
			m_Info.iActType		=_ACTION_DIE2;
			break;
		default:
			m_Info.iActType		=_ACTION_DIE0;
			break;
		}
		m_Info.iDirection	=m_Info.cmdProc.nDir;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_CONTINUE)
	{
		switch (m_Info.iActType)
		{
		case _ACTION_DIE1:
			m_Info.iActType = _ACTION_BODY1;
			break;
		case _ACTION_DIE2:
			m_Info.iActType = _ACTION_BODY2;
			break;
		case _ACTION_DIE3:
			m_Info.iActType = _ACTION_BODY3;
			break;
		default:
			m_Info.iActType = _ACTION_BODY0;
			break;
		}

		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_PROGRESS)
	{
		m_Info.cmdProc.iStatus	=_CMDSTATUS_OVER;
	}

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	g_objGameMap.Cell2World(m_Info.posMap.x, m_Info.posMap.y, m_Info.posActionEnd.x, m_Info.posActionEnd.y);

	return false;
}

BOOL CRole::DieFloat(void)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		m_Info.iActType		=_ACTION_FLY_DIE;

		m_Info.iDirection	=m_Info.cmdProc.nDir;
		m_Info.nFlyOffsetGoal = 0;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_CONTINUE)
	{
		m_Info.iActType = _ACTION_BODY0;
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}
	else if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
		m_Info.cmdProc.iStatus	=_CMDSTATUS_OVER;

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

	return false;
}

BOOL CRole::DieDelay(void)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		m_Info.iActType		=_ACTION_BRUISE0;
		m_Info.iDirection	=m_Info.cmdProc.nDir;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_DEPART;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_DEPART)
	{
		if (::RandGet(2) == 1)
			m_Info.iActType		=_ACTION_DIE0;
		else
			m_Info.iActType		=_ACTION_DIE1;
		m_Info.iDirection	=m_Info.cmdProc.nDir;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
	}
	else if (m_Info.cmdProc.iStatus	== _CMDSTATUS_CONTINUE)
	{
		if (m_Info.iActType == _ACTION_DIE0)
			m_Info.iActType = _ACTION_BODY0;
		else
			m_Info.iActType = _ACTION_BODY1;
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}
	else if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
		m_Info.cmdProc.iStatus	=_CMDSTATUS_OVER;

	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.posActionEnd.x	=m_Info.posWorld.x;
	m_Info.posActionEnd.y	=m_Info.posWorld.y;

	return false;
}

BOOL CRole::DieFlying(int nDistance)
{
	if (_CMDSTATUS_BEGIN == m_Info.cmdProc.iStatus)
	{
		m_Info.iDirection	=m_Info.cmdProc.nDir;
		int nWay = m_Info.cmdProc.nDir;
		switch (m_Info.cmdProc.dwData)
		{
		case 1:
			m_Info.iActType	=_ACTION_DIEFLY;
			nWay = (m_Info.cmdProc.nDir + 4) % 8;
			break;
		case 2:
			m_Info.iActType	=_ACTION_DIEFLY_FRONT;
			break;
		case 3:
			m_Info.iActType	=_ACTION_DIEFLY_RIGHT;
			nWay = (m_Info.cmdProc.nDir + 2) % 8;
			break;
		case 4:
			m_Info.iActType	=_ACTION_DIEFLY_LEFT;
			nWay = (m_Info.cmdProc.nDir + 6) % 8;
			break;
		default:
			break;
		}
		m_Info.posActionBegin.x	=m_Info.posWorld.x;
		m_Info.posActionBegin.y	=m_Info.posWorld.y;

		CMyPos posCell;
		this->GetPos(posCell);
		int nStep = 1;
		for (; nStep <= nDistance; nStep++)
		{
			if (!this->IsMapAttainable(posCell.x + _DELTA_X[nWay] * nStep, posCell.y + _DELTA_Y[nWay] * nStep))
				break;
		}
		posCell.x += _DELTA_X[nWay] * (nStep - 1);
		posCell.y += _DELTA_Y[nWay] * (nStep - 1);
		g_objGameMap.Cell2World(posCell.x, posCell.y, m_Info.posActionEnd.x, m_Info.posActionEnd.y);
		if (m_Info.iActType == _ACTION_DIEFLY && nStep <= nDistance / 2)
			m_Info.iActType = _ACTION_DIEFLY_SHORT;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_DEPART;
	}
	else if (m_Info.cmdProc.iStatus == _CMDSTATUS_DEPART)
	{
		switch (m_Info.iActType)
		{
		case _ACTION_DIEFLY:
			m_Info.iActType = _ACTION_DIEFLYEND;
			break;
		case _ACTION_DIEFLY_RIGHT:
			m_Info.iActType = _ACTION_DIEFLY_RIGHTEND;
			break;
		case _ACTION_DIEFLY_LEFT:
			m_Info.iActType = _ACTION_DIEFLY_LEFTEND;
			break;
		case _ACTION_DIEFLY_FRONT:
			m_Info.iActType = _ACTION_DIEFLY_FRONTEND;
			break;
		case _ACTION_DIEFLY_SHORT:
			m_Info.iActType = _ACTION_DIEFLY_SHORTEND;
			break;
		default:
			m_Info.iActType = _ACTION_BODY0;
			break;
		}
		m_Info.posActionBegin.x	=m_Info.posWorld.x;
		m_Info.posActionBegin.y	=m_Info.posWorld.y;
		m_Info.posActionEnd.x	=m_Info.posWorld.x;
		m_Info.posActionEnd.y	=m_Info.posWorld.y;

		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}
	else if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
	{
		m_Info.cmdProc.iStatus	=_CMDSTATUS_OVER;
	}

	return false;
}

BOOL CRole::Wound(void)
{
	m_Info.iDirection		=m_Info.cmdProc.nDir;
	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
	switch (m_Info.cmdProc.dwData)
	{
	case 2:
		m_Info.iActType	=_ACTION_BRUISE3;
		break;
	case 3:
		m_Info.iActType	=_ACTION_BRUISE2;
		break;
	case 4:
		m_Info.iActType	=_ACTION_BRUISE1;
		break;
	default:
		m_Info.iActType	=_ACTION_BRUISE0;
		break;
	}
	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	g_objGameMap.Cell2World(m_Info.posMap.x, m_Info.posMap.y, m_Info.posActionEnd.x, m_Info.posActionEnd.y);
	return true;
}

void CRole::ResetStepDir()
{
	if (true)
		return;

	this->ResetActionData();

	int nSteps	=m_lstStepDir.size();
	if (nSteps <= 0)
		return;

	CMyPos posRole;
	this->GetPos(posRole);

	for (int i=0; i<nSteps; i++)
	{
		int nDirection	=m_lstStepDir.front();
		posRole.x	+=_DELTA_X[nDirection];
		posRole.y	+=_DELTA_Y[nDirection];

		m_lstStepDir.pop_front();
	}

	this->SetPos(posRole.x, posRole.y);
}

BOOL CRole::Step(void)
{
	m_Info.posActionBegin.x	=m_Info.posWorld.x;
	m_Info.posActionBegin.y	=m_Info.posWorld.y;
	m_Info.iActType	=_ACTION_WALKL;
	g_objGameMap.Cell2World(m_Info.posMap.x+_DELTA_X[m_Info.iDirection],
		m_Info.posMap.y+_DELTA_Y[m_Info.iDirection],
		m_Info.posActionEnd.x,
		m_Info.posActionEnd.y);

	return true;
}

static BOOL CanStepFromHero(CMyPos posHero, int nDir)
{
	const int _DELTA_X[8]={0,-1,-1,-1,0,1,1,1};
	const int _DELTA_Y[8]={1,1,0,-1,-1,-1,0,1};
	BOOL bCanStep = true;
	int nAltitudeNext = 0;
	CellInfo* pInfo = g_objGameMap.GetCell(posHero.x + _DELTA_X[nDir], posHero.y + _DELTA_Y[nDir]);
	if (pInfo)
	{
		LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
		if (pLayerInfo)
		{
			nAltitudeNext = pLayerInfo->sAltitude;
			if (pLayerInfo->usMask != 0)
				bCanStep = false;
		}
		else
			bCanStep = false;
	}
	else
		bCanStep = false;
	int nAltitudeHero = nAltitudeNext;
	pInfo = g_objGameMap.GetCell(posHero.x, posHero.y);
	if (pInfo)
	{
		LayerInfo* pLayerInfo = g_objGameMap.GetLastLayerInfo(pInfo);
		if (pLayerInfo)
			nAltitudeHero = pLayerInfo->sAltitude;
		else
			bCanStep = false;
	}
	else
		bCanStep = false;
	if (nAltitudeHero - nAltitudeNext > 25)
		bCanStep = false;
	return bCanStep;
}

BOOL CRole::Walk(void)
{
	assert(m_Info.cmdProc.iType==_COMMAND_WALK);

#ifdef	_USE_ASTAR
	if (m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		m_Info.objPath.ClearStep();

		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, m_Info.cmdProc.posTarget);

		if(m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
		}
		else
		{
			m_Info.objPath.ClearStep();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

			StandBy();
			return false;
		}
	}

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
	{
		if(m_Info.objPath.GetStepAmount() > 0)
		{
			CMyPos posHero;
			g_objHero.GetPos(posHero);
			int nDir = m_Info.objPath.GetNextStep() % 8;
			m_Info.iDirection = nDir;
			if (CanStepFromHero(posHero, nDir))
				Step();
			else
			{
				StandBy();
				m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			}
		}
		else
		{
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
		}
	}

#else
	if(m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}

	if(m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
		m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		if(IsMapReached())
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			StandBy();
		}
		else
		{
			int iStepDir	=GetStepDir();
			if(iStepDir==-1)
				StandBy();
			else
			{
				m_Info.iDirection	=iStepDir;
				Step();

				if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
					m_Info.iActType	=_ACTION_WALKL;
				}
				else
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
					m_Info.iActType	=_ACTION_WALKR;
				}
			}
		}
	}

#endif

	if(m_Info.cmdProc.iStatus==_CMDSTATUS_ACCOMPLISH)
		return true;
	else
		return false;
}

BOOL CRole::WalkForward(void)
{
	assert(m_Info.cmdProc.iType == _COMMAND_WALKFORWARD);

	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

	this->SetDir(m_Info.cmdProc.nDir);
	this->Step();

	return true;
}

BOOL CRole::Run(void)
{
	assert(m_Info.cmdProc.iType==_COMMAND_RUN);

#ifdef	_USE_ASTAR
	if (m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		m_Info.objPath.ClearStep();

		g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, m_Info.cmdProc.posTarget);

		if(m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
		}
		else
		{
			m_Info.objPath.ClearStep();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

			StandBy();
			return false;
		}
	}

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
			m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		if(m_Info.objPath.GetStepAmount() > 0)
		{
			CMyPos posHero;
			g_objHero.GetPos(posHero);
			int nDir = m_Info.objPath.GetNextStep() % 8;
			m_Info.iDirection	=nDir;
			if (!CanStepFromHero(posHero, nDir))
			{
				m_Info.objPath.ClearStep();
				m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

				StandBy();
			}
			else
			{
				Step();

				if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
					m_Info.iActType	=_ACTION_RUNL;
				}
				else
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
					m_Info.iActType	=_ACTION_RUNR;
				}
			}
		}
		else
		{
			m_Info.objPath.ClearStep();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

			StandBy();
		}
	}

#else
	if(m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}

	if(m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS ||
			m_Info.cmdProc.iStatus == _CMDSTATUS_CONTINUE)
	{
		if(IsMapReached())
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			StandBy();
		}
		else
		{
			int iStepDir	=GetStepDir();
			if(iStepDir==-1)
				StandBy();
			else
			{
				m_Info.iDirection	=iStepDir;
				Step();

				if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
					if(!m_Info.bAdjustRun)
						m_Info.iActType	=_ACTION_RUNL;
					else
						m_Info.iActType	=_ACTION_WALKL;
				}
				else
				{
					m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
					if(!m_Info.bAdjustRun)
						m_Info.iActType	=_ACTION_RUNR;
					else
						m_Info.iActType	=_ACTION_WALKR;
				}
			}
		}
	}

#endif

	if(m_Info.cmdProc.iStatus==_CMDSTATUS_ACCOMPLISH)
		return true;
	else
		return false;
}

BOOL CRole::RunForward(void)
{
	assert(m_Info.cmdProc.iType == _COMMAND_RUNFORWARD);

	m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;

	this->SetDir(m_Info.cmdProc.nDir);
	this->Step();

	if (_ACTION_RUNR == m_nPreAction)
		m_Info.iActType	=_ACTION_RUNL;
	else if (_ACTION_RUNL == m_nPreAction)
		m_Info.iActType	=_ACTION_RUNR;
	else
		m_Info.iActType	=_ACTION_RUNL;

	return true;
}

BOOL CRole::Follow(void)
{
	assert(_COMMAND_FOLLOW == m_Info.cmdProc.iType);

	CRole*	pTarget	=g_objPlayerSet.GetPlayer(m_Info.cmdProc.idTarget);
	if(!pTarget)
	{
		if(g_objHero.GetID() == m_Info.cmdProc.idTarget)
			pTarget	=&g_objHero;
		else
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_ACCOMPLISH;
			StandBy();
			return false;
		}
	}

	int nTargetX=0, nTargetY=0;
	pTarget->GetPrePos(nTargetX, nTargetY);

	if(nTargetX != m_Info.cmdProc.posTarget.x ||
			nTargetY != m_Info.cmdProc.posTarget.y)
	{
		m_Info.cmdProc.iStatus		=_CMDSTATUS_BEGIN;
		m_Info.cmdProc.posTarget.x	=nTargetX;
		m_Info.cmdProc.posTarget.y	=nTargetY;
	}

#ifdef	_USE_ASTAR
	if (m_Info.cmdProc.iStatus==_CMDSTATUS_BEGIN)
	{
		try
		{
			g_objGameMap.FindPath(m_Info.objPath, m_Info.posMap, m_Info.cmdProc.posTarget);
		}
		catch(...)
		{
			m_Info.objPath.ClearStep();
		}

		BOOL bHavePath = false;
		if(m_Info.objPath.GetStepAmount() > 0)
			bHavePath = true;

		if(bHavePath)
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
		}
		else
		{
			m_Info.objPath.ClearStep();
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
			return false;
		}
	}

	if (m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
	{
		if(m_Info.objPath.GetStepAmount() > 0)
		{
			m_Info.iDirection	=m_Info.objPath.GetNextStep();
			Step();
		}
		else
		{
			StandBy();
			m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
		}
	}

#else
	if(m_Info.cmdProc.iStatus == _CMDSTATUS_BEGIN)
	{
		m_Info.cmdProc.iStatus	=_CMDSTATUS_PROGRESS;
	}

	if(m_Info.cmdProc.iStatus == _CMDSTATUS_PROGRESS)
	{
		if(m_Info.posMap.x == m_Info.cmdProc.posData.x &&
				m_Info.posMap.y == m_Info.cmdProc.posData.y)
		{
			m_Info.cmdProc.iStatus	=_CMDSTATUS_CONTINUE;
			StandBy();
		}
		else
		{
			int iStepDir	=GetStepDir();
			if(iStepDir==-1)
				StandBy();
			else
			{
				m_Info.iDirection	=iStepDir;
				Step();
			}
		}
	}

#endif

	if(m_Info.cmdProc.iStatus==_CMDSTATUS_ACCOMPLISH)
		return true;
	else
		return false;
}

void CRole::SetActionCmd(int nAction, int nDir , DWORD dwType , BOOL bData )
{
	CCommand cmd;
	cmd.iType	= _COMMAND_ACTION;
	cmd.iStatus	= _CMDSTATUS_BEGIN;
	cmd.nData	= nAction;
	cmd.nDir	= (-1 == nDir) ? this->GetDir() : nDir;
	cmd.dwData	= dwType;
	cmd.idTarget = dwType;
	cmd.bData	= bData;
	this->SetCommand(&cmd);
}

void CRole::SetIntoneCmd(int nAction, int nDir , DWORD dwType , BOOL bData )
{
	CCommand cmd;
	cmd.iType	= _COMMAND_INTONE;
	cmd.iStatus	= _CMDSTATUS_BEGIN;
	cmd.nData	= nAction;
	cmd.nDir	= (-1 == nDir) ? this->GetDir() : nDir;
	cmd.dwData	= dwType;
	cmd.idTarget = dwType;
	cmd.bData	= bData;
	this->SetCommand(&cmd);
}
