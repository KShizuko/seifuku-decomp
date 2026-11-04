#include "NetMsg.h"
#include <string.h>

void CNetMsg::Init()
{
	memset(m_bufMsg, 0, sizeof(m_bufMsg));
	*(USHORT*)m_bufMsg		= 0;
	*(USHORT*)(m_bufMsg+2)	= 0;
}

bool CNetMsg::Create(char* pbufMsg, USHORT unMsgSize)
{
	if (!pbufMsg || unMsgSize != *(USHORT*)pbufMsg || *(USHORT*)(pbufMsg+2) == 0)
		return false;

	memcpy(m_bufMsg, pbufMsg, unMsgSize);
	return true;
}
