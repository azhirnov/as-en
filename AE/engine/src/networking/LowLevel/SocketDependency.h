// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Check which sockets has new data and mark dependency as complete, so task may run and read from socket.

	co_await Tuple{SocketDependency{...}}		- will cancel current task on timeout
	co_await Tuple{WeakSocketDependency{...}}	- will resume current task on timeout
*/

#pragma once

#include "networking/LowLevel/BaseSocket.h"

namespace AE::Networking
{

	//
	// Socket Dependency
	//

	struct SocketDependency
	{
	public:
	  #ifdef AE_DEBUG
		static constexpr auto		c_Timeout	= seconds{60 * 30};
	  #else
		static constexpr auto		c_Timeout	= seconds{10};
	  #endif

	private:
		BaseSocket const*			_socket;
		HighResClock::time_point	_endTime;
		DEBUG_ONLY( SourceLoc		_loc;)

	public:
		explicit SocketDependency (BaseSocket const &soc,
								   const SourceLoc	&loc = SourceLoc::current())		__NE___ : SocketDependency{ soc, c_Timeout, loc } {}

		SocketDependency (BaseSocket const	&soc,
						  nanoseconds		timeout,
						  const SourceLoc	&loc = SourceLoc::current())				__NE___ : _socket{&soc}, _endTime{HighResClock::now() + timeout} DEBUG_ONLY( , _loc{loc}) { Unused( loc ); }

		SocketDependency (BaseSocket const			&soc,
						  HighResClock::time_point	endTime,
						  const SourceLoc			&loc = SourceLoc::current())		__NE___ : _socket{&soc}, _endTime{endTime} DEBUG_ONLY(, _loc{loc}) { Unused( loc ); }

		ND_ BaseSocket const&			GetSocket ()									C_NE___	{ return *_socket; }
		ND_ HighResClock::time_point	EndTime ()										C_NE___	{ return _endTime; }
		DEBUG_ONLY( ND_ SourceLoc		SourceLocation ()								C_NE___	{ return _loc; })
	};


	struct WeakSocketDependency : SocketDependency
	{
		explicit WeakSocketDependency (BaseSocket const &soc,
									   const SourceLoc	&loc = SourceLoc::current())	__NE___ : SocketDependency{ soc, loc } {}

		WeakSocketDependency (BaseSocket const	&soc,
							  nanoseconds		timeout,
							  const SourceLoc	&loc = SourceLoc::current())			__NE___ : SocketDependency{ soc, timeout, loc } {}

		WeakSocketDependency (BaseSocket const			&soc,
							  HighResClock::time_point	endTime,
							  const SourceLoc			&loc = SourceLoc::current())	__NE___ : SocketDependency{ soc, endTime, loc } {}
	};



	//
	// Socket Dependency Manager
	//

	class SocketDependencyManager final : public Threading::ITaskDependencyManager
	{
	// types
	private:
		struct Dependency
		{
			BaseSocket::Socket_t		socket;
			bool						isStrong;
			HighResClock::time_point	endTime;
			Threading::AsyncTask		task;
			DEBUG_ONLY( SourceLoc		loc;)
		};

		using DependencyArr_t	= Array< Dependency >;


		class NetworkIOService final : public Threading::IOService
		{
		private:
			SocketDependencyManager &	_mngr;
			DependencyArr_t				_sockets;
			Threading::SpinLock			_guard;

		public:
			NetworkIOService (SocketDependencyManager &mngr)	__NE___;

		// IOService //
			usize						ProcessEvents ()		__NE_OV;
			Threading::EIOServiceType	GetIOServiceType ()		C_NE_OV	{ return Threading::EIOServiceType::Network; }
		};


	// variables
	private:
		Threading::Synchronized<
			Threading::RWSpinLock,
			DependencyArr_t >			_pendingSockets;


	// methods
	public:
		SocketDependencyManager ()										__NE___;

		ND_ static bool  Register ()									__NE___;

		// ITaskDependencyManager //
		bool  Resolve (AnyTypeCRef dep, Task &, Bool defaultIsStrongDep)__NE_OV;

	};


} // AE::Networking
