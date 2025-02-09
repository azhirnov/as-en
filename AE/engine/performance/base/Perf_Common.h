// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "../../tests/shared/UnitTest_Shared.h"
#include "../../tests/shared/IntervalProfiler.h"


template <typename FN>
inline void  ForEachCoreType (FN &&fn)
{
	auto&	cpu_info = CpuArchInfo::Get();
	//AE_LOGI( cpu_info.Print() );

	for (auto core_type : IndicesOnly<ECoreType>())
	{
		if ( auto* core = cpu_info.GetCore( core_type ))
		{
			auto	SetAffinity = [core, bits = core->logicalBits.to_ullong()] () mutable
			{{
				ThreadUtils::MilliSleep( seconds{1} );
				#ifdef AE_PLATFORM_APPLE
					EThreadPriority	p = EThreadPriority::Default;
					switch ( core->type )
					{
						case ECoreType::HP :
						case ECoreType::P :		p = EThreadPriority::PerFrame;			break;
						case ECoreType::EE :	p = EThreadPriority::BackgroundLow;		break;
					}
					ThreadUtils::SetPriority( p );
				#elif 1
					for (uint i = 0;; ++i)
					{
						if ( auto idx = ExtractBitIndex( INOUT bits ); idx >= 0 ) {
							if ( ThreadUtils::SetAffinity( idx ))
								break;
						}else{
							bits = core->logicalBits.to_ullong();
							CHECK_FATAL( i < 100 );
						}
					}
				#else
					// always use the same core
					CHECK_FATAL( ThreadUtils::SetAffinity( core->FirstLogicalCore() ));
				#endif
			}};
			SetAffinity();

			fn( core_type, SetAffinity );
		}
	}
}
