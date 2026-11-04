#if !defined(AFX_FRIEND_H__175B4F68_F766_4995_9E8A_414301B232E2__INCLUDED_)
#define AFX_FRIEND_H__175B4F68_F766_4995_9E8A_414301B232E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif
#include "BaseFunc.h"

class CFriend
{
	friend struct FriendProbe;
public:
	CFriend();
	virtual ~CFriend();

private:
	OBJID	m_idFriend;
	char	m_szName[_MAX_NAMESIZE];
	BOOL	m_bFlagOnline;
	BOOL	m_bFlagNewMsg;

	DWORD	m_dwFace;
	DWORD	m_dwLevel;
	DWORD	m_dwProfessional;
	int		m_nPK;
	DWORD	m_IDSyndicate;
	DWORD	m_dwRank;
	char	m_szMate[_MAX_NAMESIZE];
public:
	static CFriend* CreateNew(OBJID id, char* pszName, BOOL m_bFlagOnline);
	OBJID	GetID(){return m_idFriend;}
	char*	GetName(){return m_szName;}
	BOOL	IsOnline(){return m_bFlagOnline;}
	BOOL	HaveNewMsg(){return m_bFlagNewMsg;}

	DWORD	GetProfessional(){return m_dwProfessional;}
	DWORD	GetFace(){return m_dwFace;}
	DWORD	GetLevel(){return m_dwLevel;}
	int		GetPK(){return m_nPK;}
	OBJID	GetSynDicateID(){return m_IDSyndicate;}
	DWORD	GetRank(){return m_dwRank;}
	char*	GetMate(){return m_szMate;}
	char*	GetProfessionalName();
	char*	GetSyndicateName();
	char*	GetRankName();

	void	SetID(OBJID id){m_idFriend = id;}
	void	SetName(char* pszName){assert(pszName); strcpy(m_szName, pszName);}
	void	Online(){m_bFlagOnline = true;}
	void	Offline(){m_bFlagOnline = false;}
	void	SetNewMsg(BOOL bMsg){m_bFlagNewMsg = bMsg;}
	void	SetFace(DWORD dwFace){m_dwFace = dwFace;}
	void	SetLevel(DWORD dwLevel){m_dwLevel = dwLevel;}
	void	SetProfessional(DWORD dwProfessional){m_dwProfessional = dwProfessional;}
	void	SetPK(int nPK){m_nPK = nPK;}
	void	SetSyndicateID(OBJID idSyndicate){m_IDSyndicate = idSyndicate;}
	void	SetRank(DWORD dwRank){m_dwRank = dwRank;}
	void	SetMate(char* pszMate){strcpy(m_szMate, pszMate);}
};
#include <deque>
using namespace std;
class CPlayer;
typedef deque<CFriend* >  DEQUE_FRIEND;
#endif
