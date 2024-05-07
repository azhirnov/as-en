// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Iterators_Test1 ()
	{
		{
			const float			temp[] = { 1.f, 2.f, 3.f, 4.f, 5.f };
			ArrayView<float>	arr {temp};
			usize				j = 0;

			for (usize i : IndicesOnly(arr))
			{
				TEST_Eq( i, j );
				++j;
			}
			TEST_Eq( j, arr.size() );

			for (usize i : ReverseIndices(arr))
			{
				--j;
				TEST_Eq( i, j );
			}
			TEST_Eq( j, 0 );
		}{
			Array<String>	arr;
			usize			j = 0;
			arr.resize( 100 );

			for (usize i : IndicesOnly(arr))
			{
				TEST_Eq( i, j );
				++j;
			}
			TEST_Eq( j, arr.size() );

			for (usize i : ReverseIndices(arr))
			{
				--j;
				TEST_Eq( i, j );
			}
			TEST_Eq( j, 0 );

			for (usize i : ReverseIndices(0))
			{
				Unused( i );
				TEST( false );
			}
		}{
			const float			temp[] = { 1.f, 2.f, 3.f, 4.f, 5.f };
			ArrayView<float>	arr {temp};
			usize				j = 0;

			for (float f : Reverse(arr))
			{
				TEST_Eq( f, temp[ arr.size()-1 - j] );
				TEST_Lt( j, arr.size() );
				j++;
			}
			TEST_Eq( j, arr.size() );

			j = 0;
			for (float f : Reverse( ArrayView<float>{arr} ))
			{
				TEST_Eq( f, temp[ arr.size()-1 - j] );
				TEST_Lt( j, arr.size() );
				j++;
			}
			TEST_Eq( j, arr.size() );
		}
	}


	static void  Iterators_Test2 ()
	{
		{
			const float			temp[] = { 1.f, 2.f, 3.f, 4.f, 5.f };
			ArrayView<float>	arr {temp};
			usize				j = 0;

			for (auto [e, i] : WithIndex(arr))
			{
				StaticAssert( IsSameTypes< decltype(e), const float >);
				StaticAssert( IsSameTypes< decltype(i), usize >);

				TEST_Eq( i, j );
				TEST_Eq( e, arr[j] );
				TEST( &e == &arr[j] );
				++j;
			}
			TEST_Eq( j, arr.size() );

			j = 0;
			for (auto [e, i] : WithIndex( ArrayView<float>{arr} ))
			{
				StaticAssert( IsSameTypes< decltype(e), const float >);
				StaticAssert( IsSameTypes< decltype(i), usize >);

				TEST_Eq( i, j );
				TEST_Eq( e, arr[j] );
				TEST( &e == &arr[j] );
				++j;
			}
			TEST_Eq( j, arr.size() );

			#if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL)
				j = 0;
				for (const auto& [e, i] : WithIndex(arr))		// error in clang
				{
					StaticAssert( IsSameTypes< decltype(e), const float >);
					StaticAssert( IsSameTypes< decltype(i), const usize >);

					TEST_Eq( i, j );
					TEST_Eq( e, arr[j] );
					TEST( &e == &arr[j] );
					++j;
				}
				TEST_Eq( j, arr.size() );
			#endif
		}
		{
			Array<float>	arr { 1.f, 2.f, 3.f, 4.f, 5.f };
			usize			j = 0;

			for (auto&& [e, i] : WithIndex(arr))
			{
				StaticAssert( IsSameTypes< decltype(e), float >);
				StaticAssert( IsSameTypes< decltype(i), usize >);

				TEST_Eq( i, j );
				TEST_Eq( e, arr[j] );
				TEST( &e == &arr[j] );
				++j;
			}
			TEST_Eq( j, arr.size() );
		}
	}


	static void  Iterators_Test3 ()
	{
		{
			usize	j = 0;
			for (usize i : IndicesOnly( 0, 100 ))
			{
				TEST_Eq( i, j );
				++j;
			}
			TEST_Eq( j, 100 );
		}
		{
			usize	j = 100;
			for (usize i : IndicesOnly( 100, 200 ))
			{
				TEST_Eq( i, j );
				++j;
			}
			TEST_Eq( j, 200 );
		}
		{
			usize	j = 100;
			for (usize i : ReverseIndices( 100 ))
			{
				--j;
				TEST_Eq( i, j );
			}
			TEST_Eq( j, 0 );
		}
	}


	static void  Iterators_Test4 ()
	{
		{
			const uint	bits	= 0x10101;
			uint		i		= 0;

			for (uint bit : BitfieldIterate( bits ))
			{
				TEST( IsSingleBitSet( bit ));
				switch ( i ) {
					case 0 :	TEST( bit == 0x1 );		break;
					case 1 :	TEST( bit == 0x100 );	break;
					case 2 :	TEST( bit == 0x10000 );	break;
				}
				++i;
			}
			TEST_Eq( i, 3 );
		}{
			const uint	bits	= 0;
			uint		i		= 0;

			for (uint bit : BitfieldIterate( bits ))
			{
				Unused( bit );
				++i;
			}
			TEST_Eq( i, 0 );
		}
	}


	static void  Iterators_Test5 ()
	{
		{
			const uint	bits	= 0x10101;
			uint		i		= 0;

			for (uint idx : BitIndexIterate(bits))
			{
				switch ( i ) {
					case 0 :	TEST_Eq( idx, 0 );	break;
					case 1 :	TEST_Eq( idx, 8 );	break;
					case 2 :	TEST_Eq( idx, 16 );	break;
				}
				++i;
			}
			TEST_Eq( i, 3 );
		}{
			const uint	bits	= 0;
			uint		i		= 0;

			for (uint idx : BitIndexIterate(bits))
			{
				Unused( idx );
				++i;
			}
			TEST_Eq( i, 0 );
		}
	}
}


extern void UnitTest_Iterators ()
{
	Iterators_Test1();
	Iterators_Test2();
	Iterators_Test3();
	Iterators_Test4();
	Iterators_Test5();

	TEST_PASSED();
}
