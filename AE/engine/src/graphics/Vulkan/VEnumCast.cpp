// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
    EResourceState_ToStageAccessLayout
=================================================
*/
    void  EResourceState_ToDstStageAccessLayout (const EResourceState value, OUT VkPipelineStageFlagBits2 &outStage, OUT VkAccessFlagBits2 &outAccess, OUT VkImageLayout &outLayout) __NE___
    {
        struct StateInfo
        {
            VkPipelineStageFlagBits2    stage;
            VkAccessFlagBits2           access;
            VkImageLayout               layout;

        #ifdef AE_DEBUG
            _EResState::EState          _dbgState;

            constexpr StateInfo (VkPipelineStageFlagBits2 stage, VkAccessFlagBits2 access, VkImageLayout layout, _EResState::EState state) :
                stage{stage}, access{access}, layout{layout}, _dbgState{state} {}
        #else

            constexpr StateInfo (VkPipelineStageFlagBits2 stage, VkAccessFlagBits2 access, VkImageLayout layout, _EResState::EState) :
                stage{stage}, access{access}, layout{layout} {}
        #endif
        };

        static constexpr VkPipelineStageFlagBits2   UseShaderStages = 1ull << 62;
        static constexpr VkPipelineStageFlagBits2   UseDSStages     = 1ull << 63;
        static constexpr StateInfo                  info_arr []     = {
            StateInfo{ VK_PIPELINE_STAGE_2_NONE,                                            VK_ACCESS_2_NONE,                                                                                                                       VK_IMAGE_LAYOUT_UNDEFINED,                                  _EResState::Unknown                         },
            StateInfo{ VK_PIPELINE_STAGE_2_NONE,                                            VK_ACCESS_2_NONE,                                                                                                                       VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::Preserve                        },
            StateInfo{ VK_PIPELINE_STAGE_2_COPY_BIT,                                        VK_ACCESS_2_TRANSFER_READ_BIT,                                                                                                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                       _EResState::CopySrc                         },
            StateInfo{ VK_PIPELINE_STAGE_2_COPY_BIT,                                        VK_ACCESS_2_TRANSFER_WRITE_BIT,                                                                                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                       _EResState::CopyDst                         },
            StateInfo{ VK_PIPELINE_STAGE_2_CLEAR_BIT,                                       VK_ACCESS_2_TRANSFER_WRITE_BIT,                                                                                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                       _EResState::ClearDst                        },
            StateInfo{ VK_PIPELINE_STAGE_2_BLIT_BIT,                                        VK_ACCESS_2_TRANSFER_READ_BIT,                                                                                                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                       _EResState::BlitSrc                         },
            StateInfo{ VK_PIPELINE_STAGE_2_BLIT_BIT,                                        VK_ACCESS_2_TRANSFER_WRITE_BIT,                                                                                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                       _EResState::BlitDst                         },
            StateInfo{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,                     VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,                                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                   _EResState::ColorAttachment                 },
            StateInfo{ UseDSStages,                                                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,                                                                                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,            _EResState::DepthStencilTest                },
            StateInfo{ UseDSStages,                                                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,                                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,           _EResState::DepthStencilAttachment_RW       },
            StateInfo{ UseDSStages,                                                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,                                         VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, _EResState::DepthTest_StencilRW             },
            StateInfo{ UseDSStages,                                                         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,                                         VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL, _EResState::DepthRW_StencilTest             },
            StateInfo{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,                     VK_ACCESS_2_NONE,                                                                                                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                            _EResState::PresentImage                    },  // swapchain semaphore creates memory dependency
            StateInfo{ VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,        VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,                                                                              VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,_EResState::ShadingRateImage               },
            StateInfo{ VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,                                VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT,                                                                             VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::General                         },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_SHADER_STORAGE_READ_BIT,                                                                                                    VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::ShaderStorage_Read              },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,                                                                                                   VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::ShaderStorage_Write             },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,                                                             VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::ShaderStorage_RW                },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_UNIFORM_READ_BIT,                                                                                                           VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::ShaderUniform                   },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,                                                                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                   _EResState::ShaderSample                    },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT,                                                                                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                   _EResState::InputColorAttachment            },
            StateInfo{ UseShaderStages | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,   VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,                 VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::InputColorAttachment_RW         },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT,                                                                                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                   _EResState::InputDepthStencilAttachment     },
            StateInfo{ UseShaderStages | UseDSStages,                                       VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::InputDepthStencilAttachment_RW  },
            StateInfo{ UseShaderStages | UseDSStages,                                       VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,                                                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,            _EResState::DepthStencilTest_ShaderSample   },
            StateInfo{ UseShaderStages | UseDSStages,                                       VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,   VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, _EResState::DepthTest_DepthSample_StencilRW },
            StateInfo{ VK_PIPELINE_STAGE_2_HOST_BIT,                                        VK_ACCESS_2_HOST_READ_BIT,                                                                                                              VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::Host_Read                       },
            StateInfo{ VK_PIPELINE_STAGE_2_HOST_BIT,                                        VK_ACCESS_2_HOST_WRITE_BIT,                                                                                                             VK_IMAGE_LAYOUT_GENERAL,                                    _EResState::Host_Write                      },
            StateInfo{ UseShaderStages,                                                     VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR,                                                                                        VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::ShaderRTAS                      },
            StateInfo{ VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,                               VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,                                                                                                  VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::IndirectBuffer                  },
            StateInfo{ VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,                                 VK_ACCESS_2_INDEX_READ_BIT,                                                                                                             VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::IndexBuffer                     },
            StateInfo{ VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,                      VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,                                                                                                  VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::VertexBuffer                    },
            StateInfo{ VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR,             VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_TRANSFER_READ_BIT,                                                        VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::CopyRTAS_Read                   },
            StateInfo{ VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR,             VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_2_TRANSFER_WRITE_BIT,                                                      VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::CopyRTAS_Write                  },
            StateInfo{ VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,            VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_SHADER_READ_BIT,                                                            VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::BuildRTAS_Read                  },
            StateInfo{ VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,            VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,                                     VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::BuildRTAS_RW                    },
            StateInfo{ VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,            VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,                                                                                                  VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::BuildRTAS_IndirectBuffer        },
            StateInfo{ VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR,                      VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR | VK_ACCESS_2_SHADER_STORAGE_READ_BIT,                                                    VK_IMAGE_LAYOUT_MAX_ENUM,                                   _EResState::RTShaderBindingTable            }
        };
        STATIC_ASSERT( CountOf(info_arr) == _EResState::_AccessCount );

        constexpr auto VertexProcessingShaders =
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT;

        DBG_CHECK_MSG( EResourceState_RequireShaderStage( value ) == AnyBits( value, EResourceState::AllShaders ),
                       "shader stage is not compatible with access mask" );

        VkPipelineStageFlagBits2    sh_stages   = Zero;
        VkPipelineStageFlagBits2    ds_stages   = Zero;

        // PreRasterizationShaders
        sh_stages |= AnyBits( value, EResourceState::MeshTaskShader )           ? VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT | VK_PIPELINE_STAGE_2_CLUSTER_CULLING_SHADER_BIT_HUAWEI : 0;
        sh_stages |= AnyBits( value, EResourceState::VertexProcessingShaders )  ? VertexProcessingShaders                       : 0;
        sh_stages |= AnyBits( value, EResourceState::TileShader )               ? VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI: 0;
        sh_stages |= AnyBits( value, EResourceState::FragmentShader )           ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT       : 0;
        // ignore PostRasterizationShaders
        sh_stages |= AnyBits( value, EResourceState::ComputeShader )            ? VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT        : 0;
        sh_stages |= AnyBits( value, EResourceState::RayTracingShaders )        ? VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR: 0;

        ds_stages |= AnyBits( value, EResourceState::DSTestBeforeFS )           ? VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT  : 0;
        ds_stages |= AnyBits( value, EResourceState::DSTestAfterFS )            ? VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT   : 0;


        auto&   info = info_arr[ ToEResState(value) ];
        outStage    = info.stage;
        outAccess   = info.access;
        outLayout   = info.layout;

        outStage    = AnyBits( outStage, UseShaderStages )  ? (outStage & ~UseShaderStages) | sh_stages  : outStage;
        outStage    = AnyBits( outStage, UseDSStages )      ? (outStage & ~UseDSStages)     | ds_stages  : outStage;

        #ifdef AE_DEBUG
        CHECK( ToEResState(value) == info._dbgState );
        #endif
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
