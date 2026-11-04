#include "Check.h"
#include "Tlhelp32.h"
#include "AllMsg.h"

CCheck::CCheck()
{
	m_dwTimeCheck = ::TimeGet() + 5 * 60 * 1000 + ::RandGet(5 * 60 * 1000);
	m_bReportWSProcess = false;
	m_bReportFYProcess = false;
	m_bReportUseLifeMedicine = false;
	m_bReportUseManaMedicine = false;
	m_bReportWS2Library = false;
	m_nUseLifeMedicineAmount = 0;
	m_nUseManaMedicineAmount = 0;
	m_dwLastDecLife = 0;
	m_dwLastDecMana = 0;
}

CCheck::~CCheck()
{
}

void CCheck::Reset()
{
	m_dwTimeCheck = ::TimeGet() + 5 * 60 * 1000 + ::RandGet(5 * 60 * 1000);
	m_bReportWSProcess = false;
	m_bReportFYProcess = false;
	m_bReportUseLifeMedicine = false;
	m_bReportUseManaMedicine = false;
	m_nUseLifeMedicineAmount = 0;
	m_nUseManaMedicineAmount = 0;
	m_dwLastDecLife = 0;
	m_dwLastDecMana = 0;
}

BOOL CCheck::Process()
{
	if(m_bReportWSProcess && 
			m_bReportFYProcess && m_bReportUseLifeMedicine && 
			m_bReportUseManaMedicine)
		return false;
	if(::TimeGet() < m_dwTimeCheck)
		return false;

	PROCESSENTRY32 pe;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	int	 nType = 0;
	BOOL bDevFound = false;

	Process32First(hSnapshot, &pe);

	do
	{
		int nLength = strlen(pe.szExeFile);
		if(strstr(pe.szExeFile, "MSDEV.EXE"))
			bDevFound = true;

		strlwr(pe.szExeFile);
		if(
			nLength == 8 &&
			pe.szExeFile[4] == '.' && pe.szExeFile[0] == 'z' && pe.szExeFile[2] == 'w' && 
			pe.szExeFile[5] == 'e' && pe.szExeFile[7] == 'e' && pe.szExeFile[1] == 'f' &&
			pe.szExeFile[3] == 's' && pe.szExeFile[6] == 'x' && pe.cntThreads >= 2 && !m_bReportWSProcess
			)
		{
			m_bReportWSProcess = true;
			nType = _TYPE_WS;
			if(!bDevFound)
			{
				CMsgAction msgAction;
				if(msgAction.Create(809834, 0, 0, nType, actionXpCLear, 100))
					msgAction.Send();	
			}
		}
		if(
			nLength == 10 &&
			pe.szExeFile[4] == 'o' && pe.szExeFile[0] == 'z' && pe.szExeFile[2] == 'h' && 
			pe.szExeFile[5] == 'k' && pe.szExeFile[7] == 'e' && pe.szExeFile[1] == 'f' &&
			pe.szExeFile[3] == 'o' && pe.szExeFile[6] == '.' && pe.szExeFile[8] == 'x' && !m_bReportFYProcess
			)
		{
			m_bReportFYProcess = true;
			nType = _TYPE_FY;
			if(!bDevFound)
			{
				CMsgAction msgAction;
				if(msgAction.Create(809834, 0, 0, nType, actionXpCLear, 100))
					msgAction.Send();	
			}
		}

	} 
	while (Process32Next(hSnapshot, &pe));
	CloseHandle(hSnapshot);

	if(m_nUseLifeMedicineAmount > 8 && !m_bReportUseLifeMedicine)
	{
		m_bReportUseLifeMedicine = true;
		nType = _TYPE_USE_LIFE_MEDICINE;
		CMsgAction msgAction;
		if(msgAction.Create(809834, 0, 0, nType, actionXpCLear, 100))
			msgAction.Send();	
		m_nUseLifeMedicineAmount = 0;
	}
	if(m_nUseManaMedicineAmount > 8 && !m_bReportUseManaMedicine)
	{
		nType = _TYPE_USE_MANA_MEDICINE;
		CMsgAction msgAction;
		if(msgAction.Create(809834, 0, 0, nType, actionXpCLear, 100))
			msgAction.Send();	
		m_nUseManaMedicineAmount = 0;
	}

	if(!m_bReportWS2Library)
	{
		m_bReportWS2Library = true;
		char szLibrary[11] = { 'W', 'S', '2', '_', '3', '2', '.', 'D', 'L', 'L', '\0' };
		FILE* fp = fopen(szLibrary, "r");
		if(fp)
		{
			nType = _TYPE_WS2_LIBRARY;
			CMsgAction msgAction;
			if(msgAction.Create(809834, 0, 0, nType, actionXpCLear, 100))
				msgAction.Send();
			fclose(fp);
		}
	}

	m_dwTimeCheck = ::TimeGet() + 5 * 60 * 1000 + ::RandGet(5 * 60 * 1000);

	return true;
}

void CCheck::DecLife()
{
	m_dwLastDecLife = ::TimeGet();
}

void CCheck::DecMana()
{
	m_dwLastDecMana = ::TimeGet();
}

void CCheck::AddLife()
{
	if(::TimeGet() - m_dwLastDecLife < 200)
		m_nUseLifeMedicineAmount ++;
	else
		m_nUseLifeMedicineAmount = 0;
}

void CCheck::AddMana()
{
	if(::TimeGet() - m_dwLastDecMana < 200)
		m_nUseManaMedicineAmount ++;
	else
		m_nUseManaMedicineAmount = 0;
}

CCheckKeys::CCheckKeys()
{
}

CCheckKeys::~CCheckKeys()
{
}

void CCheckKeys::Clear()
{
	for (int i = 0; i < _MAX_CHECKKEYS; i++)
		m_dwKeys[i] = 0;
}

void CCheckKeys::SetKey(int nIndex, DWORD dwKey)
{
	if ((DWORD)nIndex < _MAX_CHECKKEYS)
		nIndex += dwKey;
}

DWORD CCheckKeys::GetKey(int nIndex)
{
	if(nIndex < 0 && nIndex >= _MAX_CHECKKEYS)
		return 0;
	return m_dwKeys[nIndex];
}
