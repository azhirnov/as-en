// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    using ThreadID  = uint;
    using MemRanges = MemRangesTempl< ThreadID >;
    using Range     = MemRanges::Range;


    bool  CheckRangeSorting (const MemRanges &mr)
    {
        Bytes   prev = 0_b;
        for (usize i = 0; i < mr._ranges.size(); ++i)
        {
            auto&   curr = mr._ranges[i];

            if ( curr.begin != prev )
                return false;

            if ( curr.begin >= curr.end )
                return false;

            prev = curr.end;
        }
        return true;
    }


    void  Ranges_Test1 ()
    {
        MemRanges   ranges;

        ranges.Init( 128_b, ThreadID(1) );
        ranges.AcquireRelease( ThreadID(1) );
        TEST( CheckRangeSorting( ranges ));

        // append
        {
            TEST( ranges.Write( 4_b, 8_b, ThreadID(2) ));
            ranges.AcquireRelease( ThreadID(2) );
            TEST( CheckRangeSorting( ranges ));

            TEST( ranges.Read( 0_b,   4_b, ThreadID(1) ));
            TEST( ranges.Read( 4_b,   8_b, ThreadID(2) ));
            TEST( ranges.Read( 8_b, 128_b, ThreadID(1) ));
        }

        // append
        {
            TEST( ranges.Write( 16_b, 32_b, ThreadID(3) ));
            ranges.AcquireRelease( ThreadID(3) );
            TEST( CheckRangeSorting( ranges ));

            TEST( ranges.Read(  0_b,   4_b, ThreadID(1) ));
            TEST( ranges.Read(  4_b,   8_b, ThreadID(2) ));
            TEST( ranges.Read( 16_b,  32_b, ThreadID(3) ));
            TEST( ranges.Read( 32_b, 128_b, ThreadID(1) ));
        }

        // append
        {
            TEST( ranges.Write( 4_b, 36_b, ThreadID(4) ));
            ranges.AcquireRelease( ThreadID(4) );
            TEST( CheckRangeSorting( ranges ));

            TEST( ranges.Read(  0_b,   4_b, ThreadID(1) ));
            TEST( ranges.Read(  4_b,  36_b, ThreadID(4) ));
            TEST( ranges.Read( 36_b, 128_b, ThreadID(1) ));
        }
    }
}


extern void  UnitTest_Ranges ()
{
    Ranges_Test1();

    TEST_PASSED();
}
