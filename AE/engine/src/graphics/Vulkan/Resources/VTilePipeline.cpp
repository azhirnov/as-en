// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VTilePipeline.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "VPipelineHelper.inl.h"

namespace AE::Graphics
{
/*
=================================================
    destructor
=================================================
*/
    VTilePipeline::~VTilePipeline () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( not _handle );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VTilePipeline::Create (VResourceManager &resMngr, const CreateInfo &ci) __NE___
    {
        DRC_EXLOCK( _drCheck );

        CHECK_ERR( ci.shader.IsValid() );
        CHECK_ERR( (ci.specCI.dynamicState & ~EPipelineDynamicState::TilePipelineMask) == Zero );
        CHECK_ERR( not _handle and not _layout );

        auto&   dev = resMngr.GetDevice();
        CHECK_ERR( dev.GetVExtensions().subpassShadingHW );

        auto*   ppln_layout = resMngr.GetResource( ci.layoutId, True{"incRef"} );
        CHECK_ERR( ppln_layout != null );

        _layout = ppln_layout->Handle();
        _layoutId.Attach( ci.layoutId );

        AutoreleasePplnCache    cache_ptr   { resMngr, ci.cacheId };
        VkPipelineCache         ppln_cache  = cache_ptr ? cache_ptr->Handle() : Default;
        RenderPassID            rp_id       = ci.pplnPack.GetRenderPass( resMngr, CompatRenderPassName{ ci.specCI.renderPass });
        auto*                   render_pass = resMngr.GetResource( rp_id );
        CHECK_ERR( render_pass != null );

        VRenderPass::SubpassInfo const*     subpass     = null;
        uint                                subpass_idx;
        {
            auto    iter = render_pass->SubpassMap().find( ci.specCI.subpass );
            CHECK_ERR( iter != render_pass->SubpassMap().end() );
            CHECK_ERR( usize{iter->second} < render_pass->Subpasses().size() );
            subpass_idx = uint{iter->second};
            subpass     = &render_pass->Subpasses()[ subpass_idx ];
        }

        Unused( subpass );  // TODO ?

        _localSize = ushort2{
            ci.templCI.localSizeSpec.x == UMax or ci.specCI.localSize.x == UMax ? ci.templCI.defaultLocalSize.x : ci.specCI.localSize.x,
            ci.templCI.localSizeSpec.y == UMax or ci.specCI.localSize.y == UMax ? ci.templCI.defaultLocalSize.y : ci.specCI.localSize.y };
        CHECK_ERR( All( _localSize > Zero ));

        uint2   max_tile_size {~0u};
        CHECK_ERR( render_pass->GetMaxTileWorkgroupSize( dev, OUT max_tile_size ));

        CHECK_ERR( All( _localSize <= ushort2{max_tile_size} ));
        _localSize = Min( _localSize, ushort2{max_tile_size} );

        VkComputePipelineCreateInfo                 pipeline_info   = {};
        VkSubpassShadingPipelineCreateInfoHUAWEI    subpass_shading = {};

        // TODO: VkPipelineCreateFlags2CreateInfoKHR (VK_KHR_maintenance5)

        subpass_shading.sType       = VK_STRUCTURE_TYPE_SUBPASS_SHADING_PIPELINE_CREATE_INFO_HUAWEI;
        subpass_shading.renderPass  = render_pass->Handle();
        subpass_shading.subpass     = subpass_idx;

        pipeline_info.sType         = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipeline_info.pNext         = &subpass_shading;
        pipeline_info.layout        = _layout;
        pipeline_info.flags         = VEnumCast( ci.specCI.options );

        pipeline_info.stage.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_info.stage.flags   = 0;
        pipeline_info.stage.stage   = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;
        pipeline_info.stage.module  = ci.shader.module;
        pipeline_info.stage.pName   = ci.shader.Entry();

        pipeline_info.basePipelineHandle= Default;
        pipeline_info.basePipelineIndex = -1;

        const auto  AddCustomSpec = [&ci, this] (VkShaderStageFlagBits, VkSpecializationMapEntry* entryArr, uint* dataArr, OUT uint &count)
        {{
            count = Sum<uint>( ci.templCI.localSizeSpec != UMax );

            if ( entryArr != null )
            {
                uint    idx = 0;

                if ( ci.templCI.localSizeSpec.x != UMax )
                {
                    auto&   entry = entryArr[idx];
                    entry.constantID    = ci.templCI.localSizeSpec.x;
                    entry.offset        = sizeof(uint) * idx;
                    entry.size          = sizeof(uint);
                    dataArr[idx]        = _localSize.x;
                    ++idx;
                }
                if ( ci.templCI.localSizeSpec.y != UMax )
                {
                    auto&   entry = entryArr[idx];
                    entry.constantID    = ci.templCI.localSizeSpec.y;
                    entry.offset        = sizeof(uint) * idx;
                    entry.size          = sizeof(uint);
                    dataArr[idx]        = _localSize.y;
                    ++idx;
                }

                ASSERT( count == idx );
            }
        }};

        VTempLinearAllocator    allocator;
        CHECK_ERR( AddSpecialization( OUT pipeline_info.stage.pSpecializationInfo,
                                      ci.specCI.specialization, *ci.shader.shaderConstants, allocator,
                                      VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI, AddCustomSpec ));

        VK_CHECK_ERR( dev.vkCreateComputePipelines( dev.GetVkDevice(), ppln_cache, 1, &pipeline_info, null, OUT &_handle ));

        dev.SetObjectName( _handle, ci.specCI.dbgName, VK_OBJECT_TYPE_PIPELINE );

        _options        = ci.specCI.options;
        _subpassIndex   = CheckCast<ubyte>(subpass_idx);
        _dbgTrace       = ci.shader.dbgTrace;

        DEBUG_ONLY( _debugName = ci.specCI.dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VTilePipeline::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto&   dev = resMngr.GetDevice();

        if ( _handle != Default )
            dev.vkDestroyPipeline( dev.GetVkDevice(), _handle, null );

        resMngr.ImmediatelyRelease( INOUT _layoutId );

        _handle         = Default;
        _layout         = Default;
        _layoutId       = Default;
        _localSize      = Default;
        _options        = Default;
        _subpassIndex   = UMax;
        _dbgTrace       = null;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    ParseShaderTrace
=================================================
*/
    bool  VTilePipeline::ParseShaderTrace (const void *ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        return _dbgTrace and _dbgTrace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT result );
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
