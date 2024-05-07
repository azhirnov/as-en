// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
#endif

#include "threading/Primitives/Barrier.h"

#if (AE_BARRIER_MODE == 0)
namespace AE::Threading
{
	#if not (AE_PLATFORM_TARGET_VERSION_MAJOR >= 8)
	#	error Required Win8+
	#endif

/*
=================================================
	constructor
=================================================
*/
	Barrier::Barrier (usize numThreads) __NE___
	{
		CHECK( ::InitializeSynchronizationBarrier( OUT _data.Ptr<SYNCHRONIZATION_BARRIER>(), CheckCast<LONG>(numThreads), -1 ));  // win8
	}

/*
=================================================
	destructor
=================================================
*/
	Barrier::~Barrier () __NE___
	{
		CHECK( ::DeleteSynchronizationBarrier( _data.Ptr<SYNCHRONIZATION_BARRIER>() ));  // win8
	}

/*
=================================================
	Wait
=================================================
*/
	void  Barrier::Wait () __NE___
	{
		::EnterSynchronizationBarrier( _data.Ptr<SYNCHRONIZATION_BARRIER>(), SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE );  // win8
	}

} // AE::Threading
#endif // AE_BARRIER_MODE == 0
//-----------------------------------------------------------------------------


#if (AE_BARRIER_MODE == 1)
namespace AE::Threading
{
/*
=================================================
	Wait
=================================================
*/
	void  Barrier::Wait () __NE___
	{
		// flush cache
		MemoryBarrier( EMemoryOrder::Release );

		const Bitfield	old_value	= _counter.load();
		Bitfield		expected	= old_value;
		Bitfield		new_value	= old_value;

		// increment counter
		old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;

		for (; not _counter.CAS( INOUT expected, new_value );)
		{
			new_value = expected;
			old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;

			ThreadUtils::Pause();
		}


		// wait for other threads
		new_value.index = ~old_value.index;
		if ( old_value.index ) {
			new_value.counter_1 = expected.counter_1;
			new_value.counter_2 = 0;
			expected.counter_2  = _numThreads;
		}else{
			new_value.counter_1 = 0;
			new_value.counter_2 = expected.counter_2;
			expected.counter_1  = _numThreads;
		}

		for (uint p = 0;; ++p)
		{
			for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
			{
				if_likely(	_counter.CAS( INOUT expected, new_value ) or
							expected.index != old_value.index )
				{
					// invalidate cache and return
					MemoryBarrier( EMemoryOrder::Acquire );
					return;
				}

				old_value.index ? (expected.counter_2 = _numThreads) : (expected.counter_1 = _numThreads);

				ThreadUtils::Pause();
			}
			ThreadUtils::ProgressiveSleepInf( p );
		}
	}

} // AE::Threading
#endif // AE_BARRIER_MODE == 1
