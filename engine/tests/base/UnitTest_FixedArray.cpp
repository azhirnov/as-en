// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/FixedArray.h"
#include "UnitTest_Common.h"


namespace
{
	static void  FixedArray_Test1 ()
	{
		FixedArray< std::shared_ptr<int>, 16 >	arr = {{
			std::make_shared<int>(0),
			std::make_shared<int>(1),
			std::make_shared<int>(2),
			std::make_shared<int>(3),
			std::make_shared<int>(4)
		}};

		std::weak_ptr<int>	elem0 = arr[0];
		std::weak_ptr<int>	elem1 = arr[1];
		std::weak_ptr<int>	elem2 = arr[2];
		std::weak_ptr<int>	elem3 = arr[3];
		std::weak_ptr<int>	elem4 = arr[4];

		arr.clear();

		TEST( elem0.expired() );
		TEST( elem1.expired() );
		TEST( elem2.expired() );
		TEST( elem3.expired() );
		TEST( elem4.expired() );
	}


	static void  FixedArray_Test2 ()
	{
		using T = DebugInstanceCounter< int, 1 >;

		T::ClearStatistic();
		{
			FixedArray< T, 8 >	arr;

			arr.resize( 3 );
			arr[0] = T(1);
			arr[1] = T(2);
			arr[2] = T(3);

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
			ASSERT( arr.data()[2] != T(3) );

			arr.insert( 1, T(10) );
			TEST( arr[0] == T(1) );
			TEST( arr[1] == T(10) );
			TEST( arr[2] == T(2) );
			
			FixedArray< T, 8 >	arr2 {arr};
			TEST( arr2[0] == T(1) );
			TEST( arr2[1] == T(10) );
			TEST( arr2[2] == T(2) );
			
			FixedArray< T, 8 >	arr3 {RVRef(arr)};
			TEST( arr3[0] == T(1) );
			TEST( arr3[1] == T(10) );
			TEST( arr3[2] == T(2) );
			TEST( arr2 == arr3 );
			
			arr = arr2;
			TEST( arr == arr2 );
			TEST( arr == arr3 );
		}
		TEST( T::CheckStatistic() );
	}


	static void  FixedArray_Test3 ()
	{
		using T = DebugInstanceCounter< int, 2 >;

		T::ClearStatistic();
		{
			FixedArray< T, 8 >	arr1 = { T(1), T(2), T(3) };
			FixedArray< T, 16 >	arr2 = { T(6), T(7), T(8), T(9), T(3), T(4) };

			arr2 = ArrayView<T>( arr1 );
			TEST( arr2 == arr1.ToArrayView() );

			//arr1 = arr1;	// error
		}
		TEST( T::CheckStatistic() );
	}


	static void  FixedArray_Test4 ()
	{
		constexpr usize	a1 = sizeof(FixedArray<char, 8>);
		constexpr usize	a2 = alignof(FixedArray<char, 8>);

		STATIC_ASSERT( a1 == (1 + 8) );
		STATIC_ASSERT( a2 == 1 );
		
		constexpr usize	b1 = sizeof(FixedArray<double, 8>);
		constexpr usize	b2 = alignof(FixedArray<double, 8>);

	#if defined(AE_PLATFORM_ANDROID) and defined(__i386__)
		STATIC_ASSERT( alignof(double) == 4 );
		STATIC_ASSERT( b1 == (4 + 8*8) );
		STATIC_ASSERT( b2 == 4 );
	#else
		STATIC_ASSERT( b1 == (8 + 8*8) );
		STATIC_ASSERT( b2 == 8 );
	#endif
	}


	static void  FixedArray_Test5 ()
	{
		using T = DebugInstanceCounter< uint, 4 >;

		T::ClearStatistic();
		{
			constexpr uint			count = 64;
			FixedArray<T, count>	arr;

			for (uint i = 0; i < count; ++i)
				arr.push_back( T{i} );

			for (; not arr.empty();)
			{
				arr.erase( 0 );

				const uint	first = count - uint(arr.size());
				for (uint i = 0; i < arr.size(); ++i)
				{
					TEST( T{i + first} == arr[i] );
				}
			}
		}
		TEST( T::CheckStatistic() );
	}
}


extern void UnitTest_FixedArray ()
{
	FixedArray_Test1();
	FixedArray_Test2();
	FixedArray_Test3();
	FixedArray_Test4();
	FixedArray_Test5();

	TEST_PASSED();
}
