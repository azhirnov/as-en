// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/Primitives/SpinLock.h"
#include "../base/UnitTest_Common.h"
using namespace AE::Threading;

namespace
{
    static void  PtrSpinLock_Test1 ()
    {
        using LackablePtr   = PtrWithSpinLockRelaxed< int >;

        int         i = 0;
        LackablePtr p {&i};

        TEST( p.get() == &i );

        p.lock();
        TEST( not p.try_lock() );
        TEST( p.IsLocked() );
        TEST( p.get() == &i );
        p.unlock();

        TEST( p.get() == &i );
        TEST( p.try_lock() );
        p.unlock();
    }
}


extern void UnitTest_SpinLock ()
{
    PtrSpinLock_Test1();

    TEST_PASSED();
}
