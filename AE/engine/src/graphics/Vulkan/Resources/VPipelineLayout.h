// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

    //
    // Vulkan Pipeline Layout
    //

    class VPipelineLayout final
    {
    // types
    public:
        struct DescSetLayout
        {
            DescriptorSetLayoutID       layoutId;
            DescSetBinding              index;
        };

        using DescriptorSets_t      = FixedMap< DescriptorSetName::Optimized_t, DescSetLayout, GraphicsConfig::MaxDescriptorSets >;
        using PushConst             = PipelineCompiler::PushConstants::PushConst;
        using PushConstants_t       = PipelineCompiler::PushConstants::PushConstMap_t;


    // variables
    private:
        VkPipelineLayout            _layout         = Default;
        PushConstants_t             _pushConstants;
        DescriptorSets_t            _descriptorSets;
        uint                        _firstDescSet   = UMax;

        DEBUG_ONLY( DebugName_t     _debugName; )
        DRC_ONLY(   RWDataRaceCheck _drCheck;   )


    // methods
    public:
        VPipelineLayout ()                                      __NE___ {}
        ~VPipelineLayout ()                                     __NE___;

        ND_ bool  Create (VResourceManager &resMngr, const DescriptorSets_t &descSetLayouts, const PushConstants_t &pushConstants,
                          VkDescriptorSetLayout emptyLayout, StringView dbgName)    __NE___;
            void  Destroy (VResourceManager &)                                      __NE___;

        ND_ bool  GetDescriptorSetLayout (DescriptorSetName::Ref, OUT DescriptorSetLayoutID &layout, OUT DescSetBinding &binding) C_NE___;


        ND_ VkPipelineLayout        Handle ()                   C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _layout; }

        ND_ uint                    GetFirstDescriptorSet ()    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _firstDescSet; }
        ND_ DescriptorSets_t const& GetDescriptorSets ()        C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _descriptorSets; }
        ND_ PushConstants_t const&  GetPushConstants ()         C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _pushConstants; }

        DEBUG_ONLY(  ND_ StringView  GetDebugName ()            C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; })
    };

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
