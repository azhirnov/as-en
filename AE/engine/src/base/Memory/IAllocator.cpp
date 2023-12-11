// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Memory/IAllocator.h"
#include "base/Containers/InPlace.h"

namespace AE::Base
{
namespace
{
    struct DefaultAllocator
    {
        using Alloc_t = IAllocatorAdaptor< UntypedAllocator >;

        StaticRC< Alloc_t >     alloc;
        AtomicRC<IAllocator>    rc;         // TODO: Atomic<IAllocator*> may be faster

        DefaultAllocator ()
        {
            rc.store( alloc->GetRC() );
        }

        ~DefaultAllocator ()
        {
            rc.reset();
        }
    };

    static DefaultAllocator     s_DefaultAllocator;

} // namespace
} // AE::Base


namespace AE
{
    using namespace AE::Base;

    void  SetDefaultAllocator (RC<IAllocator> value) __NE___
    {
        if_likely( value )
            Base::s_DefaultAllocator.rc.store( value );
        else
            Base::s_DefaultAllocator.rc.store( Base::s_DefaultAllocator.alloc->GetRC() );
    }

    RC<IAllocator>  GetDefaultAllocator () __NE___
    {
        return Base::s_DefaultAllocator.rc.load();
    }

    Ptr<IAllocator>  GetDefaultAllocatorPtr () __NE___
    {
        return Base::s_DefaultAllocator.rc.unsafe_get();
    }

} // AE
