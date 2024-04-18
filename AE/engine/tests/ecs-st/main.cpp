// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_Archetype ();
extern void UnitTest_EntityPool ();
extern void UnitTest_Registry ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_ECSst ()
#else
int main ()
#endif
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	UnitTest_Archetype();
	UnitTest_EntityPool();
	UnitTest_Registry();

	AE_LOGI( "Tests.ECS finished" );
	return 0;
}
