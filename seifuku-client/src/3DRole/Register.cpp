#include "windows.h"
#include "Register.h"

CRegister	g_objRegister;

CRegister::CRegister()
{
	m_TxtMsgSet.ClearMsg();
}

CRegister::~CRegister()
{

}
