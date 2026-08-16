// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

extern void Perf_LLamaTokensPerSecond ();
extern void Perf_LLamaContextSize ();


TEST_ENTRY()
{
	BEGIN_TEST();

	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );

	CHECK( PlatformUtils::SetSystemSleepState( ESystemSleepState::DontSleep_AllowTurnDisplayOff ));

	RUN_TEST( Perf_LLamaTokensPerSecond );
	RUN_TEST( Perf_LLamaContextSize );

	AE_LOGI( "PerformanceTests.LangModel finished" );
	return 0;
}
