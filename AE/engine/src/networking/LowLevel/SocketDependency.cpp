// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "networking/LowLevel/SocketDependency.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

#ifdef AE_PLATFORM_WINDOWS
# ifndef far
#	define far
# endif
#endif

namespace AE::Networking
{
	using namespace AE::Threading;

/*
=================================================
	constructor
=================================================
*/
	SocketDependencyManager::SocketDependencyManager () __NE___
	{
	}

	SocketDependencyManager::NetworkIOService::NetworkIOService (SocketDependencyManager &mngr)	__NE___	:
		_mngr{mngr}
	{
		_sockets.reserve( 1024 );
	}

/*
=================================================
	Resolve
=================================================
*/
	bool  SocketDependencyManager::Resolve (AnyTypeCRef dep, Task &task, Bool defaultIsStrongDep) __NE___
	{
		ASSERT_MSG( AnyEqual( task.QueueType(), ETaskQueue::Background, ETaskQueue::Unknown ),
			"only 'Background' task should depends on SocketDependency" );

		if_likely( auto* res_p = dep.GetIf< SocketDependency >() )
		{
			TaskApi::IncWaitCounter( task );

			_pendingSockets->emplace_back( res_p->GetSocket().NativeHandle(), defaultIsStrongDep, res_p->EndTime(), task.GetRC() DEBUG_ONLY(, res_p->SourceLocation()) );
			return true;
		}

		if_likely( auto* res_p = dep.GetIf< WeakSocketDependency >() )
		{
			TaskApi::IncWaitCounter( task );

			_pendingSockets->emplace_back( res_p->GetSocket().NativeHandle(), False{"weak"}, res_p->EndTime(), task.GetRC() DEBUG_ONLY(, res_p->SourceLocation()) );
			return true;
		}

		RETURN_ERR( "unsupported dependency type" );
	}

/*
=================================================
	Register
=================================================
*/
	bool  SocketDependencyManager::Register () __NE___
	{
		auto	mngr = RC<SocketDependencyManager>( new SocketDependencyManager{} );
		auto	serv = MakeRC<NetworkIOService>( *mngr );

		CHECK_ERR( Scheduler().RegisterDependency< SocketDependency >( mngr ));
		CHECK_ERR( Scheduler().RegisterDependency< WeakSocketDependency >( mngr ));
		Scheduler().AddIOService( serv );
		return true;
	}

/*
=================================================
	ProcessEvents
----
	TODO: use poll/epoll/kqueue to detect closed sockets
	currently is it UB when socket is closed without removing from wait list
=================================================
*/
	usize  SocketDependencyManager::NetworkIOService::ProcessEvents () __NE___
	{
		DeferExLock		lock {_guard};

		if ( not lock.try_lock() )
			return 0;

		// copy pending sockets
		{
			auto	pending = _mngr._pendingSockets.WriteNoLock();
			if ( pending.try_lock() )
			{
				if ( not pending->empty() )
				{
					_sockets.reserve( _sockets.size() + pending->size() );

					for (auto& dep : *pending) {
						_sockets.push_back( RVRef(dep) );
					}
					pending->clear();
				}
			}
		}

		if ( _sockets.empty() )
			return 0;

		const auto	tp = HighResClock::now();

		fd_set	read_fds;
		FD_ZERO( &read_fds );

		int	maxfd = 0;
		for (auto it = _sockets.begin(); it != _sockets.end();)
		{
			if ( tp > it->endTime )
			{
				AE_LOG_DBG( String{it->isStrong ? "Cancel" : "Resume"} << " task '"s << it->task->DbgName() <<
							"' which waiting for socket event in " << it->loc.FunctionName() << '(' << ToString( it->loc.Line() ) << ')' );

				TaskApi::SetDependencyCompletionStatus( *it->task, Bool{it->isStrong} );
				it = _sockets.erase( it );
			}
			else
			{
                auto  s = NativeSocket_t(it->socket);
				FD_SET( s, &read_fds );

			  #ifndef AE_PLATFORM_WINDOWS
				if ( s > maxfd )
					maxfd = s;
			  #endif

				++it;
			}
		}

		if ( _sockets.empty() )
			return 0;

		#ifndef AE_PLATFORM_WINDOWS
			maxfd += 1;
		#endif

		timeval		timeout;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 0;

		int ret = ::select(
						maxfd,
						&read_fds,
						null,
						null,
						&timeout
					);

		if ( ret <= 0 )
			return 0;

		usize	count = 0;

		for (auto it = _sockets.begin(); it != _sockets.end();)
		{
			if ( FD_ISSET( NativeSocket_t(it->socket), &read_fds ))
			{
				TaskApi::SetDependencyCompletionStatus( *it->task );
				it = _sockets.erase( it );
				++count;
			}
			else
				++it;
		}

		return count;
	}


} // AE::Networking
