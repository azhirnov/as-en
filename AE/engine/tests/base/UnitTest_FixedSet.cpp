// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  FixedSet_Test1 ()
	{
		FixedSet<uint, 16>	set;
		constexpr uint		count = 10;

		for (uint i = 0; i < count; ++i)
		{
			auto[iter, ins] = set.insert( i );
			TEST( ins );
			TEST( *iter == i );
		}

		TEST( set.size() == count );

		for (uint i = 0; i < count; ++i) {
			TEST( set.find( i ) != set.end() );
		}

		uint j = 0;
		for (auto& p : set) {
			TEST( p == j );
			++j;
		}
		TEST( j == count );

		auto	iter = set.begin();
		iter = set.find( 5u );	TEST( iter != set.end() );	TEST( *iter == 5u );
		iter = set.find( 2u );	TEST( iter != set.end() );	TEST( *iter == 2u );
		iter = set.find( 7u );	TEST( iter != set.end() );	TEST( *iter == 7u );
		iter = set.find( 11u );	TEST( iter == set.end() );
	}


	static void  FixedSet_Test2 ()
	{
		FixedSet<int, 16>	set;
		constexpr uint		count = 10;

		for (uint i = 0; i < count; ++i) {
			set.insert( i );
		}

		TEST( set.size() == count );

		TEST( set.contains( 5 ));
		TEST( set.contains( 2 ));
		TEST( set.contains( 7 ));
		TEST( not set.contains( -2 ));
		TEST( not set.contains( 11 ));
	}


	static void  FixedSet_Test3 ()
	{
		using T1 = DebugInstanceCounter< int, 1 >;

		T1::ClearStatistic();
		{
			FixedSet< T1, 32 >	set;

			for (uint j = 0; j < 10; ++j)
			{
				for (int i = 0; i < 30; ++i) {
					set.insert( T1(i) );
				}

				TEST( set.size() == 30 );
				set.clear();
			}
		}
		TEST( T1::CheckStatistic() );
	}


	static void  FixedSet_Test4 ()
	{
		FixedSet<int, 32>	set;

		for (uint i = 0; i < 10; ++i) {
			set.insert( i );
		}

		TEST( set.size() == 10 );
		TEST( set.insert( 10 ).second );
		TEST( not set.insert( 1 ).second );
	}


	static void  FixedSet_Test5 ()
	{
		FixedSet<int, 32>	set1;
		FixedSet<int, 32>	set2;

		set1.insert(  1 );
		set1.insert( 11 );
		set1.insert(  6 );
		set1.insert(  9 );
		set1.insert(  3 );

		set2.insert( 11 );
		set2.insert(  3 );
		set2.insert(  6 );
		set2.insert(  1 );
		set2.insert(  9 );

		TEST( set1.size() == 5 );
		TEST( set1.size() == 5 );
		TEST( set1 == set2 );
		TEST( set1.CalcHash() == set2.CalcHash() );
	}


	static void  FixedSet_Test6 ()
	{
		using T1 = DebugInstanceCounter< int, 1 >;

		T1::ClearStatistic();
		{
			FixedSet< T1, 32 >	set;

			for (int i = 0; i < 30; ++i) {
				set.insert( T1(i) );
			}
			TEST( set.size() == 30 );

			for (int i = 20; i < 30; ++i) {
				TEST( set.erase( T1(i) ));
			}
			for (int i = 0; i < 20; ++i) {
				TEST( set.contains( T1(i) ));
			}
			for (int i = 30; i < 40; ++i) {
				TEST( set.insert( T1(i) ).second );
			}

			for (int i = 10; i < 20; ++i) {
				TEST( set.erase( T1(i) ));
			}
			for (int i = 0; i < 10; ++i) {
				TEST( set.contains( T1(i) ));
			}
			for (int i = 30; i < 40; ++i) {
				TEST( set.contains( T1(i) ));
			}
			for (int i = 40; i < 50; ++i) {
				TEST( set.insert( T1(i) ).second );
			}

			for (int i = 0; i < 10; ++i) {
				TEST( set.erase( T1(i) ));
			}
			for (int i = 30; i < 50; ++i) {
				TEST( set.contains( T1(i) ));
			}
			for (int i = 30; i < 50; ++i) {
				TEST( set.erase( T1(i) ));
			}

			TEST( set.empty() );
		}
		TEST( T1::CheckStatistic() );
	}


	static void  FixedSet_Test7 ()
	{
		struct alignas(64) Elem {};

		using Set_t = FixedSet< Elem, 8 >;

		StaticAssert( alignof(Set_t) == alignof(Elem) );
	}
}


extern void UnitTest_FixedSet ()
{
	FixedSet_Test1();
	FixedSet_Test2();
	FixedSet_Test3();
	FixedSet_Test4();
	FixedSet_Test5();
	FixedSet_Test6();
	FixedSet_Test7();

	TEST_PASSED();
}
