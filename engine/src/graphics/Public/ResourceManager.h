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
#include "graphics/Public/GraphicsCreateInfo.h"

#if defined(AE_ENABLE_VULKAN)
# include "graphics/Public/VulkanTypes.h"

#elif defined(AE_ENABLE_METAL)
# include "graphics/Public/MetalTypes.h"

#else
#	error not implemented
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
		
	  #else
	  #	error not implemented
	  #endif


	// interface
	public:
	  #if defined(AE_ENABLE_VULKAN)
		// 'desc.memType' and 'desc.usage' are required
		ND_ virtual bool  AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data)				__NE___	= 0;
		
		// 'desc.memType' and 'desc.usage' are required
		ND_ virtual bool  AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data)			__NE___	= 0;
		
	  #elif defined(AE_ENABLE_METAL)
		ND_ virtual MetalImageRC	AllocForImage (const ImageDesc &desc, OUT Storage_t &data)					__NE___	= 0;
		ND_ virtual MetalBufferRC	AllocForBuffer (const BufferDesc &desc, OUT Storage_t &data)				__NE___	= 0;

		ND_ virtual MetalAccelStructRC  AllocForAccelStruct (const RTGeometryDesc &desc, OUT Storage_t &data)	__NE___	= 0;
		ND_ virtual MetalAccelStructRC  AllocForAccelStruct (const RTSceneDesc &desc, OUT Storage_t &data)		__NE___	= 0;
		
	  #else
	  #	error not implemented
	  #endif

		// returns 'true' if deallocated
			virtual bool  Dealloc (INOUT Storage_t &data)														__NE___	= 0;

		ND_ virtual bool  GetInfo (const Storage_t &data, OUT NativeMemObjInfo_t &info)							C_NE___	= 0;

		// Memory granularity for each allocation.
		// Small align for linear/pool/etc allocator, large align for block allocator.
		ND_ virtual Bytes  MinAlignment ()																		C_NE___	= 0;
		ND_ virtual Bytes  MaxAllocationSize ()																	C_NE___	= 0;
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
			union {
				MetalBuffer		handle;			// argument buffer
				void *			args	= null;	// array of buffers, textures, samplers
			};
			StorageData_t		data;			// allocator specific data
			
			Storage () {}
		};
		
		#else
		#	error not implemented
		#endif


	// interface
	public:
		ND_ virtual bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds)		__NE___	= 0;
			virtual void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds)	__NE___	= 0;
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
			DescSetBinding						dsIndex;
			EPassType							type		= Default;
			ubyte								submitIdx	= UMax;
			DescriptorSetLayoutID				dsLayoutId;

			// graphics
			RenderPassName::Optimized_t			renderPass;
			SubpassName::Optimized_t			subpass;
			PipelinePackID						packId;
		};


	// interface
	public:
		ND_ virtual GraphicsPipelineID		GetGraphicsPipeline (const PipelineName &name)		C_NE___ = 0;
		ND_ virtual MeshPipelineID			GetMeshPipeline (const PipelineName &name)			C_NE___ = 0;
		ND_ virtual TilePipelineID			GetTilePipeline (const PipelineName &name)			C_NE___ = 0;
		ND_ virtual ComputePipelineID		GetComputePipeline (const PipelineName &name)		C_NE___ = 0;
		ND_ virtual RayTracingPipelineID	GetRayTracingPipeline (const PipelineName &name)	C_NE___ = 0;
		ND_ virtual RTShaderBindingID		GetRTShaderBinding (const RTShaderBindingName &name)C_NE___ = 0;
		ND_ virtual PassInfo				GetPass (const RenderTechPassName &pass)			C_NE___ = 0;
		ND_ virtual bool					FeatureSetSupported (const FeatureSetName &name)	C_NE___ = 0;
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
		
		#else
		#	error not implemented
		#endif

		using DescSetAndBinding_t	= Tuple< Strong<DescriptorSetID>, DescSetBinding >;
		using AsyncRTechPipelines	= Promise< RenderTechPipelinesPtr >;
		
		struct StagingBufferStat
		{
			Bytes	dynamicWrite;	// host to device
			Bytes	dynamicRead;	// device to host
			
			Bytes	staticWrite;
			Bytes	staticRead;
		};


	// interface
	public:
		virtual ~IResourceManager ()																															__NE___ {}
		
		ND_ virtual bool						IsSupported (EMemoryType memType)																				C_NE___ = 0;
		ND_ virtual bool						IsSupported (const BufferDesc &desc)																			C_NE___ = 0;
		ND_ virtual bool						IsSupported (const ImageDesc &desc)																				C_NE___ = 0;
		ND_ virtual bool						IsSupported (BufferID buffer, const BufferViewDesc &desc)														C_NE___ = 0;
		ND_ virtual bool						IsSupported (ImageID image, const ImageViewDesc &desc)															C_NE___ = 0;

		ND_ virtual Strong<ImageID>				CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE___ = 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE___ = 0;
		
		ND_ virtual Strong<ImageID>				CreateImage (const NativeImageDesc_t &desc, StringView dbgName)													__NE___	= 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName)												__NE___	= 0;

		ND_ virtual Strong<ImageViewID>			CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)						__NE___	= 0;
		ND_ virtual Strong<BufferViewID>		CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)					__NE___	= 0;
		
		ND_ virtual Strong<RTGeometryID>		CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE___	= 0;
		ND_ virtual Strong<RTSceneID>			CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE___	= 0;

		ND_ virtual RTASBuildSizes				GetRTGeometrySizes (const RTGeometryBuild &desc)																__NE___	= 0;
		ND_ virtual RTASBuildSizes				GetRTSceneSizes (const RTSceneBuild &desc)																		__NE___	= 0;

		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, TilePipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)										 __NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)															 __NE___ = 0;

		ND_ DescSetAndBinding_t					CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___;
		ND_ DescSetAndBinding_t					CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___;
		ND_ DescSetAndBinding_t					CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___;
		ND_ DescSetAndBinding_t					CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___;
		ND_ DescSetAndBinding_t					CreateDescriptorSet (TilePipelineID		  ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE___;
		ND_ Strong<DescriptorSetID>				CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)			__NE___;
		ND_ Strong<DescriptorSetID>				CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)								__NE___;

		// warning: pipeline compilation and shader loading may be slow
		ND_ virtual Strong<GraphicsPipelineID>	CreateGraphicsPipeline	(PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc	&desc, PipelineCacheID cache = Default)		__NE___	= 0;
		ND_ virtual Strong<MeshPipelineID>		CreateMeshPipeline		(PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc		&desc, PipelineCacheID cache = Default)		__NE___	= 0;
		ND_ virtual Strong<ComputePipelineID>	CreateComputePipeline	(PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc		&desc, PipelineCacheID cache = Default)		__NE___	= 0;
		ND_ virtual Strong<RayTracingPipelineID>CreateRayTracingPipeline(PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc	&desc, PipelineCacheID cache = Default)		__NE___	= 0;
		ND_ virtual Strong<TilePipelineID>		CreateTilePipeline		(PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc		&desc, PipelineCacheID cache = Default)		__NE___	= 0;

		ND_ virtual Strong<PipelineCacheID>		CreatePipelineCache ()										__NE___ = 0;
		
		// load default pack
			virtual bool						InitializeResources (const PipelinePackDesc &desc)			__NE___	= 0;
		ND_ virtual Strong<PipelinePackID>		LoadPipelinePack (const PipelinePackDesc &desc)				__NE___	= 0;
		ND_ virtual Array<RenderTechName>		GetSupportedRenderTechs (PipelinePackID id)					C_NE___	= 0;

		// Returns 'true' if resource is alive.
		ND_ virtual bool						IsResourceAlive (ImageID			id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (BufferID			id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (ImageViewID		id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (BufferViewID		id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (DescriptorSetID	id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (PipelineCacheID	id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (PipelinePackID		id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (RTGeometryID		id)						C_NE___ = 0;
		ND_ virtual bool						IsResourceAlive (RTSceneID			id)						C_NE___ = 0;

		// Decrease ref counter and delay destruction until current frame complete execution on GPU.
		// Returns 'true' if resource has been destroyed (when ref counter is zero).
			virtual bool						ReleaseResource (INOUT Strong<ImageID>				&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<BufferID>				&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<ImageViewID>			&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<BufferViewID>			&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<PipelineCacheID>		&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<PipelinePackID>		&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<GraphicsPipelineID>	&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<MeshPipelineID>		&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<ComputePipelineID>	&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<RayTracingPipelineID>	&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<TilePipelineID>		&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<DescriptorSetID>		&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<RTGeometryID>			&id)	__NE___	= 0;
			virtual bool						ReleaseResource (INOUT Strong<RTSceneID>			&id)	__NE___	= 0;

		// Returned reference is valid until resource is alive
		ND_ virtual BufferDesc const&			GetDescription (BufferID id)								C_NE___ = 0;
		ND_ virtual ImageDesc const&			GetDescription (ImageID id)									C_NE___ = 0;
		ND_ virtual BufferViewDesc const&		GetDescription (BufferViewID id)							C_NE___ = 0;
		ND_ virtual ImageViewDesc const&		GetDescription (ImageViewID id)								C_NE___ = 0;
		ND_ virtual RTShaderBindingDesc const&	GetDescription (RTShaderBindingID id)						C_NE___	= 0;

		// By default all resources destroyed when all pending command buffers are complete execution.
		// Returns 'true' if wasn't empty.
			virtual bool						ForceReleaseResources ()									__NE___	= 0;
		
		// Returns native handle.
		ND_ virtual NativeBuffer_t				GetBufferHandle (BufferID id)								C_NE___ = 0;
		ND_ virtual NativeImage_t				GetImageHandle (ImageID id)									C_NE___ = 0;
		ND_ virtual NativeBufferView_t			GetBufferViewHandle (BufferViewID id)						C_NE___ = 0;
		ND_ virtual NativeImageView_t			GetImageViewHandle (ImageViewID id)							C_NE___ = 0;

			virtual bool						GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)	C_NE___ = 0;
			virtual bool						GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)	C_NE___ = 0;

		// async loading
		//	Pipeline compilation may be distributed to multiple threads.
		ND_ virtual AsyncRTechPipelines			LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache = Default)__NE___	= 0;
		ND_ virtual RenderTechPipelinesPtr		LoadRenderTech (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache = Default)		__NE___	= 0;

		// statistics
		ND_ virtual StagingBufferStat			GetStagingBufferFrameStat (FrameUID frameId)				C_NE___ = 0;
	};

	
/*
=================================================
	CreateDescriptorSet
=================================================
*/
	inline IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (GraphicsPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (MeshPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (ComputePipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (TilePipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline Strong<DescriptorSetID>  IResourceManager::CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		Strong<DescriptorSetID>	result;
		return CreateDescriptorSets( OUT &result, 1, packId, dslName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline Strong<DescriptorSetID>  IResourceManager::CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		Strong<DescriptorSetID>	result;
		return CreateDescriptorSets( OUT &result, 1, layoutId, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}


} // AE::Graphics
