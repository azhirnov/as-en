// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Primitives/Barrier.h"

#if (AE_BARRIER_MODE == 0)

# include "base/Platforms/WindowsHeader.h"

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    Barrier::Barrier (usize numThreads)
    {
        auto*   barrier_ptr = PlacementNew<SYNCHRONIZATION_BARRIER>( OUT _data );

        CHECK( InitializeSynchronizationBarrier( barrier_ptr, CheckCast<LONG>(numThreads), -1 ));
    }

/*
=================================================
    destructor
=================================================
*/
    Barrier::~Barrier ()
    {
        CHECK( DeleteSynchronizationBarrier( BitCast<SYNCHRONIZATION_BARRIER *>( &_data[0] )));
    }

/*
=================================================
    wait
=================================================
*/
    void Barrier::wait ()
    {
        EnterSynchronizationBarrier( BitCast<SYNCHRONIZATION_BARRIER *>( &_data[0] ), SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE );
    }

} // AE::Threading

#endif // AE_BARRIER_MODE == 0
