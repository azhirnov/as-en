// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  LfFixedBlockAllocator_Test1 ()
    {
        LfFixedBlockAllocator< 64*64, 16 >      lf_alloc{ 1_Kb, 8_b };

        TEST( lf_alloc.MaxMemorySize() == 64_Mb );

        Array< RstPtr<void> >       ptrs;

        for (uint i = 0; i < 10'000; ++i) {
            ptrs.push_back( lf_alloc.Alloc() );
        }

        for (uint i = 0; i < 1'000; ++i)
        {
            TEST( lf_alloc.Dealloc( ptrs.back() ));
            ptrs.pop_back();
        }

        for (auto ptr : ptrs) {
            TEST( lf_alloc.Dealloc( ptr ));
        }
        ptrs.clear();

        lf_alloc.Release( true );
    }
}


extern void UnitTest_LfFixedBlockAllocator ()
{
    LfFixedBlockAllocator_Test1();

    TEST_PASSED();
}
