// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Timer
	//

	struct Timer
	{
	// types
	public:
		using Clock_t		= std::chrono::high_resolution_clock;
		using TimePoint_t	= Clock_t::time_point;
		using Duration_t	= Clock_t::duration;

	private:
		struct Result
		{
		private:
			Duration_t	_dt;
			bool		_ok;

		public:
			Result (Duration_t dt, bool ok)					__NE___ : _dt{dt}, _ok{ok} {}

			ND_ explicit operator bool ()					C_NE___	{ return _ok; }

			ND_ bool  operator ! ()							C_NE___	{ return not _ok; }

			template <typename R, typename P>
			ND_ operator std::chrono::duration<R,P> ()		C_NE___	{ return _dt; }

			template <typename ToDuration>
			ND_ ToDuration  As ()							C_NE___	{ return TimeCast<ToDuration>( _dt ); }
		};


	// variables
	private:
		TimePoint_t		_lastTick;
		Duration_t		_interval		{0};


	// methods
	public:
		Timer ()														__NE___	{}

		template <typename R, typename P>
		explicit Timer (std::chrono::duration<R,P> interval)			__NE___ { Start( interval ); }


		template <typename R, typename P>
			void  Start (std::chrono::duration<R,P> interval)			__NE___	{ Start( Clock_t::now(), interval ); }

		template <typename R, typename P>
			void  Start (TimePoint_t				lastTick,
						 std::chrono::duration<R,P>	interval)			__NE___;

		template <typename R, typename P>
			void  StartSignaled (std::chrono::duration<R,P> interval)	__NE___;

			void  Restart (TimePoint_t now = Clock_t::now())			__NE___;

		ND_ auto  Tick (TimePoint_t now = Clock_t::now())				__NE___;

		ND_ auto  Now ()												C_NE___	{ return _lastTick; }
		ND_ auto  Interval ()											C_NE___	{ return _interval; }
		ND_ bool  IsActive ()											C_NE___	{ return _interval.count() > Zero; }
	};


/*
=================================================
	Start
=================================================
*/
	template <typename R, typename P>
	void  Timer::Start (TimePoint_t lastTick, std::chrono::duration<R,P> interval) __NE___
	{
		_lastTick	= lastTick;
		_interval	= TimeCast<Duration_t>( interval );

		ASSERT( IsActive() );
	}

/*
=================================================
	StartSignaled
=================================================
*/
	template <typename R, typename P>
	void  Timer::StartSignaled (std::chrono::duration<R,P> interval) __NE___
	{
		_interval	= TimeCast<Duration_t>( interval );
		_lastTick	= Clock_t::now() - _interval;

		ASSERT( IsActive() );
	}

/*
=================================================
	Restart
=================================================
*/
	inline void  Timer::Restart (TimePoint_t now) __NE___
	{
		ASSERT( IsActive() );
		_lastTick = now;
	}

/*
=================================================
	Tick
=================================================
*/
	inline auto  Timer::Tick (TimePoint_t now) __NE___
	{
		ASSERT( IsActive() );

		const Duration_t	dt	= now - _lastTick;
		const bool			ok	= dt >= _interval;

		if ( ok )
			_lastTick = now;

		return Result{ dt, ok };
	}


} // AE::Base
