// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/RefCounter.h"
#include "UnitTest_Common.h"

namespace
{
    using Obj_t = DebugInstanceCounter< int, 7 >;

    class RCObj final : public EnableRC<RCObj>, public Obj_t
    {
    public:
        RCObj () : Obj_t{ 0 } {}

        explicit RCObj (int i) : Obj_t{ i } {}
    };

    using RC_t          = RC<RCObj>;
    using AtomicRC_t    = AtomicRC<RCObj>;


    static void  RC_Test1 ()
    {
        Obj_t::ClearStatistic();
        {
            RC_t    a0 = MakeRC<RCObj>( 1 );

            a0 = a0.get();
        }
        TEST( Obj_t::CheckStatistic() );
    }


    static void  AtomicRC_Test1 ()
    {
        Obj_t::ClearStatistic();
        {
            const RC_t  a0 = MakeRC<RCObj>( 1 );
            const RC_t  a1 = MakeRC<RCObj>( 2 );

            AtomicRC_t  b0;
            RC_t        b1;

            b0 = a0;

            TEST( a0.use_count() == 2 );
            TEST( a1.use_count() == 1 );

            TEST( not b0.CAS_Loop( INOUT b1, a1 ));
            TEST( b1 == a0 );
            TEST( a0.use_count() == 3 );    // b0, b1
            TEST( a1.use_count() == 1 );

            TEST( b0.CAS_Loop( INOUT b1, a1 ));
            TEST( b1 == a0 );
            TEST( b0.unsafe_get() == a1 );

            TEST( a0.use_count() == 2 );    // b1
            TEST( a1.use_count() == 2 );    // b0

            b0 = null;
            b1.reset( null );
            TEST( a0.use_count() == 1 );
            TEST( a1.use_count() == 1 );
        }
        TEST( Obj_t::CheckStatistic() );
    }
}


extern void UnitTest_RC ()
{
    RC_Test1();

    AtomicRC_Test1();

    TEST_PASSED();
}
