// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/IAllocator.h"

#if defined(AE_TEST_PIPELINE_COMPILER) or defined(AE_DEBUG)
# include "HashToName.h"
#endif

namespace AE::PipelineCompiler
{
    using namespace AE::Base;


    static constexpr uint   PackOffsets_Name            = "PackOff"_Hash;
    static constexpr uint   NameMapping_Name            = "NameMap"_Hash;

    static constexpr uint   FeatureSetPack_Version      = 1;
    static constexpr uint   FeatureSetPack_Name         = "FSpack"_Hash;

    static constexpr uint   RenderPassPack_Version      = 1;
    static constexpr uint   RenderPassPack_Name         = "RPpack"_Hash;
    static constexpr uint   RenderPassPack_VkRpBlock    = "VKRP"_Hash;
    static constexpr uint   RenderPassPack_MtlRpBlock   = "MtlRP"_Hash;
    StaticAssert( RenderPassPack_VkRpBlock != RenderPassPack_MtlRpBlock );

    static constexpr uint   SamplerPack_Version         = 1;
    static constexpr uint   SamplerPack_Name            = "SampPack"_Hash;

    static constexpr uint   PipelinePack_Version        = 2;
    static constexpr uint   PipelinePack_Name           = "PplnPack"_Hash;

    static constexpr uint   ShaderPack_Version          = 1;
    static constexpr uint   ShaderPack_Name             = "ShPack"_Hash;


    StaticAssert( PackOffsets_Name != NameMapping_Name );
    StaticAssert( PackOffsets_Name != FeatureSetPack_Name );
    StaticAssert( PackOffsets_Name != RenderPassPack_Name );
    StaticAssert( PackOffsets_Name != SamplerPack_Name );
    StaticAssert( PackOffsets_Name != PipelinePack_Name );
    StaticAssert( PackOffsets_Name != ShaderPack_Name );

    StaticAssert( NameMapping_Name != FeatureSetPack_Name );
    StaticAssert( NameMapping_Name != RenderPassPack_Name );
    StaticAssert( NameMapping_Name != SamplerPack_Name );
    StaticAssert( NameMapping_Name != PipelinePack_Name );
    StaticAssert( NameMapping_Name != ShaderPack_Name );

    StaticAssert( FeatureSetPack_Name != RenderPassPack_Name );
    StaticAssert( FeatureSetPack_Name != SamplerPack_Name );
    StaticAssert( FeatureSetPack_Name != PipelinePack_Name );
    StaticAssert( FeatureSetPack_Name != ShaderPack_Name );

    StaticAssert( RenderPassPack_Name != SamplerPack_Name );
    StaticAssert( RenderPassPack_Name != PipelinePack_Name );
    StaticAssert( RenderPassPack_Name != ShaderPack_Name );

    StaticAssert( SamplerPack_Name != PipelinePack_Name );
    StaticAssert( SamplerPack_Name != ShaderPack_Name );

    StaticAssert( PipelinePack_Name != ShaderPack_Name );


    //
    // Pipeline Pack Offsets
    //
    struct PipelinePackOffsets
    {
        using Offset_t  = ulong;

        Offset_t    allocSize           = 0;

        Offset_t    featureSetOffset    = UMax;     // FeatureSetPack_Name
        Offset_t    featureSetDataSize  = 0;

        Offset_t    samplerOffset       = UMax;     // SamplerPack_Name
        Offset_t    samplerDataSize     = 0;

        Offset_t    renderPassOffset    = UMax;     // RenderPassPack_Name
        Offset_t    renderPassDataSize  = 0;

        Offset_t    pipelineOffset      = UMax;     // PipelinePack_Name
        Offset_t    pipelineDataSize    = 0;

        Offset_t    shaderOffset        = UMax;     // ShaderPack_Name
        Offset_t    shaderDataSize      = 0;

        Offset_t    nameMappingOffset   = UMax;     // NameMapping_Name
        Offset_t    nameMappingDataSize = 0;
    };


} // AE::PipelineCompiler


namespace AE::Base
{
    template <> struct TMemCopyAvailable< AE::PipelineCompiler::PipelinePackOffsets >       { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::PipelineCompiler::PipelinePackOffsets >  { static constexpr bool  value = true; };

} // AE::Base
