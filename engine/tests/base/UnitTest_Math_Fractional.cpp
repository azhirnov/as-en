// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Fractional.h"
#include "UnitTest_Common.h"


extern void UnitTest_Math_Fractional ()
{
	FractionalI	a1 = FractionalI{1,1} + FractionalI{2,1};		TEST( a1.num == 3 and a1.den == 1 );
	FractionalI	a2 = FractionalI{5,2} + FractionalI{4,3};		TEST( a2.num == 23 and a2.den == 6 );
	FractionalI	a3 = FractionalI{3,5} - FractionalI{5,2};		TEST( a3.num == -19 and a3.den == 10 );
	FractionalI	a4 = FractionalI{3,2} * FractionalI{7,3};		TEST( a4.num == 7 and a4.den == 2 );
	FractionalI	a5 = FractionalI{5,6} / FractionalI{9,2};		TEST( a5.num == 5 and a5.den == 27 );
	
	TEST_PASSED();
}
