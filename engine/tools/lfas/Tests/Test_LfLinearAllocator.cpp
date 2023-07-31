// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "CPP_VM/VirtualMachine.h"
#include "CPP_VM/Atomic.h"
#include "CPP_VM/Mutex.h"
#include "CPP_VM/Storage.h"

#include "base/Math/BitMath.h"
#include "base/Math/Math.h"
#include "base/Memory/UntypedAllocator.h"

#include "UnitTest_Common.h"

using namespace LFAS;
using namespace LFAS::CPP;

#include "threading/Memory/LfLinearAllocator.h"

namespace
{
    using AE::Threading::LfLinearAllocator;

    enum class EAction
    {
        Allocate,
        Deallocate,
        Read,
        Write,
    };


    static void  LfLinearAllocator_Test1 ()
    {
        using T = DebugInstanceCounter< int, 1 >;

        VirtualMachine::CreateInstance();
        T::ClearStatistic();
        {
            auto&   vm = VirtualMachine::Instance();
            vm.ThreadFenceAcquireRelease();

        }
        VirtualMachine::DestroyInstance();
        TEST( T::CheckStatistic() );
    }
}


extern void Test_LfLinearAllocator ()
{
    LfLinearAllocator_Test1();

    TEST_PASSED();
}
