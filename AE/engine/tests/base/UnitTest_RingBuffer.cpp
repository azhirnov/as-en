// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	using Elem_t	= DebugInstanceCounter< int, 4 >;
	using VElem_t	= DebugInstanceCounter< int, 2 >;
	using Queue_t	= RingBuffer< Elem_t >;
	using Vector_t	= Array< VElem_t >;


	static void  RingBuffer_Test1 ()
	{
		RingBuffer<int>		q;
		Array<int>			v;
		const usize			count = 10;

		TEST( q.empty() );
		TEST( q.size() == 0 );

		const auto	Check = [&q, &v]()
		{{
			ArrayView<int> part0;
			ArrayView<int> part1;
			q.GetParts( part0, part1 );

			TEST( part0.size() + part1.size() == q.size() );
			TEST( q == ArrayView<int>{ v } );
		}};

		for (usize i = 0; i < count; ++i)
		{
			auto	s = q.size();
			TEST( s == i );
			q.push_back( int(i) + 1000 );
			v.push_back( int(i) + 1000 );
		}
		Check();

		for (usize i = 0; i < count; ++i)
		{
			TEST( q.size() == count + i );
			q.push_front( int(i) + 2000 );
			v.insert( v.begin(), int(i) + 2000 );
		}
		Check();

		for (usize i = 0; i < count; ++i)
		{
			TEST( q.size() == count*2 + i );
			q.push_back( int(i) + 3000 );
			v.push_back( int(i) + 3000 );
		}
		Check();

		for (usize i = 0; i < count; ++i)
		{
			TEST( q.size() == count*3 + i );
			q.push_front( int(i) + 4000 );
			v.insert( v.begin(), int(i) + 4000 );
		}
		Check();
	}


	static void  RingBuffer_Test2 ()
	{
		Elem_t::ClearStatistic();
		VElem_t::ClearStatistic();
		{
			const usize loops = 100;
			const usize count = 1000;

			Queue_t		q;
			Vector_t	v;

			Array<Elem_t>	aq;
			Vector_t		av;

			for (usize i = 0; i < loops; ++i)
			{
				aq.clear();
				av.clear();

				for (usize j = 0; j < count; ++j) {
					aq.push_back( Elem_t(int( i * count + j )) );
					av.push_back( VElem_t(int( i * count + j )) );
				}

				q.AppendBack( aq );
				v.insert( v.end(), av.begin(), av.end() );
				TEST( (q == ArrayView<Elem_t>( Cast<Elem_t>(std::addressof(v[0])), v.size() )) );
			}
		}
		TEST( Elem_t::CheckStatistic() );
		TEST( VElem_t::CheckStatistic() );
	}


	static void  RingBuffer_Test3 ()
	{
		Elem_t::ClearStatistic();
		VElem_t::ClearStatistic();
		{
			const usize loops = 100;
			const usize count = 1000;

			Queue_t		q;
			Vector_t	v;

			Array<Elem_t>	aq;
			Vector_t		av;

			for (usize i = 0; i < loops; ++i)
			{
				aq.clear();
				av.clear();

				for (usize j = 0; j < count; ++j) {
					aq.push_back( Elem_t(int( i * count + j )) );
					av.push_back( VElem_t(int( i * count + j )) );
				}

				q.AppendFront( aq );
				v.insert( v.begin(), av.begin(), av.end() );
				TEST( (q == ArrayView<Elem_t>( Cast<Elem_t>(std::addressof(v[0])), v.size() )) );
			}
		}
		TEST( Elem_t::CheckStatistic() );
		TEST( VElem_t::CheckStatistic() );
	}


	static void  RingBuffer_Test4 ()
	{
		Elem_t::ClearStatistic();
		{
			Queue_t		q;

			q.push_back( Elem_t{10} );
			TEST( q.size() == 1 );

			q.pop_back();
			TEST( q.size() == 0 );
			TEST( q.empty() );

			q.push_front( Elem_t{10} );
			TEST( q.size() == 1 );

			q.pop_front();
			TEST( q.size() == 0 );
			TEST( q.empty() );
		}
		TEST( Elem_t::CheckStatistic() );
	}


	static void  RingBuffer_Test5 ()
	{
		Elem_t::ClearStatistic();
		{
			Queue_t		q;

			q.push_back( Elem_t{10} );
			q.push_back( Elem_t{10} );
			q.push_back( Elem_t{10} );

			TEST_THROW( q.reserve( 1u << 31 ); );
		}
		TEST( Elem_t::CheckStatistic() );
	}


	static void  RingBuffer_Test6 ()
	{
		Elem_t::ClearStatistic();
		{
			Queue_t		q;

			for (int i = 0; i < 100; ++i) {
				q.push_back( Elem_t{i} );
			}
			TEST( q.size() == 100 );

			q.FastErase( 10 );
			TEST( q.size() == 99 );

			for (int i = 0; i < 10; ++i) {
				TEST( q[i] == Elem_t{int(i)} );
			}
			TEST( q[10] == Elem_t{99} );

			for (int i = 11; i < 99; ++i) {
				TEST( q[i] == Elem_t{int(i)} );
			}

			q.FastErase( 0 );
			TEST( q.size() == 98 );

			for (int i = 0; i < 9; ++i) {
				TEST( q[i] == Elem_t{int(i+1)} );
			}

			q.FastErase( 97 );
			TEST( q.size() == 97 );

			TEST( q[9] == Elem_t{99} );
			for (int i = 10; i < 97; ++i) {
				TEST( q[i] == Elem_t{int(i+1)} );
			}
		}
		TEST( Elem_t::CheckStatistic() );
		Elem_t::ClearStatistic();
		{
			Queue_t		q;

			for (int i = 0; i < 100; ++i) {
				q.emplace_back();
			}
			for (;;)
			{
				for (int i = 0; i < 50; ++i) {
					q.pop_front();
				}
				for (int i = 0; i < 50; ++i) {
					q.emplace_back();
				}

				ArrayView<Elem_t>	part0, part1;
				q.GetParts( OUT part0, OUT part1 );
				if ( part0.size() > 10 and part1.size() > 10 )
					break;
			}
			TEST( q.size() == 100 );
			for (int i = 0; i < 100; ++i) {
				q[i] = Elem_t{i};
			}

			q.FastErase( 10 );
			TEST( q.size() == 99 );

			for (int i = 0; i < 10; ++i) {
				TEST( q[i] == Elem_t{int(i)} );
			}
			TEST( q[10] == Elem_t{99} );

			for (int i = 11; i < 99; ++i) {
				TEST( q[i] == Elem_t{int(i)} );
			}

			q.FastErase( 0 );
			TEST( q.size() == 98 );

			for (int i = 0; i < 9; ++i) {
				TEST( q[i] == Elem_t{int(i+1)} );
			}

			q.FastErase( 97 );
			TEST( q.size() == 97 );

			TEST( q[9] == Elem_t{99} );
			for (int i = 10; i < 97; ++i) {
				TEST( q[i] == Elem_t{int(i+1)} );
			}
		}
		TEST( Elem_t::CheckStatistic() );
	}
}


extern void UnitTest_RingBuffer ()
{
	RingBuffer_Test1();
	RingBuffer_Test2();
	RingBuffer_Test3();
	RingBuffer_Test4();
	RingBuffer_Test5();
	RingBuffer_Test6();

	TEST_PASSED();
}
