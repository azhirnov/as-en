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
	DirectCtx::VideoDecode
	DirectCtx::VideoEncode

	IndirectCtx::CommandBuffer
	IndirectCtx::Transfer
	IndirectCtx::Compute
	IndirectCtx::Draw
	IndirectCtx::Graphics
	IndirectCtx::ASBuild
	IndirectCtx::RayTracing
	IndirectCtx::VideoDecode
	IndirectCtx::VideoEncode

	RenderCoro
	RenderCoro_Get()
	RenderCoro_Execute()
	RenderCoro_AddInputDependency()
	RenderCoro_SkipCommands()

	DrawCoro
	DrawCoro_Get()
	DrawCoro_Execute()

	CommandBatch
	CommandBatchPtr
	CmdBatchOnSubmit
	OnFrameNextCycle

	DrawCommandBatch
	DrawCommandBatchPtr

	DescriptorUpdater
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VTransferContext.h"
# include "graphics_rhi/Vulkan/Commands/VComputeContext.h"
# include "graphics_rhi/Vulkan/Commands/VDrawContext.h"
# include "graphics_rhi/Vulkan/Commands/VGraphicsContext.h"
# include "graphics_rhi/Vulkan/Commands/VASBuildContext.h"
# include "graphics_rhi/Vulkan/Commands/VRayTracingContext.h"

# include "graphics_rhi/Vulkan/Descriptors/VDescriptorUpdater.h"

# include "graphics_rhi/Vulkan/VRenderTaskScheduler.h"

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
	//	using VideoDecode	= VDirectVideoDecodeContext;
	//	using VideoEncode	= VDirectVideoEncodeContext;
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
	//	using VideoDecode	= VIndirectVideoDecodeContext;
	//	using VideoEncode	= VIndirectVideoEncodeContext;
	};

	using QueryManager = VQueryManager;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
# include "graphics_rhi/Metal/Commands/MTransferContext.h"
# include "graphics_rhi/Metal/Commands/MComputeContext.h"
# include "graphics_rhi/Metal/Commands/MDrawContext.h"
# include "graphics_rhi/Metal/Commands/MGraphicsContext.h"
# include "graphics_rhi/Metal/Commands/MASBuildContext.h"
# include "graphics_rhi/Metal/Commands/MRayTracingContext.h"

# include "graphics_rhi/Metal/Descriptors/MDescriptorUpdater.h"

# include "graphics_rhi/Metal/MRenderTaskScheduler.h"

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
	//	using VideoDecode	= MDirectVideoDecodeContext;
	//	using VideoEncode	= MDirectVideoEncodeContext;
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
	//	using VideoDecode	= MIndirectVideoDecodeContext;
	//	using VideoEncode	= MIndirectVideoEncodeContext;
	};
	
	using QueryManager = MQueryManager;

} // AE::Graphics

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Commands/RTransferContext.h"
# include "graphics_rhi/Remote/Commands/RComputeContext.h"
# include "graphics_rhi/Remote/Commands/RDrawContext.h"
# include "graphics_rhi/Remote/Commands/RGraphicsContext.h"
# include "graphics_rhi/Remote/Commands/RASBuildContext.h"
# include "graphics_rhi/Remote/Commands/RRayTracingContext.h"

# include "graphics_rhi/Remote/Descriptors/RDescriptorUpdater.h"

# include "graphics_rhi/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
	struct DirectCtx
	{
		using CommandBuffer	= Graphics::_hidden_::RSoftwareCmdBufPtr;
		using Transfer		= RTransferContext;
		using Compute		= RComputeContext;
		using Draw			= RDrawContext;
		using Graphics		= RGraphicsContext;
		using ASBuild		= RASBuildContext;
		using RayTracing	= RRayTracingContext;
	//	using VideoDecode	= RVideoDecodeContext;
	//	using VideoEncode	= RVideoEncodeContext;
	};

	using IndirectCtx	= DirectCtx;
	
	using QueryManager	= RQueryManager;

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
//-----------------------------------------------------------------------------



#ifdef AE_CPP_DETECT_MISMATCH

# ifdef AE_ENABLE_METAL
#	pragma detect_mismatch( "AE_ENABLE_METAL", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_METAL", "0" )
# endif

# ifdef AE_ENABLE_METAL
#  if AE_METAL_NATIVE_DEBUGGER
#	pragma detect_mismatch( "AE_METAL_NATIVE_DEBUGGER", "1" )
#  else
#	pragma detect_mismatch( "AE_METAL_NATIVE_DEBUGGER", "0" )
#  endif
# else
#	pragma detect_mismatch( "AE_METAL_NATIVE_DEBUGGER", "0" )
# endif

# ifdef AE_ENABLE_VULKAN
#	pragma detect_mismatch( "AE_ENABLE_VULKAN", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_VULKAN", "0" )
# endif

# ifdef AE_VK_TIMELINE_SEMAPHORE
#	pragma detect_mismatch( "AE_VK_TIMELINE_SEMAPHORE", "1" )
# else
#	pragma detect_mismatch( "AE_VK_TIMELINE_SEMAPHORE", "0" )
# endif

# ifdef AE_ENABLE_GLSL_TRACE
#	pragma detect_mismatch( "AE_ENABLE_GLSL_TRACE", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_GLSL_TRACE", "0" )
# endif

# ifdef AE_ENABLE_REMOTE_GRAPHICS
#	pragma detect_mismatch( "AE_ENABLE_REMOTE_GRAPHICS", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_REMOTE_GRAPHICS", "0" )
# endif

#endif // AE_CPP_DETECT_MISMATCH
