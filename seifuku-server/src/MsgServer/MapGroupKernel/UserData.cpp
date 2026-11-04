#include "UserData.h"
#include "I_mydb.h"
#include "UserManager.h"
#include "string.h"

char	szUserTable[] = _TBL_USER;

CUserData::CUserData()
{
	memset(&m_Info, 0, sizeof(UserInfoStruct));
	m_pRes	= NULL;
}

CUserData::~CUserData()
{
	if(m_pRes)
		m_pRes->Release();
}

bool CUserData::Create(OBJID idUser, IDatabase* pDb)
{
	CHECKF(!m_pRes);
	CHECKF(pDb);

	SQLBUF	szSQL;
	sprintf(szSQL, "SELECT * FROM cq_user WHERE id=%u LIMIT 1", idUser);
	IRecordset* pRes = pDb->CreateNewRecordset(szSQL);
	if(pRes)
	{
		m_pRes = pRes->CreateNewRecord();
		pRes->Release();

		return LoadInfo();
	}
	return false;
}

DWORD CUserData::CalcChkSum()
{
	DWORD	setKey[3] = { 0x1C32094, 0x2F93C2, 0x9234D5 };
	return (setKey[m_Info.dwEmoney % 3] + m_Info.idAccount) ^ m_Info.dwEmoney;
}

bool CUserData::IsChkSumOk()
{
	if (m_Info.dwEmoney > 0 && m_Info.dwChkSum != CalcChkSum())
		return false;
	return true;
}

bool CUserData::LoadInfo(void)
{
	if (!m_pRes)
		return false;

	if (m_Info.id != ID_NONE)
		return true;

	m_Info.sPk				= m_pRes->LoadInt("pk");
	m_Info.dwCoin_money		= m_pRes->LoadDWord("coin_money");
	m_Info.dwLast_login		= m_pRes->LoadDWord("last_login");
	m_Info.dwLookface		= m_pRes->LoadDWord("lookface");
	m_Info.dwHair			= m_pRes->LoadDWord("hair");
	m_Info.dwMedal			= m_pRes->LoadDWord("medal");
	m_Info.dwMedal_select	= m_pRes->LoadDWord("medal_select");
	m_Info.dwMoney			= m_pRes->LoadDWord("money");
	m_Info.dwMoney_saved	= m_pRes->LoadDWord("money_saved");
	m_Info.usRecordX		= m_pRes->LoadDWord("recordx");
	m_Info.usRecordY		= m_pRes->LoadDWord("recordy");
	m_Info.id				= m_pRes->LoadDWord("id");
	m_Info.idAccount		= m_pRes->LoadDWord("account_id");
	m_Info.idRecordmap		= m_pRes->LoadDWord("recordmap_id");
	m_Info.idSyndicate		= m_pRes->LoadDWord("syndicate_id");
	m_Info.nDeed			= m_pRes->LoadInt("deed");
	m_Info.i64Exp			= m_pRes->LoadInt64("exp");
	m_Info.ucLevel			= m_pRes->LoadInt("level");
	m_Info.dwTaskMask		= m_pRes->LoadInt("task_mask");
	m_Info.dwVirtue			= m_pRes->LoadInt("virtue");
	m_Info.idHome			= m_pRes->LoadInt("home_id");
	m_Info.dwLockKey		= m_pRes->LoadInt("lock_key");
	m_Info.dwCurrLockKey	= m_pRes->LoadInt("lock_key");

	m_pRes->LoadString(m_Info.szMate, "mate", _MAX_NAMESIZE);
	m_pRes->LoadString(m_Info.szName,		"name", _MAX_NAMESIZE);

	m_Info.usAdditional_point	= m_pRes->LoadDWord("additional_point");
	m_Info.ucAuto_allot			= m_pRes->LoadUInt("auto_allot");
	m_Info.usForce				= m_pRes->LoadDWord("strength");
	m_Info.usHealth				= m_pRes->LoadDWord("health");
	m_Info.usLife				= m_pRes->LoadDWord("life");
	m_Info.usMana				= m_pRes->LoadDWord("mana");
	m_Info.ucMetempsychosis		= m_pRes->LoadDWord("metempsychosis");
	m_Info.ucNobility			= m_pRes->LoadDWord("nobility");
	m_Info.ucProfession			= m_pRes->LoadDWord("profession");
	m_Info.ucOldProf			= m_pRes->LoadDWord("old_prof");
	m_Info.usSoul				= m_pRes->LoadDWord("soul");
	m_Info.usSpeed				= m_pRes->LoadDWord("speed");

	m_Info.dwEmoney				= m_pRes->LoadInt("emoney");
	m_Info.dwChkSum				= m_pRes->LoadInt("chk_sum");
	m_Info.dwGodStatus			= m_pRes->LoadInt("god_status");
	m_Info.dwExpBallUsage		= m_pRes->LoadInt("exp_ball_usage");
	m_Info.dwOnlineTime			= m_pRes->LoadInt("online_time");
	m_Info.dwAutoExercise		= m_pRes->LoadInt("auto_exercise");
	m_Info.dwLastLogout			= m_pRes->LoadDWord("last_logout");
	m_Info.usBigPrizeFails		= m_pRes->LoadInt("big_prize_fails");
	m_Info.usSmallPrizeFails	= m_pRes->LoadInt("small_prize_fails");
	m_Info.dwMeteLev			= m_pRes->LoadInt("mete_lev");
	m_Info.dwCurrentLayoutType	= m_pRes->LoadInt("current_layout_type");
	m_Info.dwOnlineTime2		= m_pRes->LoadInt("online_time2");
	m_Info.dwOfflineTime		= m_pRes->LoadInt("offline_time");
	m_Info.dwLastLogout2		= m_pRes->LoadInt("last_logout2");
	m_Info.i64Donation			= m_pRes->LoadInt64("donation");
	m_Info.dwBusiness			= m_pRes->LoadInt("business");

	if (!IsChkSumOk())
	{
		::MyLogSave("gmlog/emoney", "user[%s][%u] emoney chksum error!", m_Info.szName, m_Info.id);
		return false;
	}

	return true;
}

bool CUserData::SaveInfo(void)
{
	if (!m_pRes)
		return false;

	m_pRes->UpdateRecord();

	return true;
}

char* CUserData::GetMate()
{
	return m_Info.szMate;
}

void CUserData::SetMate(LPCTSTR szMate, BOOL bUpdate)
{
	SafeCopy(m_Info.szMate, szMate, _MAX_NAMESIZE);
	if (m_pRes)
	{
		m_pRes->SetString("mate", szMate, _MAX_NAMESIZE);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLife(int nLife, BOOL bUpdate)
{
	m_Info.usLife	= nLife;
	if (m_pRes)
	{
		m_pRes->SetDWord("life", nLife);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetMana(int nPower, BOOL bUpdate)
{
	m_Info.usMana	= nPower;
	if (m_pRes)
	{
		m_pRes->SetDWord("mana", nPower);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetMoney(int nMoney, BOOL bUpdate)
{
	m_Info.dwMoney	= nMoney;
	if (m_pRes)
	{
		m_pRes->SetDWord("money", nMoney);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetMoneySaved(int nMoney, BOOL bUpdate)
{
	m_Info.dwMoney_saved	= nMoney;
	if (m_pRes)
	{
		m_pRes->SetDWord("money_saved", nMoney);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetEmoney(DWORD dwEmoney)
{
	m_Info.dwEmoney	= dwEmoney;
	m_Info.dwChkSum	= CalcChkSum();
	if (m_pRes)
	{
		m_pRes->SetUInt("emoney", m_Info.dwEmoney);
		m_pRes->SetUInt("chk_sum", m_Info.dwChkSum);
		m_pRes->UpdateRecord();
	}
}

void CUserData::SetExp(__int64 i64Exp, BOOL bUpdate)
{
	m_Info.i64Exp	= i64Exp;
	if (m_pRes)
	{
		m_pRes->SetInt64("exp", i64Exp);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetPk(int nPk, BOOL bUpdate)
{
	m_Info.sPk	= nPk;
	if (m_pRes)
	{
		m_pRes->SetDWord("pk", nPk);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetRecordPos(OBJID idMap, DWORD dwX, DWORD dwY, BOOL bUpdate)
{
	m_Info.idRecordmap	= idMap;
	m_Info.usRecordX	= dwX;
	m_Info.usRecordY	= dwY;
	if (m_pRes)
	{
		m_pRes->SetDWord("recordmap_id", idMap);
		m_pRes->SetDWord("recordx", dwX);
		m_pRes->SetDWord("recordy", dwY);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLastLogin()
{
	m_Info.dwLast_login	= ::DateStamp();
	if (m_pRes)
	{
		m_pRes->SetDWord("last_login", m_Info.dwLast_login);
	}
}

#ifdef _DEBUG
void CUserData::SetName(const char* pszName)
{
	if (!pszName || strlen(pszName) >= _MAX_NAMESIZE)
		return;

	strcpy(m_Info.szName, pszName);
	if (m_pRes)
	{
		m_pRes->SetString("name", m_Info.szName, _MAX_NAMESIZE);
	}
}
#endif

int CUserData::GetLev()
{
	return m_Info.ucLevel;
}

int CUserData::GetAddPoint()
{
	return m_Info.usAdditional_point;
}

void CUserData::SetMedalSelect	(int nData, BOOL fUpdate)
{
	m_Info.dwMedal_select	= nData;
	if (m_pRes)
	{
		m_pRes->SetUInt("medal_select", nData);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

int CUserData::GetAutoAllot()
{
	return m_Info.ucAuto_allot;
}

void CUserData::SetAutoAllot(int nAutoAllot, BOOL fUpdate)
{
	m_Info.ucAuto_allot	= nAutoAllot;
	if (m_pRes)
	{
		m_pRes->SetUInt("auto_allot", nAutoAllot);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetMetempsychosis(int nMetempsychosis, BOOL fUpdate)
{
	m_Info.ucMetempsychosis	= nMetempsychosis;
	if (m_pRes)
	{
		m_pRes->SetUInt("metempsychosis", nMetempsychosis);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLev(int nLev, BOOL bUpdate)
{
	m_Info.ucLevel	= nLev;
	if (m_pRes)
	{
		m_pRes->SetUInt("level", nLev);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetForce(int nForce , BOOL bUpdate)
{
	m_Info.usForce	= nForce;
	if (m_pRes)
	{
		m_pRes->SetUInt("strength", nForce);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetAddPoint(int nAddPoint, BOOL bUpdate)
{
	m_Info.usAdditional_point	= nAddPoint;
	if (m_pRes)
	{
		m_pRes->SetUInt("additional_point", nAddPoint);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetSpeed(DWORD dwSpeed, BOOL bUpdate)
{
	m_Info.usSpeed	= dwSpeed;
	if (m_pRes)
	{
		m_pRes->SetUInt("speed", dwSpeed);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetHealth(DWORD dwHealth, BOOL fUpdate)
{
	m_Info.usHealth	= dwHealth;
	if (m_pRes)
	{
		m_pRes->SetUInt("health", dwHealth);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetSoul(DWORD dwSoul, BOOL fUpdate)
{
	m_Info.usSoul	= dwSoul;
	if (m_pRes)
	{
		m_pRes->SetUInt("soul", dwSoul);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLookFace(DWORD dwLookFace, BOOL fUpdate)
{
	m_Info.dwLookface	= dwLookFace;
	if (m_pRes)
	{
		m_pRes->SetUInt("lookface", dwLookFace);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetHair(DWORD dwHair, BOOL fUpdate)
{
	m_Info.dwHair	= dwHair;
	if (m_pRes)
	{
		m_pRes->SetUInt("hair", dwHair);
		if (fUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetOldProf(DWORD dwOldProf, BOOL bUpdate)
{
	m_Info.ucOldProf	= dwOldProf;
	if (m_pRes)
	{
		m_pRes->SetUInt("old_prof", dwOldProf);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetProfession (DWORD dwProfession, BOOL bUpdate)
{
	m_Info.ucProfession	= dwProfession;
	if (m_pRes)
	{
		m_pRes->SetUInt("profession", dwProfession);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetTaskMask (DWORD dwTask)
{
	m_Info.dwTaskMask	= dwTask;
	if (m_pRes)
	{
		m_pRes->SetUInt("task_mask", dwTask);
		m_pRes->UpdateRecord();
	}
}

void CUserData::SetVirtue (DWORD dwVirtue, BOOL bUpdate)
{
	m_Info.dwVirtue	= dwVirtue;
	if (m_pRes)
	{
		m_pRes->SetDWord("virtue", dwVirtue);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetHomeID (OBJID idHome, BOOL bUpdate)
{
	m_Info.idHome	= idHome;
	if (m_pRes)
	{
		m_pRes->SetDWord("home_id", idHome);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLockKey (DWORD dwLockKey, BOOL bUpdate)
{
	m_Info.dwLockKey		= dwLockKey;
	m_Info.dwCurrLockKey	= dwLockKey;
	if (m_pRes)
	{
		m_pRes->SetDWord("lock_key", dwLockKey);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetGodStatus(DWORD dwData, BOOL bUpdate)
{
	m_Info.dwGodStatus	= dwData;
	if (m_pRes)
	{
		m_pRes->SetDWord("god_status", dwData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetExpBallUsage(DWORD dwData, BOOL bUpdate)
{
	m_Info.dwExpBallUsage	= dwData;
	if (m_pRes)
	{
		m_pRes->SetDWord("exp_ball_usage", dwData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetOnlineTime(DWORD dwData, BOOL bUpdate)
{
	m_Info.dwOnlineTime	= dwData;
	if (m_pRes)
	{
		m_pRes->SetUInt("online_time", dwData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetAutoExercise(DWORD dwData, BOOL bUpdate)
{
	m_Info.dwAutoExercise	= dwData;
	if (m_pRes)
	{
		m_pRes->SetUInt("auto_exercise", dwData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetLastLogout(DWORD dwData, BOOL bUpdate)
{
	m_Info.dwLastLogout	= dwData;
	if (m_pRes)
	{
		m_pRes->SetDWord("last_logout", dwData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetBigPrizeFails(int nData, BOOL bUpdate)
{
	m_Info.usBigPrizeFails	= nData;
	if (m_pRes)
	{
		m_pRes->SetInt("big_prize_fails", nData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetSmallPrizeFails(int nData, BOOL bUpdate)
{
	m_Info.usSmallPrizeFails	= nData;
	if (m_pRes)
	{
		m_pRes->SetInt("small_prize_fails", nData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetMeteLev(int nData, BOOL bUpdate)
{
	m_Info.dwMeteLev	= nData;
	if (m_pRes)
	{
		m_pRes->SetInt("mete_lev", nData);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

void CUserData::SetCurrentLayoutType(BOOL bUpdate)
{
	m_Info.dwCurrentLayoutType	= 1;
	if (m_pRes)
	{
		m_pRes->SetInt("current_layout_type", 1);
		if (bUpdate)
			m_pRes->UpdateRecord();
	}
}

DWORD CUserData::GetBusiness()
{
	return m_Info.dwBusiness;
}

void CUserData::SetBusiness(DWORD dwData)
{
	m_Info.dwBusiness	= dwData;
	m_pRes->SetInt("business", dwData);
	m_pRes->UpdateRecord();
}

__int64 CUserData::GetDonation()
{
	return m_Info.i64Donation;
}

void CUserData::SetDonation(__int64 i64Data)
{
	m_Info.i64Donation	= i64Data;
	m_pRes->SetInt64("donation", i64Data);
	m_pRes->UpdateRecord();
}

DWORD CUserData::GetLastLogin()
{
	return m_Info.dwLast_login;
}
