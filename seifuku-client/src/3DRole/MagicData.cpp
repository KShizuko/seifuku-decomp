#include "MagicData.h"

CMagicData::CMagicData()
{
	m_dwIDAmount = 0;
	m_pIDIndex = NULL;
	m_fp = NULL;
	const char szFile[] = "ini/magictype.dat";
	m_fp = fopen(szFile, "rb");
	MYASSERT(m_fp);
	this->Create();
}

CMagicData::~CMagicData()
{
	this->Destroy();
}

void CMagicData::Create()
{
	fread(&m_dwIDAmount, sizeof(DWORD), 1, m_fp);
	m_pIDIndex = new DWORD[m_dwIDAmount];
	MYASSERT(m_pIDIndex);
	fread(m_pIDIndex, sizeof(DWORD), m_dwIDAmount, m_fp);
}
void CMagicData::Destroy()
{
	delete[] m_pIDIndex;
	fclose(m_fp);
	m_setMagicType.clear();
}
BOOL CMagicData::GetMagicTypeInfo(OBJID dwIDType, DWORD dwLevel, MagicTypeInfo& infoMagic)
{
	DWORD dwKey = dwIDType * 10 + dwLevel;

	MAP_MAGICTYPE::iterator it = m_setMagicType.find(dwKey);
	if (it != m_setMagicType.end())
	{
		memcpy(&infoMagic, &(*it).second, sizeof(MagicTypeInfo));
		return true;
	}

	if (!m_pIDIndex)
		return false;
	int nIndex = -1;
	for (DWORD i = 0; i < m_dwIDAmount; i ++)
	{
		if (m_pIDIndex[i] == dwKey)
		{
			nIndex = i;
			break;
		}
	}
	if (nIndex < 0)
		return false;

	fseek(m_fp, sizeof(DWORD) + sizeof(DWORD) * m_dwIDAmount + sizeof(MagicTypeInfo) * nIndex, SEEK_SET);
	fread(&infoMagic, sizeof(MagicTypeInfo), 1, m_fp);
	m_setMagicType.insert(MAP_MAGICTYPE::value_type(dwKey, infoMagic));
	return true;
}
