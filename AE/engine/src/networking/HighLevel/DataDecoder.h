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
    // Network Data Decoder (Deserializer)
    //

    class DataDecoder final
    {
    // variables
    private:
        Serializing::Deserializer   _des;


    // methods
    public:
        DataDecoder (const void* ptr, Bytes size, IAllocator &alloc)    __NE___ : _des{ FastRStream{ ptr, ptr+size }, &alloc } {}

        ND_ bool    IsComplete ()                                       C_NE___ { return _des.stream.Empty(); }
        ND_ Bytes   RemainingSize ()                                    C_NE___ { return _des.stream.RemainingSize(); }

        template <typename ...Args>
        ND_ bool    operator () (INOUT Args& ...args)                   __NE___ { return _des( args... ); }

        ND_ bool    Read (OUT void* buffer, Bytes size)                 __NE___ { return _des.stream.Read( OUT buffer, size ); }
    };


} // AE::Networking
