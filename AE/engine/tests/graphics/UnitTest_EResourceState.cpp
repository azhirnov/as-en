// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"
#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
#endif
#include "UnitTest_Common.h"

namespace
{
    static void  EResourceState_Test1 ()
    {
        TEST( EResourceState_RequireShaderStage( EResourceState::ShaderUniform ));
        TEST( not EResourceState_RequireShaderStage( EResourceState::Host_Write ));

        TEST( not EResourceState_IsReadOnly( EResourceState::ShaderStorage_Write ));
        TEST( EResourceState_IsReadOnly( EResourceState::ShaderStorage_Read ));
        TEST( EResourceState_HasReadAccess( EResourceState::ShaderStorage_ReadWrite ));
        TEST( EResourceState_HasWriteAccess( EResourceState::ShaderStorage_ReadWrite ));
        TEST( not EResourceState_HasWriteAccess( EResourceState::ShaderStorage_Read ));

        //TEST( EResourceState_IsDepthReadOnly( EResourceState::DepthStencilTest ));
        //TEST( not EResourceState_IsDepthReadOnly( EResourceState::DepthStencilAttachment_RW ));
        //TEST( EResourceState_IsStencilReadOnly( EResourceState::DepthStencilTest ));
        //TEST( not EResourceState_IsStencilReadOnly( EResourceState::DepthTest_StencilRW ));
    }


    static void  EResourceState_Test2 ()
    {
    }


#ifdef AE_ENABLE_VULKAN
    static void  EResourceState_VkTest1 ()
    {
        constexpr auto ShaderStages =
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT |
            VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT |
            VK_PIPELINE_STAGE_2_CLUSTER_CULLING_SHADER_BIT_HUAWEI |
            VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI |
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;

        VkPipelineStageFlagBits2    stage;
        VkAccessFlagBits2           access;
        VkImageLayout               layout;

        EResourceState_ToSrcStageAccessLayout( EResourceState::ShaderSample | EResourceState::FragmentShader, OUT stage, OUT access, OUT layout );
        TEST( stage == VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT );
        TEST( access == VK_ACCESS_2_SHADER_SAMPLED_READ_BIT );
        TEST( layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

        EResourceState_ToSrcStageAccessLayout( EResourceState::DepthStencilTest_ShaderSample | EResourceState::FragmentShader | EResourceState::DSTestBeforeFS, OUT stage, OUT access, OUT layout );
        TEST( stage == (VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT) );
        TEST( access == (VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT) );
        TEST( layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL );

        EResourceState_ToSrcStageAccessLayout( EResourceState::InputColorAttachment_RW | EResourceState::FragmentShader, OUT stage, OUT access, OUT layout );
        TEST( stage == (VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT) );
        TEST( access == (VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT) );
        TEST( layout == VK_IMAGE_LAYOUT_GENERAL );

        for (uint i = 0; i < _EResState::_AccessCount; ++i)
        {
            EResourceState  state   = EResourceState(i);
            const bool      req_sh  = EResourceState_RequireShaderStage( state );

            if ( req_sh )
                state |= EResourceState::ComputeShader;
            else
                state |= EResourceState::DSTestBeforeFS;

            EResourceState_ToDstStageAccessLayout( state, OUT stage, OUT access, OUT layout );
            TEST( (i < _EResState::Preserve+1) or (stage != 0) );

            if ( req_sh ){
                TEST( AnyBits( stage, ShaderStages ));
            }else{
                TEST( not AnyBits( stage, ShaderStages ) or (i == _EResState::RTShaderBindingTable) );
            }
        }
    }
#endif
}


extern void  UnitTest_EResourceState ()
{
    EResourceState_Test1();
    EResourceState_Test2();

    #ifdef AE_ENABLE_VULKAN
    EResourceState_VkTest1();
    #endif

    TEST_PASSED();
}
