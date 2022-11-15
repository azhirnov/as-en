// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/EnumBitSet.h"
#include "base/Containers/FixedString.h"
#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Worker Thread
	//

	class WorkerThread : public IThread
	{
	// types
	public:
		using ThreadMask	= EnumBitSet< EThread >;


	// variables
	private:
		std::thread				_thread;
		Atomic<uint>			_looping;
		const ThreadMask		_threadMask;
		const nanoseconds		_sleepStep;
		const nanoseconds		_maxSleepOnIdle;
		const FixedString<64>	_name;


	// methods
	public:
		WorkerThread ();
		WorkerThread (ThreadMask mask, nanoseconds sleepStep, nanoseconds maxSleepOnIdle, StringView name = "thread");

		// IThread //
		bool		Attach (uint uid)	__NE_OV;
		void		Detach ()			__NE_OV;

		StringView  DbgName ()			C_NE_OV	{ return _name; }
		usize		DbgID ()			C_NE_OV	{ return ThreadUtils::GetIntID( _thread ); }
	};


} // AE::Threading
