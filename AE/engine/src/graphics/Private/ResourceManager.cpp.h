// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX		V

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX		M

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX		R

#else
#	error not implemented
#endif

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# define ERR_MSG( _msg_, _dbgName_ )	_msg_
#elif defined(AE_DEBUG)
# define ERR_MSG( _msg_, _dbgName_ )	(_msg_ ": "s << _dbgName_).c_str()
#else
# define ERR_MSG( _msg_, _dbgName_ )	_msg_
#endif
//-----------------------------------------------------------------------------


namespace {
/*
=================================================
	Replace
----
	destroy previous resource instance and construct new instance
=================================================
*/
	template <typename ResType, typename ID, typename ...Args>
	inline void  Replace (INOUT ResourceBase<ResType,ID> &target, Args&& ...args) __NE___
	{
		Reconstruct<ResType>( INOUT target.Data(), FwdArg<Args>( args )... );
	}

/*
=================================================
	DestroyResources
=================================================
*/
	template <typename PoolT>
	inline void  DestroyResources (ResourceManager* resMngr, INOUT PoolT &pool) __NE___
	{
		pool.UnassignAll( [resMngr] (auto& res) __NE___
			{
				ASSERT( res.IsCreated() );	// all assigned resources should be created

				if_likely( res.IsCreated() )
					res.Destroy( *resMngr );
			});
	}

/*
=================================================
	LogAssignedResourcesAndDestroy
=================================================
*/
	template <typename PoolT>
	inline void  LogAssignedResourcesAndDestroy (ResourceManager* resMngr, INOUT PoolT &pool) __NE___
	{
		pool.UnassignAll( [resMngr] (auto& res) __NE___
			{
				ASSERT( res.IsCreated() );	// all assigned resources should be created

				if_unlikely( res.IsCreated() )
				{
					AE_LOG_DBG( "Resource '"s << res.Data().GetDebugName() << "' is not released" );
					res.Destroy( *resMngr );
				}
			});
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	_ResourceDestructor
=================================================
*/
	struct ResourceManager::_ResourceDestructor
	{
		ResourceManager&	resMngr;

		template <typename T, uint I>
		void operator () () __NE___
		{
			auto	name = resMngr._GetResourcePoolName( T{} );
			Unused( name );

			auto&	pool = resMngr._GetResourcePool( T{} );
			pool.Release( True{"check for assigned"} );
		}
	};

/*
=================================================
	_DestroyResource
=================================================
*/
	template <typename ID>
	void  ResourceManager::_DestroyResource (ID id) __NE___
	{
		auto&	pool = _GetResourcePool( id );

		if_likely( auto* res = pool.At( id.Index() ))
		{
			if_likely( res->GetGeneration() == id.Generation() )
			{
				ASSERT( res->GetRefCount() == 0 );
				ASSERT( res->IsCreated() );

				res->Destroy( *this );
				pool.Unassign( id.Index() );
			}
		}
	}

/*
=================================================
	AddHashToName
=================================================
*/
# if AE_DBG_GRAPHICS
	void  ResourceManager::AddHashToName (const PipelineCompiler::HashToName &value) __NE___
	{
		EXLOCK( _hashToNameGuard );
		NOTHROW( _hashToName.Merge( value ));
	}
# endif

/*
=================================================
	_CreateResource
=================================================
*/
	template <typename ID, typename ...Args>
	forceinline Strong<ID>  ResourceManager::_CreateResource (const char* msg, Args&& ...args) __NE___
	{
		ID	id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( FwdArg<Args>( args )... ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( msg );
		}

		data.AddRef();

		return Strong<ID>{ id };
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ResourceManager::ResourceManager (const Device_t &dev) __NE___ :
		_device{ dev },
		_stagingMngr{ *this },
		_queryMngr{}
	{
		_device.InitFeatureSet( OUT _featureSet );

		StaticAssert( BufferID::MaxIndex()				>= BufferPool_t::capacity() );
		StaticAssert( ImageID::MaxIndex()				>= ImagePool_t::capacity() );
		StaticAssert( MemoryID::MaxIndex()				>= MemObjPool_t::capacity() );
		StaticAssert( BufferViewID::MaxIndex()			>= BufferViewPool_t::capacity() );
		StaticAssert( ImageViewID::MaxIndex()			>= ImageViewPool_t::capacity() );
		StaticAssert( DescriptorSetLayoutID::MaxIndex()	>= DSLayoutPool_t::capacity() );
		StaticAssert( GraphicsPipelineID::MaxIndex()	>= GPipelinePool_t::capacity() );
		StaticAssert( ComputePipelineID::MaxIndex()		>= CPipelinePool_t::capacity() );
		StaticAssert( MeshPipelineID::MaxIndex()		>= MPipelinePool_t::capacity() );
		StaticAssert( RayTracingPipelineID::MaxIndex()	>= RTPipelinePool_t::capacity() );
		StaticAssert( TilePipelineID::MaxIndex()		>= TPipelinePool_t::capacity() );
		StaticAssert( DescriptorSetID::MaxIndex()		>= DescSetPool_t::capacity() );
		StaticAssert( RTGeometryID::MaxIndex()			>= RTGeomPool_t::capacity() );
		StaticAssert( RTSceneID::MaxIndex()				>= RTScenePool_t::capacity() );
		StaticAssert( PipelineLayoutID::MaxIndex()		>= PplnLayoutPool_t::capacity() );
		StaticAssert( RTShaderBindingID::MaxIndex()		>= SBTPool_t::capacity() );
		StaticAssert( SamplerID::MaxIndex()				>= SamplerPool_t::capacity() );
		StaticAssert( RenderPassID::MaxIndex()			>= RenderPassPool_t::capacity() );
		StaticAssert( PipelineCacheID::MaxIndex()		>= PipelineCachePool_t::capacity() );
		StaticAssert( PipelinePackID::MaxIndex()		>= PipelinePackPool_t::capacity() );
		StaticAssert( VideoBufferID::MaxIndex()			>= VideoBufferPool_t::capacity() );
		StaticAssert( VideoImageID::MaxIndex()			>= VideoImagePool_t::capacity() );
		StaticAssert( VideoSessionID::MaxIndex()		>= VideoSessionPool_t::capacity() );
		StaticAssert( IsSameTypes< AllResourceIDs_t::Back::type, MemoryID >);

		#ifdef AE_ENABLE_VULKAN
		StaticAssert( VFramebufferID::MaxIndex()		>= FramebufferPool_t::capacity() );
		#endif

		_InitReleaseResourceByIDFns();
	}

/*
=================================================
	destructor
=================================================
*/
	ResourceManager::~ResourceManager () __NE___
	{
		CHECK( not _largeMemAlloc );
		CHECK( not _defaultMemAlloc );
		CHECK( not _defaultDescAlloc );
	}

/*
=================================================
	OnSurfaceCreated
=================================================
*/
	bool  ResourceManager::OnSurfaceCreated (const Swapchain_t &sw) __NE___
	{
		FeatureSet::SurfaceFormatSet_t	surface_formats;
		CHECK_ERR( sw.GetSurfaceFormats( OUT surface_formats ));
		CHECK_ERR( not surface_formats.None() );

		// initialize
		if ( _featureSet.surfaceFormats.None() )
		{
			_featureSet.surfaceFormats = surface_formats;
			return true;
		}

		CHECK_ERR( _featureSet.surfaceFormats == surface_formats );
		return true;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  ResourceManager::Initialize (const GraphicsCreateInfo &info) __NE___
	{
		CHECK_ERR( _CreateEmptyDescriptorSetLayout() );
		CHECK_ERR( _CreateDefaultSampler() );
		CHECK_ERR( _stagingMngr.Initialize( info ));
		CHECK_ERR( _queryMngr.Initialize( info.maxFrames ));

		_largeMemAlloc		= info.largeGfxAllocator;
		_defaultMemAlloc	= info.defaultGfxAllocator;
		_defaultDescAlloc	= info.defaultDescAllocator;

		if ( not _defaultMemAlloc )
		{
		  #if defined(AE_ENABLE_VULKAN)
			_defaultMemAlloc = MakeRC< VUniMemAllocator >();

		  #elif defined(AE_ENABLE_METAL)
			// keep null

		  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			_defaultMemAlloc = MakeRC< RGfxMemAllocator >();

		  #else
		  #	error not implemented
		  #endif
		}

		if ( not _largeMemAlloc )
		{
		  #if defined(AE_ENABLE_VULKAN)
			_largeMemAlloc = MakeRC< VDedicatedMemAllocator >();

		  #elif defined(AE_ENABLE_METAL)
			// keep null

		  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			_largeMemAlloc = MakeRC< RGfxMemAllocator >();

		  #else
		  #	error not implemented
		  #endif
		}

		if ( not _defaultDescAlloc )
			_defaultDescAlloc = MakeRC< AE_PRIVATE_UNITE_RAW( SUFFIX, DefaultDescriptorAllocator )>();

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ResourceManager::Deinitialize () __NE___
	{
		_stagingMngr.Deinitialize();
		_queryMngr.Deinitialize();

	  #ifdef AE_ENABLE_VULKAN
		DestroyResources( this, INOUT _resPool.framebuffers );
	  #endif
		DEV_CHECK( ImmediatelyRelease2( INOUT _defaultSampler ));
		DEV_CHECK( ImmediatelyRelease2( INOUT _emptyDSLayout ));
		ForceReleaseResources();

		{ auto tmp = _defaultPack.Release();  DEV_CHECK( ImmediatelyRelease2( INOUT tmp )); }
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.pipelinePacks );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.descSet );

		DestroyResources( this, INOUT _resPool.samplers );
		DestroyResources( this, INOUT _resPool.graphicsPpln );
		DestroyResources( this, INOUT _resPool.computePpln );
		DestroyResources( this, INOUT _resPool.meshPpln );
		DestroyResources( this, INOUT _resPool.raytracePpln );
		DestroyResources( this, INOUT _resPool.tilePpln );
		DestroyResources( this, INOUT _resPool.pplnLayouts );
		DestroyResources( this, INOUT _resPool.pipelineCache );
		DestroyResources( this, INOUT _resPool.rtSBT );

	  #ifdef AE_ENABLE_VULKAN
		DestroyResources( this, INOUT _resPool.vbuffers );
		DestroyResources( this, INOUT _resPool.vimages );
		DestroyResources( this, INOUT _resPool.vsessions );
	  #endif

		DestroyResources( this, INOUT _resPool.dsLayouts );

		LogAssignedResourcesAndDestroy( this, INOUT _resPool.bufferViews );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.buffers );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.imageViews );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.images );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.rtGeom );
		LogAssignedResourcesAndDestroy( this, INOUT _resPool.rtScene );

		ForceReleaseResources();

		// memory objects may have multiple indirection
		for (;;)
		{
			LogAssignedResourcesAndDestroy( this, INOUT _resPool.memObjs );
			if ( not ForceReleaseResources() )
				break;
		}

		if ( _defaultMemAlloc )
		{
			CHECK_Eq( _defaultMemAlloc.use_count(), 1 );
			_defaultMemAlloc = null;
		}

		if ( _largeMemAlloc )
		{
			CHECK_Eq( _largeMemAlloc.use_count(), 1 );
			_largeMemAlloc = null;
		}

		if ( _defaultDescAlloc )
		{
			CHECK_Eq( _defaultDescAlloc.use_count(), 1 );
			_defaultDescAlloc = null;
		}

		DestroyResources( this, INOUT _resPool.renderPass );

		AllResourceIDs_t::Visit( _ResourceDestructor{*this} );

		ForceReleaseResources();
	}

/*
=================================================
	_ChooseMemAllocator
=================================================
*/
	GfxMemAllocatorPtr  ResourceManager::_ChooseMemAllocator (GfxMemAllocatorPtr userDefined) __NE___
	{
		if ( userDefined )
			return userDefined;

		return _defaultMemAlloc;
	}

/*
=================================================
	_ChooseDescAllocator
=================================================
*/
	DescriptorAllocatorPtr  ResourceManager::_ChooseDescAllocator (DescriptorAllocatorPtr userDefined) __NE___
	{
		if ( userDefined )
			return userDefined;

		return _defaultDescAlloc;
	}
//-----------------------------------------------------------------------------


#ifndef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
	LoadRenderTech
=================================================
*/
	RenderTechPipelinesPtr  ResourceManager::LoadRenderTech (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->LoadRenderTech( *this, name, cache );
	}

/*
=================================================
	LoadRenderTechAsync
=================================================
*/
	Promise<RenderTechPipelinesPtr>  ResourceManager::LoadRenderTechAsync (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->LoadRenderTechAsync( *this, name, cache );
	}

/*
=================================================
	CreateGraphicsPipeline
=================================================
*/
	Strong<GraphicsPipelineID>  ResourceManager::CreateGraphicsPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const GraphicsPipelineDesc &desc, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

/*
=================================================
	CreateMeshPipeline
=================================================
*/
	Strong<MeshPipelineID>  ResourceManager::CreateMeshPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const MeshPipelineDesc &desc, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

/*
=================================================
	CreateComputePipeline
=================================================
*/
	Strong<ComputePipelineID>  ResourceManager::CreateComputePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const ComputePipelineDesc &desc, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

/*
=================================================
	CreateRayTracingPipeline
=================================================
*/
	Strong<RayTracingPipelineID>  ResourceManager::CreateRayTracingPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const RayTracingPipelineDesc &desc, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

/*
=================================================
	CreateTilePipeline
=================================================
*/
	Strong<TilePipelineID>  ResourceManager::CreateTilePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const TilePipelineDesc &desc, PipelineCacheID cache) __NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

/*
=================================================
	GetSupportedRenderTechs
=================================================
*/
	Array<RenderTechName>  ResourceManager::GetSupportedRenderTechs (PipelinePackID packId) C_NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );
		return pack->GetSupportedRenderTechs();
	}

/*
=================================================
	InitializeResources
=================================================
*/
	bool  ResourceManager::InitializeResources (Strong<PipelinePackID> defaultPackId) __NE___
	{
		CHECK_ERR( defaultPackId );

		auto	old = _defaultPack.Attach( RVRef(defaultPackId) );

		CHECK_MSG( not old, "already initialized" );
		ReleaseResource( old );

		return true;
	}

/*
=================================================
	GetCompatibleRenderPass
=================================================
*/
	RenderPassID  ResourceManager::GetCompatibleRenderPass (PipelinePackID packId, CompatRenderPassName::Ref name) C_NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->GetRenderPass( name );
	}

	RenderPassID  ResourceManager::GetCompatibleRenderPass (PipelinePackID packId, RenderPassName::Ref name) C_NE___
	{
		RenderPassID	rp_id = GetRenderPass( packId, name );
		CHECK_ERR( rp_id );

	  #ifdef AE_ENABLE_VULKAN
		auto*	rp = GetResource( rp_id );
		CHECK_ERR( rp != null );

		const auto	compat_id = rp->CompatibleRP();
		DBG_CHECK_MSG( compat_id, "compatible render pass must not be added to '_renderPassRefs'" );

		return compat_id ? compat_id : rp_id;
	  #else

		return rp_id;
	  #endif
	}

#endif // AE_ENABLE_REMOTE_GRAPHICS

/*
=================================================
	CreatePipelineLayout
=================================================
*/
	Strong<PipelineLayoutID>  ResourceManager::CreatePipelineLayout (const PipelineLayout_t::CreateInfo &ci) __NE___
	{
		PipelineLayoutID	id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( *this, ci ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( "failed when creating pipeline layout: "s DEBUG_ONLY(<< ci.dbgName) );
		}

		data.AddRef();
		return Strong<PipelineLayoutID>{ id };
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	RenderPassID  ResourceManager::GetRenderPass (PipelinePackID packId, RenderPassName::Ref name) C_NE___
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->GetRenderPass( name );
	}

/*
=================================================
	CreateDescriptorSetLayout
=================================================
*/
	Strong<DescriptorSetLayoutID>  ResourceManager::CreateDescriptorSetLayout (const DescriptorSetLayout_t::CreateInfo &ci) __NE___
	{
		DescriptorSetLayoutID	id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( GetDevice(), ci ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( "failed when creating descriptor set layout: "s DEBUG_ONLY(<< ci.dbgName) );
		}

		data.AddRef();
		return Strong<DescriptorSetLayoutID>{ id };
	}
//-----------------------------------------------------------------------------



/*
=================================================
	LoadPipelinePack
=================================================
*/
	Strong<PipelinePackID>  ResourceManager::LoadPipelinePack (const PipelinePackDesc &desc) __NE___
	{
		return _CreateResource<PipelinePackID>( ERR_MSG( "failed when creating pipeline pack", desc.dbgName ), *this, desc );
	}

/*
=================================================
	Create*
=================================================
*/
	Strong<ImageID>  ResourceManager::CreateImage (const ImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<ImageID>( ERR_MSG( "failed when creating image", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<BufferID>  ResourceManager::CreateBuffer (const BufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<BufferID>( ERR_MSG( "failed when creating buffer", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<ImageID>  ResourceManager::CreateImage (const NativeImageDesc_t &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<ImageID>( ERR_MSG( "failed when creating image", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<BufferID>  ResourceManager::CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<BufferID>( ERR_MSG( "failed when creating buffer", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<ImageViewID>  ResourceManager::CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName) __NE___
	{
		return _CreateResource<ImageViewID>( ERR_MSG( "failed when creating image view", dbgName ), *this, desc, image, dbgName );
	}

	Strong<BufferViewID>  ResourceManager::CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName) __NE___
	{
		return _CreateResource<BufferViewID>( ERR_MSG( "failed when creating buffer view", dbgName ), *this, desc, buffer, dbgName );
	}

	Strong<ImageViewID>  ResourceManager::CreateImageView (const NativeImageViewDesc_t &desc, ImageID image, StringView dbgName) __NE___
	{
		return _CreateResource<ImageViewID>( ERR_MSG( "failed when creating image view", dbgName ), *this, desc, image, dbgName );
	}

	Strong<BufferViewID>  ResourceManager::CreateBufferView (const NativeBufferViewDesc_t &desc, BufferID buffer, StringView dbgName) __NE___
	{
		return _CreateResource<BufferViewID>( ERR_MSG( "failed when creating buffer view", dbgName ), *this, desc, buffer, dbgName );
	}

	Strong<RTGeometryID>  ResourceManager::CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<RTGeometryID>( ERR_MSG( "failed when creating ray tracing geometry (BLAS)", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<RTSceneID>  ResourceManager::CreateRTScene (const RTSceneDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<RTSceneID>( ERR_MSG( "failed when creating ray tracing scene (TLAS)", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<VideoSessionID>  ResourceManager::CreateVideoSession (const VideoSessionDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<VideoSessionID>( ERR_MSG( "failed when creating video session", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<VideoBufferID>  ResourceManager::CreateVideoBuffer (const VideoBufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<VideoBufferID>( ERR_MSG( "failed when creating video buffer", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

	Strong<VideoImageID>  ResourceManager::CreateVideoImage (const VideoImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
	{
		return _CreateResource<VideoImageID>( ERR_MSG( "failed when creating video image", dbgName ), *this, desc, RVRef(allocator), dbgName );
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  ResourceManager::IsSupported (EMemoryType memType) C_NE___
	{
		return _device.GetResourceFlags().memTypes.contains( memType );
	}

	bool  ResourceManager::IsSupported (const BufferDesc &desc) C_NE___
	{
		return Buffer_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const ImageDesc &desc) C_NE___
	{
		return Image_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (BufferID buffer, const BufferViewDesc &desc) C_NE___
	{
		auto*	buf = GetResource( buffer );
		CHECK_ERR( buf != null );

		return buf->IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (ImageID image, const ImageViewDesc &desc) C_NE___
	{
		auto*	img = GetResource( image );
		CHECK_ERR( img != null );

		return img->IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const VideoImageDesc &desc) C_NE___
	{
		return VideoImage_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const VideoBufferDesc &desc) C_NE___
	{
		return VideoBuffer_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const VideoSessionDesc &desc) C_NE___
	{
		return VideoSession_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const RTGeometryDesc &desc) C_NE___
	{
		return RTGeometry_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const RTGeometryBuild &build) C_NE___
	{
		return RTGeometry_t::IsSupported( *this, build );
	}

	bool  ResourceManager::IsSupported (const RTSceneDesc &desc) C_NE___
	{
		return RTScene_t::IsSupported( *this, desc );
	}

	bool  ResourceManager::IsSupported (const RTSceneBuild &build) C_NE___
	{
		return RTScene_t::IsSupported( *this, build );
	}

/*
=================================================
	GetRTGeometrySizes / GetRTSceneSizes
=================================================
*/
	RTASBuildSizes  ResourceManager::GetRTGeometrySizes (const RTGeometryBuild &desc) __NE___
	{
		return RTGeometry_t::GetBuildSizes( *this, desc );
	}

	RTASBuildSizes  ResourceManager::GetRTSceneSizes (const RTSceneBuild &desc) __NE___
	{
		return RTScene_t::GetBuildSizes( *this, desc );
	}

/*
=================================================
	GetShaderGroupStackSize
=================================================
*/
	Bytes  ResourceManager::GetShaderGroupStackSize (RayTracingPipelineID pplnId, ArrayView<RayTracingGroupName> names, ERTShaderGroup type) __NE___
	{
		auto*	ppln = GetResource( pplnId );
		CHECK_ERR( ppln != null );
		return ppln->GetShaderGroupStackSize( _device, names, type );
	}

/*
=================================================
	GetDeviceAddress
=================================================
*/
	DeviceAddress  ResourceManager::GetDeviceAddress (BufferID id) C_NE___
	{
		auto*	res = GetResource( id, False{"don't inc ref"}, True{"quiet"} );
		return res != null ? res->GetDeviceAddress() : Default;
	}

	DeviceAddress  ResourceManager::GetDeviceAddress (RTGeometryID id) C_NE___
	{
		auto*	res = GetResource( id, False{"don't inc ref"}, True{"quiet"} );
		return res != null ? res->GetDeviceAddress() : Default;
	}
//-----------------------------------------------------------------------------



#ifndef AE_ENABLE_REMOTE_GRAPHICS
/*
=================================================
	_CreateDescriptorSets
=================================================
*/
	template <typename PplnID>
	bool  ResourceManager::_CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, const usize count,
												  PplnID pplnId, DescriptorSetName::Ref dsName,
												  DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( dst != null and count > 0 );

		auto*	ppln = GetResource( pplnId );
		CHECK_ERR( ppln != null );

		auto*	ppln_layout = GetResource( ppln->LayoutId() );
		CHECK_ERR( ppln_layout != null );

		DescriptorSetLayoutID	layout_id;

		if_unlikely( not ppln_layout->GetDescriptorSetLayout( dsName, OUT layout_id, OUT binding ))
		{
			#if AE_DBG_GRAPHICS
				String	str;
				str << "Failed to find descriptor set '" << HashToName( dsName ) << "' in pipeline layout.\n"
					<< "Available sets: ";

				for (auto [ds_name, ds_layout] : ppln_layout->GetDescriptorSets()) {
					str << "'" << HashToName( ds_name ) << "', ";
				}
				str.pop_back();
				str.pop_back();

				AE_LOG_DBG( str );
			#endif
			return false;
		}

	  #if AE_DBG_GRAPHICS and (not AE_OPTIMIZE_IDS)
		String	dbg_name;
		if ( dbgName.empty() )
		{
			dbg_name	= HashToName( dsName );
			dbgName		= dbg_name;
		}
	  #endif

		allocator = _ChooseDescAllocator( RVRef(allocator) );

		usize	i		= 0;
		bool	created	= true;

		for (; created and (i < count); ++i)
		{
			dst[i]  = _CreateResource<DescriptorSetID>( ERR_MSG( "failed when creating descriptor set", dbgName ), *this, layout_id, allocator, dbgName );
			created = (dst[i].IsValid());
		}

		if ( not created ) {
			for (usize j = 0; j < i; ++j) {
				ImmediatelyRelease( INOUT dst[j] );
			}
		}

		return created;
	}
#endif // AE_ENABLE_REMOTE_GRAPHICS

/*
=================================================
	_GetPushConstantIndex
=================================================
*/
	template <typename PplnID>
	PushConstantIndex  ResourceManager::_GetPushConstantIndex (PplnID pplnId, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		auto*	ppln = GetResource( pplnId );
		CHECK_ERR( ppln != null );

		auto*	layout = GetResource( ppln->LayoutId() );
		CHECK_ERR( layout != null );

		auto&	pc = layout->GetPushConstants();
		auto	it = pc.find( pcName );
		CHECK_ERR( it != pc.end() );
		CHECK_ERR( typeName == it->second.typeName );
		CHECK_ERR( dataSize == Bytes{it->second.size} );

		#if defined(AE_ENABLE_VULKAN)
			return PushConstantIndex{ it->second.vulkanOffset, it->second.stage, it->second.typeName, it->second.size };

		#elif defined(AE_ENABLE_METAL)
			return PushConstantIndex{ it->second.metalBufferId, it->second.stage, it->second.typeName, it->second.size };

		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			return it->second.idx;

		#else
		#	error not implemented
		#endif
	}

/*
=================================================
	CreateDescriptorSets
=================================================
*/
	bool  ResourceManager::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, GraphicsPipelineID ppln,
												 DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
	}

	bool  ResourceManager::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, MeshPipelineID ppln,
												 DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
	}

	bool  ResourceManager::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, ComputePipelineID ppln,
												 DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
	}

	bool  ResourceManager::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, RayTracingPipelineID ppln,
												 DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
	}

	bool  ResourceManager::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, TilePipelineID ppln,
												 DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
	}

/*
=================================================
	GetPushConstantIndex
=================================================
*/
	PushConstantIndex  ResourceManager::GetPushConstantIndex (GraphicsPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
	}

	PushConstantIndex  ResourceManager::GetPushConstantIndex (MeshPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
	}

	PushConstantIndex  ResourceManager::GetPushConstantIndex (ComputePipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
	}

	PushConstantIndex  ResourceManager::GetPushConstantIndex (RayTracingPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
	}

	PushConstantIndex  ResourceManager::GetPushConstantIndex (TilePipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
	{
		return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	CreatePipelineCache
=================================================
*/
	Strong<PipelineCacheID>  ResourceManager::CreatePipelineCache (RC<RStream> stream, StringView dbgName) __NE___
	{
		PipelineCacheID		id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( *this, dbgName, RVRef(stream) ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( "failed when creating pipeline cache" );
		}

		data.AddRef();
		return Strong<PipelineCacheID>{ id };
	}

/*
=================================================
	SerializePipelineCache
=================================================
*/
	bool  ResourceManager::SerializePipelineCache (PipelineCacheID id, RC<WStream> dst) C_NE___
	{
		CHECK_ERR( dst and dst->IsOpen() );

		auto*	res = GetResources( id );
		CHECK_ERR( res != null );

		Array<char>	data;
		CHECK_ERR( res->GetData( GetDevice(), OUT data ));

		CHECK_ERR( dst->Write( ArrayView<char>{ data }));
		return true;
	}

/*
=================================================
	_CreateEmptyDescriptorSetLayout
=================================================
*/
	bool  ResourceManager::_CreateEmptyDescriptorSetLayout () __NE___
	{
		DescriptorSetLayoutID	id;
		CHECK_ERR( _Assign( OUT id ));

		auto&	data = _GetResourcePool( id )[ id.Index() ];
		Replace( data );

		if_unlikely( not data.Create( GetDevice(), "EmptyDSLayout" ))
		{
			data.Destroy( *this );
			_Unassign( id );
			RETURN_ERR( "failed when creating empty descriptor set layout" );
		}

		data.AddRef();

		_emptyDSLayout.Attach( id );
		return true;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	CreatePipeline
=================================================
*/
	Strong<ComputePipelineID>  ResourceManager::CreatePipeline (const ComputePipeline_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<ComputePipelineID>( ERR_MSG( "failed when creating compute pipeline", ci.specCI.dbgName ), *this, ci );
	}

	Strong<GraphicsPipelineID>  ResourceManager::CreatePipeline (const GraphicsPipeline_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<GraphicsPipelineID>( ERR_MSG( "failed when creating graphics pipeline", ci.specCI.dbgName ), *this, ci );
	}

	Strong<MeshPipelineID>  ResourceManager::CreatePipeline (const MeshPipeline_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<MeshPipelineID>( ERR_MSG( "failed when creating mesh pipeline", ci.specCI.dbgName ), *this, ci );
	}

	Strong<RayTracingPipelineID>  ResourceManager::CreatePipeline (const RayTracingPipeline_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<RayTracingPipelineID>( ERR_MSG( "failed when creating ray tracing pipeline", ci.specCI.dbgName ), *this, ci );
	}

	Strong<TilePipelineID>  ResourceManager::CreatePipeline (const TilePipeline_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<TilePipelineID>( ERR_MSG( "failed when creating tile pipeline", ci.specCI.dbgName ), *this, ci );
	}

/*
=================================================
	CreateRTShaderBinding
=================================================
*/
	Strong<RTShaderBindingID>  ResourceManager::CreateRTShaderBinding (const ShaderBindingTable_t::CreateInfo &ci) __NE___
	{
		return _CreateResource<RTShaderBindingID>( ERR_MSG( "failed when creating RT shader binding table", ci.dbgName ), *this, ci );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_ResourcePrinter
=================================================
*/
#if AE_DBG_GRAPHICS
	struct ResourceManager::_ResourcePrinter
	{
		ResourceManager&	resMngr;
		String&		log;

		template <typename T, uint I>
		void operator () () __NE___
		{
			auto	name = resMngr._GetResourcePoolName( T{} );
			log << "\nPool: " << name;

			auto&	pool = resMngr._GetResourcePool( T{} );
			pool.ForEachAssigned( [this] (auto& res) __NE___ { log << "\n  '" << res.Data().GetDebugName() << "', rc: " << ToString(res.GetRefCount()); } );
		}
	};
#endif

/*
=================================================
	PrintAllResources
=================================================
*/
	void  ResourceManager::PrintAllResources () __NE___
	{
	#if AE_DBG_GRAPHICS
		TRY{
			String	log;
			AllResourceIDs_t::Visit( _ResourcePrinter{ *this, log });
			AE_LOGI( log );
		}
		CATCH_ALL();
	#endif
	}
//-----------------------------------------------------------------------------


/*
=================================================
	GetSampler
=================================================
*/
	SamplerID  ResourceManager::GetSampler (PipelinePackID packId, SamplerName::Ref name) C_NE___
	{
		auto*		pack = GetResource( packId ? packId : _defaultPack.Get() );
		SamplerID	id	 = _defaultSampler;

		if_likely( pack != null )
			id = pack->GetSampler( name );

		return id;
	}

/*
=================================================
	_CreateDefaultSampler
=================================================
*/
	bool  ResourceManager::_CreateDefaultSampler () __NE___
	{
	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		return true;

	  #else
		CHECK_ERR( _defaultSampler == Default );

		SamplerDesc		info;
		_defaultSampler = CreateSampler( info, "Default" );
		CHECK_ERR( _defaultSampler );

		return true;
	  #endif
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_OnBeginFrame
=================================================
*/
	void  ResourceManager::_OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg) __NE___
	{
	  #ifdef AE_ENABLE_VULKAN
		_expiredResources._currentFrameId.store( frameId );
		{
			auto&	list = _expiredResources.Get( frameId );
			EXLOCK( list.guard );
			ASSERT( list.resources.empty() );
			list.frameId = frameId;
		}
	  #endif

		GetStagingManager().OnBeginFrame( frameId, cfg );
		GetQueryManager().NextFrame( frameId );
	}

/*
=================================================
	_OnEndFrame
=================================================
*/
	void  ResourceManager::_OnEndFrame (FrameUID frameId) __NE___
	{
		GetStagingManager().OnEndFrame( frameId );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_InitReleaseResourceByID
=================================================
*/
	struct ResourceManager::_InitReleaseResourceByID
	{
		ResourceManager&	resMngr;

		template <typename ID>
		static void  _Release (ResourceManager &resMngr, ExpiredResource::IDValue_t id) __NE___
		{
			resMngr._DestroyResource( BitCastRlx<ID>( id ));
		}

		template <typename ID, uint I>
		void operator () () __NE___
		{
			resMngr._releaseResIDs[I] = &_Release<ID>;
		}
	};

/*
=================================================
	_InitReleaseResourceByIDFns
=================================================
*/
	void  ResourceManager::_InitReleaseResourceByIDFns () __NE___
	{
		ExpResourceTypes_t::Visit( _InitReleaseResourceByID{*this} );
	}

/*
=================================================
	ForceReleaseResources
=================================================
*/
	bool  ResourceManager::ForceReleaseResources () __NE___
	{
		bool	non_empty = false;

		for (auto& list : _expiredResources.All())
		{
			EXLOCK( list.guard );
			list.frameId = Default;

			non_empty |= not list.resources.empty();

			for (auto& res : list.resources)
			{
				_releaseResIDs[ res.type ]( *this, res.id );
			}
			list.resources.clear();

			// TODO: trim memory
		}

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		non_empty |= _ForceReleaseResources();
	  #endif

		return non_empty;
	}

/*
=================================================
	ExpiredResources ctor
=================================================
*/
	ResourceManager::ExpiredResources::ExpiredResources () __Th___
	{
		for (auto& item : _list) {
			item.resources.reserve( 1u << 10 );
		}
	}

/*
=================================================
	ReleaseExpiredResourcesTask::Run
=================================================
*/
	void  ResourceManager::ReleaseExpiredResourcesTask::Run ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		auto&	list	 = res_mngr._expiredResources.Get( _frameId );
		EXLOCK( list.guard );

		for (auto& res : list.resources)
		{
			res_mngr._releaseResIDs[ res.type ]( res_mngr, res.id );
		}
		list.resources.clear();
	}
//-----------------------------------------------------------------------------
