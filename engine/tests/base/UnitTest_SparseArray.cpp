// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#if 0
#include "base/Containers/SparseArray.h"
#include "UnitTest_Common.h"


namespace
{
	static void  SparseArray_Test1 ()
	{
		using T = DebugInstanceCounter< int, 1 >;

		T::ClearStatistic();
		{
			SparseArray< T >	arr;

			arr.push_back( T{1} );
			arr.push_back( T{2} );
			arr.push_back( T{3} );

			TEST( arr.size() == 3 );
			TEST( arr[0] == T(1) );
			TEST( arr[1] == T(2) );
			TEST( arr[2] == T(3) );

			arr.resize( 6 );
			TEST( arr[0] == T(1) );
			TEST( arr[1] == T(2) );
			TEST( arr[2] == T(3) );
			TEST( arr[3] == T(0) );
			TEST( arr[4] == T(0) );
			TEST( arr[5] == T(0) );

			arr.resize( 2 );
			TEST( arr[0] == T(1) );
			TEST( arr[1] == T(2) );
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_SparseArray ()
{
	SparseArray_Test1();

	TEST_PASSED();
}
#endif
