#include "AllMsg.h"
#include "3DGameMap.h"
#include "Player.h"
#include "GamePlayerSet.h"
#include "Item.h"
#include "NDsound.h"

CMsgMapItem::CMsgMapItem()
{
	Init();
	m_pInfo	=(MSG_Info *)m_bufMsg;
}

CMsgMapItem::~CMsgMapItem()
{

}

BOOL CMsgMapItem::Create(OBJID idMapItem, int nPosX, int nPosY, unsigned char ucAction)
{
	if(idMapItem == ID_NONE)
		return false;
	this->Init();

	m_pInfo->unMsgSize	=sizeof(MSG_Info);
	m_pInfo->unMsgType	=_MSG_MAPITEM;
	m_pInfo->id = idMapItem;
	m_pInfo->ucAction = ucAction;
	m_pInfo->usPosX = (unsigned short)nPosX;
	m_pInfo->usPosY = (unsigned short)nPosY;
	CCheckKeys& keys = g_objHero.m_objCheckKeys;
	m_pInfo->idType = (keys.GetKey(0)
					+ keys.GetKey(15)
					+ keys.GetKey(19)
					+ (keys.GetKey(10) + 1) * (keys.GetKey(13) + 1)
					+ (keys.GetKey(18) + 1) * keys.GetKey(3)
					+ 2 * keys.GetKey(17)
					+ keys.GetKey(7) * keys.GetKey(2)
					+ 25 * keys.GetKey(12)) ^ 0x7914A09B;

	return true;
}

BOOL CMsgMapItem::Create(char* pMsgBuf, DWORD dwSize)
{
	if (!CNetMsg::Create(pMsgBuf, dwSize))
		return false;
	if(_MSG_MAPITEM != this->GetType())
		return false;

	return true;
}

void CMsgMapItem::Send(void)
{
	if (m_pInfo->ucAction == MSGMAPITEM_PICK)
		g_objHero.m_objCheckKeys.SetKey(0x11, 5);
	CNetMsg::Send();
}

void CMsgMapItem::Process(void* pInfo)
{
#ifdef _MSGDEBUG
	::LogSave("Process CMsgMapItem);
#endif

	switch(m_pInfo->ucAction)
	{
		case MSGMAPITEM_CREATE:
			{
				CMyPos posMapItem;
				posMapItem.x = m_pInfo->usPosX;
				posMapItem.y = m_pInfo->usPosY;
				g_objGameMap.AddMapItem(m_pInfo->id, m_pInfo->idType, posMapItem );
				if (g_objGameMap.IsPosVisible(posMapItem, 2))
				{
					char szSound[256] = "sound/DropItem.wav";
					OBJID idType = m_pInfo->idType;
					if (idType >= ITEMTYPEMONEY_MIN && idType < ITEMTYPEMONEY_MAX)
						strcpy(szSound, "sound/dropMed.wav");
					else if (idType == ITEMTYPEMONEY_MAX)
						strcpy(szSound, "sound/dropMoney.wav");
					else if (idType == 1088000 || idType == 1088001 || idType == 1088002)
						strcpy(szSound, "sound/bell.wav");
					else
					{
						int nSort = CItem::ItemGetSort(idType);
						if (nSort == ITEMSORT_EXPEND)
						{
							int nSubType = CItem::ItemGetSubType(idType);
							if (nSubType == EXPEND_MEDICINE || nSubType == EXPEND_POISON)
								strcpy(szSound, "sound/DropMedicine.wav");
							else if (nSubType == EXPEND_DART || nSubType == EXPEND_ARROW || nSubType == EXPEND_SPELL)
								strcpy(szSound, "sound/DropItem.wav");
							else if (nSubType == EXPEND_BOOK)
								strcpy(szSound, "sound/DropBook.wav");
						}
						else if (nSort > 0)
						{
							if (nSort <= 3)
								strcpy(szSound, "sound/dropequip1.wav");
							else if (nSort <= 5)
								strcpy(szSound, "sound/dropequip.wav");
							else if (nSort <= 8)
								strcpy(szSound, "sound/dropequip1.wav");
						}
					}
					CMyPos posWorld;
					g_objGameMap.Cell2World(posMapItem.x, posMapItem.y, posWorld.x, posWorld.y);
					::DXPlaySound(szSound, posWorld.x, posWorld.y, 2000);
				}
			}
			break;
		case MSGMAPITEM_DELETE:
			{
				g_objGameMap.DelMapItem(m_pInfo->id);
			}
			break;
		case MSGMAPITEM_PICK:
			{
				CPlayer* pPlayer = g_objPlayerSet.GetPlayer(m_pInfo->id);
				if (pPlayer)
				{
					pPlayer->ResetStepDir();
					pPlayer->SetActionCmd(_ACTION_PICKUP);
					pPlayer->SetPos(m_pInfo->usPosX, m_pInfo->usPosY);
				}
			}
			break;
		case MSGMAPITEM_CAST_TRAP:
			{
				CMyPos pos = {m_pInfo->usPosX, m_pInfo->usPosY};
				g_objGameMap.Add3DMapMagicItem(m_pInfo->idType, m_pInfo->id, pos);
			}
			break;
		case MSGMAPITEM_SYNCHRO_TRAP:
			{
				CMyPos pos = {m_pInfo->usPosX, m_pInfo->usPosY};
				g_objGameMap.Add3DMapMagicItem(m_pInfo->idType, m_pInfo->id, pos);
				g_objGameMap.LastMapMagicItem(m_pInfo->id);
			}
			break;
		case MSGMAPITEM_DROP_TRAP:
			{
				g_objGameMap.EndMapMagicItem(m_pInfo->id);
			}
			break;
		case MSGMAPITEM_SCREENEFFECT:
			{
				if (m_pInfo->id & 1)
					g_objGameMap.BeginShakeEffect(CGameMap::_SCREEN_EFFECT_NORMAL);
				if (m_pInfo->id & 2)
					g_objGameMap.BeginScaleEffect(CGameMap::_SCREEN_EFFECT_NORMAL);
				if (m_pInfo->id & 4)
					g_objGameMap.BeginColorEffect(CGameMap::_SCREEN_EFFECT_NORMAL);
			}
			break;
		default:
			break;
	}
}
