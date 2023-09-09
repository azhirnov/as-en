// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    using Elem_t    = DebugInstanceCounter< int, 4 >;
    using VElem_t   = DebugInstanceCounter< int, 2 >;
    using Queue_t   = RingBuffer< Elem_t >;
    using Vector_t  = Array< VElem_t >;


    static void  RingBuffer_Test1 ()
    {
        RingBuffer<int>     q;
        Array<int>          v;
        const usize         count = 10;

        TEST( q.empty() );
        TEST( q.size() == 0 );

        const auto  Check = [&q, &v]()
        {{
            ArrayView<int> part0;
            ArrayView<int> part1;
            q.GetParts( part0, part1 );

            TEST( part0.size() + part1.size() == q.size() );
            TEST( q == ArrayView<int>{ v } );
        }};

        for (usize i = 0; i < count; ++i)
        {
            auto    s = q.size();
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

            Queue_t     q;
            Vector_t    v;

            Array<Elem_t>   aq;
            Vector_t        av;

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
                TEST( (q == ArrayView<Elem_t>( Cast<Elem_t>(&v[0]), v.size() )) );
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

            Queue_t     q;
            Vector_t    v;

            Array<Elem_t>   aq;
            Vector_t        av;

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
                TEST( (q == ArrayView<Elem_t>( Cast<Elem_t>(&v[0]), v.size() )) );
            }
        }
        TEST( Elem_t::CheckStatistic() );
        TEST( VElem_t::CheckStatistic() );
    }


    static void  RingBuffer_Test4 ()
    {
        Elem_t::ClearStatistic();
        {
            Queue_t     q;

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
            Queue_t     q;

            q.push_back( Elem_t{10} );
            q.push_back( Elem_t{10} );
            q.push_back( Elem_t{10} );

            TEST_THROW( q.reserve( 1u << 31 ); );
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

    TEST_PASSED();
}
