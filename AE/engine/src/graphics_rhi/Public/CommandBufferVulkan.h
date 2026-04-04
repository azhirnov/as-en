// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/CommandBuffer.h"

namespace AE::Graphics
{

	//
	// Vulkan Draw Context interface
	//

	class IDrawContextVk : public IDrawContext
	{
	// interface
	public:

	// dynamic states //
		//	requires: EPipelineDynamicState::DepthBounds
		virtual	void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilCompareMask
		virtual	void  SetStencilCompareMask (uint compareMask)																		__Th___ = 0;
		virtual	void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilWriteMask
		virtual	void  SetStencilWriteMask (uint writeMask)																			__Th___ = 0;
		virtual	void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th___ = 0;
		//	requires: EPipelineDynamicState::FragmentShadingRate
		virtual	void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp)	__Th___ = 0;


	// draw commands //
		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawIndirectCommand' with stride 'stride'.
		//	'countBuffer' at 'countBufferOffset' must contains single 'uint' value.
				void  DrawIndirectCount (const DrawIndirectCountCmd &cmd)															__Th___	{ DrawIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawIndirectCount (BufferID	indirectBuffer,
										 Bytes		indirectBufferOffset,
										 BufferID	countBuffer,
										 Bytes		countBufferOffset,
										 uint		maxDrawCount,
										 Bytes		stride)																			__Th___ = 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawIndexedIndirectCommand' with stride 'stride'.
		//	'countBuffer' at 'countBufferOffset' must contains single 'uint' value.
				void  DrawIndexedIndirectCount (const DrawIndexedIndirectCountCmd &cmd)												__Th___	{ DrawIndexedIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
												Bytes		indirectBufferOffset,
												BufferID	countBuffer,
												Bytes		countBufferOffset,
												uint		maxDrawCount,
												Bytes		stride)																	__Th___ = 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawMeshTasksIndirectCommand' with stride 'stride'.
		//	'countBuffer' at 'countBufferOffset' must contains single 'uint' value.
				void  DrawMeshTasksIndirectCount (const DrawMeshTasksIndirectCountCmd &cmd)											__Th___	{ DrawMeshTasksIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  BufferID	countBuffer,
												  Bytes		countBufferOffset,
												  uint		maxDrawCount,
												  Bytes		stride)																	__Th___ = 0;

	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)							__Th___ = 0;
	};



	//
	// Vulkan Transfer Context interface
	//

	class ITransferContextVk : public ITransferContext
	{
	// interface
	public:

	// only for host-visible memory //
		ND_	virtual	bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)			__Th___ = 0;


	// only in compute/graphics queue //

		//		image: EResourceState::ClearDst
				void  ClearColorImage (ImageID image, const RGBA8u  &color, ArrayView<ImageSubresourceRange> ranges)	__Th___;
		virtual	void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;
		virtual	void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;
		virtual	void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;


	// only in graphics queue //

		//		image: EResourceState::ClearDst
		virtual	void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil,
											  ArrayView<ImageSubresourceRange> ranges)									__Th___ = 0;

		//		srcImage: EResourceState::BlitSrc
		//		dstImage: EResourceState::BlitDst
		virtual	void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)				__Th___ = 0;


	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Compute Context interface
	//

	class IComputeContextVk : public IComputeContext
	{
	// interface
	public:

		//  requires 'cooperativeVector' feature
		//		srcAddress: EResourceState::CoopVecConvert_Read
		//		dstAddress: EResourceState::CoopVecConvert_Write
		virtual void  ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd>)									__Th___ = 0;
		virtual void  ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd2>)									__Th___ = 0;


	// indirect commands //
		virtual void  PreprocessGeneratedCommands (const PreprocessGeneratedCommandsCmd &)								__Th___ = 0;
		virtual void  PreprocessGeneratedCommands (const PreprocessGeneratedCommands2Cmd &)								__Th___ = 0;


	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Graphics Context interface
	//

	class IGraphicsContextVk : public IGraphicsContext
	{
	// interface
	public:

	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Ray Tracing Context interface
	//

	class IRayTracingContextVk : public IRayTracingContext
	{
	// interface
	public:

		//	'address' must contains single object with 'TraceRayIndirectCommand' type.
		//		address: EResourceState::BuildRTAS_IndirectBuffer
		virtual	void  TraceRaysIndirectAddress (const RTShaderBindingTable &sbt, DeviceAddress address)					__Th___ = 0;

		//	'address' must contains single object with 'TraceRayIndirectCommand2' type.
		//	requires 'rayTracingPipelineTraceRaysIndirect2' feature flag.
		//		address: EResourceState::BuildRTAS_IndirectBuffer
		virtual	void  TraceRaysIndirectAddress2 (DeviceAddress address)													__Th___ = 0;


	// indirect commands //
		virtual void  PreprocessGeneratedCommands (const PreprocessGeneratedCommandsCmd &)								__Th___ = 0;
		virtual void  PreprocessGeneratedCommands (const PreprocessGeneratedCommands2Cmd &)								__Th___ = 0;


	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Acceleration Structure Build Context interface
	//

	class IASBuildContextVk : public IASBuildContext
	{
	// interface
	public:
		using IASBuildContext::Build;
		using IASBuildContext::Copy;
		using IASBuildContext::WriteProperty;
		using IASBuildContext::ReadProperty;


	// Vulkan: AS build stage - batch commands for parallel execution

		//	requires 'accelerationStructureIndirectBuild' feature flag

		//		indirectBuffer:		EResourceState::BuildRTAS_IndirectBuffer
		//		dst:				EResourceState::BuildRTAS_Read
		//		all buffers in cmd:	EResourceState::BuildRTAS_Read

		//		'indirectBuffer' for geometry build: array of 'ASBuildIndirectCommand [TriangleCount + AABBsCount]'
		virtual	void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer,
									 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)								__Th___ = 0;
		virtual	void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
									 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
									 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)								__Th___ = 0;

		//		'indirectBuffer' for scene build: single 'ASBuildIndirectCommand'
		virtual	void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer)				__Th___ = 0;
		virtual	void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
									 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)							__Th___ = 0;

		// TODO
		virtual void  BuildClusterIndirect (const RTClusterBuild &cmd)													__Th___ = 0;
		virtual void  BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd)										__Th___ = 0;


	// Vulkan: AS copy stage - batch commands for parallel execution

		//		src: EResourceState::CopyRTAS_Read
		//		dst: EResourceState::CopyRTAS_Write
		virtual	void  SerializeToMemory (RTGeometryID src, DeviceAddress dst)											__Th___ = 0;
		virtual	void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)								__Th___ = 0;

		virtual	void  SerializeToMemory (RTSceneID src, DeviceAddress dst)												__Th___ = 0;
		virtual	void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)									__Th___ = 0;

		virtual	void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)										__Th___ = 0;
		virtual	void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)							__Th___ = 0;

		virtual	void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)											__Th___ = 0;
		virtual	void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)								__Th___ = 0;


	// Vulkan: Micromap build stage - batch commands for parallel execution

		//		dst: EResourceState::BuildMicromap_Write
		//		all buffers in cmd: EResourceState::BuildMicromap_Read
		virtual void  Build (const RTMicromapBuild &cmd, RTMicromapID dst)												__Th___	= 0;

		// If used ERTASCopyMode::Compaction 'dst' must have at least 'size = ReadCompactedSize( dst )'.
		//		src: EResourceState::BuildMicromap_Read
		//		dst: EResourceState::BuildMicromap_Write
		virtual void  Copy (RTMicromapID src, RTMicromapID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)				__Th___	= 0;

		//		src: EResourceState::BuildMicromap_Read
		//		dst: EResourceState::BuildMicromap_Write
		virtual void  SerializeToMemory (RTMicromapID src, DeviceAddress dst)											__Th___ = 0;
		virtual	void  SerializeToMemory (RTMicromapID src, BufferID dst, Bytes dstOffset)								__Th___ = 0;

		virtual	void  DeserializeFromMemory (DeviceAddress src, RTMicromapID dst)										__Th___ = 0;
		virtual	void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTMicromapID dst)							__Th___ = 0;

		//		micromap:  EResourceState::BuildMicromap_Read
		//		dstBuffer: EResourceState::CopyDst
		virtual void  WriteProperty (ERTASProperty property, RTMicromapID micromap, BufferID dstBuffer, Bytes offset, Bytes size = UMax) __Th___ = 0;

		//		micromap: EResourceState::CopyRTAS_Read
		ND_ virtual Promise<Bytes>  ReadProperty (ERTASProperty property, RTMicromapID micromap)						__Th___	= 0;


	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Video Decode
	//

	class IVideoDecodeContextVk : public IVideoDecodeContext
	{
	// interface
	public:

	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};



	//
	// Vulkan Video Encode
	//

	class IVideoEncodeContextVk : public IVideoEncodeContext
	{
	// interface
	public:

	// for debugging //
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)				__Th___ = 0;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	ClearColorImage
=================================================
*/
	inline void  ITransferContextVk::ClearColorImage (ImageID image, const RGBA8u &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ClearColorImage( image, RGBA32f{color}, ranges );
	}

} // AE::Graphics
