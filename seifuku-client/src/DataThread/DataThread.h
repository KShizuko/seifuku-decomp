#ifndef _DATATHREAD_H_
#define _DATATHREAD_H_

#ifdef	DATATHREAD_EXPORTS
#define DATATHREAD_API __declspec ( dllexport )
#else
#define DATATHREAD_API __declspec ( dllimport )
#endif

class IDataThreadItem
{
public:
	virtual unsigned __int64	GetID () = 0;
	virtual void	Process () = 0;
	virtual int		IsImmediate () = 0;
	virtual void	Destroy () = 0;
};

class IDataThread
{
public:
	virtual int		Release () = 0;
	virtual void	SetPriority ( int nPriority ) = 0;
	virtual void	SetSync ( bool bSync ) = 0;
	virtual IDataThreadItem*	PopFinished () = 0;
	virtual IDataThreadItem*	Add ( IDataThreadItem* pItem ) = 0;
	virtual int		IsIdle () = 0;
	virtual int		Commit () = 0;
};

DATATHREAD_API
IDataThread* DataThreadCreateNew ();

#endif
