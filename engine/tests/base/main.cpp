// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Platforms/Platform.h"
using namespace AE;
using namespace AE::Base;


extern void UnitTest_Array ();
//extern void UnitTest_ChunkedArray ();
extern void UnitTest_Color ();
extern void UnitTest_Cast ();
extern void UnitTest_Crypto ();
extern void UnitTest_Date ();
extern void UnitTest_EnumBitSet ();
extern void UnitTest_FileSystem ();
extern void UnitTest_FixedArray ();
extern void UnitTest_FixedMap ();
extern void UnitTest_FixedSet ();
extern void UnitTest_FixedTupleArray ();
extern void UnitTest_FlatHashMap ();
extern void UnitTest_HashMap ();
extern void UnitTest_FlatHashSet ();
extern void UnitTest_HashSet ();
extern void UnitTest_FunctionInfo ();
extern void UnitTest_LinearAllocator ();
extern void UnitTest_Math ();
extern void UnitTest_Math_BitMath ();
extern void UnitTest_Math_Fractional ();
extern void UnitTest_Math_Matrix ();
extern void UnitTest_Math_PhysicalQuantity ();
extern void UnitTest_Math_Rectangle ();
extern void UnitTest_Math_SIMD ();
extern void UnitTest_Math_Transformation ();
extern void UnitTest_Math_Vec ();
extern void UnitTest_NamedID ();
extern void UnitTest_NtStringView ();
extern void UnitTest_RingBuffer ();
extern void UnitTest_StackAllocator ();
extern void UnitTest_StaticString ();
extern void UnitTest_Stream ();
extern void UnitTest_StringParser ();
extern void UnitTest_StringToID ();
extern void UnitTest_StructView ();
extern void UnitTest_ToString ();
extern void UnitTest_TypeList ();
extern void UnitTest_TypeTraits ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_Base ()
#else
int main ()
#endif
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	AE_LOGI( "\nPlatform name: "s << AE_PLATFORM_NAME <<
			 "\nCPU arch: " << AE_CPU_ARCH_NAME );
	
	UnitTest_Array();
	//UnitTest_ChunkedArray();
	UnitTest_Color();
	UnitTest_Cast();
	UnitTest_Crypto();
	UnitTest_Date();
	UnitTest_EnumBitSet();
	UnitTest_FileSystem();
	UnitTest_FixedArray();
	UnitTest_FixedMap();
	UnitTest_FixedSet();
	UnitTest_FixedTupleArray();
	UnitTest_FlatHashMap();
	UnitTest_HashMap();
	UnitTest_FlatHashSet();
	UnitTest_HashSet();
	UnitTest_FunctionInfo();
	UnitTest_LinearAllocator();
	UnitTest_Math();
	UnitTest_Math_BitMath();
	UnitTest_Math_Fractional();
	UnitTest_Math_Matrix();
	UnitTest_Math_PhysicalQuantity();
	UnitTest_Math_Rectangle();
	UnitTest_Math_SIMD();
	UnitTest_Math_Transformation();
	UnitTest_Math_Vec();
	UnitTest_NamedID();
	UnitTest_NtStringView();
	UnitTest_RingBuffer();
	UnitTest_StackAllocator();
	UnitTest_StaticString();
	UnitTest_Stream();
	UnitTest_StringParser();
	UnitTest_StructView();
	UnitTest_StringToID();
	UnitTest_ToString();
	UnitTest_TypeList();
	UnitTest_TypeTraits();
	
	AE_LOGI( "Tests.Base finished" );
	return 0;
}
