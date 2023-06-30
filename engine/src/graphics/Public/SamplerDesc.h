// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageSwizzle.h"
#include "graphics/Public/IDs.h"

namespace AE::Graphics
{
    enum class SamplerUID : uint { Unknown = ~0u };


    enum class EFilter : ubyte
    {
        Nearest,
        Linear,

        _Count,
        Unknown = 0xFF,
    };


    enum class EMipmapFilter : ubyte
    {
        None,
        Nearest,
        Linear,

        _Count,
        Unknown = 0xFF,
    };


    enum class EAddressMode : ubyte
    {
        Repeat,
        MirrorRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge,

        _Count,
        Unknown = 0xFF,
    };


    enum class EBorderColor : ubyte
    {
        FloatTransparentBlack,
        FloatOpaqueBlack,
        FloatOpaqueWhite,

        IntTransparentBlack,
        IntOpaqueBlack,
        IntOpaqueWhite,

        _Count,
        Unknown = 0xFF,
    };


    enum class EReductionMode : ubyte
    {
        Average,    // default
        Min,
        Max,

        _Count,
        Unknown = 0xFF,
    };


    enum class ESamplerUsage : ubyte
    {
        Default         = 0,
        Subsampled,
        SubsampledCoarseReconstruction,
        //ArgumentBuffer,   // for Metal    // TODO
        _Count,
        Unknown         = Default,
    };



    //
    // Sampler description
    //

    class SamplerDesc
    {
    // types
    public:
        using AddressMode3  = PackedVec< EAddressMode, 3 >;


    // variables
    public:
        ESamplerUsage           usage                   = Default;
        EFilter                 magFilter               = EFilter::Nearest;
        EFilter                 minFilter               = EFilter::Nearest;
        EMipmapFilter           mipmapMode              = EMipmapFilter::Nearest;
        AddressMode3            addressMode             = { EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat };
        EReductionMode          reductionMode           = EReductionMode::Average;
        float                   mipLodBias              = 0.0f;
        Optional<float>         maxAnisotropy;          // TODO: remove optional
        Optional<ECompareOp>    compareOp;              // TODO: remove optional
        float                   minLod                  = -1000.0f;
        float                   maxLod                  = 1000.0f;
        EBorderColor            borderColor             = EBorderColor::FloatTransparentBlack;
        bool                    unnormalizedCoordinates = false;


    // methods
    public:
        SamplerDesc () = default;
        SamplerDesc (const SamplerDesc &) = default;

        SamplerDesc&  operator = (const SamplerDesc &) = default;

        ND_ bool  operator == (const SamplerDesc &) const;
    };
//-----------------------------------------------------------------------------



    enum class ESamplerChromaLocation : ubyte
    {
        CositedEven,
        Midpoint,

        _Count,
        Unknown = 0xFF,
    };


    enum class ESamplerYcbcrModelConversion : ubyte
    {
        RGB_Identity,
        Ycbcr_Identity,
        Ycbcr_709,
        Ycbcr_601,
        Ycbcr_2020,

        _Count,
        Unknown = 0xFF,
    };


    enum class ESamplerYcbcrRange : ubyte
    {
        ITU_Full,
        ITU_Narrow,

        _Count,
        Unknown = 0xFF,
    };



    //
    // Sampler Ycbcr Conversion description
    //

    class SamplerYcbcrConversionDesc
    {
    // variables
    public:
        EPixelFormat                    format                      = Default;
        ESamplerYcbcrModelConversion    ycbcrModel                  = Default;
        ESamplerYcbcrRange              ycbcrRange                  = Default;
        ImageSwizzle                    components;
        ESamplerChromaLocation          xChromaOffset               = ESamplerChromaLocation::CositedEven;
        ESamplerChromaLocation          yChromaOffset               = ESamplerChromaLocation::CositedEven;
        EFilter                         chromaFilter                = EFilter::Nearest;
        bool                            forceExplicitReconstruction = false;


    // methods
    public:
        SamplerYcbcrConversionDesc () = default;
        SamplerYcbcrConversionDesc (const SamplerYcbcrConversionDesc &) = default;

        SamplerYcbcrConversionDesc&  operator = (const SamplerYcbcrConversionDesc &) = default;

        ND_ bool  operator == (const SamplerYcbcrConversionDesc &)  const;
    };


} // AE::Graphics
