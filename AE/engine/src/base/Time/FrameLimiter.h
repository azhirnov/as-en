// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Platforms/ThreadUtils.h"

namespace AE::Base
{

	//
	// Frame Limiter
	//

	struct FrameLimiter
	{
	// types
	public:
		using Clock_t		= HighResClock;
		using TimePoint_t	= typename Clock_t::time_point;
		using Duration_t	= typename Clock_t::duration;


	// variables
	private:
		Duration_t		_targetDuration;
		TimePoint_t		_frameStart;


	// methods
	public:
		FrameLimiter ()											__NE___ {}

		void  SetTargetFPS (int fps)							__NE___	{ SetTargetFPS( float(fps) ); }
		void  SetTargetFPS (float fps)							__NE___	{ SetTargetDuration( TimeCast<Duration_t>( secondsf{1.f / fps} )); }
		void  SetTargetDuration (Duration_t dt)					__NE___	{ _targetDuration = dt + ThreadUtils::MicroSleepTimeStep(); }

		void  FrameStart (TimePoint_t tp = Clock_t::now())		__NE___	{ _frameStart = tp; }
		void  FrameEnd (TimePoint_t tp = Clock_t::now())		__NE___;
	};



	inline void  FrameLimiter::FrameEnd (TimePoint_t tp) __NE___
	{
		Duration_t	dt = tp - _frameStart;

		if ( dt > _targetDuration )
		{
			dt -= _targetDuration;
			dt = Min( dt, Duration_t{milliseconds{100}} );

			ThreadUtils::MicroSleep( dt );
		}
	}

} // AE::Base
