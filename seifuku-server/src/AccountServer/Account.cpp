#include "StdAfx.h"
#include <stdio.h>
#include <assert.h>

#include "define.h"
#include "Account.h"
#include "Logfile.h"
#include "typedef.h"

#ifdef	DATABASE_PASSWORD
const char	SQL_ACCOUNT_STMT[]			="SELECT id,name,type,point,pointtime,online,licence,netbar_ip,ip_mask FROM account as t1 left join %s as t2 on t2.account_id = t1.id WHERE name = '%s' AND password = PASSWORD('%s')";
#else
const char	SQL_ACCOUNT_STMT[]			="SELECT id,name,type,point,pointtime,online,licence,netbar_ip,ip_mask FROM account as t1 left join %s as t2 on t2.account_id = t1.id WHERE name = '%s' AND password = '%s'";
const char	SQL_ACCOUNT_STMT2[]			="SELECT id,name,type,point,pointtime,online,licence,netbar_ip,ip_mask FROM account as t1 left join %s as t2 on t2.account_id = t1.id WHERE name = '%s'";
#endif
const char	SQL_ACCOUNTID_STMT[]			="SELECT id,name,type,point,pointtime,online,licence,netbar_ip,ip_mask FROM account as t1 left join %s as t2 on t2.account_id = t1.id WHERE id = %u";
const char	SQL_ACCOUNTISP_STMT[]			="SELECT id,name,type,point,pointtime,online,licence,netbar_ip,ip_mask FROM account as t1 left join %s as t2 on t2.account_id = t1.id WHERE type=%d AND netbar_ip='%s'";
const char	SQL_ACCOUNT_UPDATEPOINT_STMT[]	="UPDATE %s SET point=point-1 WHERE account_id=%u AND point>0 LIMIT 1";

CAccount::CAccount()
{
	m_pRes = NULL;
	memset(&m_Info,0L,sizeof(m_Info));
}

CAccount::~CAccount()
{	
	Destroy();
}

BOOL CAccount::CheckPassword (const char* pszPsw)
{
	if (!m_pRes)
		return false;

	if (0 == strcmp(pszPsw, m_Info.szPassword))
		return true;
	else
		return false;
}

void InsertBackslash(char * bufTarget, const char * pszSource)
{
	const char * pSour = pszSource;
	char * pTarg = bufTarget;
	while(*pSour)
	{
		if(*pSour == '\\' || *pSour == '\'')
			*(pTarg++) = '\\';

		*(pTarg++) = *(pSour++);
	}
	*pTarg = 0;
}

BOOL CAccount::Create(const char* pszName, const char* pszPassword)
{
	try{
		if(!pszName || _MAX_NAMESIZE <= strlen(pszName))
			return false;

		if(!pszPassword || _MAX_PSWSIZE <= strlen(pszPassword))
			return false;

		if (m_pRes != NULL)
		{
			::LogSave("WARNING: CAccount::Create() m_pRes not NULL.");
			this->Destroy();
		}

		char bufName[256];
		InsertBackslash(bufName, pszName);
		{
		}

		char bufPassword[256];
		InsertBackslash(bufPassword, pszPassword);
		{
		}

		m_pRes = new CMyRecordset(&g_db);
		if(!m_pRes)
		{
			::LogSave("ERROR: CAccount::Create() can't new CMyRecordset object.");
			return false;
		}
		if(bufPassword[0])
			sprintf(m_szSQL, SQL_ACCOUNT_STMT, POINTTABLE, bufName, bufPassword);
		else
			sprintf(m_szSQL, SQL_ACCOUNT_STMT2, POINTTABLE, bufName);

		if (!m_pRes->Open(m_szSQL))  
		{	
			::LogSave("ERROR: CAccount::Create(char*) can't Open() database for [%s]", m_szSQL);
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if (m_pRes->RecordCount() == 0)  
		{	
			m_pRes->Close();
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if(!this->LoadInfo())
		{
			::LogSave("ERROR: CAccount::Create() can't LoadInfo()");
			return false;
		}

		return true;
	}
	catch(...)
	{
		::LogSave("exception catch at CAccount::Create(%s).", pszName);
		return false;
	}
}

BOOL CAccount::Create(OBJID	idAccount)
{
	try{
		if (m_pRes != NULL)
			this->Destroy();

		m_pRes = new CMyRecordset(&g_db);
		sprintf(m_szSQL, SQL_ACCOUNTID_STMT, POINTTABLE, idAccount);

		if (!m_pRes->Open(m_szSQL))  
		{	
			::LogSave("ERROR: CAccount::Create(OBJID) can't Open() database for [%s]", m_szSQL);
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if (m_pRes->RecordCount() == 0)  
		{	
			m_pRes->Close();
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if(!this->LoadInfo())
			return false;

		return true;
	}
	catch(...)
	{
		::LogSave("exception catch at CAccount::Create(%04X).", idAccount);
		return false;
	}
}

BOOL CAccount::Create(int nType, const char* pszIPAddr)
{
	try{
		if(!pszIPAddr)
			return false;

		if (m_pRes != NULL)
			this->Destroy();

		m_pRes = new CMyRecordset(&g_db);
		sprintf(m_szSQL, SQL_ACCOUNTISP_STMT, POINTTABLE, nType, pszIPAddr);

		if (!m_pRes->Open(m_szSQL))  
		{	
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if (m_pRes->RecordCount() == 0)  
		{	
			m_pRes->Close();
			delete m_pRes;
			m_pRes = NULL;
			return false;
		}

		if(!this->LoadInfo())
			return false;

		return true;
	}
	catch(...)
	{
		::LogSave("exception catch at CAccount::Create(%d, %s).", nType, pszIPAddr);
		return false;
	}
}

void CAccount::Destroy(void)
{
	if (m_pRes != NULL)
	{	m_pRes->Close();
		delete m_pRes;
		m_pRes = NULL;
	}
	
	memset(&m_Info,0L,sizeof(m_Info));
}

BOOL CAccount::LoadInfo(void)
{
	m_Info.id	= (int)m_pRes->Fields("id");
	m_Info.nType		= m_pRes->Fields("type");
	m_Info.nPoint		= m_pRes->Fields("point");
	m_Info.nPointTime	= (int)m_pRes->Fields("pointtime");
	m_Info.nOnline		= (int)m_pRes->Fields("online");
	m_Info.nLicence		= (int)m_pRes->Fields("licence");

	strcpy(m_Info.szName, m_pRes->Fields("name"));
	strcpy(m_Info.dwNetBarIP, m_pRes->Fields("netbar_ip"));
	strcpy(m_Info.dwIPMask, m_pRes->Fields("ip_mask"));
	return true;
}

char* CAccount::GetName(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{	
		strcpy(m_Info.szName, m_pRes->Fields("name"));
	}
	return m_Info.szName;
}

void CAccount::SetName(const char* pszName, BOOL bUpdate)
{
	assert(m_pRes);
	strcpy(m_Info.szName, pszName);

	if (bUpdate)
	{	
		m_pRes->Edit();
		strcpy(m_pRes->Fields("name"), pszName);
		m_pRes->Update();
	}
}

int CAccount::GetType(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{	
		m_Info.nType = m_pRes->Fields("type");
	}

	return m_Info.nType;
}

int CAccount::GetPoint(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{	
		m_Info.nPoint = m_pRes->Fields("point");
	}

	return m_Info.nPoint;
}

int CAccount::GetPointTime(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{	
		m_Info.nPointTime = (int)m_pRes->Fields("pointtime");
	}

	return m_Info.nPointTime;
}
int CAccount::GetOnline(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{	
		m_Info.nOnline = (int)m_pRes->Fields("online");
	}

	return m_Info.nOnline;
}

int CAccount::GetLicence(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{
		m_Info.nLicence = (int)m_pRes->Fields("licence");
	}

	return m_Info.nLicence;
}

LPCTSTR CAccount::GetNetBarIP(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{
		strcpy(m_Info.dwNetBarIP, m_pRes->Fields("netbar_ip"));
	}

	return m_Info.dwNetBarIP;
}

LPCTSTR CAccount::GetIPMask(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
	{
		strcpy(m_Info.dwIPMask, m_pRes->Fields("ip_mask"));
	}

	return m_Info.dwIPMask;
}

int CAccount::GetSex(BOOL bUpdate)
{
	
	assert(m_pRes);
	if (bUpdate)
		m_Info.nSex = m_pRes->Fields("sex");

	return m_Info.nSex;
}

void CAccount::SetSex(const int nSex,BOOL bUpdate)
{
	assert(m_pRes);
	m_Info.nSex = nSex;

	if (bUpdate)
	{	
		m_pRes->Edit();
		m_pRes->Fields("sex") = m_Info.nSex;
		m_pRes->Update();
	}
}

char* CAccount::GetPhone(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
		strcpy(m_Info.szPhone, m_pRes->Fields("phone"));
	return m_Info.szPhone;
}

void CAccount::SetPhone(const char* pszPhone,BOOL bUpdate)
{
	assert(m_pRes);
	strcpy(m_Info.szPhone, pszPhone);

	if (bUpdate)
	{	
		m_pRes->Edit();
		strcpy(m_pRes->Fields("phone"), pszPhone);
		m_pRes->Update();
	}
}

char* CAccount::GetEmail(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
		strcpy(m_Info.szEmail, m_pRes->Fields("email"));
	return m_Info.szEmail;
}

void CAccount::SetEmail(const char* pszEmail, BOOL bUpdate)
{
	assert(m_pRes);
	strcpy(m_Info.szEmail, pszEmail);

	if (bUpdate)
	{	
		m_pRes->Edit();
		strcpy(m_pRes->Fields("email"), pszEmail);
		m_pRes->Update();
	}
}

char* CAccount::GetAddress(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
		strcpy(m_Info.szAddress, m_pRes->Fields("address"));
	return m_Info.szAddress;
}

void CAccount::SetAddress(const char* pszAddress, BOOL bUpdate)
{
	assert(m_pRes);
	strcpy(m_Info.szAddress, pszAddress);
	if (bUpdate)
	{	
		m_pRes->Edit();
		strcpy(m_pRes->Fields("address"), pszAddress);
		m_pRes->Update();
	}
}

char* CAccount::GetIdNumber(BOOL bUpdate)
{
	assert(m_pRes);
	if (bUpdate)
		strcpy(m_Info.szIdNumber, m_pRes->Fields("idnumber"));
	return m_Info.szIdNumber;
}

void CAccount::SetIdNumber(const char* pszIdNumber, BOOL bUpdate)
{
	assert(m_pRes);
	strcpy(m_Info.szIdNumber, pszIdNumber);
	if (bUpdate)
	{	
		m_pRes->Edit();
		strcpy(m_pRes->Fields("idnumber"), pszIdNumber);
		m_pRes->Update();
	}
}

int	CAccount::GetAge(BOOL bUpdate)
{	
	assert(m_pRes);
	if (bUpdate)
		m_Info.nAge = m_pRes->Fields("age");
	return m_Info.nAge;
}

void CAccount::SetAge(int nAge, BOOL bUpdate)
{	
	assert(m_pRes);
	m_Info.nAge = nAge;
	if (bUpdate)
	{	
		m_pRes->Edit();
		m_pRes->Fields("age") = nAge;
		m_pRes->Update();
	}
}

CAccount* CAccount::CreateNew (const char* pszName, const char* pszPassword)
{
	CAccount* pAccount	=new CAccount;
	if (!pAccount)
		return NULL;

	char	bufName[256];
	InsertBackslash(bufName, pszName);
	char	bufPassword[256];
	InsertBackslash(bufPassword, pszPassword);
	if (!pAccount->Create(bufName, bufPassword))
	{
		delete pAccount;
		return NULL;
	}

	return pAccount;
}
bool	CAccount::DecPoint(OBJID idAccount)
{
	char szSQL[256];
	sprintf( szSQL, SQL_ACCOUNT_UPDATEPOINT_STMT, POINTTABLE, idAccount);
  
	return g_db.ExecuteSQL(szSQL,NULL);
}
