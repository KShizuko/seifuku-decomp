#include "Adapter.h"
#include "LotteryActor.h"
#include "MapGroup.h"
#include "IniFile.h"
#include "NetMsg.h"

CAdapter::CAdapter(PROCESS_ID idProcess)
{
	m_idProcess	= idProcess;
	m_hCommon	= ::LoadLibrary("Common.dll");
	if (m_hCommon)
	{
		FARPROC funcCnStrInit = ::GetProcAddress(m_hCommon, "CnStrInit");
		if (funcCnStrInit)
			funcCnStrInit();
	}
}

CAdapter::~CAdapter()
{
	if (m_hCommon)
		::FreeLibrary(m_hCommon);
}

bool CAdapter::Init()
{
	CIniFile	ini("adapter.ini", "Module");
	char	szModule[16] = "";
	int		nAmount = ini.GetInt("Amount");
	for (int i = 0; i < nAmount; i++)
	{
		char	szKey[16];
		sprintf(szKey, "Module%d", i);
		ini.GetString(szModule, szKey, sizeof(szModule));
		if (szModule)
		{
			HMODULE hModule = ::LoadLibrary(szModule);
			if (!hModule)
				return false;
			MODULE_CREATE funcModuleCreate = (MODULE_CREATE)::GetProcAddress(hModule, "ModuleCreate");
			if (!funcModuleCreate)
				return false;
			IF_NOT(funcModuleCreate(this, Database()))
				return false;
		}
	}
	return true;
}

void* CAdapter::Find(int idModule, int nUnknown)
{
	MODULE_SET::iterator it = m_setModule.find(idModule);
	if (it == m_setModule.end())
		return NULL;
	return it->second;
}

void* CAdapter::GetActor(int idModule, void* pBuf)
{
	IF_NOT(pBuf)
		return NULL;

	if (idModule == ADAPTER_MODULE_LOTTERY)
		return this->CreateLotteryActor((CUser*)pBuf);
	return NULL;
}

void* CAdapter::CreateActor(LPCTSTR szName)
{
	return NULL;
}
ILotteryActor* CAdapter::CreateLotteryActor(CUser* pUser)
{
	IF_NOT(pUser)
		return NULL;

	ILotteryActor* pActor = LotteryActorCreate(pUser, m_idProcess);
	IF_NOT(pActor)
		return NULL;
	return pActor;
}

bool CAdapter::Register(int idModule, void* pModule)
{
	if (m_setModule.find(idModule) != m_setModule.end())
		return false;
	m_setModule[idModule] = pModule;
	return true;
}

bool CAdapter::SendMsg(SOCKET_ID idSocket, OBJID idMsg, const char* pBuf, DWORD dwSize)
{
	CNetMsg* pNetMsg = CNetMsg::CreateClientMsg(m_idProcess, idSocket, idMsg, pBuf, dwSize, 0);
	IF_NOT(pNetMsg)
		return false;

	bool bRet = MapGroup(m_idProcess)->QueryIntraMsg()->SendClientMsg(idSocket, pNetMsg);
	SAFE_DELETE(pNetMsg);
	return bRet;
}

IAdapter* AdapterCreate(PROCESS_ID idProcess)
{
	CAdapter* pAdapter = new CAdapter(idProcess);
	IF_NOT(pAdapter)
		return NULL;

	if (pAdapter->Init())
		return pAdapter;
	return NULL;
}
