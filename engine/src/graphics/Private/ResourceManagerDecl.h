// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX		V
#	define RESMNGR		VResourceManager

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX		M
#	define RESMNGR		MResourceManager

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


		friend class AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );

	// types
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

		using Device_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Device					);
		using Swapchain_t			= AE_PRIVATE_UNITE_RAW( SUFFIX, Swapchain				);
		using StagingBufferManager_t= AE_PRIVATE_UNITE_RAW( SUFFIX, StagingBufferManager	);
		using QueryManager_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, QueryManager			);

		using ComputePipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, ComputePipeline			);
		using GraphicsPipeline_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, GraphicsPipeline		);
		using MeshPipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, MeshPipeline			);
		using RayTracingPipeline_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, RayTracingPipeline		);
		using TilePipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, TilePipeline			);
		using Buffer_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Buffer					);
		using Image_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Image					);
		using RTGeometry_t			= AE_PRIVATE_UNITE_RAW( SUFFIX, RTGeometry				);
		using RTScene_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, RTScene					);
		using DescriptorSetLayout_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, DescriptorSetLayout		);
		using PipelineLayout_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineLayout			);
		using ShaderBindingTable_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, RTShaderBindingTable	);

		using SamplerID_t			= AE_PRIVATE_UNITE_RAW( SUFFIX, SamplerID				);
		using RenderPassID_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, RenderPassID			);
		using PipelineLayoutID_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineLayoutID		);
		using MemoryID_t			= AE_PRIVATE_UNITE_RAW( SUFFIX, MemoryID				);
		
		using MemObjPool_t			= PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, MemoryObject		),		MemoryID_t,					MaxMemoryObjs,	64 >;
		using BufferPool_t			= PoolTmpl< Buffer_t,												BufferID,					MaxBuffers,		32 >;
		using BufferViewPool_t		= PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, BufferView		),		BufferViewID,				MaxBuffers,		32 >;
		using ImagePool_t			= PoolTmpl< Image_t,												ImageID,					MaxImages,		32 >;
		using ImageViewPool_t		= PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, ImageView			),		ImageViewID,				MaxImages,		32 >;
		using DSLayoutPool_t		= PoolTmpl<	DescriptorSetLayout_t,									DescriptorSetLayoutID,		MaxDSLayouts,	 8 >;
		using GPipelinePool_t		= PoolTmpl< GraphicsPipeline_t,										GraphicsPipelineID,			MaxPipelines,	32 >;
		using CPipelinePool_t		= PoolTmpl< ComputePipeline_t,										ComputePipelineID,			MaxPipelines,	32 >;
		using MPipelinePool_t		= PoolTmpl< MeshPipeline_t,											MeshPipelineID,				MaxPipelines,	32 >;
		using RTPipelinePool_t		= PoolTmpl< RayTracingPipeline_t,									RayTracingPipelineID,		MaxPipelines,	 8 >;
		using TPipelinePool_t		= PoolTmpl< TilePipeline_t,											TilePipelineID,				MaxPipelines,	 8 >;
		using DescSetPool_t			= PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, DescriptorSet		),		DescriptorSetID,			MaxDescSets,	 8 >;
		using RTGeomPool_t			= PoolTmpl< RTGeometry_t,											RTGeometryID,				MaxRTObjects,	 8 >;
		using RTScenePool_t			= PoolTmpl< RTScene_t,												RTSceneID,					MaxRTObjects,	 8 >;
		using PplnLayoutPool_t		= PoolTmpl<	PipelineLayout_t,										PipelineLayoutID_t,			MaxDSLayouts,	 8 >;
		using SBTPool_t				= PoolTmpl<	ShaderBindingTable_t,									RTShaderBindingID,			1u << 8,		 8 >;
		using SamplerPool_t			= StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, Sampler			),		SamplerID_t,				4096 >;
		using RenderPassPool_t		= StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, RenderPass		),		RenderPassID_t,				1024 >;
		using PipelineCachePool_t	= StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineCache	),		PipelineCacheID,			  64 >;
		using PipelinePackPool_t	= StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, PipelinePack	),		PipelinePackID,				  32 >;
		
	  #ifdef AE_ENABLE_VULKAN
		using FramebufferPool_t		= StPoolTmpl< VFramebuffer,											VFramebufferID,				 128 >;
		using FramebufferCache_t	= VFramebuffer::FramebufferCache_t;
	  #endif

		struct ResourceDestructor;
		using AllResourceIDs_t		= TypeList< BufferViewID, ImageViewID, BufferID, ImageID,
												PipelineCacheID, DescriptorSetLayoutID, PipelineLayoutID_t, SamplerID_t,
											  #ifdef AE_ENABLE_VULKAN
												VFramebufferID,
											  #endif
												RenderPassID_t, GraphicsPipelineID, ComputePipelineID, MeshPipelineID, RayTracingPipelineID, TilePipelineID,
												RTGeometryID, RTSceneID, RTShaderBindingID,
												MemoryID_t		// must be in  the end
											>;

		//---- Expired resources ----//
	#ifdef AE_ENABLE_VULKAN
	public:
		class VReleaseExpiredResourcesTask;
		
	private:
		using AllVkResources_t		= TypeList< VkBuffer, VkBufferView, VkImage, VkImageView, VkPipeline, VkAccelerationStructureKHR, VMemoryID,
												// Can be added to release list to decrease reference counter
												VFramebufferID >;
		struct VExpiredResource
		{
			static constexpr usize	IDSize = AllVkResources_t::ForEach_Max< TypeListUtils::GetTypeSize >();

			using IDValue_t = BitSizeToUInt< IDSize * 8 >;

			IDValue_t	id		= UMax;		// type of AllVkResources_t[type]
			uint		type	= UMax;		// index in AllVkResources_t
		};

		struct alignas(AE_CACHE_LINE) VExpiredResArray
		{
			SpinLock					guard;
			FrameUID					frameId;
			Array< VExpiredResource >	resources;
		};
		using VExpiredResources_t	= StaticArray< VExpiredResArray, GraphicsConfig::MaxFrames*2 >;

		struct VExpiredResources
		{
			AtomicFrameUID			_currentFrameId;
			VExpiredResources_t		_list;

			VExpiredResources ()							__Th___;

			ND_ VExpiredResArray&		Get (FrameUID id)	__NE___	{ return _list[ uint(id.Unique()) % _list.size() ]; }
			ND_ VExpiredResArray&		GetCurrent ()		__NE___	{ return Get( GetFrameId() ); }
			ND_ FrameUID				GetFrameId ()		C_NE___	{ return _currentFrameId.load(); }
			ND_ VExpiredResources_t&	All ()				__NE___	{ return _list; }
		};
	#endif


	// variables
	private:
		Device_t const&			_device;

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
			SBTPool_t				rtSBT;

			RenderPassPool_t		renderPass;
			
		  #ifdef AE_ENABLE_VULKAN
			FramebufferPool_t		framebuffers;
			RWSpinLock				fbCacheGuard;
			FramebufferCache_t		fbCache;
		  #endif

			MemObjPool_t			memObjs;
		}						_resPool;
		
		GfxMemAllocatorPtr		_defaultMemAlloc;
		DescriptorAllocatorPtr	_defaultDescAlloc;

		StagingBufferManager_t	_stagingMngr;
		QueryManager_t			_queryMngr;
		
		FeatureSet				_featureSet;
		
	  #ifdef AE_ENABLE_VULKAN
		VExpiredResources		_expiredResources;
	  #endif

		StrongAtom<PipelinePackID>		_defaultPack;
		Strong<SamplerID_t>				_defaultSampler;
		Strong<DescriptorSetLayoutID>	_emptyDSLayout;

		#ifdef AE_DBG_OR_DEV
		 mutable SharedMutex			_hashToNameGuard;
		 PipelineCompiler::HashToName	_hashToName;		// for debugging
		#endif
		 
	
	// methods
	private:
		explicit RESMNGR (const Device_t &)															__Th___;

		ND_ bool  Initialize (const GraphicsCreateInfo &)											__Th___;
			void  Deinitialize ()																	__NE___;
		

	public:
		~RESMNGR ()																					__NE___;
		
		ND_ bool					OnSurfaceCreated (const Swapchain_t &sw)						__NE___;

			bool					InitializeResources (const PipelinePackDesc &desc)				__NE_OV;
		ND_ Strong<PipelinePackID>	LoadPipelinePack (const PipelinePackDesc &desc)					__NE_OV;
		ND_ Array<RenderTechName>	GetSupportedRenderTechs (PipelinePackID id)						C_NE_OV;

		ND_ bool					IsSupported (EMemoryType memType)								C_NE_OV;
		ND_ bool					IsSupported (const BufferDesc &desc)							C_NE_OV;
		ND_ bool					IsSupported (const ImageDesc &desc)								C_NE_OV;
		ND_ bool					IsSupported (BufferID buffer, const BufferViewDesc &desc)		C_NE_OV;
		ND_ bool					IsSupported (ImageID image, const ImageViewDesc &desc)			C_NE_OV;
		
		ND_ Strong<ComputePipelineID>	CreatePipeline (const ComputePipeline_t::CreateInfo    &ci)	__NE___;
		ND_ Strong<GraphicsPipelineID>	CreatePipeline (const GraphicsPipeline_t::CreateInfo   &ci)	__NE___;
		ND_ Strong<MeshPipelineID>		CreatePipeline (const MeshPipeline_t::CreateInfo       &ci)	__NE___;
		ND_ Strong<RayTracingPipelineID>CreatePipeline (const RayTracingPipeline_t::CreateInfo &ci)	__NE___;
		ND_ Strong<TilePipelineID>		CreatePipeline (const TilePipeline_t::CreateInfo       &ci)	__NE___;
		
		ND_ Strong<RTShaderBindingID>	CreateRTShaderBinding (const ShaderBindingTable_t::CreateInfo &ci)			__NE___;

		ND_ Strong<DescriptorSetLayoutID>	CreateDescriptorSetLayout (const DescriptorSetLayout_t::CreateInfo &ci)	__NE___;
		
		ND_ Strong<PipelineLayoutID_t>	CreatePipelineLayout (const PipelineLayout_t::DescriptorSets_t &descSetLayouts, const PipelineLayout_t::PushConstants_t &pushConstants, StringView dbgName = Default) __NE___;

		ND_ Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE_OV;
		ND_ Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE_OV;
		
		ND_ Strong<ImageID>			CreateImage (const NativeImageDesc_t &desc, StringView dbgName = Default)										__NE_OV;
		ND_ Strong<BufferID>		CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName = Default)										__NE_OV;
		
		ND_ Strong<ImageViewID>		CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)						__NE_OV;
		ND_ Strong<BufferViewID>	CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)					__NE_OV;
		
		ND_ Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE_OV;
		ND_ Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE_OV;
		
		ND_ RTASBuildSizes			GetRTGeometrySizes (const RTGeometryBuild &desc)																__NE_OV;
		ND_ RTASBuildSizes			GetRTSceneSizes (const RTSceneBuild &desc)																		__NE_OV;

		ND_ DescSetAndBinding_t		CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		ND_ DescSetAndBinding_t		CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		ND_ DescSetAndBinding_t		CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		ND_ DescSetAndBinding_t		CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		ND_ DescSetAndBinding_t		CreateDescriptorSet (TilePipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		
		ND_ Strong<DescriptorSetID>	CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)			__NE_OV;
		ND_ Strong<DescriptorSetID>	CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)								__NE_OV;

		ND_ NativeBuffer_t			GetBufferHandle (BufferID id)					C_NE_OV;
		ND_ NativeImage_t			GetImageHandle (ImageID id)						C_NE_OV;
		ND_ NativeBufferView_t		GetBufferViewHandle (BufferViewID id)			C_NE_OV;
		ND_ NativeImageView_t		GetImageViewHandle (ImageViewID id)				C_NE_OV;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	__NE_OV;
		ND_ RenderTechPipelinesPtr			LoadRenderTech      (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	__NE_OV;

		ND_ Strong<GraphicsPipelineID>		CreateGraphicsPipeline   (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache = Default)	__NE_OV;
		ND_ Strong<MeshPipelineID>			CreateMeshPipeline       (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache = Default)	__NE_OV;
		ND_ Strong<ComputePipelineID>		CreateComputePipeline    (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache = Default)	__NE_OV;
		ND_ Strong<RayTracingPipelineID>	CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache = Default)	__NE_OV;
		ND_ Strong<TilePipelineID>			CreateTilePipeline       (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache = Default)	__NE_OV;
		
		ND_ SamplerID_t				GetSampler (const SamplerName &name)						C_NE___;

		ND_ RenderPassID_t			GetCompatibleRenderPass (const CompatRenderPassName &name)	C_NE___;
		ND_ RenderPassID_t			GetCompatibleRenderPass (const RenderPassName &name)		C_NE___;
		ND_ RenderPassID_t			GetRenderPass (const RenderPassName &name)					C_NE___;

		ND_ Strong<PipelineCacheID>		CreatePipelineCache ()					__NE_OV;

			bool	GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)	C_NE_OV;
			bool	GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)	C_NE_OV;
			bool	GetMemoryInfo (MemoryID_t id, OUT NativeMemObjInfo_t &info)	C_NE___;

			bool	ReleaseResource (Strong<ImageID>			&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<BufferID>			&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<ImageViewID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<BufferViewID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<PipelineCacheID>	&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<PipelinePackID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<GraphicsPipelineID>	&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<MeshPipelineID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<ComputePipelineID>	&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<RayTracingPipelineID>&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<TilePipelineID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<DescriptorSetID>	&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<RTGeometryID>		&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
			bool	ReleaseResource (Strong<RTSceneID>			&id)			__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		
			template <usize IS, usize GS, uint UID>
			bool	ReleaseResource (Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)	__NE___	{ return _ReleaseResource( id.Release(), refCount ) == 0; }
		
			template <typename Arg0, typename ...Args>
			bool	ReleaseResources (Arg0 &arg0, Args& ...args)				__NE___;

		ND_ BufferDesc const&		GetDescription (BufferID id)				C_NE_OV;
		ND_ ImageDesc const&		GetDescription (ImageID id)					C_NE_OV;
		ND_ BufferViewDesc const&	GetDescription (BufferViewID id)			C_NE_OV;
		ND_ ImageViewDesc const&	GetDescription (ImageViewID id)				C_NE_OV;

		ND_ FeatureSet const&		GetFeatureSet ()							C_NE___	{ return _featureSet; }
		ND_ PipelinePackID			GetDefaultPack ()							C_NE___	{ return _defaultPack; }

		template <usize IS, usize GS, uint UID>
		ND_ bool			IsAlive (HandleTmpl<IS,GS,UID> id)					C_NE___;
		ND_ bool			IsAlive (const SamplerName &name)					C_NE___;

		ND_ bool			IsResourceAlive (BufferID			id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (ImageID			id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (BufferViewID		id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (ImageViewID		id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (DescriptorSetID	id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (PipelineCacheID	id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (PipelinePackID		id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (RTGeometryID		id)				C_NE_OV	{ return IsAlive( id ); }
		ND_ bool			IsResourceAlive (RTSceneID			id)				C_NE_OV	{ return IsAlive( id ); }
		
		template <usize IS, usize GS, uint UID>
		ND_ auto			AcquireResource (HandleTmpl<IS, GS, UID> id)								__NE___;

		template <typename ID>
		ND_ auto const*		GetResource (ID id, Bool incRef = False{}, Bool quiet = False{})			C_NE___;
		
		template <typename ID>
		ND_ auto const*		GetResource (const Strong<ID> &id, Bool incRef = False{}, Bool quiet = False{}) C_NE___;
		
		template <typename ID>
		ND_ auto*			GetResources (ID id)								C_NE___;
		
		template <typename ID0, typename ID1, typename ...IDs>
		ND_ auto			GetResources (ID0 id0, ID1 id1, IDs ...ids)			C_NE___;
		
		template <typename ID>
		ND_ auto&			GetResourcesOrThrow (ID id)							C_Th___;
		
		template <typename ID0, typename ID1, typename ...IDs>
		ND_ auto			GetResourcesOrThrow (ID0 id0, ID1 id1, IDs ...ids)	C_Th___;

			void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg)	__NE___;
			void  OnEndFrame (FrameUID frameId)									__NE___;

			bool  ForceReleaseResources ()										__NE_OV;
		
		template <usize Size, uint UID, bool Opt, uint Seed>
		ND_ String  HashToName (const NamedID< Size, UID, Opt, Seed > &name)	C_NE___;
		
		#ifdef AE_DBG_OR_DEV
			void  AddHashToName (const PipelineCompiler::HashToName &value)		__NE___;
		#endif

		ND_ Device_t const&			GetDevice ()								C_NE___	{ return _device; }
		ND_ StagingBufferManager_t&	GetStagingManager ()						__NE___	{ return _stagingMngr; }
		ND_ QueryManager_t&			GetQueryManager ()							__NE___	{ return _queryMngr; }

		ND_ StagingBufferStat		GetStagingBufferFrameStat (FrameUID frameId)C_NE_OV	{ return _stagingMngr.GetFrameStat( frameId ); }
		
		AE_GLOBALLY_ALLOC

	private:

	// resource api 
		template <typename ID>
		ND_ int  _ReleaseResource (ID id, uint refCount = 1)					__NE___;

		template <typename ID, typename ...Args>
		ND_ Strong<ID>  _CreateResource (const char* msg, Args&& ...args)		__NE___;
		
		template <typename PplnID>
		ND_ DescSetAndBinding_t  _CreateDescriptorSet (const PplnID &pplnId, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___;

	// resource pool
		ND_ auto&		_GetResourcePool (const BufferID &)						__NE___	{ return _resPool.buffers; }
		ND_ auto&		_GetResourcePool (const ImageID &)						__NE___	{ return _resPool.images; }
		ND_ auto&		_GetResourcePool (const BufferViewID &)					__NE___	{ return _resPool.bufferViews; }
		ND_ auto&		_GetResourcePool (const ImageViewID &)					__NE___	{ return _resPool.imageViews; }
		ND_ auto&		_GetResourcePool (const DescriptorSetLayoutID &)		__NE___	{ return _resPool.dsLayouts; }
		ND_ auto&		_GetResourcePool (const GraphicsPipelineID &)			__NE___	{ return _resPool.graphicsPpln; }
		ND_ auto&		_GetResourcePool (const ComputePipelineID &)			__NE___	{ return _resPool.computePpln; }
		ND_ auto&		_GetResourcePool (const MeshPipelineID &)				__NE___	{ return _resPool.meshPpln; }
		ND_ auto&		_GetResourcePool (const RayTracingPipelineID &)			__NE___	{ return _resPool.raytracePpln; }
		ND_ auto&		_GetResourcePool (const TilePipelineID &)				__NE___	{ return _resPool.tilePpln; }
		ND_ auto&		_GetResourcePool (const DescriptorSetID &)				__NE___	{ return _resPool.descSet; }
		ND_ auto&		_GetResourcePool (const PipelineCacheID &)				__NE___	{ return _resPool.pipelineCache; }
		ND_ auto&		_GetResourcePool (const PipelinePackID &)				__NE___	{ return _resPool.pipelinePacks; }
		ND_ auto&		_GetResourcePool (const RTGeometryID &)					__NE___	{ return _resPool.rtGeom; }
		ND_ auto&		_GetResourcePool (const RTSceneID &)					__NE___	{ return _resPool.rtScene; }
		ND_ auto&		_GetResourcePool (const RTShaderBindingID &)			__NE___	{ return _resPool.rtSBT; }
		ND_ auto&		_GetResourcePool (const PipelineLayoutID_t &)			__NE___	{ return _resPool.pplnLayouts; }
		ND_ auto&		_GetResourcePool (const SamplerID_t &)					__NE___	{ return _resPool.samplers; }
		ND_ auto&		_GetResourcePool (const RenderPassID_t &)				__NE___	{ return _resPool.renderPass; }
		ND_ auto&		_GetResourcePool (const MemoryID_t &)					__NE___	{ return _resPool.memObjs; }
		
	  #ifdef AE_ENABLE_VULKAN
		ND_ auto&		_GetResourcePool (const VFramebufferID &)				__NE___	{ return _resPool.framebuffers; }
	  #endif
		
		template <typename ID>
		ND_ const auto&  _GetResourceCPool (const ID &id)						C_NE___	{ return const_cast< RemoveAllQualifiers<decltype(*this)> &>(*this)._GetResourcePool( id ); }

		ND_ StringView  _GetResourcePoolName (const BufferID &)					__NE___	{ return "buffers"; }
		ND_ StringView  _GetResourcePoolName (const ImageID &)					__NE___	{ return "images"; }
		ND_ StringView  _GetResourcePoolName (const BufferViewID &)				__NE___	{ return "bufferViews"; }
		ND_ StringView  _GetResourcePoolName (const ImageViewID &)				__NE___	{ return "imageViews"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetLayoutID&)		__NE___	{ return "dsLayouts"; }
		ND_ StringView  _GetResourcePoolName (const GraphicsPipelineID &)		__NE___	{ return "graphicsPpln"; }
		ND_ StringView  _GetResourcePoolName (const ComputePipelineID &)		__NE___	{ return "computePpln"; }
		ND_ StringView  _GetResourcePoolName (const MeshPipelineID &)			__NE___	{ return "meshPpln"; }
		ND_ StringView  _GetResourcePoolName (const RayTracingPipelineID &)		__NE___	{ return "raytracePpln"; }
		ND_ StringView  _GetResourcePoolName (const TilePipelineID &)			__NE___	{ return "tilePpln"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetID &)			__NE___	{ return "descSet"; }
		ND_ StringView  _GetResourcePoolName (const PipelineCacheID &)			__NE___	{ return "pipelineCache"; }
		ND_ StringView  _GetResourcePoolName (const PipelinePackID &)			__NE___	{ return "pipelinePacks"; }
		ND_ StringView  _GetResourcePoolName (const RTGeometryID &)				__NE___	{ return "rtGeometry"; }
		ND_ StringView  _GetResourcePoolName (const RTSceneID &)				__NE___	{ return "rtScene"; }
		ND_ StringView  _GetResourcePoolName (const RTShaderBindingID &)		__NE___	{ return "rtSBT"; }
		ND_ StringView	_GetResourcePoolName (const PipelineLayoutID_t &)		__NE___	{ return "pplnLayouts"; }
		ND_ StringView	_GetResourcePoolName (const SamplerID_t &)				__NE___	{ return "samplers"; }
		ND_ StringView	_GetResourcePoolName (const RenderPassID_t &)			__NE___	{ return "renderPass"; }
		ND_ StringView	_GetResourcePoolName (const MemoryID_t &)				__NE___	{ return "memObjs"; }
		
	  #ifdef AE_ENABLE_VULKAN
		ND_ StringView	_GetResourcePoolName (const VFramebufferID &)			__NE___	{ return "framebuffers"; }
	  #endif

	// 
		template <typename ID>	ND_ bool   _Assign (OUT ID &id)					__NE___;
		template <typename ID>		void   _Unassign (ID id)					__NE___;

	// 
		template <typename ID>
		ND_ auto const&			_GetDescription (ID id)							C_NE___;

	// memory managment
		ND_ GfxMemAllocatorPtr		_ChooseMemAllocator (GfxMemAllocatorPtr userDefined)		__NE___;
		ND_ DescriptorAllocatorPtr	_ChooseDescAllocator (DescriptorAllocatorPtr userDefined)	__NE___;

	// default resources
		ND_ bool  _CreateDefaultSampler ()										__NE___;
		
	// empty descriptor set layout
		ND_ bool  _CreateEmptyDescriptorSetLayout ()							__NE___;
		ND_ auto  _GetEmptyDescriptorSetLayout ()								__NE___	{ return *_emptyDSLayout; }
		
//-----------------------------------------------------------------------------

#undef SUFFIX
