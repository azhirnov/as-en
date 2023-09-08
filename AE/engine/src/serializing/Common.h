// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Serializing.pch.h"

// for NamedID
#define AE_SERIALIZE_HASH_ONLY  1
#define AE_DEBUG_SERIALIZER     0

namespace AE::Serializing
{
    using namespace AE::Base;

    using SerializedID = NamedID< 32, 0x400, AE_OPTIMIZE_IDS, UMax >;

    struct Serializer;
    struct Deserializer;
    class ObjectFactory;

    // IsTriviallySerializable<> defined in 'base/CompileTime/TypeTraits.h'

} // AE::Serializing
