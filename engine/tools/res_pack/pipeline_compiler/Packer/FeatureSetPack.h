// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/FeatureSet.h"
#include "serializing/ISerializable.h"
#include "Packer/PackCommon.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ScriptFeatureSet.h"
#endif

namespace AE::PipelineCompiler
{
    using namespace AE::Graphics;


    //
    // Feature Set Serializer
    //

    class FeatureSetSerializer final : public Serializing::ISerializable
    {
    // types
    public:
        static constexpr uint   MaxCount = 1 << 8;


    // variables
    private:
        FeatureSet  _fs;


    // methods
    public:
        FeatureSetSerializer () {}

        ND_ FeatureSet const&  Get ()   const   { return _fs; }

        #ifdef AE_BUILD_PIPELINE_COMPILER
        bool  Create (const ScriptFeatureSet &fs);
        #endif
        #ifdef AE_TEST_PIPELINE_COMPILER
        ND_ String  ToString () const;
        #endif

        // ISerializable
        bool  Serialize (Serializing::Serializer &)     C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &) __NE_OV;
    };



#ifdef AE_BUILD_PIPELINE_COMPILER

    //
    // Feature Set Packer
    //

    class FeatureSetPacker
    {
    // methods
    public:
        static bool  Serialize (Serializing::Serializer &ser) __NE___;
    };

#endif

} // AE::PipelineCompiler
