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

	if AE_GRAPHICS_STRONG_VALIDATION enabled then 'IResourceManager::Create***()' may return error if resource description is not valid or not supported.

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/GraphicsResources.md)
*/

#pragma once

#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/PipelineDesc.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/GraphicsCreateInfo.h"
#include "graphics/Public/Video.h"
#include "graphics/Public/FeatureSet.h"

#if defined(AE_ENABLE_VULKAN)
# include "graphics/Public/VulkanTypes.h"

#elif defined(AE_ENABLE_METAL)
# include "graphics/Public/MetalTypes.h"

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
# include "graphics/Public/RemoteGraphicsTypes.h"

#else
#	error not implemented
#endif

#define NON_INTERFACE( ... )

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
		using VideoStorageArr_t		= FixedArray< Storage_t, 8 >;

	  #elif defined(AE_ENABLE_METAL)
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using NativeMemObjInfo_t	= RemoteMemoryObjInfo;

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

		ND_ virtual bool  AllocForVideoSession (VkVideoSessionKHR, EMemoryType, OUT VideoStorageArr_t &)		__NE___	{ return false; }
		ND_ virtual bool  AllocForVideoImage (VkImage, const VideoImageDesc &, OUT VideoStorageArr_t &)			__NE___	{ return false; }

		// returns 'true' if deallocated
			virtual bool  Dealloc (INOUT Storage_t &data)														__NE___	= 0;


	  #elif defined(AE_ENABLE_METAL)
		ND_ virtual MetalImageRC	AllocForImage (const ImageDesc &desc, OUT Storage_t &data)					__NE___	= 0;
		ND_ virtual MetalBufferRC	AllocForBuffer (const BufferDesc &desc, OUT Storage_t &data)				__NE___	= 0;

		ND_ virtual MetalAccelStructRC  AllocForAccelStruct (const RTGeometryDesc &desc, OUT Storage_t &data)	__NE___	= 0;
		ND_ virtual MetalAccelStructRC  AllocForAccelStruct (const RTSceneDesc &desc, OUT Storage_t &data)		__NE___	= 0;

		// returns 'true' if deallocated
			virtual bool  Dealloc (INOUT Storage_t &data)														__NE___	= 0;


	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		// not implemented

	  #else
	  #	error not implemented
	  #endif


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
			StorageData_t		data;			// allocator specific data
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

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		struct Storage
		{};

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

			ND_ bool  IsDefined ()	C_NE___	{ return type != Default; }
		};


	// interface
	public:
		ND_ virtual RenderTechName::Optimized_t	Name ()																C_NE___ = 0;

		ND_ virtual GraphicsPipelineID		GetGraphicsPipeline (PipelineName::Ref name)							C_NE___ = 0;
		ND_ virtual MeshPipelineID			GetMeshPipeline (PipelineName::Ref name)								C_NE___ = 0;
		ND_ virtual TilePipelineID			GetTilePipeline (PipelineName::Ref name)								C_NE___ = 0;
		ND_ virtual ComputePipelineID		GetComputePipeline (PipelineName::Ref name)								C_NE___ = 0;
		ND_ virtual RayTracingPipelineID	GetRayTracingPipeline (PipelineName::Ref name)							C_NE___ = 0;
		ND_ virtual RTShaderBindingID		GetRTShaderBinding (RTShaderBindingName::Ref name)						C_NE___ = 0;
		ND_ virtual PassInfo				GetPass (RenderTechPassName::Ref pass)									C_NE___ = 0;

		ND_ virtual EPixelFormat			GetAttachmentFormat (RenderTechPassName::Ref pass, AttachmentName::Ref)	C_NE___ = 0;
		ND_ virtual bool					FeatureSetSupported (FeatureSetName::Ref name)							C_NE___ = 0;	// for debugging
	};
	using RenderTechPipelinesPtr = RC< IRenderTechPipelines >;



	//
	// Resource Manager interface
	//

	class NO_VTABLE IResourceManager
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
		using NativeImageViewDesc_t	= VulkanImageViewDesc;
		using NativeBufferViewDesc_t= VulkanBufferViewDesc;
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;

		#elif defined(AE_ENABLE_METAL)
		using NativeBuffer_t		= MetalBuffer;
		using NativeImage_t			= MetalImage;
		using NativeBufferView_t	= MetalImage;
		using NativeImageView_t		= MetalImage;
		using NativeImageDesc_t		= MetalImageDesc;
		using NativeBufferDesc_t	= MetalBufferDesc;
		using NativeImageViewDesc_t	= MetalImageViewDesc;
		using NativeBufferViewDesc_t= MetalBufferViewDesc;
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using NativeBuffer_t		= RmBufferID;
		using NativeImage_t			= RmImageID;
		using NativeBufferView_t	= RmBufferViewID;
		using NativeImageView_t		= RmImageViewID;
		using NativeImageDesc_t		= RemoteImageDesc;
		using NativeBufferDesc_t	= RemoteBufferDesc;
		using NativeImageViewDesc_t	= RemoteImageViewDesc;
		using NativeBufferViewDesc_t= RemoteBufferViewDesc;
		using NativeMemObjInfo_t	= RemoteMemoryObjInfo;

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
		ND_ virtual bool						IsSupported (const VideoImageDesc &desc)																		C_NE___ = 0;
		ND_ virtual bool						IsSupported (const VideoBufferDesc &desc)																		C_NE___ = 0;
		ND_ virtual bool						IsSupported (const VideoSessionDesc &desc)																		C_NE___ = 0;
		ND_ virtual bool						IsSupported (BufferID buffer, const BufferViewDesc &desc)														C_NE___ = 0;
		ND_ virtual bool						IsSupported (ImageID image, const ImageViewDesc &desc)															C_NE___ = 0;
		ND_ virtual bool						IsSupported (const RTGeometryDesc &desc)																		C_NE___ = 0;
		ND_ virtual bool						IsSupported (const RTGeometryBuild &build)																		C_NE___ = 0;
		ND_ virtual bool						IsSupported (const RTSceneDesc &desc)																			C_NE___ = 0;
		ND_ virtual bool						IsSupported (const RTSceneBuild &build)																			C_NE___ = 0;

		ND_ virtual Strong<ImageID>				CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE___ = 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE___ = 0;

		ND_ virtual Strong<ImageID>				CreateImage (const NativeImageDesc_t &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)	__NE___	= 0;
		ND_ virtual Strong<BufferID>			CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE___	= 0;

		ND_ virtual Strong<ImageViewID>			CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)						__NE___	= 0;
		ND_ virtual Strong<BufferViewID>		CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)					__NE___	= 0;

		ND_ virtual Strong<ImageViewID>			CreateImageView (const NativeImageViewDesc_t &desc, ImageID image, StringView dbgName = Default)				__NE___ = 0;
		ND_ virtual Strong<BufferViewID>		CreateBufferView (const NativeBufferViewDesc_t &desc, BufferID buffer, StringView dbgName = Default)			__NE___ = 0;

		ND_ virtual Strong<RTGeometryID>		CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE___	= 0;
		ND_ virtual Strong<RTSceneID>			CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE___	= 0;

		ND_ virtual Bytes						GetShaderGroupStackSize (RayTracingPipelineID ppln, ArrayView<RayTracingGroupName> names, ERTShaderGroup type)	__NE___ = 0;
		ND_ virtual RTASBuildSizes				GetRTGeometrySizes (const RTGeometryBuild &desc)																__NE___	= 0;
		ND_ virtual RTASBuildSizes				GetRTSceneSizes (const RTSceneBuild &desc)																		__NE___	= 0;

		ND_ virtual DeviceAddress				GetDeviceAddress (BufferID		id)																				C_NE___ = 0;
		ND_ virtual DeviceAddress				GetDeviceAddress (RTGeometryID	id)																				C_NE___ = 0;

		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, GraphicsPipelineID   ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, MeshPipelineID       ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, ComputePipelineID    ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, TilePipelineID       ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, PipelinePackID packId, DSLayoutName::Ref dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)										__NE___ = 0;
		ND_ virtual bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)															__NE___ = 0;

		template <typename PplnID>
		ND_			DescSetAndBinding_t			CreateDescriptorSet (const PplnID &ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___;
		ND_			Strong<DescriptorSetID>		CreateDescriptorSet (PipelinePackID packId, DSLayoutName::Ref dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)	__NE___;
		ND_			Strong<DescriptorSetID>		CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)						__NE___;

		template <typename T, typename PplnID>
		ND_			PushConstantIndex			GetPushConstantIndex (PplnID               ppln, PushConstantName::Ref pcName)																	__NE___;
		ND_ virtual PushConstantIndex			GetPushConstantIndex (GraphicsPipelineID   ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)					__NE___ = 0;
		ND_ virtual PushConstantIndex			GetPushConstantIndex (MeshPipelineID       ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)					__NE___ = 0;
		ND_ virtual PushConstantIndex			GetPushConstantIndex (ComputePipelineID    ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)					__NE___ = 0;
		ND_ virtual PushConstantIndex			GetPushConstantIndex (RayTracingPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)					__NE___ = 0;
		ND_ virtual PushConstantIndex			GetPushConstantIndex (TilePipelineID       ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)					__NE___ = 0;

		// warning: pipeline compilation and shader loading may be slow
		ND_ virtual Strong<GraphicsPipelineID>	CreateGraphicsPipeline	(PipelinePackID packId, PipelineTmplName::Ref name, const GraphicsPipelineDesc	 &, PipelineCacheID cache = Default)	__NE___	= 0;
		ND_ virtual Strong<MeshPipelineID>		CreateMeshPipeline		(PipelinePackID packId, PipelineTmplName::Ref name, const MeshPipelineDesc		 &, PipelineCacheID cache = Default)	__NE___	= 0;
		ND_ virtual Strong<ComputePipelineID>	CreateComputePipeline	(PipelinePackID packId, PipelineTmplName::Ref name, const ComputePipelineDesc	 &, PipelineCacheID cache = Default)	__NE___	= 0;
		ND_ virtual Strong<RayTracingPipelineID>CreateRayTracingPipeline(PipelinePackID packId, PipelineTmplName::Ref name, const RayTracingPipelineDesc &, PipelineCacheID cache = Default)	__NE___	= 0;
		ND_ virtual Strong<TilePipelineID>		CreateTilePipeline		(PipelinePackID packId, PipelineTmplName::Ref name, const TilePipelineDesc		 &, PipelineCacheID cache = Default)	__NE___	= 0;

		// video
		ND_ virtual Strong<VideoSessionID>		CreateVideoSession (const VideoSessionDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)							__NE___	= 0;
		ND_ virtual Strong<VideoBufferID>		CreateVideoBuffer  (const VideoBufferDesc  &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)							__NE___	= 0;
		ND_ virtual Strong<VideoImageID>		CreateVideoImage   (const VideoImageDesc   &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)							__NE___	= 0;

		ND_ virtual Strong<PipelineCacheID>		CreatePipelineCache ()												__NE___ = 0;

		// load default pack
		ND_	virtual bool						InitializeResources (Strong<PipelinePackID> defaultPackId)			__NE___	= 0;
		ND_ virtual Strong<PipelinePackID>		LoadPipelinePack (const PipelinePackDesc &desc)						__NE___	= 0;
		ND_ virtual Array<RenderTechName>		GetSupportedRenderTechs (PipelinePackID id)							C_NE___	= 0;


		NON_INTERFACE(
		// Returns 'true' if resource is alive.
		template <typename ID>
		ND_			bool						IsAlive (ID id)														C_NE___;

		// Increment ref counter and returns ID if resource is alive, returns empty ID otherwise.
		template <typename ID>
		ND_			Strong<ID>					AcquireResource (ID id)												__NE___;

		// Decrease ref counter and delay destruction until current frame complete execution on GPU.
		// Returns 'true' if resource has been destroyed (when ref counter is zero).
		template <typename ID>
					bool						ReleaseResource (INOUT Strong<ID> &id)								__NE___;

		// Call 'ReleaseResource()' for each element of array.
		template <typename ArrayType>
					void						ReleaseResourceArray (INOUT ArrayType &arr)							__NE___;

		// Call 'ReleaseResource()' for each argument.
		template <typename ...Args>
					void						ReleaseResources (Args& ...args)									__NE___;

		// Decrease ref counter and immediately destroy resource if ref counter is zero.
		// Returns 'true' if resource has been destroyed (when ref counter is zero).
		template <typename ID>
		ND_			bool						ImmediatelyRelease2 (INOUT Strong<ID> &id)							__NE___;

		// Returns pointer to resource object or null.
		// Resource members are implementation dependent.
		template <typename ID>
		ND_ auto const*							GetResource (ID id, Bool incRef = False{}, Bool quiet = False{})	C_NE___;

		// Returns reference to resource objects or throw exception.
		template <typename ID0, typename ID1, typename ...IDs>
		ND_ auto								GetResourcesOrThrow (ID0 id0, ID1 id1, IDs ...ids)					C_Th___;
		)


		// Returned reference is valid until resource is alive
		ND_ virtual BufferDesc const&			GetDescription (BufferID id)										C_NE___ = 0;
		ND_ virtual ImageDesc const&			GetDescription (ImageID id)											C_NE___ = 0;
		ND_ virtual BufferViewDesc const&		GetDescription (BufferViewID id)									C_NE___ = 0;
		ND_ virtual ImageViewDesc const&		GetDescription (ImageViewID id)										C_NE___ = 0;
		ND_ virtual RTShaderBindingDesc const&	GetDescription (RTShaderBindingID id)								C_NE___	= 0;
		ND_ virtual RTGeometryDesc const&		GetDescription (RTGeometryID id)									C_NE___ = 0;
		ND_ virtual RTSceneDesc const&			GetDescription (RTSceneID id)										C_NE___ = 0;
		ND_ virtual VideoImageDesc const&		GetDescription (VideoImageID id)									C_NE___	= 0;
		ND_ virtual VideoBufferDesc const&		GetDescription (VideoBufferID id)									C_NE___	= 0;
		ND_ virtual VideoSessionDesc const&		GetDescription (VideoSessionID id)									C_NE___	= 0;

		// By default all resources destroyed when all pending command buffers are complete execution.
		// Returns 'true' if wasn't empty.
			virtual bool						ForceReleaseResources ()											__NE___	= 0;

		// Returns native handle.
		ND_ virtual NativeBuffer_t				GetBufferHandle (BufferID id)										C_NE___ = 0;
		ND_ virtual NativeImage_t				GetImageHandle (ImageID id)											C_NE___ = 0;
		ND_ virtual NativeBufferView_t			GetBufferViewHandle (BufferViewID id)								C_NE___ = 0;
		ND_ virtual NativeImageView_t			GetImageViewHandle (ImageViewID id)									C_NE___ = 0;

			virtual bool						GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)			C_NE___ = 0;
			virtual bool						GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)			C_NE___ = 0;

		// async loading
		//	Pipeline compilation may be distributed to multiple threads.
		ND_ virtual AsyncRTechPipelines			LoadRenderTechAsync (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache = Default)	__NE___	= 0;
		ND_ virtual RenderTechPipelinesPtr		LoadRenderTech (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache = Default)		__NE___	= 0;

		// statistics
		ND_ virtual StagingBufferStat			GetStagingBufferFrameStat (FrameUID frameId)						C_NE___ = 0;

		ND_ virtual FeatureSet const&			GetFeatureSet ()													C_NE___	= 0;

		// memory allocators
		ND_ virtual GfxMemAllocatorPtr			CreateLinearGfxMemAllocator (Bytes pageSize = 0_b)					C_NE___ = 0;
		ND_ virtual GfxMemAllocatorPtr			CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize)		C_NE___ = 0;
		ND_ virtual GfxMemAllocatorPtr			CreateUnifiedGfxMemAllocator (Bytes pageSize = 0_b)					C_NE___ = 0;
		ND_ virtual GfxMemAllocatorPtr			CreateLargeSizeGfxMemAllocator ()									C_NE___ = 0;
		ND_ virtual GfxMemAllocatorPtr			GetDefaultGfxMemAllocator ()										C_NE___ = 0;

		// descriptor allocators
		//ND_ virtual DescriptorAllocatorPtr	CreateLinearDescriptorAllocator ()									C_NE___ = 0;
		ND_ virtual DescriptorAllocatorPtr		GetDefaultDescriptorAllocator ()									C_NE___ = 0;
	};


/*
=================================================
	CreateDescriptorSet
=================================================
*/
	template <typename PplnID>
	IResourceManager::DescSetAndBinding_t  IResourceManager::CreateDescriptorSet (const PplnID &ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DescSetAndBinding_t	result;
		return CreateDescriptorSets( OUT result.Get<1>(), OUT &result.Get<0>(), 1, ppln, dsName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline Strong<DescriptorSetID>  IResourceManager::CreateDescriptorSet (PipelinePackID packId, DSLayoutName::Ref dslName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		Strong<DescriptorSetID>	result;
		return CreateDescriptorSets( OUT &result, 1, packId, dslName, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

	inline Strong<DescriptorSetID>  IResourceManager::CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		Strong<DescriptorSetID>	result;
		return CreateDescriptorSets( OUT &result, 1, layoutId, RVRef(allocator), dbgName ) ? RVRef(result) : Default;
	}

/*
=================================================
	GetPushConstantIndex
=================================================
*/
	template <typename T, typename PplnID>
	PushConstantIndex  IResourceManager::GetPushConstantIndex (PplnID ppln, PushConstantName::Ref pcName) __NE___
	{
		return GetPushConstantIndex( ppln, pcName, T::TypeName, SizeOf<T> );
	}


} // AE::Graphics

#undef NON_INTERFACE
