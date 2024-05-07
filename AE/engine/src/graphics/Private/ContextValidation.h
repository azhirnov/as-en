// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/CommandBuffer.h"
#include "graphics/Public/FeatureSet.h"
#include "graphics/Private/Defines.h"

#if AE_VALIDATE_GCTX
# define VALIDATE_GCTX( /*fn call*/... )	{Validator_t:: __VA_ARGS__;}
#else
# define VALIDATE_GCTX( ... )				{}
#endif

#ifdef AE_COMPILER_MSVC
# pragma warning (push)
# pragma warning (disable: 4100)
# pragma warning (disable: 4189)
#endif

namespace AE::Graphics::_hidden_
{
#if AE_VALIDATE_GCTX == 0

	class TransferContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class ComputeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class DrawContextValidation final : Noninstanceable
	{
	public:
		//static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class GraphicsContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class ASBuildContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class RayTracingContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class VideoDecodeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

	class VideoEncodeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};

//-----------------------------------------------------------------------------
#else


	//
	// Transfer Context Validation
	//
	class TransferContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue)																			__Th___;

		static void  ClearColorImage (const ImageDesc &img, ArrayView<ImageSubresourceRange> ranges)							__Th___;
		static void  ClearDepthStencilImage (const ImageDesc &img, ArrayView<ImageSubresourceRange> ranges)						__Th___;

		static void  FillBuffer (const BufferDesc &buf, Bytes offset, Bytes size)												__Th___;

		static void  UpdateBuffer (const BufferDesc &buf, Bytes offset, Bytes size, const void* data)							__Th___;
		static void  UploadBuffer (const BufferDesc &buf, Bytes offset, Bytes size, BufferMemView &memView)						__Th___;
		static void  UploadImage (const ImageDesc &img)																			__Th___;
		static void  UploadImage (const VideoImageDesc &img)																	__Th___;

		static void  ReadbackBuffer (const BufferDesc &buf, Bytes offset, Bytes size)											__Th___;
		static void  ReadbackImage (const ImageDesc &img)																		__Th___;
		static void  ReadbackImage (const VideoImageDesc &img)																	__Th___;

		static void  MapHostBuffer (const BufferDesc &buf, Bytes offset, Bytes size)											__Th___;

		static void  CopyBuffer (const BufferDesc &src_buf, const BufferDesc &dst_buf, ArrayView<BufferCopy>)					__Th___;
		static void  CopyImage (const ImageDesc &src_img, const ImageDesc &dst_img, ArrayView<ImageCopy>)						__Th___;
		static void  CopyBufferToImage (const BufferDesc &src_buf, const ImageDesc &dst_img, ArrayView<BufferImageCopy>)		__Th___;
		static void  CopyBufferToImage (const BufferDesc &src_buf, const ImageDesc &dst_img, ArrayView<BufferImageCopy2>)		__Th___;
		static void  CopyImageToBuffer (const ImageDesc &src_img, const BufferDesc &dst_buf, ArrayView<BufferImageCopy>)		__Th___;
		static void  CopyImageToBuffer (const ImageDesc &src_img, const BufferDesc &dst_buf, ArrayView<BufferImageCopy2>)		__Th___;

		static void  BlitImage (const ImageDesc &src_img, const ImageDesc &dst_img, EBlitFilter, ArrayView<ImageBlit>)			__Th___;
		static void  ResolveImage (const ImageDesc &src_img, const ImageDesc &dst_img, ArrayView<ImageResolve> ranges)			__Th___;

		static void  GenerateMipmaps (const ImageDesc &img, ArrayView<ImageSubresourceRange> ranges)							__Th___;


	#ifdef AE_ENABLE_VULKAN
		static void  ClearColorImage (VkImage image, ArrayView<VkImageSubresourceRange> ranges)									__Th___;
		static void  ClearDepthStencilImage (VkImage image, ArrayView<VkImageSubresourceRange> ranges)							__Th___;
		static void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size)														__Th___;

		static void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)									__Th___;

		static void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)						__Th___;
		static void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)								__Th___;
		static void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)				__Th___;
		static void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)				__Th___;
		static void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)			__Th___;
		static void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)						__Th___;
		static void  GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges)			__Th___;
	#endif

	#ifdef AE_ENABLE_METAL
	#endif
	};



	//
	// Compute Context Validation
	//
	class ComputeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue)																			__Th___;

		static void  PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName)					__Th___;

		static void  DispatchIndirect (const BufferDesc &indirectBufferDesc, Bytes offset)										__Th___;

	#ifdef AE_ENABLE_VULKAN
		static void  Dispatch (VkPipelineLayout, const uint3 &groupCount)														__Th___;
		static void  DispatchBase (VkPipelineLayout, const uint3 &baseGroup, const uint3 &groupCount)							__Th___;
		static void  DispatchIndirect (VkPipelineLayout, VkBuffer buffer)														__Th___;

		static void  BindDescriptorSet (VkPipelineLayout, DescSetBinding index, VkDescriptorSet ds)								__Th___;
		static void  PushConstant (VkPipelineLayout, Bytes offset, Bytes size, const void *, EShaderStages)						__Th___;
	#endif

	#ifdef AE_ENABLE_METAL
	#endif
	};



	//
	// Draw Context Validation
	//
	class DrawContextValidation final : Noninstanceable
	{
	public:
		//static void  CtxInit (EQueueMask rtaskQueue) __Th___;

		static void  PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName)					__Th___;

		static void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)				__Th___;

		static void  ClearAttachment (const RectI &rect)																		__Th___;

		static void  DrawIndirect (const BufferDesc &	indirectBufferDesc,
								   Bytes				indirectBufferOffset,
								   uint					drawCount,
								   Bytes				stride)																	__Th___;
		static void  DrawIndirect (uint drawCount, Bytes stride)																__Th___;

		static void  DrawIndexedIndirect (const BufferDesc &	indirectBufferDesc,
										  Bytes					indirectBufferOffset,
										  uint					drawCount,
										  Bytes					stride)															__Th___;
		static void  DrawIndexedIndirect (uint drawCount, Bytes stride)															__Th___;

		static void  DrawIndirectCount (const BufferDesc &	indirectBufferDesc,
										Bytes				indirectBufferOffset,
										const BufferDesc &	countBufferDesc,
										Bytes				countBufferOffset,
										uint				maxDrawCount,
										Bytes				stride)																__Th___;
		static void  DrawIndirectCount (uint maxDrawCount, Bytes stride)														__Th___;

		static void  DrawIndexedIndirectCount (const BufferDesc &	indirectBufferDesc,
											   Bytes				indirectBufferOffset,
											   const BufferDesc &	countBufferDesc,
											   Bytes				countBufferOffset,
											   uint					maxDrawCount,
											   Bytes				stride)														__Th___;
		static void  DrawIndexedIndirectCount (uint maxDrawCount, Bytes stride)													__Th___;

		static void  DrawMeshTasks (const uint3 &taskCount)																		__Th___;

		static void  DrawMeshTasksIndirect (const BufferDesc &	indirectBufferDesc,
											Bytes				indirectBufferOffset,
											uint				drawCount,
											Bytes				stride)															__Th___;
		static void  DrawMeshTasksIndirect (uint drawCount, Bytes stride)														__Th___;

		static void  DrawMeshTasksIndirectCount (const BufferDesc &	indirectBufferDesc,
												 Bytes				indirectBufferOffset,
												 const BufferDesc &	countBufferDesc,
												 Bytes				countBufferOffset,
												 uint				maxDrawCount,
												 Bytes				stride)														__Th___;
		static void  DrawMeshTasksIndirectCount (uint maxDrawCount, Bytes stride)												__Th___;

		static void  SetDepthBias (EPipelineDynamicState dynState, float depthBiasClamp)										__Th___;
		static void  SetDepthBounds (EPipelineDynamicState dynState)															__Th___;
		static void  SetStencilCompareMask (EPipelineDynamicState dynState)														__Th___;
		static void  SetStencilWriteMask (EPipelineDynamicState dynState)														__Th___;
		static void  SetStencilReference (EPipelineDynamicState dynState)														__Th___;
		static void  SetBlendConstants (EPipelineDynamicState dynState)															__Th___;
		static void  SetFragmentShadingRate (EPipelineDynamicState, EShadingRate, EShadingRateCombinerOp, EShadingRateCombinerOp) __Th___;


	#ifdef AE_ENABLE_VULKAN
		static void  SetViewport (uint first, ArrayView<VkViewport> viewports)													__Th___;
		static void  SetScissor (uint first, ArrayView<VkRect2D> scissors)														__Th___;

		static void  BindDescriptorSet (VkPipelineLayout, DescSetBinding index, VkDescriptorSet ds)								__Th___;
		static void  PushConstant (VkPipelineLayout, Bytes offset, Bytes size, const void *, EShaderStages)						__Th___;

		static void  BindIndexBuffer (VkBuffer buffer, EIndex indexType)														__Th___;
		static void  BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets)				__Th___;

		static void  Draw (VkPipelineLayout	layout)																				__Th___;
		static void  DrawIndexed (VkPipelineLayout	layout)																		__Th___;

		static void  DrawIndirect (VkPipelineLayout	layout,
								   VkBuffer			indirectBuffer,
								   uint				drawCount,
								   Bytes			stride)																		__Th___;

		static void  DrawIndexedIndirect (VkPipelineLayout	layout,
										  VkBuffer			indirectBuffer,
										  uint				drawCount,
										  Bytes				stride)																__Th___;

		static void  DrawIndirectCount (VkPipelineLayout	layout,
										VkBuffer			indirectBuffer,
										VkBuffer			countBuffer,
										uint				maxDrawCount,
										Bytes				stride)																__Th___;
		static void  DrawIndexedIndirectCount (VkPipelineLayout	layout,
											   VkBuffer			indirectBuffer,
											   VkBuffer			countBuffer,
											   uint				maxDrawCount,
											   Bytes			stride)															__Th___;

		static void  DrawMeshTasks (VkPipelineLayout layout, const uint3 &taskCount)											__Th___;
		static void  DrawMeshTasksIndirect (VkPipelineLayout layout,
											VkBuffer		 indirectBuffer,
											uint			 drawCount,
											Bytes			 stride)															__Th___;
		static void  DrawMeshTasksIndirectCount (VkPipelineLayout layout,
												 VkBuffer	indirectBuffer,
												 VkBuffer	countBuffer,
												 uint		maxDrawCount,
												 Bytes		stride)																__Th___;

		static void  DispatchTile (VkPipelineLayout layout)																		__Th___;
	#endif

	#ifdef AE_ENABLE_METAL
	#endif
	};



	//
	// Graphics Context Validation
	//
	class GraphicsContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};



	//
	// Acceleration Structure Build Context Validation
	//
	class ASBuildContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue)																			__Th___;

		static void  Copy (const RTGeometryDesc &srcGeometryDesc, const RTGeometryDesc &dstGeometryDesc, ERTASCopyMode mode)	__Th___;
		static void  Copy (const RTSceneDesc &srcSceneDesc, const RTSceneDesc &dstSceneDesc, ERTASCopyMode mode)				__Th___;

		static void  Build (const RTGeometryDesc &geometryDesc,
							const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset)										__Th___;
		static void  Build (const RTSceneDesc &sceneDesc,
							const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
							const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset)										__Th___;

		static void  Update (const RTGeometryDesc &srcGeometryDesc, const RTGeometryDesc &dstGeometryDesc,
							 const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset)										__Th___;
		static void  Update (const RTSceneDesc &srcSceneDesc, const RTSceneDesc &dstSceneDesc,
							 const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
							 const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset)										__Th___;

		static void  ReadProperty (ERTASProperty property)																		__Th___;
		static void  WriteProperty (ERTASProperty property, const BufferDesc &dstBufferDesc, Bytes dstOffset, Bytes size)		__Th___;

		static void  SerializeToMemory (const BufferDesc &dstBufferDesc, Bytes dstOffset)										__Th___;
		static void  SerializeToMemory (DeviceAddress dst)																		__Th___;

		static void  DeserializeFromMemory (const BufferDesc &srcBufferDesc, Bytes srcOffset)									__Th___;
		static void  DeserializeFromMemory (DeviceAddress src)																	__Th___;

		static void  BuildIndirect (const RTGeometryBuild &cmd,
									const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset,
									Bytes indirectStride)																		__Th___;
		static void  BuildIndirect (const RTSceneBuild &cmd,
									const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset)							__Th___;

		static void  BuildIndirect (const RTGeometryDesc &geometryDesc,
									const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
									Bytes indirectStride)																		__Th___;
		static void  BuildIndirect (const RTSceneDesc &sceneDesc,
									const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
									const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset)								__Th___;

	#ifdef AE_ENABLE_VULKAN
		static void  BuildIndirect (const RTGeometryDesc &geometryDesc,
									const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
									VkDeviceAddress indirectMem, Bytes indirectStride)											__Th___;
		static void  BuildIndirect (const RTSceneDesc &sceneDesc,
									const BufferDesc &scratchBufDesc, Bytes scratchBufferOffset,
									const BufferDesc &instanceBufDesc, Bytes instanceBufferOffset,
									VkDeviceAddress indirectMem)																__Th___;
	#endif
	};



	//
	// Ray Tracing Context Validation
	//
	class RayTracingContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue)																			__Th___;

		static void  PushConstant (const PushConstantIndex &idx, Bytes size, ShaderStructName::Ref typeName)					__Th___;

		static void  TraceRays (const uint3 &dim)																				__Th___;
		static void  TraceRaysIndirect (const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset)						__Th___;
		static void  TraceRaysIndirect2 (const BufferDesc &indirectBufferDesc, Bytes indirectBufferOffset)						__Th___;


	#ifdef AE_ENABLE_VULKAN
		static void  BindDescriptorSet (VkPipelineLayout, DescSetBinding index, VkDescriptorSet ds)								__Th___;
		static void  PushConstant (VkPipelineLayout, Bytes offset, Bytes size, const void *, EShaderStages)						__Th___;

		static void  TraceRays (VkPipelineLayout layout, const uint3 &dim)														__Th___;
		static void  TraceRaysIndirect (VkPipelineLayout, VkDeviceAddress indirectDeviceAddress)								__Th___;
		static void  TraceRaysIndirect2 (VkPipelineLayout, VkDeviceAddress indirectDeviceAddress)								__Th___;
	#endif

	#ifdef AE_ENABLE_METAL
	#endif
	};



	//
	// Video Decode Context Validation
	//
	class VideoDecodeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};



	//
	// Video Encode Context Validation
	//
	class VideoEncodeContextValidation final : Noninstanceable
	{
	public:
		static void  CtxInit (EQueueMask rtaskQueue) __Th___;
	};


#endif // AE_VALIDATE_GCTX

} // AE::Graphics::_hidden_

#ifdef AE_COMPILER_MSVC
# pragma warning (pop)
#endif
