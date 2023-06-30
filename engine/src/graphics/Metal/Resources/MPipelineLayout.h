// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

    //
    // Metal Pipeline Layout
    //

    class MPipelineLayout final
    {
    // types
    public:
        struct DescSetLayout
        {
            DescriptorSetLayoutID           layoutId;
            DescSetBinding                  index;
        };

        using DescriptorSets_t  = FixedMap< DescriptorSetName::Optimized_t, DescSetLayout, GraphicsConfig::MaxDescriptorSets >;
        using PushConst         = PipelineCompiler::PushConstants::PushConst;
        using PushConstants_t   = PipelineCompiler::PushConstants::PushConstMap_t;


    // variables
    private:
        DescriptorSets_t            _descriptorSets;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        MPipelineLayout ()                                      __NE___ {}
        ~MPipelineLayout ()                                     __NE___ {}

        ND_ bool  Create (MResourceManager &resMngr, const DescriptorSets_t &descSetLayouts, const PushConstants_t &pushConstants,
                          MetalArrayOfArgumentDescriptor emptyLayout, StringView dbgName)                                               __NE___;
            void  Destroy (MResourceManager &)                                                                                          __NE___;

        ND_ bool  GetDescriptorSetLayout (const DescriptorSetName &id, OUT DescriptorSetLayoutID &layout, OUT DescSetBinding &binding)  C_NE___;

        ND_ DescriptorSets_t const& GetDescriptorSets ()        C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _descriptorSets; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()            C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_METAL
