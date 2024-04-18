// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_Ranges ();
extern void Test_SpinLock ();
extern void Test_LfIndexedPool2 ();
extern void Test_LfIndexedPool3 ();
extern void Test_LfStaticPool ();
extern void Test_LfLinearAllocator ();


int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope	log{};

	#ifdef AE_DEBUG
		return 0;
	#endif

	UnitTest_Ranges();
	Test_SpinLock();
	Test_LfIndexedPool2();
	Test_LfIndexedPool3();
	Test_LfStaticPool();
	Test_LfLinearAllocator();

	// TODO:
	//	LfFixedBlockAllocator
	//	LfStaticQueue
	//	LfStaticIndexedPool

	return 0;
}
