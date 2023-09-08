// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  LinearAllocator_Test1 ()
    {
        using T     = DebugInstanceCounter<uint, 1>;
        using LA    = LinearAllocator<>;
        using StdLA = StdAllocatorRef< T, LA* >;

        LA  allocator;
        allocator.SetBlockSize( 4_Mb );

        T::ClearStatistic();
        {
            Array< T, StdLA >   vec{ StdLA{&allocator} };

            vec.resize( 100 );
            vec.push_back( T(101) );
        }
        TEST( T::CheckStatistic() );
    }


    static void  LinearAllocator_Test2 ()
    {
        using T     = DebugInstanceCounter<uint, 2>;
        using LA    = LinearAllocator<>;
        using StdLA = StdAllocatorRef< T, LA* >;

        LA  allocator;
        allocator.SetBlockSize( 4_Mb );

        T::ClearStatistic();
        {
            std::unordered_set< T, std::hash<T>, std::equal_to<T>, StdLA >      set{ StdLA{&allocator} };

            set.reserve( 100 );
            set.insert( T(101) );
            set.insert( T(2) );
            set.insert( T(101) );
            set.clear();
        }
        TEST( T::CheckStatistic() );
    }
}


extern void UnitTest_LinearAllocator ()
{
    LinearAllocator_Test1();
    LinearAllocator_Test2();

    TEST_PASSED();
}
