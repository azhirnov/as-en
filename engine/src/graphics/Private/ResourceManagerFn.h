
	public:

		ND_ bool				OnSurfaceCreated (const VSwapchain &sw)						__NE___;
		
		bool					InitializeResources (const PipelinePackDesc &desc)			__NE_OV;
		Strong<PipelinePackID>	LoadPipelinePack (const PipelinePackDesc &desc)				__NE_OV;
		Array<RenderTechName>	GetSupportedRenderTechs (PipelinePackID id)					C_NE_OV;

		bool					IsSupported (EMemoryType memType)							C_NE_OV;
		bool					IsSupported (const BufferDesc &desc)						C_NE_OV;
		bool					IsSupported (const ImageDesc &desc)							C_NE_OV;
		bool					IsSupported (BufferID buffer, const BufferViewDesc &desc)	C_NE_OV;
		bool					IsSupported (ImageID image, const ImageViewDesc &desc)		C_NE_OV;

		Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE_OV;
		Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE_OV;
		
		Strong<ImageID>			CreateImage (const NativeImageDesc_t &desc, StringView dbgName = Default)										__NE_OV;
		Strong<BufferID>		CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName = Default)										__NE_OV;
		
		Strong<ImageViewID>		CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)						__NE_OV;
		Strong<BufferViewID>	CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)					__NE_OV;
		
		Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE_OV;
		Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE_OV;
		
		RTASBuildSizes			GetRTGeometrySizes (const RTGeometryBuild &desc)																__NE_OV;
		RTASBuildSizes			GetRTSceneSizes (const RTSceneBuild &desc)																		__NE_OV;

		DescSetAndBinding_t		CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		DescSetAndBinding_t		CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		DescSetAndBinding_t		CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		DescSetAndBinding_t		CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		DescSetAndBinding_t		CreateDescriptorSet (TilePipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) __NE_OV;
		
		Strong<DescriptorSetID>	CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)			__NE_OV;
		Strong<DescriptorSetID>	CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)								__NE_OV;

		NativeBuffer_t			GetBufferHandle (BufferID id)			C_NE_OV;
		NativeImage_t			GetImageHandle (ImageID id)				C_NE_OV;
		NativeBufferView_t		GetBufferViewHandle (BufferViewID id)	C_NE_OV;
		NativeImageView_t		GetImageViewHandle (ImageViewID id)		C_NE_OV;

		Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	__NE_OV;
		RenderTechPipelinesPtr			LoadRenderTech      (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	__NE_OV;

		Strong<GraphicsPipelineID>		CreateGraphicsPipeline   (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache = Default)	__NE_OV;
		Strong<MeshPipelineID>			CreateMeshPipeline       (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache = Default)	__NE_OV;
		Strong<ComputePipelineID>		CreateComputePipeline    (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache = Default)	__NE_OV;
		Strong<RayTracingPipelineID>	CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache = Default)	__NE_OV;
		Strong<TilePipelineID>			CreateTilePipeline       (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache = Default)	__NE_OV;

		Strong<PipelineCacheID>			CreatePipelineCache ()				__NE_OV;
		
		bool	GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)	C_NE_OV;
		bool	GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)	C_NE_OV;

		bool	ReleaseResource (Strong<ImageID>			&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferID>			&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ImageViewID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferViewID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelineCacheID>	&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelinePackID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<GraphicsPipelineID>	&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<MeshPipelineID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ComputePipelineID>	&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RayTracingPipelineID>&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<TilePipelineID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<DescriptorSetID>	&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTGeometryID>		&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTSceneID>			&id)		__NE_OV	{ return _ReleaseResource( id.Release() ) == 0; }
		
		template <usize IS, usize GS, uint UID>
		bool	ReleaseResource (Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)	__NE___	{ return _ReleaseResource( id.Release(), refCount ) == 0; }
		
		template <typename Arg0, typename ...Args>
		bool	ReleaseResources (Arg0 &arg0, Args& ...args)			__NE___;

		BufferDesc const&		GetDescription (BufferID id)			C_NE_OV;
		ImageDesc const&		GetDescription (ImageID id)				C_NE_OV;
		BufferViewDesc const&	GetDescription (BufferViewID id)		C_NE_OV;
		ImageViewDesc const&	GetDescription (ImageViewID id)			C_NE_OV;

		ND_ FeatureSet const&	GetFeatureSet ()						C_NE___	{ return _featureSet; }
		ND_ PipelinePackID		GetDefaultPack ()						C_NE___	{ return _defaultPack; }

		template <usize IS, usize GS, uint UID>
		ND_ bool			IsAlive (HandleTmpl<IS,GS,UID> id)			C_NE___;
		ND_ bool			IsAlive (const SamplerName &name)			C_NE___;

		bool				IsResourceAlive (BufferID			id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageID			id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (BufferViewID		id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageViewID		id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (DescriptorSetID	id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelineCacheID	id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelinePackID		id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (RTGeometryID		id)		C_NE_OV	{ return IsAlive( id ); }
		bool				IsResourceAlive (RTSceneID			id)		C_NE_OV	{ return IsAlive( id ); }
		
		template <usize IS, usize GS, uint UID>
		ND_ auto			AcquireResource (HandleTmpl<IS, GS, UID> id)								__NE___;

		template <typename ID>
		ND_ auto const*		GetResource (ID id, Bool incRef = false, Bool quiet = false)				C_NE___;
		
		template <typename ID>
		ND_ auto const*		GetResource (const Strong<ID> &id, Bool incRef = false, Bool quiet = false) C_NE___;

		void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg)		__NE___;
		void  OnEndFrame (FrameUID frameId)										__NE___;

		bool  ForceReleaseResources ()											__NE_OV;
		
		template <usize Size, uint UID, bool Opt, uint Seed>
		ND_ String  HashToName (const NamedID< Size, UID, Opt, Seed > &name)	C_NE___;
		
		#ifdef AE_DBG_OR_DEV
		void  AddHashToName (const PipelineCompiler::HashToName &value)			__NE___;
		#endif

		StagingBufferStat  GetStagingBufferFrameStat (FrameUID frameId)			C_NE_OV	{ return _stagingMngr.GetFrameStat( frameId ); }


	private:

	// resource api 
		template <typename ID>
		ND_ int  _ReleaseResource (ID id, uint refCount = 1)				__NE___;

		template <typename ID, typename ...Args>
		ND_ Strong<ID>  _CreateResource (const char* msg, Args&& ...args)	__NE___;

	// resource pool
		ND_ auto&  _GetResourcePool (const BufferID &)						__NE___	{ return _resPool.buffers; }
		ND_ auto&  _GetResourcePool (const ImageID &)						__NE___	{ return _resPool.images; }
		ND_ auto&  _GetResourcePool (const BufferViewID &)					__NE___	{ return _resPool.bufferViews; }
		ND_ auto&  _GetResourcePool (const ImageViewID &)					__NE___	{ return _resPool.imageViews; }
		ND_ auto&  _GetResourcePool (const DescriptorSetLayoutID &)			__NE___	{ return _resPool.dsLayouts; }
		ND_ auto&  _GetResourcePool (const GraphicsPipelineID &)			__NE___	{ return _resPool.graphicsPpln; }
		ND_ auto&  _GetResourcePool (const ComputePipelineID &)				__NE___	{ return _resPool.computePpln; }
		ND_ auto&  _GetResourcePool (const MeshPipelineID &)				__NE___	{ return _resPool.meshPpln; }
		ND_ auto&  _GetResourcePool (const RayTracingPipelineID &)			__NE___	{ return _resPool.raytracePpln; }
		ND_ auto&  _GetResourcePool (const TilePipelineID &)				__NE___	{ return _resPool.tilePpln; }
		ND_ auto&  _GetResourcePool (const DescriptorSetID &)				__NE___	{ return _resPool.descSet; }
		ND_ auto&  _GetResourcePool (const PipelineCacheID &)				__NE___	{ return _resPool.pipelineCache; }
		ND_ auto&  _GetResourcePool (const PipelinePackID &)				__NE___	{ return _resPool.pipelinePacks; }
		ND_ auto&  _GetResourcePool (const RTGeometryID &)					__NE___	{ return _resPool.rtGeom; }
		ND_ auto&  _GetResourcePool (const RTSceneID &)						__NE___	{ return _resPool.rtScene; }
		
		template <typename ID>
		ND_ const auto&  _GetResourceCPool (const ID &id)					C_NE___	{ return const_cast<VResourceManager *>(this)->_GetResourcePool( id ); }

		ND_ StringView  _GetResourcePoolName (const BufferID &)				__NE___	{ return "buffers"; }
		ND_ StringView  _GetResourcePoolName (const ImageID &)				__NE___	{ return "images"; }
		ND_ StringView  _GetResourcePoolName (const BufferViewID &)			__NE___	{ return "bufferViews"; }
		ND_ StringView  _GetResourcePoolName (const ImageViewID &)			__NE___	{ return "imageViews"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetLayoutID&)	__NE___	{ return "dsLayouts"; }
		ND_ StringView  _GetResourcePoolName (const GraphicsPipelineID &)	__NE___	{ return "graphicsPpln"; }
		ND_ StringView  _GetResourcePoolName (const ComputePipelineID &)	__NE___	{ return "computePpln"; }
		ND_ StringView  _GetResourcePoolName (const MeshPipelineID &)		__NE___	{ return "meshPpln"; }
		ND_ StringView  _GetResourcePoolName (const RayTracingPipelineID &)	__NE___	{ return "raytracePpln"; }
		ND_ StringView  _GetResourcePoolName (const TilePipelineID &)		__NE___	{ return "tilePpln"; }
		ND_ StringView  _GetResourcePoolName (const VFramebufferID &)		__NE___	{ return "framebuffers"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetID &)		__NE___	{ return "descSet"; }
		ND_ StringView  _GetResourcePoolName (const PipelineCacheID &)		__NE___	{ return "pipelineCache"; }
		ND_ StringView  _GetResourcePoolName (const PipelinePackID &)		__NE___	{ return "pipelinePacks"; }
		ND_ StringView  _GetResourcePoolName (const RTGeometryID &)			__NE___	{ return "rtGeometry"; }
		ND_ StringView  _GetResourcePoolName (const RTSceneID &)			__NE___	{ return "rtScene"; }

	// 
		template <typename ID>	ND_ bool   _Assign (OUT ID &id)				__NE___;
		template <typename ID>		void   _Unassign (ID id)				__NE___;

	// 
		template <typename ID>
		ND_ auto const&			_GetDescription (ID id)						C_NE___;

