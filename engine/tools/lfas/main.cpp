// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_Ranges ();
extern void Test_SpinLock ();
extern void Test_LfIndexedPool2 ();
extern void Test_LfIndexedPool3 ();
extern void Test_LfStaticPool ();


int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope	log{};

	UnitTest_Ranges();
	Test_SpinLock();
	Test_LfIndexedPool2();
	Test_LfIndexedPool3();
	Test_LfStaticPool();

	// TODO:
	//	LfFixedBlockAllocator, LfFixedBlockAllocator3
	//	LfStaticQueue
	//	LfStaticIndexedPool
}
