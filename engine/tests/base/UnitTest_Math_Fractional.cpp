// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Fractional.h"
#include "UnitTest_Common.h"


extern void UnitTest_Math_Fractional ()
{
	FractionalI	a1 = FractionalI{1,1} + FractionalI{2,1};		TEST( a1.numerator == 3 and a1.denominator == 1 );
	FractionalI	a2 = FractionalI{5,2} + FractionalI{4,3};		TEST( a2.numerator == 23 and a2.denominator == 6 );
	FractionalI	a3 = FractionalI{3,5} - FractionalI{5,2};		TEST( a3.numerator == -19 and a3.denominator == 10 );
	FractionalI	a4 = FractionalI{3,2} * FractionalI{7,3};		TEST( a4.numerator == 7 and a4.denominator == 2 );
	FractionalI	a5 = FractionalI{5,6} / FractionalI{9,2};		TEST( a5.numerator == 5 and a5.denominator == 27 );
	
	TEST_PASSED();
}
