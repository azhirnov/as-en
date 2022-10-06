// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Math.h"
#include "base/CompileTime/Math.h"
#include "base/Math/GLM.h"
#include "base/Platforms/CPUInfo.h"
#include "UnitTest_Common.h"

namespace
{
	static void  CheckIntrinsics ()
	{
		auto&	info = CpuArchInfo::Get();
		
		TEST( info.IsGLMSupported() );

		AE_LOGI( info.Print() );
	}


	static void  IsIntersects_Test1 ()
	{
		TEST( IsIntersects( 2, 6, 5, 8 ));
		TEST( IsIntersects( 2, 6, 0, 3 ));
		TEST( IsIntersects( 2, 6, 3, 5 ));
		TEST( not IsIntersects( 2, 6, 6, 8 ));
		TEST( not IsIntersects( 2, 6, -3, 2 ));
	}


	static void  Wrap_Test1 ()
	{
		float b0 = Wrap( 1.0f, 2.0f, 5.0f );	TEST( Equals( b0, 4.0f ));
		float b1 = Wrap( 6.0f, 2.0f, 5.0f );	TEST( Equals( b1, 3.0f ));
		float b2 = Wrap( 4.0f, 2.0f, 5.0f );	TEST( Equals( b2, 4.0f ));
		float b4 = Wrap( 1.5f, 2.0f, 5.0f );	TEST( Equals( b4, 4.5f ));
		float b5 = Wrap( 5.5f, 2.0f, 5.0f );	TEST( Equals( b5, 2.5f ));
		float b6 = Wrap( 15.0f, 0.0f, 5.0f );	TEST( Equals( b6, 0.0f ));
		float b7 = Wrap( 2.0f, -5.0f, 0.0f );	TEST( Equals( b7, -3.0f ));
		float b10 = Wrap( 3.99f, 0.0f, 4.0f );	TEST( Equals( b10, 3.99f ));
	}


	static void  Half_Test1 ()
	{
		const float		f1	= 0.f;
		const half		h1	{f1};
		const float		hf1	= float{h1};
		TEST( hf1 == f1 );
		
		const float		f2	= 1.11f;
		const half		h2	{f2};
		const float		hf2	= float{h2};
		TEST( Equals( hf2, f2, 0.0004f ));
		
		const float		f3	= -0.3456f;
		const half		h3	{f3};
		const float		hf3	= float{h3};
		TEST( Equals( hf3, f3, 0.0004f ));
	}
}


extern void UnitTest_Math ()
{
	CheckIntrinsics();
	IsIntersects_Test1();
	Wrap_Test1();
	Half_Test1();

	TEST_PASSED();
}
