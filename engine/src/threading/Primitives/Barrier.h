// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	AE_BARRIER_MODE:
		0 - WinAPI native barrier implementation, requires Windows 8 desctop.
		1 - implementation based only on atomics.
		2 - implementation based on boost::fibers::barrier, shows same performance as native WinAPI barrier.
		3 - wrapper around std::barrier, requires C++20.
*/

#pragma once

#include "threading/Common.h"

#ifdef AE_STD_BARRIER
#	define AE_BARRIER_MODE	3
#elif defined(AE_WINDOWS_TARGET_VERSION) and (AE_WINDOWS_TARGET_VERSION >= 8)
#	define AE_BARRIER_MODE	0
#else
#	define AE_BARRIER_MODE	2
#endif


#if (AE_BARRIER_MODE == 0)

namespace AE::Threading
{

	//
	// Barrier (requires Windows 8)
	//

	struct Barrier
	{
	// variables
	private:
		alignas(ulong) ubyte	_data [32];


	// methods
	public:
		explicit Barrier (uint numThreads);
		~Barrier ();

		Barrier (Barrier &&) = delete;
		Barrier (const Barrier &) = delete;

		Barrier& operator = (const Barrier &) = delete;
		Barrier& operator = (Barrier &&) = delete;

		void wait ();
	};

}	// AE::Threading


#elif (AE_BARRIER_MODE == 1)

namespace AE::Threading
{

	//
	// Barrier
	//

	struct alignas(AE_CACHE_LINE) Barrier
	{
	// types
	private:
		struct Bitfield {
			uint	counter_1	: 15;
			uint	counter_2	: 15;
			uint	index		: 1;
		};

	
	// variables
	private:
		Atomic< Bitfield >	_counter;
		const uint			_numThreads;


	// methods
	public:
		explicit Barrier (uint numThreads) :
			_counter{Bitfield{ 0, 0, 0 }}, _numThreads{numThreads}
		{
			ASSERT( numThreads > 0 );
		}

		Barrier (Barrier &&) = delete;
		Barrier (const Barrier &) = delete;

		Barrier& operator = (const Barrier &) = delete;
		Barrier& operator = (Barrier &&) = delete;

		void wait ()
		{
			// flush cache
			ThreadFence( EMemoryOrder::Release );

			const Bitfield	old_value	= _counter.load();
			Bitfield		expected	= old_value;
			Bitfield		new_value	= old_value;

			// increment counter
			old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;

			for (; not _counter.CAS( INOUT expected, new_value );)
			{
				new_value = expected;
				old_value.index ? ++new_value.counter_2 : ++new_value.counter_1;
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

			for (uint i = 0;
				 not _counter.CAS( INOUT expected, new_value );
				 ++i)
			{
				if ( expected.index != old_value.index )
					break;
				
				old_value.index ? (expected.counter_2 = _numThreads) : (expected.counter_1 = _numThreads);

				if ( i > 1000 ) {
					i = 0;
					ThreadUtils::Yield();
				}
			}

			// invalidate cache
			ThreadFence( EMemoryOrder::Acquire );
		}
	};

}	// AE::Threading


#elif (AE_BARRIER_MODE == 2)

namespace AE::Threading
{

	//
	// Barrier (based on boost::fibers::barrier)
	//

	struct Barrier
	{
	// variables
	private:
		uint					_value;
		uint					_cycle;
		const uint				_numThreads;
		Mutex					_mutex;
		std::condition_variable	_cv;


	// methods
	public:
		explicit Barrier (uint numThreads) :
			_value{numThreads}, _cycle{0}, _numThreads{numThreads}
		{
			ASSERT( numThreads > 0 );
		}

		~Barrier ()
		{}

		Barrier (Barrier &&) = delete;
		Barrier (const Barrier &) = delete;

		Barrier& operator = (const Barrier &) = delete;
		Barrier& operator = (Barrier &&) = delete;

		void wait ()
		{
			std::unique_lock	lock{ _mutex };

			if ( (--_value) == 0 )
			{
				++_cycle;
				_value = _numThreads;

				lock.unlock();
				_cv.notify_all();
				return;
			}

			_cv.wait( lock, [this, cycle = _cycle] () { return cycle != _cycle; });
		}
	};

}	// AE::Threading


#elif (AE_BARRIER_MODE == 3)

namespace AE::Threading
{

	//
	// Barrier (wraps std::barrier or std::experimental::barrier)
	//

	struct Barrier
	{
	// variables
	private:
		std::barrier	_barrier;


	// methods
	public:
		explicit Barrier (uint numThreads) : _barrier{numThreads}
		{}

		Barrier (Barrier &&) = delete;
		Barrier (const Barrier &) = delete;

		Barrier& operator = (const Barrier &) = delete;
		Barrier& operator = (Barrier &&) = delete;

		void wait ()
		{
			_barrier.arrive_and_wait();
		}
	};

}	// AE::Threading

#else
#	error not supported!

#endif	// AE_BARRIER_MODE


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_BARRIER_MODE == 0
#	pragma detect_mismatch( "AE_BARRIER_MODE", "0" )
#  elif AE_BARRIER_MODE == 1
#	pragma detect_mismatch( "AE_BARRIER_MODE", "1" )
#  elif AE_BARRIER_MODE == 2
#	pragma detect_mismatch( "AE_BARRIER_MODE", "2" )
#  elif AE_BARRIER_MODE == 3
#	pragma detect_mismatch( "AE_BARRIER_MODE", "3" )
#  else
#	error fix me!
#  endif

#endif	// AE_CPP_DETECT_MISMATCH
