// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "CPP_VM/VirtualMachine.h"

namespace LFAS::CPP
{

	//
	// Mutex
	//

	class Mutex
	{
	// variables
	private:
		std::recursive_mutex	_guard;
		int						_count	= 0;


	// methods
	public:

		bool  try_lock ()
		{
			bool	res = _guard.try_lock();
			VirtualMachine::Instance().ThreadFenceAcquireRelease();
			return res;
		}

		void  lock ()
		{
			_guard.lock();

			if ( _count++ ) {
				AE_LOGE( "Recursive mutex lock!" );
			}

			VirtualMachine::Instance().ThreadFenceAcquireRelease();
		}

		void  unlock ()
		{
			VirtualMachine::Instance().ThreadFenceAcquireRelease();

			--_count;
			ASSERT( _count >= 0 );

			_guard.unlock();
		}
	};


} // LFAS::CPP
