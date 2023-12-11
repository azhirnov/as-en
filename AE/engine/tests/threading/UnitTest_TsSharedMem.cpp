// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  TsSharedMem_Test1 ()
    {
        using Alloc_t = IAllocatorAdaptor< UntypedAllocator >;

        RC<IAllocator>  alloc = MakeRC<Alloc_t>();
        {
            auto    mem = TsSharedMem::Create( alloc, SizeAndAlign{ 1024_b, 16_b });
            TEST( mem );
            TEST( mem->Size() == 1024_b );
            TEST( mem->Align() == 16_b );
            TEST( mem->AllocatorRC() == alloc );
        }
        TEST( alloc.use_count() == 1 );
    }
}


extern void UnitTest_TsSharedMem ()
{
    TsSharedMem_Test1();

    TEST_PASSED();
}
