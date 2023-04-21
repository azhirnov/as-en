// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/Utils/Threading.h"
#include "base/Utils/SourceLoc.h"

namespace AE::Base
{

	//
	// Time Profiler
	//

	struct TimeProfiler
	{
	private:
		using Clock_t		= std::chrono::high_resolution_clock;
		using TimePoint_t	= std::chrono::time_point< Clock_t >;

		String				_message;
		const TimePoint_t	_startTime	= Clock_t::now();
		const SourceLoc		_srcLoc;


	public:
		TimeProfiler (StringView name)
		{
			_message = name;
			
			CompilerBarrier( EMemoryOrder::Release );
		}


		TimeProfiler (StringView name, const char *func, const SourceLoc &loc) :
			_srcLoc{ loc }
		{
			_message << "time profiler: " << name << (name.empty() ? "" : ", ") << "function: " << func;
			
			CompilerBarrier( EMemoryOrder::Release );
		}
		

		~TimeProfiler ()
		{
			CompilerBarrier( EMemoryOrder::Acquire );

			_message << "; TIME: " << ToString( Clock_t::now() - _startTime, 3 );

			if ( not _srcLoc.file.empty() ) {
				AE_PRIVATE_LOG_I( _message, _srcLoc.file, _srcLoc.line );
			}else{
				AE_LOGI( _message );
			}
		}
	};


# if 1
#	define AE_TIMEPROFILER( /* debug_name */... ) \
		AE::Base::TimeProfiler	AE_PRIVATE_UNITE_RAW( __timeProf, __COUNTER__ ) ( \
										AE_PRIVATE_GETRAW( AE_PRIVATE_GETARG_0( "" __VA_ARGS__, "no name" )), \
										AE_FUNCTION_NAME, \
										SourceLoc_Current() )
# else
#	define AE_TIMEPROFILER( /* debug_name */... )
# endif

} // AE::Base
