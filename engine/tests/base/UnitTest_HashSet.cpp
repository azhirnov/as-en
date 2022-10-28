// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	template <template <typename V> class Set>
	static void  HashSet_Test1 (bool haveSameOrder)
	{
		Set<uint>		set;
		constexpr uint	count = 10;
		
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
		
		if ( haveSameOrder )
		{
			uint j = 0;
			for (auto& p : set) {
				TEST( p == j );
				++j;
			}
			TEST( j == count );
		}
		else
		{
			uint j = 0;
			for (auto& p : set) {
				TEST( p < count );
				++j;
			}
			TEST( j == count );
		}

		auto	iter = set.begin();
		iter = set.find( 5u );	TEST( iter != set.end() );	TEST( *iter == 5u );
		iter = set.find( 2u );	TEST( iter != set.end() );	TEST( *iter == 2u );
		iter = set.find( 7u );	TEST( iter != set.end() );	TEST( *iter == 7u );
		iter = set.find( 11u );	TEST( iter == set.end() );
	}

	
	template <template <typename V> class Set>
	static void  HashSet_Test2 ()
	{
		Set<int>		set;
		constexpr uint	count = 10;

		for (uint i = 0; i < count; ++i) {
			set.insert( i );
		}
	
		TEST( set.size() == count );

		TEST( set.count( 5 ) == 1 );
		TEST( set.count( 2 ) == 1 );
		TEST( set.count( 7 ) == 1 );
		TEST( set.count( -2 ) == 0 );
		TEST( set.count( 11 ) == 0 );
	}

	
	template <template <typename V> class Set>
	static void  HashSet_Test3 ()
	{
		using T1 = DebugInstanceCounter< int, 1 >;

		T1::ClearStatistic();
		{
			Set< T1 >	set;

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

	
	template <template <typename V> class Set>
	static void  HashSet_Test4 ()
	{
		Set<int>	set;

		for (uint i = 0; i < 10; ++i) {
			set.insert( i );
		}

		TEST( set.size() == 10 );
		TEST( set.insert( 10 ).second );
		TEST( not set.insert( 1 ).second );
	}

	
	template <template <typename V> class Set>
	static void  HashSet_Test5 ()
	{
		Set<int>	set1;
		Set<int>	set2;

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
	}

	
	template <template <typename V> class Set>
	static void  HashSet_Test6 ()
	{
		using T = DebugInstanceCounter< uint, 1 >;

		T::ClearStatistic();
		{
			Set<T>			set;
			constexpr uint	count = 10;
		
			for (uint i = 0; i < count; ++i) {
				set.insert( T(i) );
			}
		
			const auto	Check = [&set](ulong except)
			{
				for (uint i = 0; i < count; ++i)
				{
					auto	iter = set.find( T(i) );

					if ( AllBits( except, 1ull << i ))
					{
						TEST( iter == set.end() );
					}
					else
					{
						TEST( iter != set.end() );
						TEST( *iter  == T(i) );
					}
				}
			};

			ulong	except	= 0;
			Check( except );
			set.erase( T(2) );		except |= 1ull << 2;	Check( except );	TEST( set.size() == count-1 );
			set.erase( T(5) );		except |= 1ull << 5;	Check( except );	TEST( set.size() == count-2 );
			set.erase( T(0) );		except |= 1ull << 0;	Check( except );	TEST( set.size() == count-3 );
			set.erase( T(9) );		except |= 1ull << 9;	Check( except );	TEST( set.size() == count-4 );
			set.erase( T(11) );		TEST( set.size() == count-4 );
			set.erase( T(33) );		TEST( set.size() == count-4 );
			set.erase( T(3) );		except |= 1ull << 3;	Check( except );	TEST( set.size() == count-5 );
		}
		TEST( T::CheckStatistic() );
	}
	
#ifdef AE_ENABLE_ABSEIL
	template <typename V>	using TFlatHashSet	= FlatHashSet< V >;
#endif
	template <typename V>	using THashSet		= HashSet< V >;
}


extern void UnitTest_FlatHashSet ()
{
#ifdef AE_ENABLE_ABSEIL
	HashSet_Test1< TFlatHashSet >( false );
	HashSet_Test2< TFlatHashSet >();
	HashSet_Test3< TFlatHashSet >();
	HashSet_Test4< TFlatHashSet >();
	HashSet_Test5< TFlatHashSet >();
	HashSet_Test6< TFlatHashSet >();

	TEST_PASSED();
#endif
}


extern void UnitTest_HashSet ()
{
	HashSet_Test1< THashSet >( false );
	HashSet_Test2< THashSet >();
	HashSet_Test3< THashSet >();
	HashSet_Test4< THashSet >();
	HashSet_Test5< THashSet >();
	HashSet_Test6< THashSet >();

	TEST_PASSED();
}
