// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

extern void PerfTest_HashSet ();
extern void PerfTest_HashMap ();
extern void PerfTest_MinSleepTime ();
extern void PerfTest_Utf8 ();
extern void PerfTest_FindSubString ();
extern void PerfTest_LogicOp ();
extern void PerfTest_SIMD ();
extern void PerfTest_SIMD2 ();
extern void PerfTest_SIMD3 ();
extern void PerfTest_VertSIMD ();
extern void PerfTest_SimdGEMM ();
extern void PerfTest_SimdGEMV ();
extern void PerfTest_MemCopy ();
extern void PerfTest_CacheSize ();
extern void PerfTest_Compression ();
extern void PerfTest_RadixSort ();


TEST_ENTRY()
{
  #ifdef AE_RELEASE
	BEGIN_TEST();

	Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));

	//test_name = "PerfTest_SimdGEMV";

	AE_LOGI( CpuArchInfo::Get().Print() );

	RUN_TEST( PerfTest_HashSet );
	RUN_TEST( PerfTest_HashMap );

	RUN_TEST( PerfTest_MinSleepTime );
	RUN_TEST( PerfTest_Utf8 );
	RUN_TEST( PerfTest_FindSubString );
	RUN_TEST( PerfTest_LogicOp );			// compare logic ops with bit ops for boolean expressions

	RUN_TEST( PerfTest_SIMD );				// measure FLOPS, without any memory access
	RUN_TEST( PerfTest_SIMD2 );				// measure FLOPS with memory access
	RUN_TEST( PerfTest_SIMD3 );				// measure FLOPS with mixed SIMD & scalar
	RUN_TEST( PerfTest_VertSIMD );
	RUN_TEST( PerfTest_SimdGEMM );			// mat += mat * mat
	RUN_TEST( PerfTest_SimdGEMV );			// vec += vec * mat

	RUN_TEST( PerfTest_MemCopy );			// find fastest memcpy implementation
	RUN_TEST( PerfTest_CacheSize );			// measure cache sizes

	RUN_TEST( PerfTest_Compression );		// compare compression algorithms
	RUN_TEST( PerfTest_RadixSort );

	AE_LOGI( "PerformanceTests.Base finished" );

  #else
	Unused( argc, argv );
  #endif

	return 0;
}
