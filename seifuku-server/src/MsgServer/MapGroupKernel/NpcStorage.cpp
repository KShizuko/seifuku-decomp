#include "define.h"
#include "NpcStorage.h"
#include "MapGroup.h"
#include "Package.h"

MYHEAP_IMPLEMENTATION(CNpcTrunk,s_heap)
CNpcTrunk::CNpcTrunk()
{
	m_pPackage = NULL;
}
CNpcTrunk::~CNpcTrunk()
{
	if(m_pPackage)
		m_pPackage->Release();
}
bool CNpcTrunk::Create(PROCESS_ID idProcess, OBJID idRecordNpc, int nSize)
{
	m_idProcess = idProcess;
	m_nSize		= nSize;
	m_pPackage	= CPackage::CreateNew();
	CHECKF(m_pPackage && m_pPackage->Create(ITEMPOSITION_TRUNK, idRecordNpc, ID_NONE, Database()));

	return true;
}
