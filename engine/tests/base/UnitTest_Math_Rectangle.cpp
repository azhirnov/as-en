// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Rectangle.h"
#include "UnitTest_Common.h"


namespace
{
	static void  Rectangle_Test1 ()
	{
		RectF	rect{ 1.0f, 1.0f, 3.0f, 4.0f };
	
		float2	p0{ 2.0f, 2.5f };		TEST( rect.Intersects( p0 ));
		float2	p1{ -1.0f, 1.1f };		TEST( not rect.Intersects( p1 ));
		float2	p2{ 1.0f, 1.0f };		TEST( rect.Intersects( p2 ));
		float2	p3{ 3.0f, 4.0f };		TEST( not rect.Intersects( p3 ));
	}


	static void  Rectangle_Test2 ()
	{
		RectF	rect{ 1.0f, 1.0f, 3.0f, 4.0f };

		RectF	r0{ 1.5f, 1.5f, 2.5f, 3.0f };		TEST( rect.Intersects( r0 ));
		RectF	r1{ 0.0f, -1.0f, 1.1f, 0.5f };		TEST( not rect.Intersects( r1 ));
		RectF	r2{ 0.0f, -1.0f, 1.1f, 1.5f };		TEST( rect.Intersects( r2 ));
	}
	

	static void  Rectangle_Test3 ()
	{
		RectI	rect{ -8, -8, 8, 8 };

		RectI	r0{ -16, -16, -8, -8 };		TEST( not rect.Intersects( r0 ));
		RectI	r1{ -8, -8, 0, 0 };			TEST( rect.Intersects( r1 ));
		RectI	r2{ 0, 0, 8, 8 };			TEST( rect.Intersects( r2 ));
		RectI	r3{ 8, 0, 16, 8 };			TEST( not rect.Intersects( r3 ));
	}
}


extern void UnitTest_Math_Rectangle ()
{
	Rectangle_Test1();
	Rectangle_Test2();
	Rectangle_Test3();

	TEST_PASSED();
}
