#include "AllMsg.h"
#include "3DGameMap.h"
#include "GamePlayerSet.h"
#include "Hero.h"
#include "GameDataSet.h"
#include "GameMsg.h"
#include "MagicData.h"
#include "NDsound.h"
#include "Pet.h"
#include "FloorLayer.h"
#include "3DRoleData.h"

CMsgInteract::CMsgInteract()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgInteract::~CMsgInteract()
{

}

BOOL CMsgInteract::Create(USHORT usType, OBJID idSender, OBJID idTarget, USHORT usPosX, USHORT usPosY, USHORT usData0, USHORT usData1, OBJID idCopy)
{
	if (idSender == ID_NONE)
		return false;

	m_pInfo->unMsgSize		=sizeof(MSG_Info);
	m_pInfo->unMsgType		=_MSG_INTERACT;

	m_pInfo->dwTimeStamp	=::TimeGet();
	m_pInfo->usData0		=usData0;
	m_pInfo->usData1		=(usData1+0x100*(m_pInfo->dwTimeStamp%0x100))^0x3721;
	m_pInfo->idSender		=idSender;
	m_pInfo->idTarget		=idTarget;
	m_pInfo->unPosX			=usPosX;
	m_pInfo->unPosY			=usPosY;
	m_pInfo->unType			=usType;

	switch(usData0)
	{
	case 1000:
	case 1001:
	case 1002:
	case 1030:
	case 1125:
	case 1150:
	case 1165:
	case 1160:
		{
			m_pInfo->usData1 ^= 0x6279;
		}
		break;
	}

	return true;
}

BOOL CMsgInteract::Create(USHORT usType, OBJID idSender, OBJID idTarget, OBJID idBullet)
{
	if (idSender == ID_NONE || idTarget == ID_NONE)
		return false;

	m_pInfo->unMsgSize		=sizeof(MSG_Info);
	m_pInfo->unMsgType		=_MSG_INTERACT;

	m_pInfo->dwTimeStamp	=::TimeGet();
	m_pInfo->idSender		=idSender;
	m_pInfo->idTarget		=idTarget;
	m_pInfo->unType			=usType;
	m_pInfo->idBullet		=idBullet;

	return true;
}

BOOL CMsgInteract::Create(USHORT usType, OBJID idSender, OBJID idTarget, USHORT usPosX, USHORT usPosY, DWORD dwData)
{
	if (idSender == ID_NONE || idTarget == ID_NONE)
		return false;

	m_pInfo->unMsgSize		=sizeof(MSG_Info);
	m_pInfo->unMsgType		=_MSG_INTERACT;

	m_pInfo->dwTimeStamp	=::TimeGet();
	m_pInfo->idSender		=idSender;
	m_pInfo->idTarget		=idTarget;
	m_pInfo->unPosX			=usPosX;
	m_pInfo->unPosY			=usPosY;
	m_pInfo->unType			=usType;
	m_pInfo->dwData			=dwData;

	return true;
}

BOOL CMsgInteract::Create(char* pbufMsg, DWORD dwMsgSize)
{
	if (!CNetMsg::Create(pbufMsg, dwMsgSize))
		return false;
	if(_MSG_INTERACT != this->GetType())
		return false;

	return true;
}

void CMsgInteract::Send(void)
{
	if (m_pInfo->unType == INTERACT_ATTACK)
		g_objHero.m_objCheckKeys.SetKey(0x12, 0x0B);
	else if (m_pInfo->unType == INTERACT_MAGICATTACK)
		g_objHero.m_objCheckKeys.SetKey(0x13, 3);
	CNetMsg::Send();
}

void CMsgInteract::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process MsgSteal, idStealer:%u, idTarget:%u, CurPos:(%d,%d)",
						m_pInfo->idSender,
						m_pInfo->idTarget,
						m_pInfo->unPosX,
						m_pInfo->unPosY);
#endif
	BOOL bIncludeHero = true;
	CPlayer* pSender = g_objPlayerSet.GetPlayer(m_pInfo->idSender, bIncludeHero);
	if (!pSender)
	{
		CMyPos posHero = g_objHero.GetAlignPos();
		{
			CMsgAction msg;
			if (msg.Create(g_objHero.GetID(), posHero.x, posHero.y, g_objHero.GetDir(), actionQueryPlayer, m_pInfo->idSender))
				msg.Send();
		}
	}
	CPlayer* pTarget	=g_objPlayerSet.GetPlayer(m_pInfo->idTarget, bIncludeHero);
	int nType = g_objPlayerSet.GetRestrainMultiple(pSender, pTarget);
	switch(m_pInfo->unType)
	{
	case INTERACT_COURT:
		{
			if (g_objPlayerSet.GetPlayer(m_pInfo->idSender))
				::PostCmd(CMD_BECOURT, m_pInfo->idSender);
		}
		break;
	case INTERACT_BUMP:
		{
			if(pSender && pTarget )
			{
				if ( pTarget->GetID() == g_objHero.GetID())
					g_objHero.SetIDWhoAttackMe(pSender->GetID());
				if (pSender->GetID() == g_objHero.GetID())
					g_objHero.SetIDMyTarget(pTarget->GetID());
				if (pTarget->IsMyPet())
					((CPet*)pTarget)->SetHeroTarget(pSender->GetID());
			}
			int nForward	= m_pInfo->dwData/0x10000000;
			int	nDir		= (m_pInfo->dwData/0x01000000)%0x10;
			CMyPos posTarget = { m_pInfo->unPosX, m_pInfo->unPosY };
			if (pSender)
			{
				if (pSender->GetID() != g_objHero.GetID())
				{
					CMyPos posSender;
					pSender->GetPos(posSender);
					if (nDir >= _MAX_DIRECTION)
					{
						pSender->SetPos(posTarget.x, posTarget.y);
					}
					else
					{
						DWORD dwType = 1;
						if (nForward > 0)
							dwType = 0;
						pSender->SetActionCmd(_ACTION_BUMP, nDir, dwType);
						if (posSender.x != posTarget.x-_DELTA_X[nDir]
								|| posSender.y != posTarget.y-_DELTA_Y[nDir])
						{
							pSender->SetPos(posTarget.x-_DELTA_X[nDir], posTarget.y-_DELTA_Y[nDir]);
						}
					}
				}
				else
				{
					g_objHero.SetAlignPos(posTarget.x, posTarget.y);
				}
			}
			if (pTarget)
			{
				DWORD dwDamage	= m_pInfo->dwData&0x00ffffff;
				if (pTarget->GetID() == g_objHero.GetID())
				{
					if (dwDamage > 0)
					{
						if (nDir < _MAX_DIRECTION)
						{
							CMyPos posHero;
							g_objHero.GetPos(posHero);
							if (posHero.x == posTarget.y
									|| posHero.y == posTarget.y)
							{
								if (!g_objHero.IsMoving())
									g_objHero.ResetActionData();
								g_objHero.SetActionCmd(_ACTION_POPOFF, nDir, 1);
								g_objHero.SetAlignPos(posHero.x+_DELTA_X[nDir], posHero.y+_DELTA_Y[nDir]);
							}
							else
							{
								CMsgAction msg;
								IF_SUC (msg.Create(g_objHero.GetID(), 0, 0, g_objHero.GetDir(), actionSynchro, posHero.x, posHero.y))
									msg.Send();
							}
						}
						else
						{
							if (!g_objHero.IsMoving())
							{
								g_objHero.ResetActionData();
								g_objHero.SetActionCmd(_ACTION_BRUISE0, g_objHero.GetDir(pSender));
							}
						}
						g_objHero.AddZoomNum(dwDamage, nType, _COLOR_YELLOW, 300, 30);
					}
				}
				else
				{
					if (dwDamage > 0)
					{
						pTarget->ResetStepDir();
						if (nDir < _MAX_DIRECTION)
							pTarget->SetActionCmd(_ACTION_POPOFF, nDir, 1);
						else
							pTarget->SetActionCmd(_ACTION_BRUISE0, pTarget->GetDir(pSender));
						pTarget->AddZoomNum(dwDamage, nType, _COLOR_WHITE, 300, 30);
						if (pTarget->IsNpc())
						{
							int nLife = (int)pTarget->GetData(CPlayer::_PLAYER_LIFE)-(int)dwDamage;
							pTarget->SetData(CPlayer::_PLAYER_LIFE, __max(0, nLife));
						}
					}
				}
			}
		}
		break;
	case INTERACT_ATTACK:
		{
			if (pSender)
			{
				if (pTarget)
				{
					if ( pTarget->GetID() == g_objHero.GetID())
						g_objHero.SetIDWhoAttackMe(pSender->GetID());
					if (pSender->GetID() == g_objHero.GetID())
						g_objHero.SetIDMyTarget(pTarget->GetID());
					if (pTarget->IsMyPet())
						((CPet*)pTarget)->SetHeroTarget(pSender->GetID());
				}
				if (g_objHero.GetID() == pSender->GetID())
				{
					if (g_objHero.m_bFirstAtk)
					{
						g_objHero.m_bFirstAtk = false;
					}
					if (!g_objHero.IsAttacking())
					{
						int nDir	= g_objHero.GetDir(pTarget);
						g_objHero.SetActionCmd(_ACTION_ATTACK0, nDir, m_pInfo->idTarget);
					}
				}
				else
				{
					CCommand cmd;
					cmd.iType	= _COMMAND_ATTACK;
					cmd.iStatus	= _CMDSTATUS_BEGIN;
					cmd.nDir	= pSender->GetDir(pTarget);
					pSender->SetCommand(&cmd);
				}
			}
			if (pTarget)
			{
				int nDir = pTarget->GetDir();
				if(pSender)
					nDir = pTarget->GetDir(pSender);
				if (pTarget->GetID() == g_objHero.GetID())
				{
					if (m_pInfo->dwData > 0)
					{
						g_objHero.AddZoomNum(m_pInfo->dwData, nType, _COLOR_YELLOW, 300, 30);
						if (!g_objHero.IsMoving()
								&& !g_objHero.IsAttacking())
							g_objHero.SetActionCmd(_ACTION_BRUISE0, nDir);
					}
					else
					{
						if (!g_objHero.IsMoving()
								&& !g_objHero.IsAttacking())
							g_objHero.SetActionCmd(_ACTION_DODGE0, nDir);
					}
				}
				else
				{
					if (m_pInfo->dwData > 0)
					{
						pTarget->AddZoomNum(m_pInfo->dwData, nType, _COLOR_WHITE, 300, 30);

						CCommand cmd;
						cmd.iType	= _COMMAND_WOUND;
						cmd.iStatus	= _CMDSTATUS_BEGIN;
						cmd.nDir	= pTarget->GetDir();
						cmd.nData	= pSender ? pSender->GetHitDir(pTarget) : -1;

						CONST CActionDelayInfo* pInfoDelay = NULL;
						CONST CWeaponInfo* pInfoWeapon = NULL;
						if (pSender)
						{
							DWORD dwKey = pSender->GetActionDelayInfoKey(pTarget);
							pInfoDelay = RoleDataQuery()->GetActionDelayInfo(dwKey);

							dwKey = pSender->GetWeaponInfoKey(pTarget);
							pInfoWeapon = RoleDataQuery()->GetWeaponInfo(dwKey);
						}

						DWORD tmDelay = 200;
						if (pInfoDelay)
							tmDelay = pInfoDelay->tmWoundDelay;

						CONST CHAR* pszEffect	= NULL;
						CONST CHAR* pszSound	= NULL;
						if (pInfoWeapon)
						{
							pszEffect	= pInfoWeapon->strHitEffect.c_str();
							pszSound	= pInfoWeapon->strHitSound.c_str();
						}

						pTarget->SetTimerCommand(cmd, ::TimeGet() + tmDelay, pszEffect, pszSound);

						if (pTarget->IsNpc())
						{
							int nLife = (int)pTarget->GetData(CPlayer::_PLAYER_LIFE)-(int)m_pInfo->dwData;
							pTarget->SetData(CPlayer::_PLAYER_LIFE, __max(0, nLife));
						}
						pTarget->SetLastDamage(m_pInfo->dwData);
					}
					else
					{
						CCommand cmd;
						cmd.iType	= _COMMAND_ACTION;
						cmd.iStatus	= _CMDSTATUS_BEGIN;
						cmd.nData	= _ACTION_DODGE0;
						cmd.nDir	= nDir;
						cmd.dwData	= 0;
						cmd.bData	= false;

						CONST CActionDelayInfo* pInfoDelay = NULL;
						CONST CWeaponInfo* pInfoWeapon = NULL;
						if (pSender)
						{
							DWORD dwKey = pSender->GetActionDelayInfoKey(pTarget);
							pInfoDelay = RoleDataQuery()->GetActionDelayInfo(dwKey);

							dwKey = pSender->GetWeaponInfoKey(pTarget);
							pInfoWeapon = RoleDataQuery()->GetWeaponInfo(dwKey);
						}

						DWORD tmDelay = 200;
						if (pInfoDelay)
							tmDelay = pInfoDelay->tmBlockDelay;

						CONST CHAR* pszEffect	= NULL;
						CONST CHAR* pszSound	= NULL;
						if (pInfoWeapon)
						{
							pszEffect	= pInfoWeapon->strBlkEffect.c_str();
							pszSound	= pInfoWeapon->strBlkSound.c_str();
						}

						pTarget->SetTimerCommand(cmd, ::TimeGet() + tmDelay, pszEffect, pszSound);
					}
				}
			}
		}
		break;
	case INTERACT_RUSHATK:
		{
			if (pSender)
			{
			}
		}
		break;
	case INTERACT_KILL:
		{
			if (pTarget)
			{
				pTarget->SetData(CPlayer::_PLAYER_LIFE, 0);
				pTarget->ResetStepDir();
				if (g_objHero.GetID() == m_pInfo->idTarget)
				{
					pTarget->SetPos(m_pInfo->unPosX, m_pInfo->unPosY);
					::PostCmd(CMD_DIE);
				}
				if (!pTarget->IsNpc())
				{
					CCommand cmd;
					cmd.iType	= _COMMAND_DIE;
					cmd.iStatus	= _CMDSTATUS_BEGIN;
					cmd.nDir		=pTarget->GetDir();
					cmd.nData		=pSender ? pSender->GetHitDir(pTarget) : -1;
					cmd.dwData		=1;
					pTarget->SetCommand(&cmd);
				}
				else
				{
					CONST CActionDelayInfo* pInfoDelay = NULL;
					CONST CWeaponInfo* pInfoWeapon = NULL;
					if (pSender)
					{
						DWORD dwKey = pSender->GetActionDelayInfoKey(pTarget);
						pInfoDelay = RoleDataQuery()->GetActionDelayInfo(dwKey);

						dwKey = pSender->GetWeaponInfoKey(pTarget);
						pInfoWeapon = RoleDataQuery()->GetWeaponInfo(dwKey);
					}

					DWORD tmDelay = 200;
					if (pInfoDelay)
						tmDelay = pInfoDelay->tmDieDelay;

					CONST CHAR* pszEffect	= NULL;
					CONST CHAR* pszSound	= NULL;
					if (pInfoWeapon)
					{
						pszEffect	= pInfoWeapon->strBlkEffect.c_str();
						pszSound	= pInfoWeapon->strBlkSound.c_str();
					}

					pTarget->ResetActionData();
					pTarget->ClrTimerCommand();

					int nPercent = pTarget->GetLastDamage()*100/__max(1, pTarget->GetMaxLife());
					if (nPercent < 30)
						nType = 1;
					else if (nPercent < 70)
						nType = ::RandGet(100) < 30 ? 2 : 1;
					else if (nPercent < 100)
						nType = ::RandGet(100) < 50 ? 2 : 1;
					else if (nPercent < 200)
						nType = 2;
					else
						nType = nPercent >= 400 ? 4 : 3;
					if (m_pInfo->usData0 == 2)
						nType = 2;
					if (!pTarget->IsMonster())
						nType = 1;
					if (pSender)
						pSender->GetHitDir(pTarget);
					if (pTarget->IsMonster())
					{
						CONST CMonsterInfo* pInfo = RoleDataQuery()->GetMonsterInfo(pTarget->GetName());
						if (pInfo && (pInfo->nActResCtrl & 1))
							nType = 1;
					}

					CCommand cmd;
					cmd.iType	= _COMMAND_DIE;
					cmd.iStatus	= _CMDSTATUS_BEGIN;
					cmd.nDir	=pTarget->GetDir();
					cmd.nData	=pSender ? pSender->GetHitDir(pTarget) : -1;
					cmd.dwData	=nType;

					pTarget->SetTimerCommand(cmd, ::TimeGet() + tmDelay, pszEffect, pszSound);
				}
				if (pSender && !pTarget->IsNpc())
				{
					const OBJID idMsgDie = 100123;
					g_objGameMsg.AddMsg(_TXTATR_TALK, GameDataSetQuery()->GetStr(idMsgDie), pTarget->GetName(), pSender->GetName());
				}
			}
			if (pSender)
			{
				if (pSender->GetID() == g_objHero.GetID() && pTarget && pTarget->IsMonster())
				{
					g_objHero.AddKoCount();
				}
				pSender->SetKoAmount(m_pInfo->usData1);
				if (!pSender->IsNpc())
				{
					CMagicEffect* pEffect =
						CMagicEffect::CreateNew(m_pInfo->idTarget, m_pInfo->idSender,
						0, 0, MAGICEFFECT_SOUL);
					if(pEffect)
					{
						g_objGameMap.AddMagicEffect(pEffect);
					}
				}
			}
		}
		break;
	case INTERACT_SHOOT:
		{
			if (!pSender)
				break;
			if (pTarget)
			{
				if ( pTarget->GetID() == g_objHero.GetID())
					g_objHero.SetIDWhoAttackMe(pSender->GetID());
				if (pSender->GetID() == g_objHero.GetID())
					g_objHero.SetIDMyTarget(pTarget->GetID());
				if (pTarget->IsMyPet())
					((CPet*)pTarget)->SetHeroTarget(pSender->GetID());
			}
			if (g_objHero.GetID() == pSender->GetID())
			{
				if (g_objHero.m_bFirstAtk)
				{
					g_objHero.m_bFirstAtk = false;
				}
				else
				{
					int nDir	= g_objHero.GetDir(pTarget);
					g_objHero.SetActionCmd(_ACTION_ATTACK0, nDir);
					CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(pSender->GetID(), pTarget->GetID(), pSender->GetLWeapon(), 0);
					if (pEffect)
					{
						pEffect->AddRoleInfo(m_pInfo->idTarget, m_pInfo->dwData);
						g_objGameMap.AddMagicEffect(pEffect);
					}
				}
				if (pTarget)
				{
					if (m_pInfo->dwData > 0)
					{
						pTarget->SetActionCmd(_ACTION_BRUISE0, pTarget->GetDir(pSender));
						DWORD dwColor = _COLOR_WHITE;
						if (g_objHero.GetID() == pTarget->GetID())
							dwColor = _COLOR_YELLOW;
						pTarget->AddZoomNum(m_pInfo->dwData, nType, dwColor, 300, 30);
						if (pTarget->IsNpc())
						{
							int nLife = (int)pTarget->GetData(CPlayer::_PLAYER_LIFE)-(int)m_pInfo->dwData;
							pTarget->SetData(CPlayer::_PLAYER_LIFE, __max(0, nLife));
						}
					}
					else
					{
						pTarget->SetActionCmd(_ACTION_DODGE0, pTarget->GetDir(pSender));
					}
				}
			}
			else
			{
				CCommand cmd;
				cmd.iType	= _COMMAND_ATTACK;
				cmd.iStatus	= _CMDSTATUS_BEGIN;
				cmd.nDir	= pSender->GetDir(pTarget);
				pSender->SetCommand(&cmd);
				if (pTarget)
				{
					CMagicEffect* pEffect = CMagicEffect::CreateNewArrow(pSender->GetID(), pTarget->GetID(), pSender->GetLWeapon(), 0);
					if (pEffect)
					{
						pEffect->AddRoleInfo(m_pInfo->idTarget, m_pInfo->dwData);
						g_objGameMap.AddMagicEffect(pEffect);
					}
				}
			}
		}
		break;
	case INTERACT_SENDFLOWERS:
		{
			CMyPos posWorld;
			g_objGameMap.Cell2World(m_pInfo->unPosX, m_pInfo->unPosY, posWorld.x, posWorld.y);
			if (g_objGameMap.GetFloorLayer())
				g_objGameMap.GetFloorLayer()->CreateRose(posWorld.x, posWorld.y, m_pInfo->unType, m_pInfo->dwData);
		}
		break;
	case INTERACT_REFLECTWEAPON:
	case INTERACT_REFLECTMAGIC:
		{
			if (pTarget)
			{
				if (pTarget->m_objEffect)
				{
					if (m_pInfo->unType == INTERACT_REFLECTWEAPON)
						pTarget->m_objEffect->Add("WeaponReflect");
					else
						pTarget->m_objEffect->Add("MagicReflect");
				}
				if (m_pInfo->dwData > 0)
				{
					DWORD dwColor = _COLOR_WHITE;
					if (pTarget->GetID() == g_objHero.GetID())
						dwColor = _COLOR_YELLOW;
					pTarget->AddZoomNum(m_pInfo->dwData, nType, dwColor, 300, 30);
					if (pTarget->IsNpc())
					{
						int nLife = (int)pTarget->GetData(CPlayer::_PLAYER_LIFE)-(int)m_pInfo->dwData;
						pTarget->SetData(CPlayer::_PLAYER_LIFE, __max(0, nLife));
					}
				}
			}
		}
		break;
	default:
		break;
	}
}
