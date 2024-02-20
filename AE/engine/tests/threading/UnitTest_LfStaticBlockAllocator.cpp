// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  LfStaticBlockAllocator_Test1 ()
    {
        using LfAlloc   = LfStaticBlockAllocator< 64*64, 16 >;
        using Alloc     = UntypedAllocator;

        const Bytes     block_size  = 1_Kb;
        const Bytes     block_align = 8_b;
        const Bytes     total_size  = LfAlloc::CalcStorageSize( block_size );

        void*           storage     = Alloc::Allocate( SizeAndAlign{ total_size, block_align });
        LfAlloc         lf_alloc    { storage, total_size, block_size, block_align };

        TEST( storage != null );
        TEST_Eq( lf_alloc.MaxMemorySize(), 64_Mb );

        Array< RstPtr<void> >       ptrs;
        ptrs.reserve( 64*64*16 );

        for (uint i = 0; i < 64*64*16; ++i)
        {
            auto    ptr = lf_alloc.AllocBlock();
            TEST( ptr );
            ptrs.push_back( ptr );
        }
        TEST( lf_alloc.AllocBlock() == null );

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

        Alloc::Deallocate( storage, SizeAndAlign{ total_size, block_align });
    }
}


extern void UnitTest_LfStaticBlockAllocator ()
{
    LfStaticBlockAllocator_Test1();

    TEST_PASSED();
}
