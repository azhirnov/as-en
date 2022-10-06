// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "threading/Common.h"

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
		const char *		_file		= null;
		const int			_line		= 0;


	public:
		TimeProfiler (StringView name)
		{
			_message = name;
			
			using namespace AE::Threading;
			CompilerFence( EMemoryOrder::Release );
		}


		TimeProfiler (StringView name, const char *func, const char *file, int line) :
			_file{ file }, _line{ line }
		{
			_message << "time profiler: " << name << (name.empty() ? "" : ", ") << "function: " << func;
			
			using namespace AE::Threading;
			CompilerFence( EMemoryOrder::Release );
		}
		

		~TimeProfiler ()
		{
			using namespace AE::Threading;
			CompilerFence( EMemoryOrder::Acquire );

			_message << "; TIME: " << ToString( Clock_t::now() - _startTime, 3 );

			if ( _file ) {
				AE_PRIVATE_LOGI( _message, _file, _line );
			}else{
				AE_LOGI( _message );
			}
		}
	};


#	define AE_TIMEPROFILER( /* debug_name */... ) \
		::AE::Base::TimeProfiler	AE_PRIVATE_UNITE_RAW( __timeProf, __COUNTER__ ) ( \
										AE_PRIVATE_GETRAW( AE_PRIVATE_GETARG_0( "" __VA_ARGS__, "no name" )), \
										AE_FUNCTION_NAME, \
										__FILE__, \
										__LINE__ ) \


}	// AE::Base
