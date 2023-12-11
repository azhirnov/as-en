// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

// [Multithreading test]()
// [Performance test](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_MtAllocator.cpp)

#include "UnitTest_Common.h"

namespace
{
    static void  LfFixedBlockAllocator3_Test1 ()
    {
        LfFixedBlockAllocator< usize{64*64_b}, 16 >     lf_alloc{ 1_Kb, 8_b };

        TEST( lf_alloc.MaxMemorySize() == 64_Mb );

        Array< RstPtr<void> >       ptrs;

        for (uint i = 0; i < 10'000; ++i) {
            ptrs.push_back( lf_alloc.AllocBlock() );
        }

        for (uint i = 0; i < 1'000; ++i)
        {
            TEST( lf_alloc.DeallocBlock( ptrs.back() ));
            ptrs.pop_back();
        }

        for (auto ptr : ptrs) {
            TEST( lf_alloc.DeallocBlock( ptr ));
        }
        ptrs.clear();

        lf_alloc.Release( True{} );
    }
}


extern void UnitTest_LfFixedBlockAllocator3 ()
{
    LfFixedBlockAllocator3_Test1();

    TEST_PASSED();
}
