// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/ObjC/NS.h"

#include <Foundation/NSArray.h>

#ifndef AE_PLATFORM_APPLE
#   error must be included in objc (.mm) file!
#endif

namespace AE::NS
{

    template <typename Dst, typename Src>
    inline void  NSCastRetain (OUT Dst &dst, Src* src) __NE___
    {
        dst.Retain( src ? (__bridge void *) src : null );
    }

    template <typename Dst, typename Src>
    inline void  NSCastAttach (OUT Dst &dst, Src* src) __NE___
    {
        dst.Attach( src ? (__bridge void *) src : null );
    }

    namespace _hidden_
    {
        template <typename ObjType>
        void  MutableArrayBase::SetItem (UInteger index, void* obj) __NE___
        {
            ASSERT( index < size() );
            [(__bridge NSMutableArray *)Ptr() setObject : (__bridge ObjType *)obj
                                     atIndexedSubscript : index];
        }

    } // _hidden_

} // AE::NS
