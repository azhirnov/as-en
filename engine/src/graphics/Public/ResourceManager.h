// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	IGfxMemAllocator
		thread-safe:	yes

	IDescriptorAllocator
		thread-safe:	yes
		
	IRenderTechPipelines
		thread-safe:	yes

	IResourceManager
		thread-safe:	yes
*/

#pragma once

#include "base/Containers/UntypedStorage.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/PipelineDesc.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/FrameUID.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/VulkanTypes.h"
#endif
#ifdef AE_ENABLE_METAL
# include "graphics/Public/MetalTypes.h"
#endif

namespace AE::Graphics
{

	//
	// Graphics Memory Allocator interface
	//

	class IGfxMemAllocator : public EnableRC<IGfxMemAllocator>
	{
	// types
	public:
		using Storage_t = UntypedStorage< sizeof(ulong) * 4, alignof(ulong) >;

	  #if defined(AE_ENABLE_VULKAN)
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;

	  #elif defined(AE_ENABLE_METAL)
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;
	  #endif


	// interface
	public:
	  #if defined(AE_ENABLE_VULKAN)
		// 'desc.memType' and 'desc.usage' are required
		ND_ virtual bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) = 0;
		
		// 'desc.memType' and 'desc.usage' are required
		ND_ virtual bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) = 0;
		
	  #elif defined(AE_ENABLE_METAL)
		ND_ virtual MetalImageRC	AllocForImage (const ImageDesc &desc, OUT Storage_t &data) = 0;
		
		ND_ virtual MetalBufferRC	AllocForBuffer (const BufferDesc &desc, OUT Storage_t &data) = 0;
	  #endif

		// returns 'true' if deallocated
			virtual bool  Dealloc (INOUT Storage_t &data) = 0;

		ND_ virtual bool  GetInfo (const Storage_t &data, OUT NativeMemObjInfo_t &info) const = 0;

		// Memory granularity for each allocation.
		// Small align for linear/pool/etc allocator, large align for block allocator.
		ND_ virtual Bytes  MinAlignment ()		const = 0;
		ND_ virtual Bytes  MaxAllocationSize ()	const = 0;
	};
	


	//
	// Descriptor Allocator
	//

	class IDescriptorAllocator : public EnableRC<IDescriptorAllocator>
	{
	// types
	public:
		using StorageData_t = UntypedStorage< sizeof(ulong), alignof(ulong) >;

		#if defined(AE_ENABLE_VULKAN)
		struct Storage
		{
			VkDescriptorSet		handle	= Default;
			StorageData_t		data;	// allocator specific data
		};

		#elif defined(AE_ENABLE_METAL)
		struct Storage
		{
			MetalBufferRC		handle;	// argument buffer
			StorageData_t		data;	// allocator specific data
		};
		#endif


	// interface
	public:
		ND_ virtual bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds) = 0;
			virtual void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds) = 0;
	};



	//
	// Render Technique Pipelines interface
	//

	class IRenderTechPipelines : public EnableRC<IRenderTechPipelines>
	{
	// types
	public:
		enum class EPassType : ubyte
		{
			Unknown		= 0,
			Graphics,
			Compute,
			Transfer,
		};

		struct PassInfo
		{
			//RenderTechPassName::Optimized_t	name;
			ushort								index		= 0;
			EPassType							type		= Default;
			DescriptorSetLayoutID				dsLayoutId;

			// graphics
			RenderPassName::Optimized_t			renderPass;
			SubpassName::Optimized_t			subpass;
		};


	// interface
	public:
		ND_ virtual GraphicsPipelineID		GetGraphicsPipeline (const PipelineName &name)	const = 0;
		ND_ virtual MeshPipelineID			GetMeshPipeline (const PipelineName &name)		const = 0;
		ND_ virtual TilePipelineID			GetTilePipeline (const PipelineName &name)		const = 0;
		ND_ virtual ComputePipelineID		GetComputePipeline (const PipelineName &name)	const = 0;
		ND_ virtual RayTracingPipelineID	GetRayTracingPipeline (const PipelineName &name)const = 0;
		ND_ virtual PassInfo				GetPass (const RenderTechPassName &pass)		const = 0;
	};
	using RenderTechPipelinesPtr = RC< IRenderTechPipelines >;



	//
	// Resource Manager interface
	//

	class IResourceManager
	{
	// types
	public:
		#if defined(AE_ENABLE_VULKAN)
		using NativeBuffer_t		= VkBuffer;
		using NativeImage_t			= VkImage;
		using NativeBufferView_t	= VkBufferView;
		using NativeImageView_t		= VkImageView;
		using NativeImageDesc_t		= VulkanImageDesc;
		using NativeBufferDesc_t	= VulkanBufferDesc;
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;
		
		#elif defined(AE_ENABLE_METAL)
		using NativeBuffer_t		= MetalBuffer;
		using NativeImage_t			= MetalImage;
		using NativeBufferView_t	= MetalImage;
		using NativeImageView_t		= MetalImage;
		using NativeImageDesc_t		= MetalImageDesc;
		using NativeBufferDesc_t	= MetalBufferDesc;
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;
		#endif

		using DescSetAndBinding_t = Tuple< Strong<DescriptorSetID>, uint >;
		
		struct StagingBufferStat
		{
			Bytes	dynamicWrite;	// host to device
			Bytes	dynamicRead;	// device to host
			
			Bytes	staticWrite;
			Bytes	staticRead;
		};


	// interface
	public:
		virtual ~IResourceManager () {}
		
		ND_ virtual bool						IsSupported (EMemoryType memType) const = 0;
		ND_ virtual bool						IsSupported (const BufferDesc &desc) const = 0;
		ND_ virtual bool						IsSupported (const ImageDesc &desc) const = 0;
		ND_ virtual bool						IsSupported (BufferID buffer, const BufferViewDesc &desc) const = 0;
		ND_ virtual bool						IsSupported (ImageID image, const ImageViewDesc &desc) const = 0;

		ND_ virtual Strong<ImageID>				CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) = 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) = 0;
		
		ND_ virtual Strong<ImageID>				CreateImage (const NativeImageDesc_t &desc, StringView dbgName) = 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName) = 0;

		ND_ virtual Strong<ImageViewID>			CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default) = 0;
		ND_ virtual Strong<BufferViewID>		CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default) = 0;
		
		ND_ virtual Strong<RTGeometryID>		CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) = 0;
		ND_ virtual Strong<RTSceneID>			CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) = 0;

		ND_ virtual RTASBuildSizes				GetRTGeometrySizes (const RTGeometryBuild &desc) = 0;
		ND_ virtual RTASBuildSizes				GetRTSceneSizes (const RTSceneBuild &desc) = 0;

		ND_ virtual DescSetAndBinding_t			CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		ND_ virtual DescSetAndBinding_t			CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		ND_ virtual DescSetAndBinding_t			CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		ND_ virtual DescSetAndBinding_t			CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		ND_ virtual DescSetAndBinding_t			CreateDescriptorSet (TilePipelineID		  ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		
		ND_ virtual Strong<DescriptorSetID>		CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;
		ND_ virtual Strong<DescriptorSetID>		CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) = 0;

		// warning: pipeline compilation and shader loading may be slow
		ND_ virtual Strong<GraphicsPipelineID>	CreateGraphicsPipeline	(PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc	&desc, PipelineCacheID cache = Default) = 0;
		ND_ virtual Strong<MeshPipelineID>		CreateMeshPipeline		(PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc		&desc, PipelineCacheID cache = Default) = 0;
		ND_ virtual Strong<ComputePipelineID>	CreateComputePipeline	(PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc		&desc, PipelineCacheID cache = Default) = 0;
		ND_ virtual Strong<RayTracingPipelineID>CreateRayTracingPipeline(PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc	&desc, PipelineCacheID cache = Default) = 0;
		ND_ virtual Strong<TilePipelineID>		CreateTilePipeline		(PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc		&desc, PipelineCacheID cache = Default) = 0;

		ND_ virtual Strong<PipelineCacheID>		CreatePipelineCache () = 0;
		ND_ virtual Strong<PipelineCacheID>		LoadPipelineCache (RC<RStream> stream) = 0;
		
		// load default pack
			virtual bool						InitializeResources (const PipelinePackDesc &desc) = 0;
		ND_ virtual Strong<PipelinePackID>		LoadPipelinePack (const PipelinePackDesc &desc) = 0;
		ND_ virtual Array<RenderTechName>		GetSupportedRenderTechs (PipelinePackID id) const = 0;

		ND_ virtual bool						IsResourceAlive (ImageID			id) const = 0;
		ND_ virtual bool						IsResourceAlive (BufferID			id) const = 0;
		ND_ virtual bool						IsResourceAlive (ImageViewID		id) const = 0;
		ND_ virtual bool						IsResourceAlive (BufferViewID		id) const = 0;
		ND_ virtual bool						IsResourceAlive (DescriptorSetID	id) const = 0;
		ND_ virtual bool						IsResourceAlive (PipelineCacheID	id) const = 0;
		ND_ virtual bool						IsResourceAlive (PipelinePackID		id) const = 0;
		ND_ virtual bool						IsResourceAlive (RTGeometryID		id) const = 0;
		ND_ virtual bool						IsResourceAlive (RTSceneID			id) const = 0;

		// returns 'true' if resource has been destroyed (when ref counter is zero).
			virtual bool						ReleaseResource (INOUT Strong<ImageID>				&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<BufferID>				&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<ImageViewID>			&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<BufferViewID>			&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<PipelineCacheID>		&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<PipelinePackID>		&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<GraphicsPipelineID>	&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<MeshPipelineID>		&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<ComputePipelineID>	&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<RayTracingPipelineID>	&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<TilePipelineID>		&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<DescriptorSetID>		&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<RTGeometryID>			&id) = 0;
			virtual bool						ReleaseResource (INOUT Strong<RTSceneID>			&id) = 0;

		// returned reference is valid until resource is alive
		ND_ virtual BufferDesc const&			GetDescription (BufferID id) const = 0;
		ND_ virtual ImageDesc const&			GetDescription (ImageID id) const = 0;
		ND_ virtual BufferViewDesc const&		GetDescription (BufferViewID id) const = 0;
		ND_ virtual ImageViewDesc const&		GetDescription (ImageViewID id) const = 0;

		// By default all resources destroyed when all pending command buffers are complete execution.
		// Returns 'true' if wasn't empty.
			virtual bool						ForceReleaseResources () = 0;
		
		// native
		ND_ virtual NativeBuffer_t				GetBufferHandle (BufferID id) const = 0;
		ND_ virtual NativeImage_t				GetImageHandle (ImageID id) const = 0;
		ND_ virtual NativeBufferView_t			GetBufferViewHandle (BufferViewID id) const = 0;
		ND_ virtual NativeImageView_t			GetImageViewHandle (ImageViewID id) const = 0;

			virtual bool						GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info) const = 0;
			virtual bool						GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info) const = 0;

		// async loading
		//	Pipeline compilation may be distributed to multiple threads.
		ND_ virtual Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache = Default) = 0;
		ND_ virtual RenderTechPipelinesPtr			LoadRenderTech (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache = Default) = 0;


		// statistics
		ND_ virtual StagingBufferStat			GetStagingBufferFrameStat (FrameUID frameId) const = 0;
	};


} // AE::Graphics
