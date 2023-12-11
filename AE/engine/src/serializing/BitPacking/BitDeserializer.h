// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/BitPacking/BitSerializer.h"

namespace AE::Serializing
{

    //
    // Bit Deserializer
    //

    struct BitDeserializer final : Noncopyable
    {
    // types
    public:

    // variables
    public:
        FastRStream             stream;
        Ptr<ObjectFactory>      factory;
        RC<IAllocator>          allocator;


    // methods
    public:
        explicit BitDeserializer (FastRStream rstream, RC<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{ RVRef(alloc) } {}
        explicit BitDeserializer (RC<RStream> rstream, RC<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{ RVRef(alloc) } {}

            template <typename ...Args>
        ND_ bool  operator () (INOUT Args& ...args) __NE___;
        ND_ bool  operator () (INOUT void *)        __NE___;

        ND_ bool  IsEnd ()                          C_NE___ { return stream.Empty(); }

    private:
    };


} // AE::Serializing
