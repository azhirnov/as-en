// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

extern void UnitTest_Array ();
extern void UnitTest_Atomic ();
extern void UnitTest_ChunkList ();
extern void UnitTest_Color ();
extern void UnitTest_Cast ();
extern void UnitTest_CPUInfo ();
extern void UnitTest_Date ();
extern void UnitTest_DataSource (const Path &curr);
extern void UnitTest_EnumSet ();
extern void UnitTest_Iterators ();
extern void UnitTest_FileSystem (const Path &curr);
extern void UnitTest_FixedArray ();
extern void UnitTest_FixedMap ();
extern void UnitTest_FixedSet ();
extern void UnitTest_FixedString ();
extern void UnitTest_FixedTupleArray ();
extern void UnitTest_FlatHashMap ();
extern void UnitTest_HashMap ();
extern void UnitTest_FlatHashSet ();
extern void UnitTest_FileWatch (const Path &curr);
extern void UnitTest_HashSet ();
extern void UnitTest_FunctionInfo ();
extern void UnitTest_LinearAllocator ();
extern void UnitTest_Math ();
extern void UnitTest_Math_BitMath ();
extern void UnitTest_Math_Fractional ();
extern void UnitTest_Math_Frustum ();
extern void UnitTest_Math_Matrix ();
extern void UnitTest_Math_PhysicalQuantity ();
extern void UnitTest_Math_Rectangle ();
extern void UnitTest_Math_SIMD ();
extern void UnitTest_Math_SimdVector ();
extern void UnitTest_Math_SimdMatrix ();
extern void UnitTest_Math_Transformation ();
extern void UnitTest_Math_Vec ();
extern void UnitTest_MemChunkList ();
extern void UnitTest_NamedID ();
extern void UnitTest_NtStringView ();
extern void UnitTest_RingBuffer ();
extern void UnitTest_RC ();
extern void UnitTest_StackAllocator ();
extern void UnitTest_Parser ();
extern void UnitTest_SourceLoc ();
extern void UnitTest_StructView ();
extern void UnitTest_StringUtils ();
extern void UnitTest_ToString ();
extern void UnitTest_TypeList ();
extern void UnitTest_TypeTraits ();

#ifdef AE_EXPERIMENTAL
  extern void UnitTest_Crypto ();
#endif


TEST_ENTRY()
{
	BEGIN_TEST();

	String	config = "\n\tConfig: ";
	#ifdef CMAKE_INTDIR
		config << CMAKE_INTDIR;
	#else
	# ifdef AE_CFG_DEBUG
		config << "Debug";
	# elif defined(AE_CFG_DEVELOP)
		config << "Develop";
	# elif defined(AE_CFG_PROFILE)
		config << "Profile";
	# elif defined(AE_CFG_RELEASE)
		config << "Release";
	# else
	#	error unknown config
	# endif
	#endif

	AE_LOGI( "Engine: "s <<  AE_ENGINE_NAME << " (" << ToString( AE_VERSION ) << ')' );
	AE_LOGI(
		"\n\tPlatform name: "s << AE_PLATFORM_NAME <<
		"\n\tCPU arch: " << AE_CPU_ARCH_NAME <<
		"\n\tCompiler: " << AE_COMPILER_NAME <<
		"\n\tCompiler ver: " << ToString(AE_COMPILER_VERSION) <<
		config <<
		"\n\tOS name: " << PlatformUtils::GetOSName() <<
		"\n\tOS ver: " << ToString(PlatformUtils::GetOSVersion()) <<
		"\n\tStack size: " << ToString(PlatformUtils::GetDefaultStackSize()) <<
		"\n\tMemory page size: "s << ToString(PlatformUtils::GetMemoryPageInfo().pageSize)
	);

	RUN_TEST( UnitTest_Array );
	RUN_TEST( UnitTest_Atomic );
	RUN_TEST( UnitTest_ChunkList );
	RUN_TEST( UnitTest_Color );
	RUN_TEST( UnitTest_Cast );
	RUN_TEST( UnitTest_Date );
	RUN_TEST( UnitTest_DataSource, curr );
	RUN_TEST( UnitTest_EnumSet );
	RUN_TEST( UnitTest_Iterators );
	RUN_TEST( UnitTest_FileSystem, curr );
	RUN_TEST( UnitTest_FileWatch, curr );
	RUN_TEST( UnitTest_FixedArray );
	RUN_TEST( UnitTest_FixedMap );
	RUN_TEST( UnitTest_FixedSet );
	RUN_TEST( UnitTest_FixedString );
	RUN_TEST( UnitTest_FixedTupleArray );
	RUN_TEST( UnitTest_FlatHashMap );
	RUN_TEST( UnitTest_HashMap );
	RUN_TEST( UnitTest_FlatHashSet );
	RUN_TEST( UnitTest_HashSet );
	RUN_TEST( UnitTest_FunctionInfo );
	RUN_TEST( UnitTest_LinearAllocator );
	RUN_TEST( UnitTest_Math );
	RUN_TEST( UnitTest_Math_BitMath );
	RUN_TEST( UnitTest_Math_Fractional );
	RUN_TEST( UnitTest_Math_Frustum );
	RUN_TEST( UnitTest_Math_Matrix );
	RUN_TEST( UnitTest_Math_PhysicalQuantity );
	RUN_TEST( UnitTest_Math_Rectangle );
	RUN_TEST( UnitTest_Math_SIMD );
	RUN_TEST( UnitTest_Math_SimdVector );
	RUN_TEST( UnitTest_Math_SimdMatrix );
	RUN_TEST( UnitTest_Math_Transformation );
	RUN_TEST( UnitTest_Math_Vec );
	RUN_TEST( UnitTest_MemChunkList );
	RUN_TEST( UnitTest_NamedID );
	RUN_TEST( UnitTest_NtStringView );
	RUN_TEST( UnitTest_RingBuffer );
	RUN_TEST( UnitTest_RC );
	RUN_TEST( UnitTest_StackAllocator );
	RUN_TEST( UnitTest_Parser );
	RUN_TEST( UnitTest_SourceLoc );
	RUN_TEST( UnitTest_StructView );
	RUN_TEST( UnitTest_StringUtils );
	RUN_TEST( UnitTest_ToString );
	RUN_TEST( UnitTest_TypeList );
	RUN_TEST( UnitTest_TypeTraits );
	RUN_TEST( UnitTest_CPUInfo );

	#ifdef AE_EXPERIMENTAL
		RUN_TEST( UnitTest_Crypto );
	#endif
	RC_TRACK_ALL_REFS(
		Base::RefCounterUtils::PrintRefs();
	)

	AE_LOGI( "Tests.Base finished" );
	return 0;
}
