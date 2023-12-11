// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Primitives/Barrier.h"

#if (AE_BARRIER_MODE == 0)
# include "base/Platforms/WindowsHeader.cpp.h"

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    Barrier::Barrier (usize numThreads) __NE___
    {
        CHECK( ::InitializeSynchronizationBarrier( OUT _data.Ptr<SYNCHRONIZATION_BARRIER>(), CheckCast<LONG>(numThreads), -1 ));
    }

/*
=================================================
    destructor
=================================================
*/
    Barrier::~Barrier () __NE___
    {
        CHECK( ::DeleteSynchronizationBarrier( _data.Ptr<SYNCHRONIZATION_BARRIER>() ));
    }

/*
=================================================
    wait
=================================================
*/
    void  Barrier::wait () __NE___
    {
        ::EnterSynchronizationBarrier( _data.Ptr<SYNCHRONIZATION_BARRIER>(), SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE );
    }

} // AE::Threading

#endif // AE_BARRIER_MODE == 0
