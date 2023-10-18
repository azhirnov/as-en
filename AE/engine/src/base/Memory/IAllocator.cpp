// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Memory/IAllocator.h"
#include "base/Containers/InPlace.h"

namespace AE::Base
{
namespace
{
    struct DefaultAllocator
    {
        using Alloc_t = AllocatorImpl< UntypedAllocator >;

        InPlace< Alloc_t >      alloc;
        AtomicRC<IAllocator>    rc;

        DefaultAllocator ()
        {
            alloc.CustomCtor( &StaticRC::New<Alloc_t> );
            rc = alloc->GetRC();
        }

        ~DefaultAllocator ()
        {
            rc = null;
            alloc.CustomDtor( &StaticRC::Delete<Alloc_t> );
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
        if ( value )
            Base::s_DefaultAllocator.rc = value;
        else
            Base::s_DefaultAllocator.rc = Base::s_DefaultAllocator.alloc->GetRC();
    }

    RC<IAllocator>  GetDefaultAllocator () __NE___
    {
        return Base::s_DefaultAllocator.rc.load();
    }

} // AE
