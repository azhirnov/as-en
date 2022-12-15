// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/FeatureSet.h"
#include "UnitTest_Common.h"

namespace
{
	static void  FeatureSet_Test1 ()
	{
		FeatureSet	a;
		FeatureSet	b;

		a.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );
		b.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );

		a.Validate();
		b.Validate();

		TEST( a.IsValid() );
		TEST( a == b );
		TEST( b == a );
		TEST( a >= b );
		TEST( a.CalcHash() == b.CalcHash() );
	}
	

	static void  FeatureSet_Test2 ()
	{
		FeatureSet	a;
		FeatureSet	b;

		a.minShaderVersion.spirv = 140;
		a.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );
		b.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );

		a.Validate();
		b.Validate();
		
		TEST( a.IsValid() );
		TEST( b.IsValid() );
		TEST( a != b );
		TEST( b != a );
		TEST( a >= b );
		TEST( not (b >= a) );
	}
	

	static void  FeatureSet_Test3 ()
	{
		FeatureSet	a;
		FeatureSet	b;
		
		a.attachmentFormats.insert( EPixelFormat::RGBA8_SNorm );
		a.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );
		a.attachmentFormats.insert( EPixelFormat::RGBA8U );

		b.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );
		
		a.Validate();
		b.Validate();

		TEST( a.IsValid() );
		TEST( b.IsValid() );
		TEST( a != b );
		TEST( b != a );
		TEST( a >= b );
		TEST( not (b >= a) );
	}
	

	static void  FeatureSet_Test4 ()
	{
		FeatureSet	a;
		FeatureSet	b;
		
		a.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );
		b.attachmentFormats.insert( EPixelFormat::RGBA8_UNorm );

		a.geometryShader = EFeature::RequireTrue;
		
		a.Validate();
		b.Validate();

		TEST( a.IsValid() );
		TEST( b.IsValid() );
		TEST( a == b );
		TEST( b == a );

		b.geometryShader = EFeature::RequireFalse;
		
		TEST( a.IsValid() );
		TEST( b.IsValid() );
		TEST( a != b );
		TEST( b != a );
		TEST( a >= b );
		TEST( not (b >= a) );
		
		a.geometryShader = EFeature::Ignore;
		
		TEST( a.IsValid() );
		TEST( b.IsValid() );
		TEST( a == b );
		TEST( b == a );
		TEST( a >= b );
		TEST( b >= a );
	}
}


extern void UnitTest_FeatureSet ()
{
	ASSERT_Eq( OffsetOf( &FeatureSet::subgroupOperations ),			8 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minSubgroupSize ),			28 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minSubsampledArrayLayers ),	60 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minRayRecursionDepth ),		76 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minVertAmplification ),		220 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minRasterOrderGroups ),		312 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minImageArrayLayers ),		468 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minSamplerLodBias ),			624 );
	
	ASSERT_Eq( OffsetOf( &FeatureSet::framebufferColorSampleCounts ),	628 );
	ASSERT_Eq( OffsetOf( &FeatureSet::framebufferDepthSampleCounts ),	632 );
	ASSERT_Eq( OffsetOf( &FeatureSet::minFramebufferLayers ),			636 );
	ASSERT_Eq( OffsetOf( &FeatureSet::metalArgBufferTier ),				644 );
	ASSERT_Eq( OffsetOf( &FeatureSet::vendorIds ),						652 );

	ASSERT_Eq( OffsetOf( &FeatureSet::devicesIds ),					664 );

	STATIC_ASSERT( sizeof(FeatureSet) == 680 );

	FeatureSet_Test1();
	FeatureSet_Test2();
	FeatureSet_Test3();
	FeatureSet_Test4();

	TEST_PASSED();
}
