// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Memory/MemUtils.h"

namespace AE::Base
{   
namespace _hidden_
{
/*
=================================================
    SecureZeroMemFallback
=================================================
*/
    using MemsetFn_t = decltype(&std::memset);

    static volatile MemsetFn_t  memset_fn = &std::memset;

    void  SecureZeroMemFallback (OUT void* ptr, Bytes size) __NE___
    {
        memset_fn( OUT ptr, 0, usize(size) );
    }
}

} // AE::Base
