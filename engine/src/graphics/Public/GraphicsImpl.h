// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VTransferContext.h"
# include "graphics/Vulkan/Commands/VComputeContext.h"
# include "graphics/Vulkan/Commands/VDrawContext.h"
# include "graphics/Vulkan/Commands/VGraphicsContext.h"
# include "graphics/Vulkan/Commands/VASBuildContext.h"
# include "graphics/Vulkan/Commands/VRayTracingContext.h"

# include "graphics/Vulkan/Commands/VCmdBufferDebugger.h"

# include "graphics/Vulkan/Descriptors/VDescriptorUpdater.h"

# include "graphics/Vulkan/VRenderTaskScheduler.h"

# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/Allocators/VDedicatedMemAllocator.h"
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Allocators/VUniMemAllocator.h"

namespace AE::Graphics
{
	struct DirectCtx
	{
		using CommandBuffer	= VCommandBuffer;
		using Transfer		= VDirectTransferContext;
		using Compute		= VDirectComputeContext;
		using Draw			= VDirectDrawContext;
		using Graphics		= VDirectGraphicsContext;
		using ASBuild		= VDirectASBuildContext;
		using RayTracing	= VDirectRayTracingContext;
	};
	
	struct IndirectCtx
	{
		using CommandBuffer	= Graphics::_hidden_::VSoftwareCmdBufPtr;
		using Transfer		= VIndirectTransferContext;
		using Compute		= VIndirectComputeContext;
		using Draw			= VIndirectDrawContext;
		using Graphics		= VIndirectGraphicsContext;
		using ASBuild		= VIndirectASBuildContext;
		using RayTracing	= VIndirectRayTracingContext;
	};

	// RenderTask
	// CoroutineRenderTask
	// RenderTask_Get
	// RenderTask_Execute()
	// RenderTask_AddInputDependency()

	// DrawTask
	// CoroutineDrawTask
	// DrawTask_Get
	// DrawTask_Execute()

	using GRenderTaskScheduler	= VRenderTaskScheduler;

	using CommandBatch			= VCommandBatch;
	using CommandBatchPtr		= RC< CommandBatch >;
	// CmdBatchOnSubmit

	using DrawCommandBatch		= VDrawCommandBatch;
	using DrawCommandBatchPtr	= RC< DrawCommandBatch >;

	using DescriptorUpdater		= VDescriptorUpdater;

	using GfxLinearMemAllocator	= VLinearMemAllocator;
	using GfxBlockMemAllocator	= VBlockMemAllocator;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#if not defined(AE_ENABLE_VULKAN) and defined(AE_ENABLE_METAL)
# include "graphics/Metal/Commands/MTransferContext.h"
# include "graphics/Metal/Commands/MComputeContext.h"
# include "graphics/Metal/Commands/MDrawContext.h"
# include "graphics/Metal/Commands/MGraphicsContext.h"
# include "graphics/Metal/Commands/MASBuildContext.h"
# include "graphics/Metal/Commands/MRayTracingContext.h"

# include "graphics/Metal/Descriptors/MDescriptorUpdater.h"

# include "graphics/Metal/MRenderTaskScheduler.h"

# include "graphics/Metal/Allocators/MLinearMemAllocator.h"

namespace AE::Graphics
{
	struct DirectCtx
	{
		using CommandBuffer	= MCommandBuffer;
		using Transfer		= MDirectTransferContext;
		using Compute		= MDirectComputeContext;
		using Draw			= MDirectDrawContext;
		using Graphics		= MDirectGraphicsContext;
		using ASBuild		= MDirectASBuildContext;
		using RayTracing	= MDirectRayTracingContext;
	};
	
	struct IndirectCtx
	{
		using CommandBuffer	= Graphics::_hidden_::MSoftwareCmdBufPtr;
		using Transfer		= MIndirectTransferContext;
		using Compute		= MIndirectComputeContext;
		using Draw			= MIndirectDrawContext;
		using Graphics		= MIndirectGraphicsContext;
		using ASBuild		= MIndirectASBuildContext;
		using RayTracing	= MIndirectRayTracingContext;
	};
	
	// RenderTask
	// CoroutineRenderTask
	// RenderTask_Get
	// RenderTask_Execute()
	// RenderTask_AddInputDependency()

	// DrawTask
	// CoroutineDrawTask
	// DrawTask_Get
	// DrawTask_Execute()

	using GRenderTaskScheduler	= MRenderTaskScheduler;

	using CommandBatch			= MCommandBatch;
	using CommandBatchPtr		= RC< CommandBatch >;
	// CmdBatchOnSubmit
	
	using DrawCommandBatch		= MDrawCommandBatch;
	using DrawCommandBatchPtr	= RC< DrawCommandBatch >;

	using DescriptorUpdater		= MDescriptorUpdater;
	
	using GfxLinearMemAllocator	= MLinearMemAllocator;
//	using GfxBlockMemAllocator	= MBlockMemAllocator;

} // AE::Graphics

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RTransferContext.h"
# include "graphics/Remote/Commands/RComputeContext.h"
# include "graphics/Remote/Commands/RDrawContext.h"
# include "graphics/Remote/Commands/RGraphicsContext.h"
# include "graphics/Remote/Commands/RASBuildContext.h"

namespace AE::Graphics
{
	struct DirectCtx
	{
		using CommandBuffer	= RCommandBuffer;
		using Transfer		= RDirectTransferContext;
		using Compute		= RDirectComputeContext;
		using Draw			= RDirectDrawContext;
		using Graphics		= RDirectGraphicsContext;
		using ASBuild		= RDirectASBuildContext;
		using RayTracing	= RDirectRayTracingContext;
	};
	
	struct IndirectCtx
	{
		using CommandBuffer	= Graphics::_hidden_::RSoftwareCmdBufPtr;
		using Transfer		= RIndirectTransferContext;
		using Compute		= RIndirectComputeContext;
		using Draw			= RIndirectDrawContext;
		using Graphics		= RIndirectGraphicsContext;
		using ASBuild		= RIndirectASBuildContext;
		using RayTracing	= RIndirectRayTracingContext;
	};
	
	// RenderTask
	// CoroutineRenderTask
	// RenderTask_Get
	// RenderTask_Execute()
	// RenderTask_AddInputDependency()

	// DrawTask
	// CoroutineDrawTask
	// DrawTask_Get
	// DrawTask_Execute()

	using GRenderTaskScheduler	= RRenderTaskScheduler;

	using CommandBatch			= RCommandBatch;
	using CommandBatchPtr		= RC< CommandBatch >;
	// CmdBatchOnSubmit

	using DrawCommandBatch		= RDrawCommandBatch;
	using DrawCommandBatchPtr	= RC< DrawCommandBatch >;

	using DescriptorUpdater		= RDescriptorUpdater;

	using GfxLinearMemAllocator	= RLinearMemAllocator;
	using GfxBlockMemAllocator	= RBlockMemAllocator;

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
