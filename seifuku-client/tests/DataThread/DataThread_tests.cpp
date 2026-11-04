#include "CppUnitTest.h"
#include <windows.h>
#include "DataThread.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	class CCountingItem : public IDataThreadItem
	{
	public:
		CCountingItem ( bool bImmediate ) : m_bImmediate ( bImmediate ), m_nProcessed ( 0 ), m_dwThread ( 0 ), m_nDestroyed ( 0 ) {}
		virtual unsigned __int64	GetID ()	{ return 0; }
		virtual void	Process ()		{ m_nProcessed++; m_dwThread = GetCurrentThreadId (); }
		virtual int		IsImmediate ()	{ return m_bImmediate ? 1 : 0; }
		virtual void	Destroy ()		{ m_nDestroyed++; }

		bool	m_bImmediate;
		int		m_nProcessed;
		DWORD	m_dwThread;
		int		m_nDestroyed;
	};

	static bool WaitProcessed ( IDataThread* pThread, CCountingItem& item )
	{
		for ( int i = 0; i < 500; i++ )
		{
			if ( pThread->IsIdle () && item.m_nProcessed > 0 )
				return true;
			Sleep ( 10 );
		}
		return false;
	}
}

TEST_CLASS(DataThreadTests)
{
public:
	TEST_METHOD(Pointers32Bit)
	{
		static_assert ( sizeof ( void* ) == 4, "");
	}

	TEST_METHOD(CreateIsIdleAndEmpty)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		Assert::IsNotNull ( pThread );
		Assert::AreEqual ( 1, pThread->IsIdle () );
		Assert::IsNull ( pThread->PopFinished () );
		Assert::AreEqual ( 0, pThread->Commit () );
		Assert::AreEqual ( 0, pThread->Release () );
	}

	TEST_METHOD(ImmediateItemRunsInAdd)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		CCountingItem item ( true );
		Assert::IsTrue ( pThread->Add ( &item ) == &item );
		Assert::AreEqual ( 1, item.m_nProcessed );
		Assert::AreEqual ( GetCurrentThreadId (), item.m_dwThread );
		Assert::AreEqual ( 0, pThread->Commit () );
		Assert::IsNull ( pThread->PopFinished () );
		pThread->Release ();
	}

	TEST_METHOD(SyncModeRunsEveryItemInAdd)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		pThread->SetSync ( true );
		CCountingItem item ( false );
		Assert::IsTrue ( pThread->Add ( &item ) == &item );
		Assert::AreEqual ( 1, item.m_nProcessed );
		Assert::AreEqual ( GetCurrentThreadId (), item.m_dwThread );
		Assert::IsNull ( pThread->PopFinished () );
		pThread->Release ();
	}

	TEST_METHOD(QueuedItemRunsOnTheThreadAfterCommit)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		CCountingItem item ( false );
		Assert::IsNull ( pThread->Add ( &item ) );
		Sleep ( 50 );
		Assert::AreEqual ( 0, item.m_nProcessed );
		pThread->Commit ();
		Assert::IsTrue ( WaitProcessed ( pThread, item ), L"the worker thread did not process the item" );
		Assert::AreEqual ( 1, item.m_nProcessed );
		Assert::AreNotEqual ( GetCurrentThreadId (), item.m_dwThread );
		Assert::AreEqual ( 1, pThread->Commit () );
		Assert::IsTrue ( pThread->PopFinished () == &item );
		Assert::IsNull ( pThread->PopFinished () );
		Assert::AreEqual ( 0, pThread->Commit () );
		pThread->Release ();
	}

	TEST_METHOD(BatchKeepsItsOrder)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		CCountingItem a ( false ), b ( false ), c ( false );
		pThread->Add ( &a );
		pThread->Add ( &b );
		pThread->Add ( &c );
		pThread->Commit ();
		Assert::IsTrue ( WaitProcessed ( pThread, c ), L"the worker thread did not process the batch" );
		Assert::IsTrue ( pThread->PopFinished () == &a );
		Assert::IsTrue ( pThread->PopFinished () == &b );
		Assert::IsTrue ( pThread->PopFinished () == &c );
		Assert::IsNull ( pThread->PopFinished () );
		pThread->Release ();
	}

	TEST_METHOD(ReleaseDoesNotDestroyPendingItems)
	{
		IDataThread* pThread = DataThreadCreateNew ();
		CCountingItem item ( false );
		Assert::IsNull ( pThread->Add ( &item ) );
		Assert::AreEqual ( 0, pThread->Release () );
		Assert::AreEqual ( 0, item.m_nDestroyed );
		Assert::AreEqual ( 0, item.m_nProcessed );
		pThread = DataThreadCreateNew ();
		pThread->Commit ();
		Assert::IsTrue ( WaitProcessed ( pThread, item ), L"the pending item was not processed by the next thread" );
		Assert::IsTrue ( pThread->PopFinished () == &item );
		pThread->Release ();
	}
};
