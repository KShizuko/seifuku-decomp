#if !defined(AFX_TASKDIALOG_H__9E7E3639_6462_47E2_B177_8B81CFAF046B__INCLUDED_)
#define AFX_TASKDIALOG_H__9E7E3639_6462_47E2_B177_8B81CFAF046B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseFunc.h"

const int _TASKDIALOG_TEXT		= 0;

const int _TASKDIALOG_LINK		= 1;

const int _TASKDIALOG_EDIT		= 2;

const int _TASKDIALOG_PIC		= 3;

const int _TASKDIALOG_DEFAULT	= 4;

const int _TASKDIALOG_LIST	= 5;

typedef struct
{
	int		nType;
	DWORD	dwData;
	int		nIndex;
	char	szStr[_MAX_STRING];
	CMyPos	posLine;
}TaskDialogLine;

#include <deque>  
using namespace std;
typedef deque<TaskDialogLine*> DEQUE_TASKDIALOGLINE;

class CTaskDialog  
{
	friend struct TaskDialogProbe;
public:
	CTaskDialog();
	virtual ~CTaskDialog();
private:
	BOOL	m_bCreate;
	DEQUE_TASKDIALOGLINE	m_setTaskDialogLine;
public:
	void			AddLine(TaskDialogLine* pLine);
	void			Destroy();
	int				GetLineAmount(){return m_setTaskDialogLine.size();}
	TaskDialogLine*	GetLineByIndex(int nIndex);
	void			AddLine(int nType, DWORD dwData, int nIndex, char* szStr, CMyPos posLine);
	void			Create(){m_bCreate = true;}
};

#endif
