// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
types:

    DirectCtx::CommandBuffer
    DirectCtx::Transfer
    DirectCtx::Compute
    DirectCtx::Draw
    DirectCtx::Graphics
    DirectCtx::ASBuild
    DirectCtx::RayTracing

    IndirectCtx::CommandBuffer
    IndirectCtx::Transfer
    IndirectCtx::Compute
    IndirectCtx::Draw
    IndirectCtx::Graphics
    IndirectCtx::ASBuild
    IndirectCtx::RayTracing

    RenderTask
    RenderTaskCoro
    RenderTask_Get, RenderTask_GetRef
    RenderTask_Execute()
    RenderTask_AddInputDependency()

    DrawTask
    DrawTaskCoro
    DrawTask_Get, DrawTask_GetRef
    DrawTask_Execute()

    CommandBatch
    CommandBatchPtr
    CmdBatchOnSubmit

    DrawCommandBatch
    DrawCommandBatchPtr

    DescriptorUpdater
*/

#pragma once

#if defined(AE_ENABLE_VULKAN) and not defined(AE_ENABLE_REMOTE_GRAPHICS)
# include "graphics/Vulkan/Commands/VTransferContext.h"
# include "graphics/Vulkan/Commands/VComputeContext.h"
# include "graphics/Vulkan/Commands/VDrawContext.h"
# include "graphics/Vulkan/Commands/VGraphicsContext.h"
# include "graphics/Vulkan/Commands/VASBuildContext.h"
# include "graphics/Vulkan/Commands/VRayTracingContext.h"

# include "graphics/Vulkan/Descriptors/VDescriptorUpdater.h"

# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{
    struct DirectCtx
    {
        using CommandBuffer = VCommandBuffer;
        using Transfer      = VDirectTransferContext;
        using Compute       = VDirectComputeContext;
        using Draw          = VDirectDrawContext;
        using Graphics      = VDirectGraphicsContext;
        using ASBuild       = VDirectASBuildContext;
        using RayTracing    = VDirectRayTracingContext;
    };

    struct IndirectCtx
    {
        using CommandBuffer = Graphics::_hidden_::VSoftwareCmdBufPtr;
        using Transfer      = VIndirectTransferContext;
        using Compute       = VIndirectComputeContext;
        using Draw          = VIndirectDrawContext;
        using Graphics      = VIndirectGraphicsContext;
        using ASBuild       = VIndirectASBuildContext;
        using RayTracing    = VIndirectRayTracingContext;
    };

    using GRenderTaskScheduler  = VRenderTaskScheduler;

    using CommandBatch          = VCommandBatch;
    using CommandBatchPtr       = RC< CommandBatch >;

    using DrawCommandBatch      = VDrawCommandBatch;
    using DrawCommandBatchPtr   = RC< DrawCommandBatch >;

    using DescriptorUpdater     = VDescriptorUpdater;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_CPP_DETECT_MISMATCH

# ifdef AE_ENABLE_VULKAN
#   pragma detect_mismatch( "AE_ENABLE_VULKAN", "1" )
# else
#   pragma detect_mismatch( "AE_ENABLE_VULKAN", "0" )
# endif

# ifdef AE_VK_TIMELINE_SEMAPHORE
#   pragma detect_mismatch( "AE_VK_TIMELINE_SEMAPHORE", "1" )
# else
#   pragma detect_mismatch( "AE_VK_TIMELINE_SEMAPHORE", "0" )
# endif

# ifdef AE_ENABLE_GLSL_TRACE
#   pragma detect_mismatch( "AE_ENABLE_GLSL_TRACE", "1" )
# else
#   pragma detect_mismatch( "AE_ENABLE_GLSL_TRACE", "0" )
# endif

#endif // AE_CPP_DETECT_MISMATCH
