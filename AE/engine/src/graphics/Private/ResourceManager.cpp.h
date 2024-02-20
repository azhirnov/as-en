// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define SUFFIX       V

#elif defined(AE_ENABLE_METAL)
#   define SUFFIX       M

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   define SUFFIX       R

#else
#   error not implemented
#endif

#define RESMNGR     AE_PRIVATE_UNITE_RAW( SUFFIX, ResourceManager )
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
    inline void  DestroyResources (RESMNGR* resMngr, INOUT PoolT &pool) __NE___
    {
        pool.UnassignAll( [resMngr] (auto& res) __NE___
            {
                ASSERT( res.IsCreated() );  // all assigned resources should be created

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
    inline void  LogAssignedResourcesAndDestroy (RESMNGR* resMngr, INOUT PoolT &pool) __NE___
    {
        pool.UnassignAll( [resMngr] (auto& res) __NE___
            {
                ASSERT( res.IsCreated() );  // all assigned resources should be created

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
    struct RESMNGR::_ResourceDestructor
    {
        RESMNGR&    resMngr;

        template <typename T, uint I>
        void operator () () __NE___
        {
            auto    name = resMngr._GetResourcePoolName( T{} );
            Unused( name );

            auto&   pool = resMngr._GetResourcePool( T{} );
            pool.Release( True{"check for assigned"} );
        }
    };

/*
=================================================
    _DestroyResource
=================================================
*/
    template <typename ID>
    void  RESMNGR::_DestroyResource (ID id) __NE___
    {
        auto&   pool = _GetResourcePool( id );

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
# ifdef AE_DEBUG
    void  RESMNGR::AddHashToName (const PipelineCompiler::HashToName &value) __NE___
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
    Strong<ID>  RESMNGR::_CreateResource (const char* msg, Args&& ...args) __NE___
    {
        ID  id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
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

/*
=================================================
    IsAlive
=================================================
*
    bool  RESMNGR::IsAlive (SamplerName::Ref name) const __NE___
    {
        auto    id = GetSampler( name );

        if ( id == Default )
            return false;

        return IsAlive( id );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    RESMNGR::RESMNGR (const Device_t &dev) __NE___ :
        _device{ dev },
        _stagingMngr{ *this },
        _queryMngr{}
    {
        _device.InitFeatureSet( OUT _featureSet );

        StaticAssert( BufferID::MaxIndex()              >= BufferPool_t::capacity() );
        StaticAssert( ImageID::MaxIndex()               >= ImagePool_t::capacity() );
        StaticAssert( MemoryID::MaxIndex()              >= MemObjPool_t::capacity() );
        StaticAssert( BufferViewID::MaxIndex()          >= BufferViewPool_t::capacity() );
        StaticAssert( ImageViewID::MaxIndex()           >= ImageViewPool_t::capacity() );
        StaticAssert( DescriptorSetLayoutID::MaxIndex() >= DSLayoutPool_t::capacity() );
        StaticAssert( GraphicsPipelineID::MaxIndex()    >= GPipelinePool_t::capacity() );
        StaticAssert( ComputePipelineID::MaxIndex()     >= CPipelinePool_t::capacity() );
        StaticAssert( MeshPipelineID::MaxIndex()        >= MPipelinePool_t::capacity() );
        StaticAssert( RayTracingPipelineID::MaxIndex()  >= RTPipelinePool_t::capacity() );
        StaticAssert( TilePipelineID::MaxIndex()        >= TPipelinePool_t::capacity() );
        StaticAssert( DescriptorSetID::MaxIndex()       >= DescSetPool_t::capacity() );
        StaticAssert( RTGeometryID::MaxIndex()          >= RTGeomPool_t::capacity() );
        StaticAssert( RTSceneID::MaxIndex()             >= RTScenePool_t::capacity() );
        StaticAssert( PipelineLayoutID::MaxIndex()      >= PplnLayoutPool_t::capacity() );
        StaticAssert( RTShaderBindingID::MaxIndex()     >= SBTPool_t::capacity() );
        StaticAssert( SamplerID::MaxIndex()             >= SamplerPool_t::capacity() );
        StaticAssert( RenderPassID::MaxIndex()          >= RenderPassPool_t::capacity() );
        StaticAssert( PipelineCacheID::MaxIndex()       >= PipelineCachePool_t::capacity() );
        StaticAssert( PipelinePackID::MaxIndex()        >= PipelinePackPool_t::capacity() );
        StaticAssert( IsSameTypes< AllResourceIDs_t::Back::type, MemoryID >);

        #ifdef AE_ENABLE_VULKAN
        StaticAssert( VideoBufferID::MaxIndex()     >= VideoBufferPool_t::capacity() );
        StaticAssert( VideoImageID::MaxIndex()          >= VideoImagePool_t::capacity() );
        StaticAssert( VideoSessionID::MaxIndex()        >= VideoSessionPool_t::capacity() );
        StaticAssert( VFramebufferID::MaxIndex()        >= FramebufferPool_t::capacity() );

        _InitReleaseResourceByIDFns();
        #endif
    }

/*
=================================================
    destructor
=================================================
*/
    RESMNGR::~RESMNGR () __NE___
    {
        CHECK( not _defaultMemAlloc );
        CHECK( not _defaultDescAlloc );
    }

/*
=================================================
    OnSurfaceCreated
=================================================
*/
    bool  RESMNGR::OnSurfaceCreated (const Swapchain_t &sw) __NE___
    {
        FeatureSet::SurfaceFormatSet_t  surface_formats;
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
    bool  RESMNGR::Initialize (const GraphicsCreateInfo &info) __NE___
    {
        CHECK_ERR( _CreateEmptyDescriptorSetLayout() );
        CHECK_ERR( _CreateDefaultSampler() );
        CHECK_ERR( _stagingMngr.Initialize( info ));
        CHECK_ERR( _queryMngr.Initialize( info.maxFrames ));

        _defaultMemAlloc    = info.defaultGfxAllocator;
        _defaultDescAlloc   = info.defaultDescAllocator;

      #ifdef AE_ENABLE_VULKAN
        if ( not _defaultMemAlloc )
            _defaultMemAlloc = MakeRC< VUniMemAllocator >();
      #endif

        if ( not _defaultDescAlloc )
            _defaultDescAlloc = MakeRC< AE_PRIVATE_UNITE_RAW( SUFFIX, DefaultDescriptorAllocator )>();

        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  RESMNGR::Deinitialize () __NE___
    {
        _stagingMngr.Deinitialize();
        _queryMngr.Deinitialize();

      #ifdef AE_ENABLE_VULKAN
        DestroyResources( this, INOUT _resPool.framebuffers );
      #endif
        ImmediatelyRelease( _defaultSampler );
        ImmediatelyRelease( _emptyDSLayout );
        ForceReleaseResources();

        { auto tmp = _defaultPack.Release();  ImmediatelyRelease( tmp ); }
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
    GfxMemAllocatorPtr  RESMNGR::_ChooseMemAllocator (GfxMemAllocatorPtr userDefined) __NE___
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
    DescriptorAllocatorPtr  RESMNGR::_ChooseDescAllocator (DescriptorAllocatorPtr userDefined) __NE___
    {
        if ( userDefined )
            return userDefined;

        return _defaultDescAlloc;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    LoadRenderTech
=================================================
*/
    RenderTechPipelinesPtr  RESMNGR::LoadRenderTech (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->LoadRenderTech( *this, name, cache );
    }

/*
=================================================
    LoadRenderTechAsync
=================================================
*/
    Promise<RenderTechPipelinesPtr>  RESMNGR::LoadRenderTechAsync (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->LoadRenderTechAsync( *this, name, cache );
    }

/*
=================================================
    CreateGraphicsPipeline
=================================================
*/
    Strong<GraphicsPipelineID>  RESMNGR::CreateGraphicsPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const GraphicsPipelineDesc &desc, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->CreatePipeline( *this, name, desc, cache );
    }

/*
=================================================
    CreateMeshPipeline
=================================================
*/
    Strong<MeshPipelineID>  RESMNGR::CreateMeshPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const MeshPipelineDesc &desc, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->CreatePipeline( *this, name, desc, cache );
    }

/*
=================================================
    CreateComputePipeline
=================================================
*/
    Strong<ComputePipelineID>  RESMNGR::CreateComputePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const ComputePipelineDesc &desc, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->CreatePipeline( *this, name, desc, cache );
    }

/*
=================================================
    CreateRayTracingPipeline
=================================================
*/
    Strong<RayTracingPipelineID>  RESMNGR::CreateRayTracingPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const RayTracingPipelineDesc &desc, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->CreatePipeline( *this, name, desc, cache );
    }

/*
=================================================
    CreateTilePipeline
=================================================
*/
    Strong<TilePipelineID>  RESMNGR::CreateTilePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const TilePipelineDesc &desc, PipelineCacheID cache) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->CreatePipeline( *this, name, desc, cache );
    }

/*
=================================================
    CreateDescriptorSetLayout
=================================================
*/
    Strong<DescriptorSetLayoutID>  RESMNGR::CreateDescriptorSetLayout (const DescriptorSetLayout_t::CreateInfo &ci) __NE___
    {
        DescriptorSetLayoutID   id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
        Replace( data );

        if_unlikely( not data.Create( GetDevice(), ci ))
        {
            data.Destroy( *this );
            _Unassign( id );
            RETURN_ERR( "failed when creating descriptor set layout" );
        }

        data.AddRef();
        return Strong<DescriptorSetLayoutID>{ id };
    }

/*
=================================================
    CreatePipelineLayout
=================================================
*/
    Strong<PipelineLayoutID>  RESMNGR::CreatePipelineLayout (const PipelineLayout_t::DescriptorSets_t &descSetLayouts,
                                                             const PipelineLayout_t::PushConstants_t &pushConstants,
                                                             StringView dbgName) __NE___
    {
        PipelineLayoutID    id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
        Replace( data );

        auto*   empty_ds = GetResource( _emptyDSLayout );
        CHECK_ERR( empty_ds );

        if_unlikely( not data.Create( *this, descSetLayouts, pushConstants, empty_ds->Handle(), dbgName ))
        {
            data.Destroy( *this );
            _Unassign( id );
            RETURN_ERR( "failed when creating pipeline layout" );
        }

        data.AddRef();
        return Strong<PipelineLayoutID>{ id };
    }

/*
=================================================
    GetSupportedRenderTechs
=================================================
*/
    Array<RenderTechName>  RESMNGR::GetSupportedRenderTechs (PipelinePackID packId) C_NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );
        return pack->GetSupportedRenderTechs();
    }

/*
=================================================
    LoadPipelinePack
=================================================
*/
    Strong<PipelinePackID>  RESMNGR::LoadPipelinePack (const PipelinePackDesc &desc) __NE___
    {
        return _CreateResource<PipelinePackID>( "failed when creating pipeline pack", *this, desc );
    }

/*
=================================================
    InitializeResources
=================================================
*/
    bool  RESMNGR::InitializeResources (const PipelinePackDesc &desc) __NE___
    {
        CHECK_ERR( not _defaultPack );
        CHECK_ERR( not desc.parentPackId.IsValid() );

        auto    id = _CreateResource<PipelinePackID>( "failed when creating default pipeline pack", *this, desc );
        CHECK_ERR( id );

        auto    old = _defaultPack.Attach( RVRef(id) );
        CHECK_ERR( not old );

        return true;
    }

/*
=================================================
    Create*
=================================================
*/
    Strong<ImageID>  RESMNGR::CreateImage (const ImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<ImageID>( "failed when creating image", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<BufferID>  RESMNGR::CreateBuffer (const BufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<BufferID>( "failed when creating buffer", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<ImageID>  RESMNGR::CreateImage (const NativeImageDesc_t &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<ImageID>( "failed when creating image", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<BufferID>  RESMNGR::CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<BufferID>( "failed when creating buffer", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<ImageViewID>  RESMNGR::CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName) __NE___
    {
        return _CreateResource<ImageViewID>( "failed when creating image view", *this, desc, image, dbgName );
    }

    Strong<BufferViewID>  RESMNGR::CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName) __NE___
    {
        return _CreateResource<BufferViewID>( "failed when creating buffer view", *this, desc, buffer, dbgName );
    }

    Strong<ImageViewID>  RESMNGR::CreateImageView (const NativeImageViewDesc_t &desc, ImageID image, StringView dbgName) __NE___
    {
        return _CreateResource<ImageViewID>( "failed when creating image view", *this, desc, image, dbgName );
    }

    Strong<BufferViewID>  RESMNGR::CreateBufferView (const NativeBufferViewDesc_t &desc, BufferID buffer, StringView dbgName) __NE___
    {
        return _CreateResource<BufferViewID>( "failed when creating buffer view", *this, desc, buffer, dbgName );
    }

    Strong<RTGeometryID>  RESMNGR::CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<RTGeometryID>( "failed when creating ray tracing geometry (BLAS)", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<RTSceneID>  RESMNGR::CreateRTScene (const RTSceneDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<RTSceneID>( "failed when creating ray tracing scene (TLAS)", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<VideoSessionID>  RESMNGR::CreateVideoSession (const VideoSessionDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<VideoSessionID>( "failed when creating video session", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<VideoBufferID>  RESMNGR::CreateVideoBuffer (const VideoBufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<VideoBufferID>( "failed when creating video buffer", *this, desc, RVRef(allocator), dbgName );
    }

    Strong<VideoImageID>  RESMNGR::CreateVideoImage (const VideoImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator) __NE___
    {
        return _CreateResource<VideoImageID>( "failed when creating video image", *this, desc, RVRef(allocator), dbgName );
    }

/*
=================================================
    IsSupported
=================================================
*/
    bool  RESMNGR::IsSupported (EMemoryType memType) C_NE___
    {
        return _device.GetResourceFlags().memTypes.contains( memType );
    }

    bool  RESMNGR::IsSupported (const BufferDesc &desc) C_NE___
    {
        return Buffer_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const ImageDesc &desc) C_NE___
    {
        return Image_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (BufferID buffer, const BufferViewDesc &desc) C_NE___
    {
        auto*   buf = GetResource( buffer );
        CHECK_ERR( buf != null );

        return buf->IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (ImageID image, const ImageViewDesc &desc) C_NE___
    {
        auto*   img = GetResource( image );
        CHECK_ERR( img != null );

        return img->IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const VideoImageDesc &desc) C_NE___
    {
        return VideoImage_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const VideoBufferDesc &desc) C_NE___
    {
        return VideoBuffer_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const VideoSessionDesc &desc) C_NE___
    {
        return VideoSession_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const RTGeometryDesc &desc) C_NE___
    {
        return RTGeometry_t::IsSupported( *this, desc );
    }

    bool  RESMNGR::IsSupported (const RTSceneDesc &desc) C_NE___
    {
        return RTScene_t::IsSupported( *this, desc );
    }

/*
=================================================
    GetRTGeometrySizes / GetRTSceneSizes
=================================================
*/
    RTASBuildSizes  RESMNGR::GetRTGeometrySizes (const RTGeometryBuild &desc) __NE___
    {
        return RTGeometry_t::GetBuildSizes( *this, desc );
    }

    RTASBuildSizes  RESMNGR::GetRTSceneSizes (const RTSceneBuild &desc) __NE___
    {
        return RTScene_t::GetBuildSizes( *this, desc );
    }

/*
=================================================
    GetDeviceAddress
=================================================
*/
    DeviceAddress  RESMNGR::GetDeviceAddress (BufferID id) C_NE___
    {
        auto*   res = GetResource( id, False{"don't inc ref"}, True{"quiet"} );
        return res != null ? res->GetDeviceAddress() : Default;
    }

    DeviceAddress  RESMNGR::GetDeviceAddress (RTGeometryID id) C_NE___
    {
        auto*   res = GetResource( id, False{"don't inc ref"}, True{"quiet"} );
        return res != null ? res->GetDeviceAddress() : Default;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    GetCompatibleRenderPass
=================================================
*/
    RenderPassID  RESMNGR::GetCompatibleRenderPass (PipelinePackID packId, CompatRenderPassName::Ref name) C_NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->GetRenderPass( *this, name );
    }

    RenderPassID  RESMNGR::GetCompatibleRenderPass (PipelinePackID packId, RenderPassName::Ref name) C_NE___
    {
        RenderPassID    rp_id = GetRenderPass( packId, name );
        CHECK_ERR( rp_id );

      #ifdef AE_ENABLE_VULKAN
        auto*   rp = GetResource( rp_id );
        CHECK_ERR( rp );

        const auto  compat_id = rp->CompatibleRP();
        DBG_CHECK_MSG( compat_id, "compatible render pass must not be added to '_renderPassRefs'" );

        return compat_id ? compat_id : rp_id;
      #else

        return rp_id;
      #endif
    }

/*
=================================================
    GetRenderPass
=================================================
*/
    RenderPassID  RESMNGR::GetRenderPass (PipelinePackID packId, RenderPassName::Ref name) C_NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack != null );

        return pack->GetRenderPass( *this, name );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    _CreateDescriptorSets
=================================================
*/
    template <typename PplnID>
    bool  RESMNGR::_CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, const usize count,
                                          const PplnID &pplnId, DescriptorSetName::Ref dsName,
                                          DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        CHECK_ERR( dst != null and count > 0 );

        auto*   ppln = GetResource( pplnId );
        CHECK_ERR( ppln );

        auto*   ppln_layout = GetResource( ppln->LayoutId() );
        CHECK_ERR( ppln_layout );

        DescriptorSetLayoutID   layout_id;

        if_unlikely( not ppln_layout->GetDescriptorSetLayout( dsName, OUT layout_id, OUT binding ))
        {
            #ifdef AE_DEBUG
                String  str;
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

      #if defined(AE_DEBUG) and (not AE_OPTIMIZE_IDS)
        String  dbg_name;
        if ( dbgName.empty() )
        {
            dbg_name    = HashToName( dsName );
            dbgName     = dbg_name;
        }
      #endif

        allocator = _ChooseDescAllocator( RVRef(allocator) );

        usize   i       = 0;
        bool    created = true;

        for (; created & (i < count); ++i)
        {
            dst[i]  = _CreateResource<DescriptorSetID>( "failed when creating descriptor set", *this, layout_id, allocator, dbgName );
            created = (dst[i].IsValid());
        }

        if ( not created ) {
            for (usize j = 0; j < i; ++j) {
                ImmediatelyRelease( INOUT dst[j] );
            }
        }

        return created;
    }

/*
=================================================
    CreateDescriptorSets
=================================================
*/
    bool  RESMNGR::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         GraphicsPipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
    }

    bool  RESMNGR::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         MeshPipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
    }

    bool  RESMNGR::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         ComputePipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
    }

    bool  RESMNGR::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
    }

    bool  RESMNGR::CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         TilePipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName );
    }

/*
=================================================
    _GetPushConstantIndex
=================================================
*/
    template <typename PplnID>
    PushConstantIndex  RESMNGR::_GetPushConstantIndex (PplnID pplnId, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        auto*   ppln = GetResource( pplnId );
        CHECK_ERR( ppln != null );

        auto*   layout = GetResource( ppln->LayoutId() );
        CHECK_ERR( layout != null );

        auto&   pc = layout->GetPushConstants();
        auto    it = pc.find( pcName );
        CHECK_ERR( it != pc.end() );
        CHECK_ERR( typeName == it->second.typeName );
        CHECK_ERR( dataSize == Bytes{it->second.size} );

        #if defined(AE_ENABLE_VULKAN)
            return PushConstantIndex{ it->second.vulkanOffset, it->second.stage, it->second.typeName, it->second.size };

        #elif defined(AE_ENABLE_METAL)
            return PushConstantIndex{ it->second.metalBufferId, it->second.stage, it->second.typeName, it->second.size };

        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            return PushConstantIndex{ it->second.vulkanOffset, it->second.stage, it->second.typeName, it->second.size };    // TODO

        #else
        #   error not implemented
        #endif
    }

/*
=================================================
    GetPushConstantIndex
=================================================
*/
    PushConstantIndex  RESMNGR::GetPushConstantIndex (GraphicsPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
    }

    PushConstantIndex  RESMNGR::GetPushConstantIndex (MeshPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
    }

    PushConstantIndex  RESMNGR::GetPushConstantIndex (ComputePipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
    }

    PushConstantIndex  RESMNGR::GetPushConstantIndex (RayTracingPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
    }

    PushConstantIndex  RESMNGR::GetPushConstantIndex (TilePipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize) __NE___
    {
        return _GetPushConstantIndex( ppln, pcName, typeName, dataSize );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CreatePipelineCache
=================================================
*/
    Strong<PipelineCacheID>  RESMNGR::CreatePipelineCache () __NE___
    {
        PipelineCacheID     id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
        Replace( data );

        if_unlikely( not data.Create( *this, Default ))
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
    _CreateEmptyDescriptorSetLayout
=================================================
*/
    bool  RESMNGR::_CreateEmptyDescriptorSetLayout () __NE___
    {
        DescriptorSetLayoutID   id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
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
    Strong<ComputePipelineID>  RESMNGR::CreatePipeline (const ComputePipeline_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<ComputePipelineID>( "failed when creating compute pipeline", *this, ci );
    }

    Strong<GraphicsPipelineID>  RESMNGR::CreatePipeline (const GraphicsPipeline_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<GraphicsPipelineID>( "failed when creating graphics pipeline", *this, ci );
    }

    Strong<MeshPipelineID>  RESMNGR::CreatePipeline (const MeshPipeline_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<MeshPipelineID>( "failed when creating mesh pipeline", *this, ci );
    }

    Strong<RayTracingPipelineID>  RESMNGR::CreatePipeline (const RayTracingPipeline_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<RayTracingPipelineID>( "failed when creating ray tracing pipeline", *this, ci );
    }

    Strong<TilePipelineID>  RESMNGR::CreatePipeline (const TilePipeline_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<TilePipelineID>( "failed when creating tile pipeline", *this, ci );
    }

/*
=================================================
    CreateRTShaderBinding
=================================================
*/
    Strong<RTShaderBindingID>  RESMNGR::CreateRTShaderBinding (const ShaderBindingTable_t::CreateInfo &ci) __NE___
    {
        return _CreateResource<RTShaderBindingID>( "failed when creating RT shader binding table", *this, ci );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    _ResourcePrinter
=================================================
*/
#ifdef AE_DEBUG
    struct RESMNGR::_ResourcePrinter
    {
        RESMNGR&    resMngr;
        String&     log;

        template <typename T, uint I>
        void operator () () __NE___
        {
            auto    name = resMngr._GetResourcePoolName( T{} );
            log << "\nPool: " << name;

            auto&   pool = resMngr._GetResourcePool( T{} );
            pool.ForEachAssigned( [this] (auto& res) __NE___ { log << "\n  '" << res.Data().GetDebugName() << "', rc: " << ToString(res.GetRefCount()); } );
        }
    };
#endif

/*
=================================================
    PrintAllResources
=================================================
*/
    void  RESMNGR::PrintAllResources () __NE___
    {
    #ifdef AE_DEBUG
        TRY{
            String  log;
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
    SamplerID  RESMNGR::GetSampler (PipelinePackID packId, SamplerName::Ref name) C_NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );

        if_likely( pack != null )
            return pack->GetSampler( name );

        return _defaultSampler;
    }

/*
=================================================
    _CreateDefaultSampler
=================================================
*/
    bool  RESMNGR::_CreateDefaultSampler () __NE___
    {
        CHECK_ERR( _defaultSampler == Default );

        SamplerDesc     info;

        _defaultSampler = CreateSampler( info, "Default" );
        CHECK_ERR( _defaultSampler );

        return true;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    OnBeginFrame
=================================================
*/
    void  RESMNGR::OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg) __NE___
    {
      #ifdef AE_ENABLE_VULKAN
        _expiredResources._currentFrameId.store( frameId );
        {
            auto&   list = _expiredResources.Get( frameId );
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
    OnEndFrame
=================================================
*/
    void  RESMNGR::OnEndFrame (FrameUID frameId) __NE___
    {
        GetStagingManager().OnEndFrame( frameId );
    }
//-----------------------------------------------------------------------------


#undef RESMNGR
