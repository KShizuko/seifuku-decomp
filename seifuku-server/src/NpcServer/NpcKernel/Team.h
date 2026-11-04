#if !defined(AFX_TEAM_H__096C275F_D6F9_40F1_8B39_471D69EBA7CC__INCLUDED_)
#define AFX_TEAM_H__096C275F_D6F9_40F1_8B39_471D69EBA7CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "define.h"
#include <windows.h>
#include "Myheap.h"

#include <vector>
#include <algorithm>
using namespace std;

#define		_MAX_TEAMAMOUNT			5

typedef struct
{
	char			szName[_MAX_NAMESIZE];
	OBJID			id;
	DWORD			dwLookFace;
	unsigned short	usMaxHp;
	unsigned short	usHp;
}TeamMemberInfo;

class CNetMsg;
class IRole;
class CTeam  
{
public:
	CTeam();
	virtual ~CTeam();

public:
	static CTeam*	CreateNew	();
	void	Release				()								{ delete this; }
	BOOL	Create				(CAgent* pAgent);

public:
	bool	IsValid				()								{ return GetMemberAmount() > 0; }
	OBJID	GetLeaderID			();

public:
	bool	AddMember			(int nAmount, const TeamMemberInfo* setInfo);
	BOOL	DelMember			(OBJID idMember);
	void	Dismiss				(OBJID idLeader)				{ Init(); }
	BOOL	Apply				(OBJID idUser);

protected:
	int		GetMemberAmount		();

protected:
	void	Init				();

protected:
	typedef	vector<TeamMemberInfo>		USERIDSET;
	USERIDSET					m_setMember;
	CAgent*						m_pOwner;

private:
	BOOL						m_fClosed;

protected:
	MYHEAP_DECLARATION(s_heap)
};

#endif
