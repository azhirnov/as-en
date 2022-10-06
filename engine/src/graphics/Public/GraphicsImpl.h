// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VTransferContext.h"
# include "graphics/Vulkan/Commands/VComputeContext.h"
# include "graphics/Vulkan/Commands/VDrawContext.h"
# include "graphics/Vulkan/Commands/VGraphicsContext.h"
# include "graphics/Vulkan/Commands/VASBuildContext.h"
# include "graphics/Vulkan/Commands/VRayTracingContext.h"

# include "graphics/Vulkan/Commands/VRenderTaskScheduler.h"

# include "graphics/Vulkan/Descriptors/VDescriptorUpdater.h"

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
		using CommandBuffer	= _hidden_::VSoftwareCmdBufPtr;
		using Transfer		= VIndirectTransferContext;
		using Compute		= VIndirectComputeContext;
		using Draw			= VIndirectDrawContext;
		using Graphics		= VIndirectGraphicsContext;
		using ASBuild		= VIndirectASBuildContext;
		using RayTracing	= VIndirectRayTracingContext;
	};

	using RenderTask			= VRenderTask;
	using DrawTask				= VDrawTask;
	using CommandBatch			= VCommandBatch;
	using CommandBatchPtr		= RC< VCommandBatch >;
	using DrawCommandBatchPtr	= RC< VDrawCommandBatch >;
	using CmdBatchOnSubmit		= VCmdBatchOnSubmit;

	using DescriptorUpdater		= VDescriptorUpdater;

	using GfxLinearMemAllocator	= VLinearMemAllocator;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#if defined(AE_PLATFORM_APPLE) and defined(AE_ENABLE_METAL)
# include "graphics/Metal/Commands/MTransferContext.h"
# include "graphics/Metal/Commands/MComputeContext.h"
# include "graphics/Metal/Commands/MDrawContext.h"
# include "graphics/Metal/Commands/MGraphicsContext.h"
//# include "graphics/Metal/Commands/MASBuildContext.h"

# include "graphics/Metal/Commands/MRenderTaskScheduler.h"

# include "graphics/Metal/Descriptors/MDescriptorUpdater.h"

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
		//using ASBuild		= MDirectASBuildContext;
	};
	
	struct IndirectCtx
	{
		using CommandBuffer	= _hidden_::MSoftwareCmdBuf;
		using Transfer		= MIndirectTransferContext;
		using Compute		= MIndirectComputeContext;
		using Draw			= MIndirectDrawContext;
		using Graphics		= MIndirectGraphicsContext;
		//using ASBuild		= MIndirectASBuildContext;
	};
	
	using RenderTask			= MRenderTask;
	using DrawTask				= MDrawTask;
	using CommandBatch			= MCommandBatch;
	using CommandBatchPtr		= RC< MCommandBatch >;
	using DrawCommandBatchPtr	= RC< MDrawCommandBatch >;
	using CmdBatchOnSubmit		= MCmdBatchOnSubmit;

	using DescriptorUpdater		= MDescriptorUpdater;
	
	using GfxLinearMemAllocator	= MLinearMemAllocator;

} // AE::Graphics

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------
