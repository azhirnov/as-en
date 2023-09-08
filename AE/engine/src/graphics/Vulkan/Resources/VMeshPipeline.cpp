// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VMeshPipeline.h"
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
    VMeshPipeline::~VMeshPipeline () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( not _handle );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VMeshPipeline::Create (VResourceManager &resMngr, const CreateInfo &ci) __NE___
    {
        DRC_EXLOCK( _drCheck );

        CHECK_ERR( not ci.shaders.empty() );
        CHECK_ERR( (ci.specCI.dynamicState & ~EPipelineDynamicState::GraphicsPipelineMask) == Zero );
        CHECK_ERR( not _handle and not _layout );
        CHECK_ERR( ci.specCI.renderStatePtr );
        CHECK_ERR( resMngr.GetFeatureSet().meshShader == EFeature::RequireTrue );

        const auto& render_state    = *ci.specCI.renderStatePtr;
        auto*       ppln_layout     = resMngr.GetResource( ci.layoutId, True{"incRef"} );
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

        _meshLocalSize = ushort3{
            ci.templCI.meshLocalSizeSpec.x == UMax or ci.specCI.meshLocalSize.x == UMax ? ci.templCI.meshDefaultLocalSize.x : ci.specCI.meshLocalSize.x,
            ci.templCI.meshLocalSizeSpec.y == UMax or ci.specCI.meshLocalSize.y == UMax ? ci.templCI.meshDefaultLocalSize.y : ci.specCI.meshLocalSize.y,
            ci.templCI.meshLocalSizeSpec.z == UMax or ci.specCI.meshLocalSize.z == UMax ? ci.templCI.meshDefaultLocalSize.z : ci.specCI.meshLocalSize.z };
        _taskLocalSize = ushort3{
            ci.templCI.taskLocalSizeSpec.x == UMax or ci.specCI.taskLocalSize.x == UMax ? ci.templCI.taskDefaultLocalSize.x : ci.specCI.taskLocalSize.x,
            ci.templCI.taskLocalSizeSpec.y == UMax or ci.specCI.taskLocalSize.y == UMax ? ci.templCI.taskDefaultLocalSize.y : ci.specCI.taskLocalSize.y,
            ci.templCI.taskLocalSizeSpec.z == UMax or ci.specCI.taskLocalSize.z == UMax ? ci.templCI.taskDefaultLocalSize.z : ci.specCI.taskLocalSize.z };
        CHECK_ERR( All( _meshLocalSize > Zero ));

        for (auto& sh : ci.shaders) {
            if ( AllBits( sh.stage, VK_SHADER_STAGE_TASK_BIT_EXT ))
                CHECK_ERR( All( _taskLocalSize > Zero ));
        }

        VkGraphicsPipelineCreateInfo            pipeline_info       = {};
        VkPipelineInputAssemblyStateCreateInfo  input_assembly_info = {};
        VkPipelineColorBlendStateCreateInfo     blend_info          = {};
        VkPipelineDepthStencilStateCreateInfo   depth_stencil_info  = {};
        VkPipelineMultisampleStateCreateInfo    multisample_info    = {};
        VkPipelineRasterizationStateCreateInfo  rasterization_info  = {};
        VkPipelineDynamicStateCreateInfo        dynamic_state_info  = {};
        VkPipelineVertexInputStateCreateInfo    vertex_input_info   = {};
        VkPipelineViewportStateCreateInfo       viewport_info       = {};
        VTempLinearAllocator                    allocator;

        // TODO: VkPipelineCreateFlags2CreateInfoKHR (VK_KHR_maintenance5)

        input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        const auto  AddCustomSpec = [&ci, this] (VkShaderStageFlagBits stage, VkSpecializationMapEntry* entryArr, uint* dataArr, OUT uint &count)
        {{
            if ( stage == VK_SHADER_STAGE_MESH_BIT_EXT and Any( ci.templCI.meshLocalSizeSpec != UMax ))
            {
                count = Sum<uint>( ci.templCI.meshLocalSizeSpec != UMax );

                if ( entryArr != null )
                {
                    uint    idx = 0;

                    if ( ci.templCI.meshLocalSizeSpec.x != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.meshLocalSizeSpec.x;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _meshLocalSize.x;
                        ++idx;
                    }
                    if ( ci.templCI.meshLocalSizeSpec.y != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.meshLocalSizeSpec.y;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _meshLocalSize.y;
                        ++idx;
                    }
                    if ( ci.templCI.meshLocalSizeSpec.z != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.meshLocalSizeSpec.z;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _meshLocalSize.z;
                        ++idx;
                    }
                }
            }
            if ( stage == VK_SHADER_STAGE_TASK_BIT_EXT and Any( ci.templCI.taskLocalSizeSpec != UMax ))
            {
                count = Sum<uint>( ci.templCI.taskLocalSizeSpec != UMax );

                if ( entryArr != null )
                {
                    uint    idx = 0;

                    if ( ci.templCI.taskLocalSizeSpec.x != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.taskLocalSizeSpec.x;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _taskLocalSize.x;
                        ++idx;
                    }
                    if ( ci.templCI.taskLocalSizeSpec.y != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.taskLocalSizeSpec.y;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _taskLocalSize.y;
                        ++idx;
                    }
                    if ( ci.templCI.taskLocalSizeSpec.z != UMax )
                    {
                        auto&   entry   = entryArr[idx];
                        entry.constantID= ci.templCI.taskLocalSizeSpec.z;
                        entry.offset    = sizeof(uint) * idx;
                        entry.size      = sizeof(uint);
                        dataArr[idx]    = _taskLocalSize.z;
                        ++idx;
                    }
                }
            }
        }};

        CHECK_ERR( SetShaderStages( OUT pipeline_info.pStages, OUT pipeline_info.stageCount, ci.shaders, ci.specCI.specialization, allocator, AddCustomSpec ));
        CHECK_ERR( SetDynamicState( OUT dynamic_state_info, ci.specCI.dynamicState, true, allocator ));
        SetMultisampleState( OUT multisample_info, render_state.multisample );
        SetDepthStencilState( OUT depth_stencil_info, render_state.depth, render_state.stencil );
        SetRasterizationState( OUT rasterization_info, render_state.rasterization );
        SetViewportState( OUT viewport_info, ci.specCI.viewportCount );
        CHECK_ERR( SetColorBlendState( OUT blend_info, render_state.color, *subpass, allocator ));

        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        pipeline_info.sType                 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.flags                 = VEnumCast( ci.specCI.options );
        pipeline_info.basePipelineIndex     = -1;
        pipeline_info.basePipelineHandle    = Default;
        pipeline_info.pInputAssemblyState   = &input_assembly_info;
        pipeline_info.pRasterizationState   = &rasterization_info;
        pipeline_info.pColorBlendState      = &blend_info;
        pipeline_info.pDepthStencilState    = &depth_stencil_info;
        pipeline_info.pMultisampleState     = &multisample_info;
        pipeline_info.pVertexInputState     = &vertex_input_info;
        pipeline_info.pDynamicState         = &dynamic_state_info;
        pipeline_info.layout                = _layout;
        pipeline_info.renderPass            = render_pass->Handle();
        pipeline_info.subpass               = subpass_idx;

        if ( not rasterization_info.rasterizerDiscardEnable )
        {
            pipeline_info.pViewportState        = &viewport_info;
        }else{
            pipeline_info.pViewportState        = null;
            pipeline_info.pMultisampleState     = null;
            pipeline_info.pDepthStencilState    = null;
            pipeline_info.pColorBlendState      = null;
        }

        auto&   dev = resMngr.GetDevice();
        VK_CHECK_ERR( dev.vkCreateGraphicsPipelines( dev.GetVkDevice(), ppln_cache, 1, &pipeline_info, null, OUT &_handle ));

        dev.SetObjectName( _handle, ci.specCI.dbgName, VK_OBJECT_TYPE_PIPELINE );

        _dynamicState   = ci.specCI.dynamicState;
        _options        = ci.specCI.options;
        _subpassIndex   = CheckCast<ubyte>(subpass_idx);

        CopyShaderTrace( ci.shaders, ci.allocator, OUT _dbgTrace );

        DEBUG_ONLY( _debugName = ci.specCI.dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VMeshPipeline::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto&   dev = resMngr.GetDevice();

        if ( _handle != Default )
            dev.vkDestroyPipeline( dev.GetVkDevice(), _handle, null );

        resMngr.ImmediatelyRelease( INOUT _layoutId );

        _handle         = Default;
        _layout         = Default;
        _layoutId       = Default;
        _meshLocalSize  = Default;
        _taskLocalSize  = Default;
        _dynamicState   = Default;
        _options        = Default;
        _subpassIndex   = UMax;
        _dbgTrace       = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    ParseShaderTrace
=================================================
*/
    bool  VMeshPipeline::ParseShaderTrace (const void *ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
    {
        result.clear();
        DRC_SHAREDLOCK( _drCheck );

        for (auto& trace : _dbgTrace)
        {
            Array<String>   temp;
            CHECK_ERR( trace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT temp ));
            result.insert( result.end(), temp.begin(), temp.end() );
        }
        return true;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
