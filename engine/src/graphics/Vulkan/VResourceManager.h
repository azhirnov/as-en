// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VSwapchain.h"

# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VBufferView.h"
# include "graphics/Vulkan/Resources/VDescriptorSet.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"
# include "graphics/Vulkan/Resources/VFramebuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/Resources/VImageView.h"
# include "graphics/Vulkan/Resources/VPipelineLayout.h"
# include "graphics/Vulkan/Resources/VComputePipeline.h"
# include "graphics/Vulkan/Resources/VGraphicsPipeline.h"
# include "graphics/Vulkan/Resources/VRayTracingPipeline.h"
# include "graphics/Vulkan/Resources/VMeshPipeline.h"
# include "graphics/Vulkan/Resources/VTilePipeline.h"
# include "graphics/Vulkan/Resources/VPipelinePack.h"
# include "graphics/Vulkan/Resources/VPipelineCache.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"
# include "graphics/Vulkan/Resources/VMemoryObject.h"
# include "graphics/Vulkan/Resources/VStagingBufferManager.h"

# include "threading/Containers/LfIndexedPool3.h"
# include "threading/Containers/LfStaticIndexedPool.h"

namespace AE::Graphics
{

	//
	// Vulkan Resource Manager
	//

	class VResourceManager final : public IResourceManager
	{
		friend class VRenderTaskScheduler;

	// types
	public:
		class ReleaseExpiredResourcesTask;

	private:
		template <typename T, typename ID, usize ChunkSize, usize MaxChunks>
		using PoolTmpl		= Threading::LfIndexedPool3< ResourceBase<T>, typename ID::Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

		template <typename T, typename ID, usize PoolSize>
		using StPoolTmpl	= Threading::LfStaticIndexedPool< ResourceBase<T>, typename ID::Index_t, PoolSize, GlobalLinearAllocatorRef >;

		// chunk size
		static constexpr uint	MaxImages		= 1u << 10;
		static constexpr uint	MaxBuffers		= 1u << 10;
		static constexpr uint	MaxMemoryObjs	= 1u << 10;
		static constexpr uint	MaxDSLayouts	= 1u << 9;
		static constexpr uint	MaxDescSets		= 1u << 9;
		static constexpr uint	MaxRTObjects	= 1u << 9;
		static constexpr uint	MaxPipelines	= 1u << 10;
		
		using MemObjPool_t			= PoolTmpl< VMemoryObject,			VMemoryID,				MaxMemoryObjs,	64 >;
		using BufferPool_t			= PoolTmpl< VBuffer,				BufferID,				MaxBuffers,		32 >;
		using BufferViewPool_t		= PoolTmpl< VBufferView,			BufferViewID,			MaxBuffers,		32 >;
		using ImagePool_t			= PoolTmpl< VImage,					ImageID,				MaxImages,		32 >;
		using ImageViewPool_t		= PoolTmpl< VImageView,				ImageViewID,			MaxImages,		32 >;
		using DSLayoutPool_t		= PoolTmpl<	VDescriptorSetLayout,	DescriptorSetLayoutID,	MaxDSLayouts,	 8 >;
		using PplnLayoutPool_t		= PoolTmpl<	VPipelineLayout,		VPipelineLayoutID,		MaxDSLayouts,	 8 >;
		using GPipelinePool_t		= PoolTmpl< VGraphicsPipeline,		GraphicsPipelineID,		MaxPipelines,	32 >;
		using CPipelinePool_t		= PoolTmpl< VComputePipeline,		ComputePipelineID,		MaxPipelines,	32 >;
		using MPipelinePool_t		= PoolTmpl< VMeshPipeline,			MeshPipelineID,			MaxPipelines,	32 >;
		using RTPipelinePool_t		= PoolTmpl< VRayTracingPipeline,	RayTracingPipelineID,	MaxPipelines,	 8 >;
		using TPipelinePool_t		= PoolTmpl< VTilePipeline,			TilePipelineID,			MaxPipelines,	 8 >;
		using DescSetPool_t			= PoolTmpl< VDescriptorSet,			DescriptorSetID,		MaxDescSets,	 8 >;
		using RTGeomPool_t			= PoolTmpl< VRTGeometry,			RTGeometryID,			MaxRTObjects,	 8 >;
		using RTScenePool_t			= PoolTmpl< VRTScene,				RTSceneID,				MaxRTObjects,	 8 >;
		using SamplerPool_t			= StPoolTmpl< VSampler,				VSamplerID,				4096 >;
		using RenderPassPool_t		= StPoolTmpl< VRenderPass,			VRenderPassID,			1024 >;
		using PipelineCachePool_t	= StPoolTmpl< VPipelineCache,		PipelineCacheID,		  64 >;
		using FramebufferPool_t		= StPoolTmpl< VFramebuffer,			VFramebufferID,			 128 >;
		using PipelinePackPool_t	= StPoolTmpl< VPipelinePack,		PipelinePackID,			  32 >;
		using FramebufferCache_t	= VFramebuffer::FramebufferCache_t;

		struct ResourceDestructor;
		using AllResourceIDs_t		= TypeList< BufferViewID, ImageViewID, BufferID, ImageID,
												PipelineCacheID, DescriptorSetLayoutID, VPipelineLayoutID,
												VSamplerID, VFramebufferID, VRenderPassID, 
												GraphicsPipelineID, ComputePipelineID, MeshPipelineID, RayTracingPipelineID, TilePipelineID,
												RTGeometryID, RTSceneID,
												VMemoryID	// must be in  the end
											>;

		//---- Expired resources ----//
		using AllVkResources_t		= TypeList< VkBuffer, VkBufferView, VkImage, VkImageView, VkPipeline, VkAccelerationStructureKHR, VMemoryID,
												// Can be added to release list to decrease reference counter
												VFramebufferID >;
		struct ExpiredResource
		{
			static constexpr usize	IDSize = AllVkResources_t::ForEach_Max< TypeListUtils::GetTypeSize >();

			using IDValue_t = BitSizeToUInt< IDSize * 8 >;

			IDValue_t	id		= UMax;		// type of AllVkResources_t[type]
			uint		type	= UMax;		// index in AllVkResources_t
		};

		struct alignas(AE_CACHE_LINE) ExpiredResArray
		{
			SpinLock					guard;
			FrameUID					frameId;
			Array< ExpiredResource >	resources;
		};
		using ExpiredResources_t	= StaticArray< ExpiredResArray, GraphicsConfig::MaxFrames*2 >;

		struct ExpiredResources
		{
			AtomicFrameUID			_currentFrameId;
			ExpiredResources_t		_list;

			ExpiredResources ();

			ND_ ExpiredResArray&	Get (FrameUID id)		{ return _list[ uint(id.Unique()) % _list.size() ]; }
			ND_ ExpiredResArray&	GetCurrent ()			{ return Get( GetFrameId() ); }
			ND_ FrameUID			GetFrameId ()	const	{ return _currentFrameId.load(); }
			ND_ ExpiredResources_t&	All ()					{ return _list; }
		};


	// variables
	private:
		VDevice const&			_device;

		struct {
			BufferPool_t			buffers;
			ImagePool_t				images;
			
			BufferViewPool_t		bufferViews;
			ImageViewPool_t			imageViews;

			SamplerPool_t			samplers;

			PipelinePackPool_t		pipelinePacks;
			PipelineCachePool_t		pipelineCache;

			DSLayoutPool_t			dsLayouts;
			PplnLayoutPool_t		pplnLayouts;
			DescSetPool_t			descSet;

			GPipelinePool_t			graphicsPpln;
			CPipelinePool_t			computePpln;
			MPipelinePool_t			meshPpln;
			RTPipelinePool_t		raytracePpln;
			TPipelinePool_t			tilePpln;
			
			RTGeomPool_t			rtGeom;
			RTScenePool_t			rtScene;

			RenderPassPool_t		renderPass;
			FramebufferPool_t		framebuffers;
			
			RWSpinLock				fbCacheGuard;
			FramebufferCache_t		fbCache;

			MemObjPool_t			memObjs;
		}						_resPool;
		
		GfxMemAllocatorPtr		_defaultMemAlloc;
		DescriptorAllocatorPtr	_defaultDescAlloc;

		VStagingBufferManager	_stagingMngr;
		
		FeatureSet				_featureSet;
		ExpiredResources		_expiredResources;

		Strong<PipelinePackID>			_defaultPack;
		Strong<VSamplerID>				_defaultSampler;
		Strong<DescriptorSetLayoutID>	_emptyDSLayout;

		#ifdef AE_DEV_OR_DBG
		 mutable SharedMutex			_hashToNameGuard;
		 PipelineCompiler::HashToName	_hashToName;		// for debugging
		#endif


	// methods
	private:
		explicit VResourceManager (const VDevice &);

		ND_ bool  Initialize (const GraphicsCreateInfo &);
			void  Deinitialize ();
		

	public:
		~VResourceManager ();

		ND_ bool				OnSurfaceCreated (const VSwapchain &sw);
		
		bool					InitializeResources (const PipelinePackDesc &desc) override;
		Strong<PipelinePackID>	LoadPipelinePack (const PipelinePackDesc &desc) override;
		Array<RenderTechName>	GetSupportedRenderTechs (PipelinePackID id) const override;

		bool					IsSupported (EMemoryType memType)		const override;
		bool					IsSupported (const BufferDesc &desc)	const override;
		bool					IsSupported (const ImageDesc &desc)		const override;
		bool					IsSupported (BufferID buffer, const BufferViewDesc &desc)	const override;
		bool					IsSupported (ImageID image, const ImageViewDesc &desc)		const override;

		ND_ Strong<VMemoryID>	CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ Strong<VMemoryID>	CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);

		Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) override;
		Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) override;
		
		Strong<ImageID>			CreateImage (const VulkanImageDesc &desc, StringView dbgName = Default)		override;
		Strong<BufferID>		CreateBuffer (const VulkanBufferDesc &desc, StringView dbgName = Default)	override;
		
		Strong<ImageViewID>		CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)		override;
		Strong<BufferViewID>	CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)	override;
		
		Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)	override;
		Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			override;
		
		RTASBuildSizes			GetRTGeometrySizes (const RTGeometryBuild &desc) override;
		RTASBuildSizes			GetRTSceneSizes (const RTSceneBuild &desc)		 override;

		DescSetAndBinding_t		CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (TilePipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		
		Strong<DescriptorSetID>	CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		Strong<DescriptorSetID>	CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;

		Strong<PipelineCacheID>	CreatePipelineCache () override;
		Strong<PipelineCacheID>	LoadPipelineCache (RC<RStream> stream) override;
		
		Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	override;
		RenderTechPipelinesPtr			LoadRenderTech      (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	override;

		Strong<GraphicsPipelineID>		CreateGraphicsPipeline   (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache = Default)	override;
		Strong<MeshPipelineID>			CreateMeshPipeline       (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache = Default)	override;
		Strong<ComputePipelineID>		CreateComputePipeline    (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache = Default)	override;
		Strong<RayTracingPipelineID>	CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache = Default)	override;
		Strong<TilePipelineID>			CreateTilePipeline       (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache = Default)	override;

		ND_ Strong<ComputePipelineID>	CreatePipeline (const VComputePipeline::CreateInfo    &ci);
		ND_ Strong<GraphicsPipelineID>	CreatePipeline (const VGraphicsPipeline::CreateInfo   &ci);
		ND_ Strong<MeshPipelineID>		CreatePipeline (const VMeshPipeline::CreateInfo       &ci);
		ND_ Strong<RayTracingPipelineID>CreatePipeline (const VRayTracingPipeline::CreateInfo &ci);
		ND_ Strong<TilePipelineID>		CreatePipeline (const VTilePipeline::CreateInfo       &ci);

		ND_ Strong<DescriptorSetLayoutID>	CreateDescriptorSetLayout (const VDescriptorSetLayout::CreateInfo &ci);
		ND_ Strong<VPipelineLayoutID>		CreatePipelineLayout (const VPipelineLayout::DescriptorSets_t &descSetLayouts, const VPipelineLayout::PushConstants_t &pushConstants, StringView dbgName = Default);
		
		ND_ Strong<VSamplerID>		CreateSampler (const VkSamplerCreateInfo &info, StringView dbgName = Default);
		ND_ VSamplerID				GetSampler (const SamplerName &name) const;
		ND_ VkSampler				GetVkSampler (const SamplerName &name) const;
		
		ND_ Strong<VRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, VRenderPassID compatId);
		ND_ VRenderPassID			GetCompatibleRenderPass (const CompatRenderPassName &name) const;
		ND_ VRenderPassID			GetCompatibleRenderPass (const RenderPassName &name) const;
		ND_ VRenderPassID			GetRenderPass (const RenderPassName &name) const;
		ND_ VFramebufferID			CreateFramebuffer (const RenderPassDesc &desc);
			void					RemoveFramebufferCache (VFramebuffer::CachePtr_t iter);	// call from VFramebuffer

		bool	ReleaseResource (Strong<ImageID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ImageViewID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferViewID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelineCacheID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelinePackID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<GraphicsPipelineID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<MeshPipelineID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ComputePipelineID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RayTracingPipelineID>&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<TilePipelineID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<DescriptorSetID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTGeometryID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTSceneID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		
		template <usize IS, usize GS, uint UID>
		bool	ReleaseResource (Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)	{ return _ReleaseResource( id.Release(), refCount ) == 0; }
		
		template <typename Arg0, typename ...Args>
		bool	ReleaseResources (Arg0 &arg0, Args& ...args);

		BufferDesc const&		GetDescription (BufferID id)		const override;
		ImageDesc const&		GetDescription (ImageID id)			const override;
		BufferViewDesc const&	GetDescription (BufferViewID id)	const override;
		ImageViewDesc const&	GetDescription (ImageViewID id)		const override;

		VkBuffer			GetBufferHandle (BufferID id)			const override;
		VkImage				GetImageHandle (ImageID id)				const override;
		VkBufferView		GetBufferViewHandle (BufferViewID id)	const override;
		VkImageView			GetImageViewHandle (ImageViewID id)		const override;

			bool			GetMemoryInfo (ImageID id, OUT VulkanMemoryObjInfo &info)	const override;
			bool			GetMemoryInfo (BufferID id, OUT VulkanMemoryObjInfo &info)	const override;
		ND_ bool			GetMemoryInfo (VMemoryID id, OUT VulkanMemoryObjInfo &info)	const;
		
		template <usize IS, usize GS, uint UID>
		ND_ auto			AcquireResource (HandleTmpl<IS, GS, UID> id);

		template <usize IS, usize GS, uint UID>
		ND_ bool			IsAlive (HandleTmpl<IS,GS,UID> id)	const;
		ND_ bool			IsAlive (const SamplerName &name)	const;

		bool				IsResourceAlive (BufferID			id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageID			id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (BufferViewID		id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageViewID		id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (DescriptorSetID	id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelineCacheID	id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelinePackID		id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (RTGeometryID		id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (RTSceneID			id) const override		{ return IsAlive( id ); }

		template <typename ID>
		ND_ auto const*		GetResource (ID id, Bool incRef = false, Bool quiet = false) const;
		
		template <typename ID>
		ND_ auto const*		GetResource (const Strong<ID> &id, Bool incRef = false, Bool quiet = false) const;

		ND_ VDevice const&			GetDevice ()			const	{ return _device; }
		ND_ VStagingBufferManager&	GetStagingManager ()			{ return _stagingMngr; }
		ND_ FeatureSet const&		GetFeatureSet ()		const	{ return _featureSet; }
		ND_ PipelinePackID			GetDefaultPack ()		const	{ return _defaultPack; }

		bool  ForceReleaseResources () override;

		template <typename ID>
		void  DelayedDestroy (ID res);

		void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg);
		void  OnEndFrame (FrameUID frameId);
		
		template <usize Size, uint UID, bool Opt, uint Seed>
		ND_ String  HashToName (const NamedID< Size, UID, Opt, Seed > &name) const;
		
		#ifdef AE_DEV_OR_DBG
		void  AddHashToName (const PipelineCompiler::HashToName &value);
		#endif
		
		StagingBufferStat  GetStagingBufferFrameStat (FrameUID frameId) const override	{ return _stagingMngr.GetFrameStat( frameId ); }

		AE_GLOBALLY_ALLOC

	private:

	// resource api 
		template <typename ID>
		ND_ int  _ReleaseResource (ID id, uint refCount = 1);

			void  _DestroyVkResource (const ExpiredResource &res);

		template <typename ID, typename ...Args>
		ND_ Strong<ID>  _CreateResource (const char* msg, Args&& ...args);

		template <typename ID, typename FnInitialize, typename FnCreate>
		ND_ Strong<ID>  _CreateCachedResource (StringView errorStr, FnInitialize&& fnInit, FnCreate&& fnCreate);
		
		template <typename PplnID>
		ND_ DescSetAndBinding_t  _CreateDescriptorSet (const PplnID &pplnId, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName);

	// resource pool
		ND_ auto&  _GetResourcePool (const BufferID &)						{ return _resPool.buffers; }
		ND_ auto&  _GetResourcePool (const ImageID &)						{ return _resPool.images; }
		ND_ auto&  _GetResourcePool (const BufferViewID &)					{ return _resPool.bufferViews; }
		ND_ auto&  _GetResourcePool (const ImageViewID &)					{ return _resPool.imageViews; }
		ND_ auto&  _GetResourcePool (const DescriptorSetLayoutID &)			{ return _resPool.dsLayouts; }
		ND_ auto&  _GetResourcePool (const VPipelineLayoutID &)				{ return _resPool.pplnLayouts; }
		ND_ auto&  _GetResourcePool (const GraphicsPipelineID &)			{ return _resPool.graphicsPpln; }
		ND_ auto&  _GetResourcePool (const ComputePipelineID &)				{ return _resPool.computePpln; }
		ND_ auto&  _GetResourcePool (const MeshPipelineID &)				{ return _resPool.meshPpln; }
		ND_ auto&  _GetResourcePool (const RayTracingPipelineID &)			{ return _resPool.raytracePpln; }
		ND_ auto&  _GetResourcePool (const TilePipelineID &)				{ return _resPool.tilePpln; }
		ND_ auto&  _GetResourcePool (const VSamplerID &)					{ return _resPool.samplers; }
		ND_ auto&  _GetResourcePool (const VRenderPassID &)					{ return _resPool.renderPass; }
		ND_ auto&  _GetResourcePool (const VFramebufferID &)				{ return _resPool.framebuffers; }
		ND_ auto&  _GetResourcePool (const DescriptorSetID &)				{ return _resPool.descSet; }
		ND_ auto&  _GetResourcePool (const PipelineCacheID &)				{ return _resPool.pipelineCache; }
		ND_ auto&  _GetResourcePool (const PipelinePackID &)				{ return _resPool.pipelinePacks; }
		ND_ auto&  _GetResourcePool (const RTGeometryID &)					{ return _resPool.rtGeom; }
		ND_ auto&  _GetResourcePool (const RTSceneID &)						{ return _resPool.rtScene; }
		ND_ auto&  _GetResourcePool (const VMemoryID &)						{ return _resPool.memObjs; }
		
		template <typename ID>
		ND_ const auto&  _GetResourceCPool (const ID &id)	const			{ return const_cast<VResourceManager *>(this)->_GetResourcePool( id ); }
		
		ND_ StringView  _GetResourcePoolName (const BufferID &)				{ return "buffers"; }
		ND_ StringView  _GetResourcePoolName (const ImageID &)				{ return "images"; }
		ND_ StringView  _GetResourcePoolName (const BufferViewID &)			{ return "bufferViews"; }
		ND_ StringView  _GetResourcePoolName (const ImageViewID &)			{ return "imageViews"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetLayoutID&)	{ return "dsLayouts"; }
		ND_ StringView  _GetResourcePoolName (const VPipelineLayoutID &)	{ return "pplnLayouts"; }
		ND_ StringView  _GetResourcePoolName (const GraphicsPipelineID &)	{ return "graphicsPpln"; }
		ND_ StringView  _GetResourcePoolName (const ComputePipelineID &)	{ return "computePpln"; }
		ND_ StringView  _GetResourcePoolName (const MeshPipelineID &)		{ return "meshPpln"; }
		ND_ StringView  _GetResourcePoolName (const RayTracingPipelineID &)	{ return "raytracePpln"; }
		ND_ StringView  _GetResourcePoolName (const TilePipelineID &)		{ return "tilePpln"; }
		ND_ StringView  _GetResourcePoolName (const VSamplerID &)			{ return "samplers"; }
		ND_ StringView  _GetResourcePoolName (const VRenderPassID &)		{ return "renderPass"; }
		ND_ StringView  _GetResourcePoolName (const VFramebufferID &)		{ return "framebuffers"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetID &)		{ return "descSet"; }
		ND_ StringView  _GetResourcePoolName (const PipelineCacheID &)		{ return "pipelineCache"; }
		ND_ StringView  _GetResourcePoolName (const PipelinePackID &)		{ return "pipelinePacks"; }
		ND_ StringView  _GetResourcePoolName (const RTGeometryID &)			{ return "rtGeometry"; }
		ND_ StringView  _GetResourcePoolName (const RTSceneID &)			{ return "rtScene"; }
		ND_ StringView  _GetResourcePoolName (const VMemoryID &)			{ return "memObjs"; }

	// memory managment
		ND_ GfxMemAllocatorPtr		_ChooseMemAllocator (GfxMemAllocatorPtr userDefined);
		ND_ DescriptorAllocatorPtr	_ChooseDescAllocator (DescriptorAllocatorPtr userDefined);

	// 
		template <typename ID>	ND_ bool   _Assign (OUT ID &id);
		template <typename ID>		void   _Unassign (ID id);


	// empty descriptor set layout
		ND_ bool  _CreateEmptyDescriptorSetLayout ();
		ND_ auto  _GetEmptyDescriptorSetLayout ()		{ return *_emptyDSLayout; }

		ND_ bool  _CreateDefaultSampler ();

	// 
		template <typename ID>
		ND_ auto const&			_GetDescription (ID id) const;
	};

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
