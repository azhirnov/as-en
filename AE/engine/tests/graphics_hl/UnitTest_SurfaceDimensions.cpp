// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphicsHL.pch.h"

namespace
{
	static void  SurfaceDimensions_Test1 ()
	{
		SurfaceDimensions	src_sd;	src_sd.SetDimensions( uint2{1900, 1200}, 0.1f );
		SurfaceDimensions	sd;		sd.CopyDimensions( src_sd );

		TEST_Eq( sd.GetMmToPixels(),	0.1f );
		TEST_Eq( sd.GetPixelsToMm(),	1.f / 0.1f );
		TEST_Eq( sd.AspectRatio(),		1900.f/1200.f );

		const auto	acc = EnabledBitCount(20);
		TEST_BitEq( sd.GetMmToViewport(),		2.f * 0.1f / float2(1900.f, 1200.f),	acc );
		TEST_BitEq( sd.GetPixelsToViewport(),	2.f / float2(1900.f, 1200.f),			acc );
		TEST_BitEq( sd.GetInvSurfaceSize(),		1.f / float2(1900.f, 1200.f),			acc );
		TEST_BitEq( sd.GetSurfaceSize(),		float2(1900.f, 1200.f),					acc );
		TEST_BitEq( sd.AspectCorrection(),		float2(1.f, 1900.f / 1200.f),			acc );
	}
}


extern void UnitTest_SurfaceDimensions ()
{
	SurfaceDimensions_Test1();

	TEST_PASSED();
}
