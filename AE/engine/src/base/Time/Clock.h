// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Clock
	//

	struct Clock
	{
	// types
	public:
		using Clock_t		= HighResClock;
		using TimePoint_t	= typename Clock_t::time_point;
		using Duration_t	= typename Clock_t::duration;


	// variables
	private:
		TimePoint_t		_lastTick;
		TimePoint_t		_start;


	// methods
	public:
		Clock ()												__NE___ { Start(); }
		explicit Clock (TimePoint_t lastTick)					__NE___ { Start( lastTick ); }

			void		Start (TimePoint_t lastTick)			__NE___	{ _start = _lastTick = lastTick; }
			void		Start ()								__NE___	{ _start = _lastTick = Clock_t::now(); }

		template <typename ToDuration>
		ND_ ToDuration	Tick (TimePoint_t now = Clock_t::now())	__NE___	{ return TimeCast<ToDuration>( Tick( now )); }
		ND_ Duration_t	Tick (TimePoint_t now = Clock_t::now())	__NE___;

		template <typename ToDuration>
		ND_ ToDuration	TimeSince ()							C_NE___	{ return TimeCast<ToDuration>( TimeSince() ); }
		ND_ Duration_t	TimeSince ()							C_NE___	{ return Clock_t::now() - _lastTick; }

		template <typename ToDuration>
		ND_ ToDuration	TimeSinceStart ()						C_NE___	{ return TimeCast<ToDuration>( TimeSinceStart() ); }
		ND_ Duration_t	TimeSinceStart ()						C_NE___	{ return _lastTick - _start; }

		ND_ TimePoint_t	Now ()									C_NE___	{ return _lastTick; }
	};


/*
=================================================
	Tick
----
	returns duration from previous 'Tick()'
=================================================
*/
	inline Clock::Duration_t  Clock::Tick (TimePoint_t now) __NE___
	{
		const auto	dt = now - _lastTick;
		_lastTick = now;
		return dt;
	}


} // AE::Base
