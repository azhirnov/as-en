// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no
*/

#pragma once

#include "networking/Common.h"
#include "pch/Serializing.h"

namespace AE::Networking
{
    using namespace AE::Base;


    //
    // Network Data Encoder (Serializer)
    //

    class DataEncoder final
    {
    // variables
    private:
        Serializing::Serializer     _ser;


    // methods
    public:
        DataEncoder (void* ptr, Bytes size)                 __NE___ : _ser{FastWStream{ ptr, ptr+size }} {}
        DataEncoder (void* ptr, const void* end)            __NE___ : _ser{FastWStream{ ptr, end }} {}

        ND_ Bytes   RemainingSize ()                        C_NE___ { return _ser.stream.RemainingSize(); }
        ND_ bool    IsFull ()                               C_NE___ { return _ser.stream.Empty(); }

        template <typename ...Args>
        ND_ bool    operator () (const Args& ...args)       __NE___ { return _ser( args... ); }

        ND_ bool    Write (const void* buffer, Bytes size)  __NE___ { return _ser.stream.Write( buffer, size ); }
    };


} // AE::Networking
