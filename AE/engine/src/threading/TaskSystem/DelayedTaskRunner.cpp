// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "threading/TaskSystem/DelayedTaskRunner.h"

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	DelayedTaskRunner::DelayedTaskRunner () __NE___
	{}

	DelayedTaskRunner::DelayedTaskRunnerService::DelayedTaskRunnerService (DelayedTaskRunner &mngr)	__NE___ :
		_mngr{mngr}
	{
		_delayedTasks.reserve( 128 );
	}

/*
=================================================
	Resolve
=================================================
*/
	bool  DelayedTaskRunner::Resolve (AnyTypeCRef dep, Task &task, Bool) __NE___
	{
		ASSERT_MSG( AnyEqual( task.QueueType(), ETaskQueue::Background, ETaskQueue::Unknown ),
			"only 'Background' task should depends on DelayedRun" );

		if_likely( auto* res_p = dep.GetIf< DelayedRun >() )
		{
			TaskApi::IncWaitCounter( task );

			_pendingTasks->emplace_back( res_p->_endTime, task.GetRC() DEBUG_ONLY(, res_p->_loc) );
			return true;
		}

		RETURN_ERR( "unsupported dependency type" );
	}

/*
=================================================
	Register
=================================================
*/
	bool  DelayedTaskRunner::Register () __NE___
	{
		auto	mngr = RC<DelayedTaskRunner>( new DelayedTaskRunner{} );
		auto	serv = MakeRC<DelayedTaskRunnerService>( *mngr );

		CHECK_ERR( Scheduler().RegisterDependency< DelayedRun >( mngr ));
		Scheduler().AddIOService( serv );
		return true;
	}

/*
=================================================
	ProcessEvents
=================================================
*/
	usize  DelayedTaskRunner::DelayedTaskRunnerService::ProcessEvents () __NE___
	{
		DeferExLock		lock {_guard};

		if ( not lock.try_lock() )
			return 0;

		// copy pending tasks
		{
			auto	pending = _mngr._pendingTasks.WriteNoLock();
			if ( pending.try_lock() )
			{
				if ( not pending->empty() )
				{
					_delayedTasks.reserve( _delayedTasks.size() + pending->size() );

					for (auto& dep : *pending) {
						_delayedTasks.push_back( RVRef(dep) );
					}
					pending->clear();
				}
			}
		}

		if ( _delayedTasks.empty() )
			return 0;

		const auto	tp		= HighResClock::now();
		usize		count	= 0;

		for (auto it = _delayedTasks.begin(); it != _delayedTasks.end();)
		{
			if ( tp > it->endTime )
			{
				TaskApi::SetDependencyCompletionStatus( *it->task, False{"resume"} );
				it = _delayedTasks.erase( it );
				++count;
			}
			else
			{
				++it;
			}
		}

		return count;
	}


} // AE::Threading
