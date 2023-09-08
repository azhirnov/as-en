// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/FeatureSet.h"
#include "UnitTest_Common.h"

namespace
{
    static void  FeatureSet_Test1 ()
    {
        FeatureSet  a;
        FeatureSet  b;

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
        FeatureSet  a;
        FeatureSet  b;

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
        FeatureSet  a;
        FeatureSet  b;

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
        FeatureSet  a;
        FeatureSet  b;

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


    static void  FeatureSet_Test5 ()
    {
        FeatureSet  a;
        FeatureSet  b;

        a.attachmentFragmentShadingRate = EFeature::RequireTrue;
        b.pipelineFragmentShadingRate   = EFeature::RequireTrue;

        a.fragmentShadingRates.push_back( EShadingRate::Size4x4 | EShadingRate::Samples1 );
        b.fragmentShadingRates.push_back( EShadingRate::Size2x2 | EShadingRate::Samples1_2 );

        a.Validate();
        b.Validate();

        TEST( a.IsValid() );
        TEST( b.IsValid() );

        TEST( a != b );
        TEST( b != a );
        TEST( a >= b );
        TEST( a.fragmentShadingRates.size() == 1 );
        TEST( b.fragmentShadingRates.size() == 1 );
    }


    static void  FeatureSet_Test6 ()
    {
        FeatureSet  a;
        FeatureSet  b;

        a.attachmentFragmentShadingRate = EFeature::RequireTrue;
        b.pipelineFragmentShadingRate   = EFeature::RequireTrue;

        a.fragmentShadingRates.push_back( EShadingRate::Size2x2 | EShadingRate::Samples1_2_4 );
        b.fragmentShadingRates.push_back( EShadingRate::Size2x2 | EShadingRate::Samples1 );

        a.MergeMax( b );

        a.Validate();
        TEST( a.IsValid() );

        TEST( a != b );
        TEST( b != a );
        TEST( a >= b );
        TEST( a.fragmentShadingRates.size() == 1 );
        TEST( a.fragmentShadingRates[0] == (EShadingRate::Size2x2 | EShadingRate::Samples1_2_4) );
    }


    static void  FeatureSet_Test7 ()
    {
        FeatureSet  a;
        FeatureSet  b;

        a.attachmentFragmentShadingRate = EFeature::RequireTrue;
        b.pipelineFragmentShadingRate   = EFeature::RequireTrue;

        a.fragmentShadingRates.push_back( EShadingRate::Size2x2 | EShadingRate::Samples1_2_4 );
        b.fragmentShadingRates.push_back( EShadingRate::Size2x2 | EShadingRate::Samples1_2 );

        a.MergeMin( b );

        a.Validate();
        TEST( a.IsValid() );

        TEST( a != b );
        TEST( b != a );
        TEST( a >= b );
        TEST( a.fragmentShadingRates.size() == 1 );
        TEST( a.fragmentShadingRates[0] == (EShadingRate::Size2x2 | EShadingRate::Samples1_2) );
    }
}


extern void UnitTest_FeatureSet ()
{
    const auto  h = FeatureSet::GetHashOfFS();
    ASSERT( h == FeatureSet::GetHashOfFS_Precalculated() );
    /*
    ASSERT_Eq( OffsetOf( &FeatureSet::subgroupOperations ),               8 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minSubgroupSize ),                 28 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minSubsampledArrayLayers ),        64 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minRayRecursionDepth ),            80 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minVertAmplification ),           224 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minRasterOrderGroups ),           316 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minImageArrayLayers ),            476 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minSamplerLodBias ),              660 );

    ASSERT_Eq( OffsetOf( &FeatureSet::framebufferColorSampleCounts ),   664 );
    ASSERT_Eq( OffsetOf( &FeatureSet::framebufferDepthSampleCounts ),   668 );
    ASSERT_Eq( OffsetOf( &FeatureSet::minFramebufferLayers ),           672 );
    ASSERT_Eq( OffsetOf( &FeatureSet::metalArgBufferTier ),             680 );
    ASSERT_Eq( OffsetOf( &FeatureSet::vendorIds ),                      688 );

    ASSERT_Eq( OffsetOf( &FeatureSet::devicesIds ),                     696 );

    STATIC_ASSERT( sizeof(FeatureSet) == 696 );*/

    FeatureSet_Test1();
    FeatureSet_Test2();
    FeatureSet_Test3();
    FeatureSet_Test4();
    FeatureSet_Test5();
    FeatureSet_Test6();
    FeatureSet_Test7();

    TEST_PASSED();
}
