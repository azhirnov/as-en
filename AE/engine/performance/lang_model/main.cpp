// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

extern void Perf_LLamaTokensPerSecond ();
extern void Perf_LLamaContextSize ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Perf_LangModel (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	std::filesystem::current_path( Path{argv[0]}.parent_path() );

	StaticLogger::LoggerScope log{0};
	StaticLogger::AddLogger( ILogger::CreateConsoleOutput() );
	StaticLogger::AddLogger( ILogger::CreateIDEOutput() );

	#ifdef AE_PLATFORM_WINDOWS
		CHECK( WindowsUtils::SetSystemSleepState( ESystemSleepState::DontSleep_AllowTurnDisplayOff ));
	#endif

	Perf_LLamaTokensPerSecond();
//	Perf_LLamaContextSize();

	AE_LOGI( "PerformanceTests.LangModel finished" );
	return 0;
}
