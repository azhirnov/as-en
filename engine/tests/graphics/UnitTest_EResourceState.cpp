// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"
#include "graphics/Private/EnumToString.h"
#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
#endif
#include "UnitTest_Common.h"

namespace
{
    static void  EResourceState_Test1 ()
    {
        TEST( EResourceState_RequireShaderStage( EResourceState::ShaderUniform ));
        TEST( not EResourceState_RequireShaderStage( EResourceState::Host_RW ));

        TEST( not EResourceState_IsReadOnly( EResourceState::ShaderStorage_Write ));
        TEST( EResourceState_IsReadOnly( EResourceState::ShaderStorage_Read ));
        TEST( EResourceState_HasReadAccess( EResourceState::ShaderStorage_ReadWrite ));
        TEST( EResourceState_HasWriteAccess( EResourceState::ShaderStorage_ReadWrite ));
        TEST( not EResourceState_HasWriteAccess( EResourceState::ShaderStorage_Read ));

        TEST( EResourceState_IsDepthReadOnly( EResourceState::DepthStencilTest ));
        TEST( not EResourceState_IsDepthReadOnly( EResourceState::DepthStencilAttachment_RW ));
        TEST( EResourceState_IsStencilReadOnly( EResourceState::DepthStencilTest ));
        TEST( not EResourceState_IsStencilReadOnly( EResourceState::DepthTest_StencilRW ));

        //TEST( EResourceState_ToQueueType( EResourceState::ShaderSample | EResourceState::FragmentShader ) == EQueueType::Graphics );
        //TEST( EResourceState_ToQueueType( EResourceState::ShaderSample | EResourceState::ComputeShader ) == EQueueType::AsyncCompute );
        //TEST( EResourceState_ToQueueType( EResourceState::CopyDst ) == EQueueType::AsyncTransfer );
    }

#ifdef AE_ENABLE_VULKAN
    static void  EResourceState_VkTest1 ()
    {
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
    }
#endif
}


extern void  UnitTest_EResourceState ()
{
    EResourceState_Test1();

    #ifdef AE_ENABLE_VULKAN
    EResourceState_VkTest1();
    #endif

    TEST_PASSED();
}
