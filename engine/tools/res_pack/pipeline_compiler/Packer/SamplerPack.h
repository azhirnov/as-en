// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/SamplerDesc.h"
#include "serializing/ISerializable.h"
#include "Packer/PackCommon.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ScriptSampler.h"
#endif


namespace AE::PipelineCompiler
{
    using namespace AE::Graphics;


    //
    // Sampler Serializer
    //

    class SamplerSerializer final : public Serializing::ISerializable
    {
    // types
    public:
        using FSNameArr_t = ArrayView< FeatureSetName::Optimized_t >;

        static constexpr uint   MaxCount = 1 << 16;


    // variables
    private:
        FSNameArr_t                             _features;
        SamplerDesc                             _desc;
        Optional<SamplerYcbcrConversionDesc>    _ycbcrDesc;


    // methods
    public:
        SamplerSerializer () {}

        ND_ FSNameArr_t                         GetFeatures ()      const   { return _features; }
        ND_ SamplerDesc const&                  GetDesc ()          const   { return _desc; }

        ND_ SamplerYcbcrConversionDesc const&   YcbcrDesc ()        const   { return *_ycbcrDesc; }
        ND_ bool                                HasYcbcr ()         const   { return _ycbcrDesc.has_value(); }
        ND_ auto const&                         YcbcrDescOpt ()     const   { return _ycbcrDesc; }

        #ifdef AE_BUILD_PIPELINE_COMPILER
        ND_ bool  Create (const ScriptSampler &samp);
        ND_ bool  operator == (const ScriptSampler &rhs) const;
        #endif
        #ifdef AE_TEST_PIPELINE_COMPILER
        ND_ String  ToString (const HashToName &) const;
        #endif

        // ISerializable
        bool  Serialize (Serializing::Serializer &)     C_NE_OV;
        bool  Deserialize (Serializing::Deserializer &) __NE_OV;
    };


#ifdef AE_BUILD_PIPELINE_COMPILER

    //
    // Sampler Packer
    //

    class SamplerPacker
    {
    // methods
    public:
        static bool  Serialize (Serializing::Serializer &ser) __NE___;
    };

#endif

} // AE::PipelineCompiler

