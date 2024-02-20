// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VPipelineLayout.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VPipelineLayout::~VPipelineLayout () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( _layout == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VPipelineLayout::Create (VResourceManager &resMngr, const DescriptorSets_t &descSetLayouts, const PushConstants_t &pushConstants,
                                   VkDescriptorSetLayout emptyLayout, StringView dbgName) __NE___
    {
        using VkDescriptorSetLayouts_t  = StaticArray< VkDescriptorSetLayout, GraphicsConfig::MaxDescriptorSets >;
        using VkPushConstantRanges_t    = FixedArray< VkPushConstantRange, GraphicsConfig::MaxPushConstants >;

        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _layout == Default );

        _descriptorSets = descSetLayouts;
        _pushConstants  = pushConstants;

        VkDescriptorSetLayouts_t    vk_layouts  = {};
        VkPushConstantRanges_t      vk_ranges   = {};

        for (auto& layout : vk_layouts) {
            layout = emptyLayout;
        }

        uint    min_set = uint(vk_layouts.size());
        uint    max_set = 1;

        for (auto [name, ds] : _descriptorSets)
        {
            CHECK_ERR( ds.index.vkIndex != UMax );
            CHECK_ERR( vk_layouts[ ds.index.vkIndex ] == emptyLayout ); // already set

            auto*   ds_layout = resMngr.GetResource( ds.layoutId );
            CHECK_ERR( ds_layout != null );

            vk_layouts[ ds.index.vkIndex ] = ds_layout->Handle();
            min_set = Min( min_set, ds.index.vkIndex );
            max_set = Max( max_set, ds.index.vkIndex + 1 );
        }

        for (auto pc : _pushConstants)
        {
            VkPushConstantRange range = {};
            range.offset        = uint( pc.second.vulkanOffset );
            range.size          = uint( pc.second.size );
            range.stageFlags    = VEnumCast( pc.second.stage );

            vk_ranges.push_back( range );
        }

        VkPipelineLayoutCreateInfo          layout_info = {};
        layout_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_info.setLayoutCount          = max_set;
        layout_info.pSetLayouts             = vk_layouts.data();
        layout_info.pushConstantRangeCount  = uint(vk_ranges.size());
        layout_info.pPushConstantRanges     = vk_ranges.data();

        auto&   dev = resMngr.GetDevice();
        VK_CHECK_ERR( dev.vkCreatePipelineLayout( dev.GetVkDevice(), &layout_info, null, OUT &_layout ));

        _firstDescSet = min_set;

        dev.SetObjectName( _layout, dbgName, VK_OBJECT_TYPE_PIPELINE_LAYOUT );

        DEBUG_ONLY( _debugName = dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VPipelineLayout::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( _layout != Default )
        {
            auto&   dev = resMngr.GetDevice();
            dev.vkDestroyPipelineLayout( dev.GetVkDevice(), _layout, null );
        }

        _descriptorSets.clear();
        _pushConstants.clear();

        _layout         = Default;
        _firstDescSet   = UMax;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    GetDescriptorSetLayout
=================================================
*/
    bool  VPipelineLayout::GetDescriptorSetLayout (DescriptorSetName::Ref id, OUT DescriptorSetLayoutID &layout, OUT DescSetBinding &binding) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _descriptorSets.find( id );

        if_likely( iter != _descriptorSets.end() )
        {
            layout  = iter->second.layoutId;
            binding = iter->second.index;
            return true;
        }

        return false;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
