// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define PPLNPACK         VPipelinePack

#elif defined(AE_ENABLE_METAL)
#   define PPLNPACK         MPipelinePack

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   define PPLNPACK         RPipelinePack

#else
#   error not implemented
#endif
//-----------------------------------------------------------------------------


namespace AE::PipelineCompiler
{
    ND_ bool  Deserialize_Uniform (Serializing::Deserializer& des, uint samplerStorageSize, OUT DescriptorSetLayoutDesc::Uniform &un);
}

namespace AE::Graphics
{
    using namespace AE::PipelineCompiler;

/*
=================================================
    constructor
=================================================
*/
    PPLNPACK::RenderPassRefs::RenderPassRefs (IAllocatorTS* alloc) __Th___ :
        specMap  { StdAlloc_t< Pair< const RenderPassName::Optimized_t,       RenderPassID >>{ alloc }},
        compatMap{ StdAlloc_t< Pair< const CompatRenderPassName::Optimized_t, RenderPassID >>{ alloc }}
    {}
//-----------------------------------------------------------------------------


/*
=================================================
    _Destroy
=================================================
*/
    void  PPLNPACK::_Destroy (ResMngr_t &resMngr) __NE___
    {
        if ( _allocator == null )
            return;

        resMngr.ImmediatelyRelease( INOUT _parentPackId );

        _parentPackId   = Default;
        _surfaceFormat  = Default;
        _shaders        = Default;
        _shaderOffset   = Default;
        _shaderDataSize = Default;

        // none of the resources has strong reference on sampler
        for (auto& [name, id] : *_samplerRefs) {
            CHECK( resMngr.ImmediatelyRelease( INOUT id ) == 0 );
        }
        _samplerRefs.Destroy();

        // none of the resources has strong reference on render pass
        for (auto& [name, id] : _renderPassRefs->specMap) {
            CHECK( resMngr.ImmediatelyRelease( INOUT id ) == 0 );
        }

        // release pipelines
        for (uint i = 0, cnt = _renTechs.Get<0>(); i < cnt; ++i) {
            _renTechs.Get<1>()[i]->Destroy( resMngr );
            PlacementDelete( INOUT _renTechs.Get<1>()[i] );
        }
        _renTechs = Default;

        // pipelines has strong reference on compatible render pass
        for (auto& [name, id] : _renderPassRefs->compatMap) {
            CHECK_Eq( resMngr.ImmediatelyRelease( INOUT id ), 0 );
        }
        _renderPassRefs.Destroy();

        // pipelines has strong reference on pipeline layout
        for (uint i = 0, cnt = _pplnLayouts.Get<0>(); i < cnt; ++i) {
            CHECK( resMngr.ImmediatelyRelease( INOUT _pplnLayouts.Get<1>()[i] ) == 0 );
        }
        _pplnLayouts = Default;

        // pipeline layout has strong reference on descriptor set layout
        for (uint i = 0, cnt = _dsLayouts.Get<0>(); i < cnt; ++i) {
            CHECK( resMngr.ImmediatelyRelease( INOUT _dsLayouts.Get<1>()[i] ) == 0 );
        }
        _dsLayouts = Default;

        DEBUG_ONLY(
            _allFeatureSets.Destroy();
        )
        _unsupportedFS.Destroy();
        _pplnTemplMap.Destroy();
        _dsLayoutMap.Destroy();
        _renTechMap.Destroy();

        _serGPplnTempl  = Default;
        _serMPplnTempl  = Default;
        _serCPplnTempl  = Default;
        _serRTPplnTempl = Default;

        _serGPplnSpec   = Default;
        _serMPplnSpec   = Default;
        _serCPplnSpec   = Default;
        _serRTPplnSpec  = Default;

        ASSERT( _allocator.use_count() == 1 );
        _allocator = null;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    _Create
=================================================
*/
    bool  PPLNPACK::_Create (ResMngr_t &resMngr, const PipelinePackDesc &desc) __Th___
    {
        CHECK_ERR( desc.stream );
        CHECK_ERR( desc.stream->IsOpen() );

        ASSERT( not _allocator );
        _allocator = MakeRC< LinearAllocator_t >();

        _unsupportedFS  .CreateTh( StdAlloc_t< FeatureSetName::Optimized_t                                           >{ _allocator.get() });
        _renTechMap     .CreateTh( StdAlloc_t< Pair< const RenderTechName::Optimized_t,     ushort                  >>{ _allocator.get() });
        _pplnTemplMap   .CreateTh( StdAlloc_t< Pair< const PipelineTmplName::Optimized_t,   PipelineTemplUID        >>{ _allocator.get() });
        _dsLayoutMap    .CreateTh( StdAlloc_t< Pair< const DescriptorSetName::Optimized_t,  DescriptorSetLayoutID   >>{ _allocator.get() });
        _samplerRefs    .CreateTh( StdAlloc_t< Pair< const SamplerName::Optimized_t,        SamplerID               >>{ _allocator.get() });
        _renderPassRefs .CreateTh( _allocator.get() );
        DEBUG_ONLY( _allFeatureSets.CreateTh( StdAlloc_t< FeatureSetName::Optimized_t >{ _allocator.get() }); )

        EXLOCK( _fileGuard );

        _file           = desc.stream;
        _surfaceFormat  = desc.surfaceFormat;

        // validate parentPackId
        {
            if ( desc.parentPackId )
            {
                _parentPackId = resMngr.AcquireResource( desc.parentPackId );
                CHECK_ERR( _parentPackId );
            }
            else
            if ( resMngr.GetDefaultPack() )
            {
                _parentPackId = resMngr.AcquireResource( resMngr.GetDefaultPack() );
                CHECK_ERR( _parentPackId );
            }

            if ( _parentPackId )
            {
                auto*       parent_pack     = resMngr.GetResource( _parentPackId );
                const auto  parent_surf_fmt = parent_pack->GetSurfaceFormat();

                CHECK_ERR( _surfaceFormat == parent_surf_fmt    or
                           _surfaceFormat == Default            or
                           parent_surf_fmt == Default );
            }
        }

        uint    hdr_name = 0;
        CHECK_ERR( _file->Read( OUT hdr_name ) and hdr_name == PackOffsets_Name );

        PipelinePackOffsets     offsets;
        CHECK_ERR( _file->Read( OUT offsets ));

        if ( AllBits( desc.options, EPipelinePackOpt::Pipelines ) and offsets.shaderDataSize > 8 )
        {
            _shaderOffset   = Bytes{offsets.shaderOffset}   + 8_b;  // skip block name and version
            _shaderDataSize = Bytes{offsets.shaderDataSize} - 8_b;
            CHECK_ERR( offsets.shaderOffset + offsets.shaderDataSize <= _file->Size() );
        }

        if ( offsets.nameMappingOffset < ulong(_file->Size()) )
            CHECK_ERR( _LoadNameMapping( resMngr, Bytes{offsets.nameMappingOffset}, Bytes{offsets.nameMappingDataSize} ));

        if ( AllBits( desc.options, EPipelinePackOpt::FeatureSets ) and offsets.featureSetOffset < ulong(_file->Size()) ) {
            CHECK_ERR( _LoadFeatureSets( resMngr, Bytes{offsets.featureSetOffset}, Bytes{offsets.featureSetDataSize} ));    // throw
        }else{
            CHECK_ERR( _CopyFeatureSets( resMngr ));
        }

        if ( AllBits( desc.options, EPipelinePackOpt::Samplers ) and offsets.samplerOffset < ulong(_file->Size()) )
            CHECK_ERR( _LoadSamplers( resMngr, Bytes{offsets.samplerOffset}, Bytes{offsets.samplerDataSize} ));             // throw

        if ( AllBits( desc.options, EPipelinePackOpt::RenderPasses ) and offsets.renderPassOffset < ulong(_file->Size()) )
            CHECK_ERR( _LoadRenderPasses( resMngr, Bytes{offsets.renderPassOffset}, Bytes{offsets.renderPassDataSize} ));   // throw

        if ( AllBits( desc.options, EPipelinePackOpt::Pipelines ) and offsets.pipelineOffset < ulong(_file->Size()) )
            CHECK_ERR( _LoadPipelineBlock( resMngr, Bytes{offsets.pipelineOffset}, Bytes{offsets.pipelineDataSize} ));      // throw

        DEBUG_ONLY( _debugName = desc.dbgName; )
        return true;
    }

/*
=================================================
    GetRenderPass
=================================================
*/
    RenderPassID  PPLNPACK::GetRenderPass (const ResMngr_t &resMngr, const RenderPassName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _renderPassRefs->specMap.find( name );

        if_likely( it != _renderPassRefs->specMap.end() )
            return it->second;

        // search in parent pack
        auto*   parent_pack = resMngr.GetResource( _parentPackId );
        if_likely( parent_pack != null )
            return parent_pack->GetRenderPass( resMngr, name );

        return Default;
    }

/*
=================================================
    GetRenderPass
=================================================
*/
    RenderPassID  PPLNPACK::GetRenderPass (const ResMngr_t &resMngr, const CompatRenderPassName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _renderPassRefs->compatMap.find( name );

        if_likely( it != _renderPassRefs->compatMap.end() )
            return it->second;

        // search in parent pack
        auto*   parent_pack = resMngr.GetResource( _parentPackId );
        if_likely( parent_pack != null )
            return parent_pack->GetRenderPass( resMngr, name );

        return Default;
    }

/*
=================================================
    GetSampler
=================================================
*/
    SamplerID  PPLNPACK::GetSampler (const SamplerName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _samplerRefs->find( SamplerName::Optimized_t{name} );

        if ( it != _samplerRefs->end() )
            return it->second;

        return Default;
    }

/*
=================================================
    GetSupportedRenderTechs
=================================================
*/
    Array<RenderTechName>  PPLNPACK::GetSupportedRenderTechs () C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        Array<RenderTechName>   supported;
        supported.reserve( _renTechs.Get<0>() / 2 );    // throw

        for (auto& [name, idx] : *_renTechMap)
        {
            ASSERT( idx < _renTechs.Get<0>() );

            auto&   rt = *_renTechs.Get<1>()[ idx ];
            if ( rt.IsSupported() )
                supported.push_back( RenderTechName{ name });   // throw
        }
        return supported;
    }

/*
=================================================
    _GetPipelineLayout
=================================================
*/
    PipelineLayoutID  PPLNPACK::_GetPipelineLayout (PipelineLayoutUID uid) C_NE___
    {
        CHECK_ERR( uint(uid) < _pplnLayouts.Get<0>() );

        return _pplnLayouts.Get<1>()[ uint(uid) ];
    }

/*
=================================================
    _Extract
=================================================
*/
    template <PipelineCompiler::PipelineTemplUID TemplMask, typename TemplType>
    const typename TypeList<TemplType>::template Get<1>
        PPLNPACK::_Extract (ResMngr_t &resMngr, const PipelineTmplName &name, const TemplType &templArr) C_NE___
    {
        auto    iter = _pplnTemplMap->find( name );
        CHECK_ERR( iter != _pplnTemplMap->end() );

        CHECK_ERR( AllBits( iter->second, TemplMask ));

        const uint  idx = uint(iter->second) & ~uint(PipelineTemplUID::_Mask);
        CHECK_ERR( idx < templArr.template Get<0>() );

        auto&   ppln        = templArr.template Get<1>()[ idx ];
        bool    supported   = true;

        for (auto& fs_name : ppln.features)
        {
            supported &= (not _unsupportedFS->contains( fs_name ));
            DEBUG_ONLY(
                if ( not _allFeatureSets->contains( fs_name ))
                    AE_LOG_SE( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
            )
        }

        if_unlikely( not supported )
        {
            DEBUG_ONLY( AE_LOG_SE( "Pipeline template '"s << resMngr.HashToName( name ) << "' is NOT supported" ));
            return null;
        }
        Unused( resMngr );

        return &ppln;
    }

/*
=================================================
    CreatePipeline
=================================================
*/
    Strong<ComputePipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, const PipelineTmplName &name,
                                                         const ComputePipelineDesc &desc, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto*   cppln = _Extract< PipelineTemplUID::Compute >( resMngr, name, _serCPplnTempl );
        CHECK_ERR( cppln != null );

        return resMngr.CreatePipeline( ComputePipeline_t::CreateInfo{
                                            *cppln, desc,
                                            _GetPipelineLayout( cppln->layout ),
                                            _GetShader( resMngr, cppln->shader, EShader::Compute ), cacheId
                                        });
    }

/*
=================================================
    CreatePipeline
=================================================
*/
    Strong<GraphicsPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, const PipelineTmplName &name,
                                                          const GraphicsPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto*   gppln = _Extract< PipelineTemplUID::Graphics >( resMngr, name, _serGPplnTempl );
        CHECK_ERR( gppln != null );

        FixedArray< ShaderModuleRef, 5 >    shaders;
        for (auto [type, uid] : gppln->shaders) {
            shaders.push_back( _GetShader( resMngr, uid, type ));
        }

        return resMngr.CreatePipeline( GraphicsPipeline_t::CreateInfo{
                                            *this,
                                            *gppln, desc,
                                            _GetPipelineLayout( gppln->layout ),
                                            shaders, cacheId,
                                            _allocator.get()
                                        });
    }

/*
=================================================
    CreatePipeline
=================================================
*/
    Strong<MeshPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, const PipelineTmplName &name,
                                                      const MeshPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto*   mppln = _Extract< PipelineTemplUID::Mesh >( resMngr, name, _serMPplnTempl );
        CHECK_ERR( mppln != null );

        FixedArray< ShaderModuleRef, 3 >    shaders;
        for (auto [type, uid] : mppln->shaders) {
            shaders.push_back( _GetShader( resMngr, uid, type ));
        }

        return resMngr.CreatePipeline( MeshPipeline_t::CreateInfo{
                                            *this,
                                            *mppln, desc,
                                            _GetPipelineLayout( mppln->layout ),
                                            shaders, cacheId,
                                            _allocator.get()
                                        });
    }

/*
=================================================
    CreatePipeline
=================================================
*/
    Strong<RayTracingPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, const PipelineTmplName &name,
                                                            const RayTracingPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto*   rtppln = _Extract< PipelineTemplUID::RayTracing >( resMngr, name, _serRTPplnTempl );
        CHECK_ERR( rtppln != null );

        TempLinearAllocator_t   allocator;

        auto*   shaders = allocator.Allocate< ShaderModuleRef >( rtppln->shaderArr.size() );
        CHECK_ERR( shaders != null );

        for (usize i = 0; i < rtppln->shaderArr.size(); ++i)
        {
            auto&   sh = rtppln->shaderArr[i];
            shaders[i] = _GetShader( resMngr, sh.Get<ShaderUID>(), sh.Get<EShader>() );
        }

        return resMngr.CreatePipeline( RayTracingPipeline_t::CreateInfo{
                                            *rtppln, desc,
                                            _GetPipelineLayout( rtppln->layout ),
                                            ArrayView< ShaderModuleRef >{ shaders, rtppln->shaderArr.size() }, cacheId,
                                            _allocator.get(),
                                            &allocator
                                        });
    }

/*
=================================================
    CreatePipeline
=================================================
*/
    Strong<TilePipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, const PipelineTmplName &name,
                                                      const TilePipelineDesc &desc, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto*   tppln = _Extract< PipelineTemplUID::Tile >( resMngr, name, _serTPplnTempl );
        CHECK_ERR( tppln != null );

        return resMngr.CreatePipeline( TilePipeline_t::CreateInfo{
                                            *this,
                                            *tppln, desc,
                                            _GetPipelineLayout( tppln->layout ),
                                            _GetShader( resMngr, tppln->shader, EShader::Tile ), cacheId
                                        });
    }

/*
=================================================
    GetDSLayout
=================================================
*/
    DescriptorSetLayoutID  PPLNPACK::GetDSLayout (const DSLayoutName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _dsLayoutMap->find( name );

        if_likely( iter != _dsLayoutMap->end() )
            return iter->second;

        return Default;
    }

/*
=================================================
    LoadRenderTechAsync
=================================================
*/
    Promise<RenderTechPipelinesPtr>  PPLNPACK::LoadRenderTechAsync (ResMngr_t &resMngr, const RenderTechName &name, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _renTechMap->find( RenderTechName::Optimized_t{name} );
        if_unlikely( iter == _renTechMap->end() )
            return Default;

        CHECK_ERR( uint(iter->second) < _renTechs.Get<0>() );

        auto&   rt = *_renTechs.Get<1>()[ uint(iter->second) ];
        return rt.LoadAsync( resMngr, cacheId );
    }

/*
=================================================
    LoadRenderTech
=================================================
*/
    RenderTechPipelinesPtr  PPLNPACK::LoadRenderTech (ResMngr_t &resMngr, const RenderTechName &name, PipelineCacheID cacheId) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _renTechMap->find( RenderTechName::Optimized_t{name} );
        if_unlikely( iter == _renTechMap->end() )
            return Default;

        CHECK_ERR( uint(iter->second) < _renTechs.Get<0>() );

        auto&   rt = *_renTechs.Get<1>()[ uint(iter->second) ];
        CHECK_ERR( rt.Load( resMngr, cacheId ));

        return rt.GetRC();
    }

/*
=================================================
    _LoadNameMapping
=================================================
*/
    bool  PPLNPACK::_LoadNameMapping (ResMngr_t &resMngr, Bytes offset, Bytes size) __Th___
    {
    #ifdef AE_DEBUG
        CHECK_ERR( _file->SeekSet( offset ));

        auto    mem_stream = MakeRC<MemRStream>();
        CHECK_ERR( mem_stream->LoadRemaining( *_file, size ));

        Serializing::Deserializer   des{ RVRef(mem_stream) };
        CHECK_ERR( not des.stream.Empty() );

        uint    hdr_name    = 0;
        CHECK_ERR( des( OUT hdr_name ));
        CHECK_ERR( hdr_name == NameMapping_Name );

        PipelineCompiler::HashToName    hash_to_name;
        CHECK_ERR( hash_to_name.Deserialize( des ));

        CHECK( des.IsEnd() );

        resMngr.AddHashToName( hash_to_name );
    #else
        Unused( resMngr, offset, size );
    #endif
        return true;
    }

/*
=================================================
    _LoadFeatureSets
=================================================
*/
    bool  PPLNPACK::_LoadFeatureSets (ResMngr_t &resMngr, Bytes offset, Bytes size) __Th___
    {
        CHECK_ERR( _file->SeekSet( offset ));

        auto    mem_stream = MakeRC<MemRStream>();
        CHECK_ERR( mem_stream->LoadRemaining( *_file, size ));

        Serializing::Deserializer   des{ RVRef(mem_stream) };
        CHECK_ERR( not des.stream.Empty() );

        uint    hdr_name    = 0;
        uint    version     = 0;
        CHECK_ERR( des( OUT hdr_name, OUT version ));
        CHECK_ERR( hdr_name == FeatureSetPack_Name and version == FeatureSetPack_Version );

        ulong   fs_hash = 0;
        uint    count   = 0;
        CHECK_ERR( des( OUT fs_hash, OUT count ));
        CHECK_ERR( count <= FeatureSetSerializer::MaxCount );
        CHECK_ERR( fs_hash == ulong(FeatureSet::GetHashOfFS_Precalculated()) );

        size -= 8_b;    // header & version
        size -= 12_b;   // fs_hash & count
        CHECK_ERR( IsMultipleOf( size, count ));
        CHECK_ERR( size == (SizeOf<FeatureSet> + 4_b) * count );

        auto*   parent_pack = resMngr.GetResource( _parentPackId, False{"don't inc ref"}, True{"quiet"} );
        if_likely( parent_pack != null )
        {
            *_unsupportedFS = parent_pack->GetUnsupportedFS();          // throw
            _unsupportedFS->reserve( _unsupportedFS->size() + count );  // throw

            DEBUG_ONLY(
                *_allFeatureSets = parent_pack->GetAllFS();                     // throw
                _allFeatureSets->reserve( _allFeatureSets->size() + count );    // throw
            )
        }
        else
        {
            _unsupportedFS->reserve( count );                   // throw
            DEBUG_ONLY( _allFeatureSets->reserve( count ));     // throw
        }

        const auto& current_fs = resMngr.GetFeatureSet();

        for (uint i = 0; i < count; ++i)
        {
            FeatureSetName::Optimized_t fs_name;
            FeatureSetSerializer        fs;

            CHECK_ERR( des( OUT fs_name, OUT fs ));
            CHECK( fs_name.IsDefined() );
            CHECK( fs.Get().IsValid() );

            if ( not current_fs.IsCompatible( fs.Get() ))
            {
                DEBUG_ONLY( AE_LOG_SE( "Feature set '"s << resMngr.HashToName( fs_name ) << "' is NOT supported" ));

                CHECK( _unsupportedFS->insert( fs_name ).second );  // throw
            }
            else
            {
                DEBUG_ONLY( AE_LOGI( "Feature set '"s << resMngr.HashToName( fs_name ) << "' is supported" ));
            }

            DEBUG_ONLY(
                CHECK( _allFeatureSets->insert( fs_name ).second ); // throw
            )
        }

        CHECK( des.IsEnd() );
        return true;
    }

/*
=================================================
    _CopyFeatureSets
=================================================
*/
    bool  PPLNPACK::_CopyFeatureSets (ResMngr_t &resMngr) __Th___
    {
        auto*   parent_pack = resMngr.GetResource( _parentPackId, False{"don't inc ref"}, True{"quiet"} );
        if ( parent_pack != null )
        {
            *_unsupportedFS = parent_pack->GetUnsupportedFS();          // throw
            DEBUG_ONLY( *_allFeatureSets = parent_pack->GetAllFS();)    // throw
        }
        return true;
    }

/*
=================================================
    _LoadSamplers
=================================================
*/
    bool  PPLNPACK::_LoadSamplers (ResMngr_t &resMngr, Bytes offset, Bytes size) __Th___
    {
        CHECK_ERR( _file->SeekSet( offset ));

        auto    mem_stream = MakeRC<MemRStream>();
        CHECK_ERR( mem_stream->LoadRemaining( *_file, size ));

        Serializing::Deserializer   des{ RVRef(mem_stream) };
        CHECK_ERR( not des.stream.Empty() );

        // for feature set array
        des.allocator = _allocator.get();

        uint    hdr_name    = 0;
        uint    version     = 0;
        CHECK_ERR( des( OUT hdr_name, OUT version ));
        CHECK_ERR( hdr_name == SamplerPack_Name and version == SamplerPack_Version );

        Array<Pair< SamplerName::Optimized_t, uint >>   samp_names;
        CHECK_ERR( des( OUT samp_names ));

        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= SamplerSerializer::MaxCount );

        if ( count == 0 )
            return true;

        using SamplerYcbcrArr_t = Array< Optional< SamplerYcbcrConversionDesc >>;

        Array< SamplerID >      samplers;           samplers.resize( count );
        Array< SamplerDesc >    samp_desc;          samp_desc.resize( count );
        SamplerYcbcrArr_t       ycbcr_conv;         ycbcr_conv.resize( count );
        Array< bool >           samp_supported;     samp_supported.resize( count );

        for (uint i = 0; i < count; ++i)
        {
            SamplerSerializer   samp;
            CHECK_ERR( samp.Deserialize( des ));

            bool    supported = true;
            for (auto fs_name : samp.GetFeatures())
            {
                supported &= (not _unsupportedFS->contains( fs_name ));
                DEBUG_ONLY(
                    if ( not _allFeatureSets->contains( fs_name ))
                        AE_LOG_SE( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
                )
            }

            samp_desc[i]        = samp.GetDesc();
            ycbcr_conv[i]       = samp.YcbcrDescOpt();
            samp_supported[i]   = supported;
        }

        _samplerRefs->reserve( count );     // throw

        for (auto& [name, uid] : samp_names)
        {
            const usize i = usize(uid);
            CHECK_ERR( i < samplers.size() );

            if_likely( samp_supported[i] )
            {
                // if not created
                if_unlikely( samplers[i] == Default )
                {
                    String  samp_name;
                    #ifdef AE_DEBUG
                        samp_name = resMngr.HashToName( name );
                    #endif

                    samplers[i] = _CreateSampler( resMngr, samp_desc[i], ycbcr_conv[i], samp_name );
                    CHECK_ERR_MSG( samplers[i],
                                   "Failed to create sampler '"s << resMngr.HashToName( name ) << "'" );
                }

                CHECK( _samplerRefs->insert_or_assign( name, Strong<SamplerID>{samplers[i]} ).second );  // throw
            }else{
                DEBUG_ONLY( AE_LOG_SE( "Sampler '"s << resMngr.HashToName( name ) << "' is NOT supported" ));
            }
        }

        CHECK( des.IsEnd() );
        return true;
    }

/*
=================================================
    _LoadPipelineBlock
=================================================
*/
    bool  PPLNPACK::_LoadPipelineBlock (ResMngr_t &resMngr, Bytes offset, Bytes size) __Th___
    {
        CHECK_ERR( _file->SeekSet( offset ));

        auto    mem_stream = MakeRC<MemRStream>();
        CHECK_ERR( mem_stream->LoadRemaining( *_file, size ));

        Serializing::Deserializer   des{ RVRef(mem_stream) };
        CHECK_ERR( not des.stream.Empty() );

        // for feature set array
        des.allocator = _allocator.get();

        uint    hdr_name    = 0;
        uint    version     = 0;
        CHECK_ERR( des( OUT hdr_name, OUT version ));
        CHECK_ERR( hdr_name == PipelinePack_Name and version == PipelinePack_Version );

        StackAllocator_t        stack_alloc;

        EnumBitSet< EMarker >   unique_marker;
        unique_marker.insert( EMarker::Unknown );

        for (EMarker marker = Default;;)
        {
            if_unlikely( not des( OUT marker ))
                break;

            CHECK_ERR( uint(marker) < unique_marker.size() and not unique_marker.contains( marker ));
            unique_marker.insert( marker );

            BEGIN_ENUM_CHECKS();
            switch ( marker )
            {
                case EMarker::RenderStates :            CHECK_ERR( _LoadRenderStates( des ));                   break;
                case EMarker::DepthStencilStates :      CHECK_ERR( _LoadDepthStencilStates( resMngr, des ));    break;

                case EMarker::DescrSetLayouts :         CHECK_ERR( _LoadDescrSetLayouts( resMngr, des, stack_alloc, OUT *_dsLayoutMap ));   break;  // throw
                case EMarker::PipelineLayouts :         CHECK_ERR( _LoadPipelineLayouts( resMngr, des ));               break;

                case EMarker::PipelineTemplNames :      CHECK_ERR( _LoadPipelineNames( des, OUT *_pplnTemplMap ));      break;  // throw

                case EMarker::GraphicsPipelineTempl :   CHECK_ERR( _LoadPipelineArray( des, OUT _serGPplnTempl ));      break;
                case EMarker::MeshPipelineTempl :       CHECK_ERR( _LoadPipelineArray( des, OUT _serMPplnTempl ));      break;
                case EMarker::ComputePipelineTempl :    CHECK_ERR( _LoadPipelineArray( des, OUT _serCPplnTempl ));      break;
                case EMarker::RayTracingPipelineTempl : CHECK_ERR( _LoadPipelineArray( des, OUT _serRTPplnTempl ));     break;
                case EMarker::TilePipelineTempl :       CHECK_ERR( _LoadPipelineArray( des, OUT _serTPplnTempl ));      break;

                case EMarker::GraphicsPipelineSpec :    CHECK_ERR( _LoadPipelineArray( des, OUT _serGPplnSpec ));       break;
                case EMarker::MeshPipelineSpec :        CHECK_ERR( _LoadPipelineArray( des, OUT _serMPplnSpec ));       break;
                case EMarker::ComputePipelineSpec :     CHECK_ERR( _LoadPipelineArray( des, OUT _serCPplnSpec ));       break;
                case EMarker::RayTracingPipelineSpec :  CHECK_ERR( _LoadPipelineArray( des, OUT _serRTPplnSpec ));      break;
                case EMarker::TilePipelineSpec :        CHECK_ERR( _LoadPipelineArray( des, OUT _serTPplnSpec ));       break;

                case EMarker::RenderTechniques :        CHECK_ERR( _LoadRenderTechniques( resMngr, des ));              break;  // throw

                case EMarker::RTShaderBindingTable :    CHECK_ERR( _LoadRTShaderBindingTable( des ));                   break;

              #ifdef AE_ENABLE_VULKAN
                case EMarker::SpirvShaders :            CHECK_ERR( _LoadShaders( des ));                                break;
              #else
                case EMarker::SpirvShaders :
              #endif

              #ifdef AE_PLATFORM_IOS
                case EMarker::MetaliOSShaders :         CHECK_ERR( _LoadShaders( des ));                                break;
              #else
                case EMarker::MetaliOSShaders :
              #endif

              #ifdef AE_PLATFORM_MACOS
                case EMarker::MetalMacShaders :         CHECK_ERR( _LoadShaders( des ));                                break;
              #else
                case EMarker::MetalMacShaders :
              #endif

                case EMarker::Unknown :
                case EMarker::_Count :
                default_unlikely :                      DBG_WARNING( "unknown marker" );                                break;
            }
            END_ENUM_CHECKS();
        }

        CHECK( des.IsEnd() );
        return true;
    }

/*
=================================================
    _LoadRenderStates
=================================================
*/
    bool  PPLNPACK::_LoadRenderStates (Serializing::Deserializer &des) __NE___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxStateCount );

        if ( count == 0 )
            return true;

        auto    rs_ptr = _allocator->Allocate< PipelineCompiler::SerializableRenderState >( count );
        CHECK_ERR( rs_ptr != null );
        _renderStates = Tuple{ count, rs_ptr };

        for (uint i = 0; i < count; ++i)
        {
            CHECK_ERR( des( OUT rs_ptr[i] ));
        }
        return true;
    }

/*
=================================================
    _GetRenderState
=================================================
*/
    RenderState const*  PPLNPACK::_GetRenderState (PipelineCompiler::RenderStateUID uid) C_NE___
    {
        const uint  idx = uint(uid);

        return  idx < _renderStates.Get<0>()  ?
                    &_renderStates.Get<1>()[idx].rs :
                    null;
    }

/*
=================================================
    _LoadDepthStencilStatesImpl
=================================================
*/
    bool  PPLNPACK::_LoadDepthStencilStatesImpl (Serializing::Deserializer &des) __NE___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxStateCount );

        if ( count == 0 )
            return true;

        auto    ds_ptr = _allocator->Allocate< PipelineCompiler::SerializableDepthStencilState >( count );
        CHECK_ERR( ds_ptr != null );
        _depthStencilStates = Tuple{ count, ds_ptr };

        for (uint i = 0; i < count; ++i)
        {
            CHECK_ERR( des( OUT ds_ptr[i] ));
        }
        return true;
    }

/*
=================================================
    _LoadDescrSetLayouts
=================================================
*/
    bool  PPLNPACK::_LoadDescrSetLayouts (ResMngr_t &resMngr, Serializing::Deserializer &des, StackAllocator_t &stackAlloc, OUT DSLayoutMap_t &layoutMap) __Th___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxDSLayoutCount );

        if ( count == 0 )
            return true;

        layoutMap.reserve( count );  // throw

        _dsLayouts = Tuple{ count, _allocator->Allocate< Strong<DescriptorSetLayoutID> >( count )};
        CHECK_ERR( _dsLayouts.Get<1>() != null );

        auto*   parent_pack = resMngr.GetResource( _parentPackId, False{"don't inc ref"}, True{"quiet"} );  // TODO: shared lock

        for (uint i = 0; i < count; ++i)
        {
            const auto                  bm              = stackAlloc.PushAuto();
            SamplerID *                 samplers        = null;
            uint                        sampler_count   = 0;
            DSLayoutName::Optimized_t   dsl_name;
            EDescSetUsage               usage           = Default;
            EShaderStages               stages          = Default;
            FSNameArr_t                 features;
            UniformOffsets_t            desc_offsets    = {};

            CHECK_ERR( des( OUT dsl_name, OUT usage, OUT stages, OUT features, OUT desc_offsets, OUT sampler_count ));

            const uint  uniform_count = desc_offsets.back();
            CHECK_ERR( sampler_count <= DescriptorSetLayoutDesc::MaxSamplers );
            CHECK_ERR( uniform_count <= DescriptorSetLayoutDesc::MaxUniforms );

            bool    supported = true;
            for (auto fs_name : features)
            {
                supported &= (not _unsupportedFS->contains( fs_name ));
                DEBUG_ONLY(
                    if ( not _allFeatureSets->contains( fs_name ))
                        AE_LOG_SE( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
                )
            }

            if ( sampler_count > 0 )
            {
                samplers = stackAlloc.Allocate< SamplerID >( sampler_count );
                CHECK_ERR( samplers != null );

                for (uint j = 0; j < sampler_count; ++j)
                {
                    SamplerName::Optimized_t    samp_name;
                    CHECK_ERR( des( OUT samp_name ));

                    SamplerID   samp_id;

                    if ( not supported )
                        continue;

                    // search in current pack
                    if ( auto samp_it = _samplerRefs->find( samp_name );  samp_it != _samplerRefs->end() )
                    {
                        CHECK_ERR( samp_it->second );
                        samp_id = samp_it->second;
                    }

                    // search in parent pack
                    if ( not samp_id and parent_pack != null )
                    {
                        samp_id = parent_pack->GetSampler( SamplerName{samp_name} );
                    }
                    CHECK_ERR( samp_id );

                    samplers[j] = samp_id;
                }
            }

            const bool  is_upd_tmpl = AllBits( usage, EDescSetUsage::UpdateTemplate );  // Vulkan only
            auto*       un_names    = _allocator->Allocate< UniformName::Optimized_t >( uniform_count );
            auto*       un_data     = _allocator->Allocate< Uniform_t >( uniform_count );
            auto*       un_offsets  = is_upd_tmpl ? _allocator->Allocate< Byte16u >( uniform_count ) : null;
            bool        result      = true;
            CHECK_ERR( un_names != null and un_data != null );

            for (uint j = 0; result & (j < uniform_count); ++j) {
                result = des( OUT un_names[j] );
            }
            CHECK_ERR( result );

            for (uint j = 0; result & (j < uniform_count); ++j)
            {
                PlacementNew<Uniform_t>( OUT un_data + j );
                result = Deserialize_Uniform( des, sampler_count, OUT un_data[j] );
            }
            CHECK_ERR( result );

            if ( not supported )
            {
                PlacementNew< Strong<DescriptorSetLayoutID> >( OUT _dsLayouts.Get<1>() + i );

                DEBUG_ONLY( AE_LOG_SE( "DescriptorSetLayout '"s << resMngr.HashToName( dsl_name ) << "' is NOT supported" ));
                continue;
            }

            Strong<DescriptorSetLayoutID>   item = _CreateDescriptorSetLayout( resMngr,
                                                                               Uniforms_t{ uniform_count, un_names, un_data, un_offsets },
                                                                               ArrayView{ samplers, sampler_count },
                                                                               desc_offsets,
                                                                               usage, stages, stackAlloc );
            CHECK_ERR( item );

            layoutMap.emplace( dsl_name, DescriptorSetLayoutID{item} );  // throw

            PlacementNew< Strong<DescriptorSetLayoutID> >( OUT _dsLayouts.Get<1>() + i, RVRef(item) );
        }

        return true;
    }

/*
=================================================
    _LoadPipelineLayouts
=================================================
*/
    bool  PPLNPACK::_LoadPipelineLayouts (ResMngr_t &resMngr, Serializing::Deserializer &des) __NE___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxPplnLayoutCount );

        if ( count == 0 )
            return true;

        _pplnLayouts = Tuple{ count, _allocator->Allocate< Strong<PipelineLayoutID> >( count )};
        CHECK_ERR( _pplnLayouts.Get<1>() != null );

        for (uint i = 0; i < count; ++i)
        {
            PipelineLayoutDesc  desc;
            CHECK_ERR( des( OUT desc ));

            PipelineLayout_t::DescriptorSets_t      desc_sets;
            bool                                    supported = true;

            for (usize j = 0; j < desc.descrSets.size(); ++j)
            {
                auto    src = desc.descrSets[j];
                CHECK_ERR( uint(src.second.uid) < _dsLayouts.Get<0>() );

                DescriptorSetLayoutID   id = _dsLayouts.Get<1>()[ uint(src.second.uid) ];
                DescSetBinding          binding;

                supported &= (id != Default);

                #if defined(AE_ENABLE_VULKAN)
                    binding.vkIndex = src.second.vkIndex;
                #elif defined(AE_ENABLE_METAL)
                    binding.mtlIndex = src.second.mtlIndex;
                #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
                    binding.vkIndex = src.second.vkIndex;
                #else
                #   error not implemented
                #endif

                desc_sets.insert_or_assign( src.first, PipelineLayout_t::DescSetLayout{ id, binding }); // nothrow
            }

            if ( not supported )
            {
                PlacementNew< Strong<PipelineLayoutID> >( OUT _pplnLayouts.Get<1>() + i );
                //DEBUG_ONLY( AE_LOG_SE( "PipelineLayout is NOT supported" ));
                continue;
            }

            auto    item = resMngr.CreatePipelineLayout( desc_sets, desc.pushConstants.items  DEBUG_ONLY(, ToString(i)) );
            CHECK_ERR( item );

            PlacementNew< Strong<PipelineLayoutID> >( OUT _pplnLayouts.Get<1>() + i, RVRef(item) );
        }
        return true;
    }

/*
=================================================
    _LoadPipelineNames
=================================================
*/
    template <typename TName, typename TUID>
    bool  PPLNPACK::_LoadPipelineNames (Serializing::Deserializer &des, OUT THashMap<TName, TUID> &nameMap) __Th___
    {
        CHECK_ERR( nameMap.empty() );

        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxPipelineNameCount );

        if ( count == 0 )
            return true;

        nameMap.reserve( count );   // throw

        bool    result = true;
        for (uint i = 0; result & (i < count); ++i)
        {
            TName   name;
            TUID    uid;
            result = des( OUT name, OUT uid );

            auto [iter, inserted] = nameMap.emplace( name, uid );   // throw
            CHECK( inserted );
        }

        return result;
    }

/*
=================================================
    _LoadPipelineArray
=================================================
*/
    template <typename T>
    bool  PPLNPACK::_LoadPipelineArray (Serializing::Deserializer &des, OUT Tuple<uint, const T*> &arr) __NE___
    {
        CHECK_ERR(( arr == Tuple<uint, const T*>{ 0, null }));

        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxPipelineCount );

        if ( count == 0 )
            return true;

        auto    ptr = _allocator->Allocate<T>( count );
        CHECK_ERR( ptr != null );
        arr = Tuple{ count, ptr };

        bool    result = true;
        for (uint i = 0; result & (i < count); ++i)
        {
            auto&   ppln = ptr[i];
            PlacementNew<T>( OUT &ppln );
            result = ppln.Deserialize( des );
        }
        return result;
    }

/*
=================================================
    _LoadRenderTechniques
=================================================
*/
    bool  PPLNPACK::_LoadRenderTechniques (ResMngr_t &resMngr, Serializing::Deserializer &des) __Th___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxRenTechCount );

        if ( count == 0 )
            return true;

        _renTechMap->reserve( count );  // throw

        _renTechs = Tuple{ count, _allocator->Allocate< StaticRC<RenderTech> >( count )};
        CHECK_ERR( _renTechs.Get<1>() != null );

        bool    result = true;
        for (uint i = 0; result & (i < count); ++i)
        {
            auto&   rt = _renTechs.Get<1>()[i];
            PlacementNew< StaticRC<RenderTech> >( &rt, *this );

            result = rt->Deserialize( resMngr, des );   // throw

            if ( rt->IsSupported() )
                _renTechMap->emplace( rt->Name(), ushort(i) );  // throw
        }
        return result;
    }

/*
=================================================
    _LoadRTShaderBindingTable
=================================================
*/
    bool  PPLNPACK::_LoadRTShaderBindingTable (Serializing::Deserializer &des) __NE___
    {
        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxSBTCount );

        if ( count == 0 )
            return true;

        auto    ptr = _allocator->Allocate< PipelineCompiler::SerializableRTShaderBindingTable >( count );
        CHECK_ERR( ptr != null );
        _serRTSBTs = SerRTSBTs_t{ count, ptr };

        bool    result = true;
        for (uint i = 0; result & (i < count); ++i)
        {
            auto&   sbt = ptr[i];
            PlacementNew< PipelineCompiler::SerializableRTShaderBindingTable >( OUT &sbt );
            result = sbt.Deserialize( des );
        }
        return result;
    }

/*
=================================================
    _LoadShaders
=================================================
*/
    bool  PPLNPACK::_LoadShaders (Serializing::Deserializer &des) __NE___
    {
        using Types_t = TypeList< ShaderBytecode::Bytecode_t >; // union to typelist

        uint    count = 0;
        CHECK_ERR( des( OUT count ));
        CHECK_ERR( count <= PipelineStorage::MaxShaderCount );

        if ( count == 0 )
            return true;

        _shaders = Tuple{ count, _allocator->Allocate< ShaderModule >( count )};
        CHECK_ERR( _shaders.Get<1>() != null );

        const Byte32u   total_size {_shaderDataSize};

        bool    result = true;
        for (uint i = 0; result & (i < count); ++i)
        {
            auto&   sh = _shaders.Get<1>()[i];
            PlacementNew< ShaderModule >( OUT &sh );

            result  = des( OUT sh.offset, OUT sh.dataSize, OUT sh.shaderTypeIdx );
            result &= (sh.offset < total_size) & (sh.offset + sh.dataSize <= total_size);

            CHECK_ERR( sh.offset < total_size );
            CHECK_ERR( sh.offset + sh.dataSize <= total_size );

            #if defined(AE_ENABLE_VULKAN)
                CHECK_ERR(  sh.shaderTypeIdx == Types_t::Index<SpirvBytecode_t> or
                            sh.shaderTypeIdx == Types_t::Index<SpirvWithTrace> );
            #elif defined(AE_ENABLE_METAL)
                CHECK_ERR(  sh.shaderTypeIdx == Types_t::Index<MetalBytecode_t> );
            #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            #else
            #   error not implemented
            #endif
        }

        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    PPLNPACK::RenderTech::RenderTech (PPLNPACK& pack) __NE___ :
        _pack{ pack },
        _isSupported{ false },  _isLoaded{ false },     _wasAttampToLoad{ false },
        _pipelines{ StdAlloc_t< Pair< const PipelineName::Optimized_t, PipelineInfo >>{ pack._allocator.get() }},
        _rtSbtMap{ StdAlloc_t< Pair< const RTShaderBindingName::Optimized_t, SBTInfo >>{ pack._allocator.get() }}
    {}

    PPLNPACK::RenderTech::~RenderTech () __NE___
    {}

/*
=================================================
    Deserialize
=================================================
*/
    bool  PPLNPACK::RenderTech::Deserialize (ResMngr_t &resMngr, Serializing::Deserializer &des) __Th___
    {
        DRC_EXLOCK( _drCheck );
        Unused( resMngr );

        uint    fs_count = 0;
        CHECK_ERR( des( OUT _name, OUT fs_count ));
        CHECK_ERR( fs_count <= FeatureSetSerializer::MaxCount );

        // feature sets
        {
            bool    result      = true;
            bool    supported   = true;

            for (uint i = 0; result & (i < fs_count); ++i)
            {
                FeatureSetName::Optimized_t     fs_name;
                result = des( OUT fs_name );

                supported &= (not _pack._unsupportedFS->contains( fs_name ));
                DEBUG_ONLY(
                    if ( _pack._unsupportedFS->contains( fs_name ))
                        AE_LOG_DBG( "Render technique '"s << resMngr.HashToName( _name ) << "' requires '" << resMngr.HashToName( fs_name ) << "' FeatureSet" );

                    if ( not _pack._allFeatureSets->contains( fs_name ))
                        AE_LOG_SE( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
                )
            }

            CHECK_ERR( result );
            _isSupported = supported;

            if ( not _isSupported ) {
                DEBUG_ONLY( AE_LOG_SE( "Render technique '"s << resMngr.HashToName( _name ) << "' is NOT supported" ))
            }
        }

        // passes
        {
            uint    pass_count = 0;
            CHECK_ERR( des( OUT pass_count ) and pass_count > 0 );
            CHECK_ERR( pass_count <= SerializableRenderTechnique::MaxPassCount );

            _passes = Tuple{ pass_count, _pack._allocator->Allocate< SerializableRenderTechnique::Pass >( pass_count )};

            bool    result = true;
            for (uint i = 0; result & (i < pass_count); ++i)
            {
                auto&   pass = _passes.Get<1>()[i];
                PlacementNew< SerializableRenderTechnique::Pass >( OUT &pass );
                result = pass.Deserialize( des );
            }
            CHECK_ERR( result );
        }

        // pipelines
        {
            uint    ppln_count = 0;
            CHECK_ERR( des( OUT ppln_count ) and ppln_count > 0 );
            CHECK_ERR( ppln_count <= SerializableRenderTechnique::MaxPipelineCount );

            _pipelines.reserve( ppln_count );   // throw

            bool    result = true;
            for (uint i = 0; result & (i < ppln_count); ++i)
            {
                PipelineName::Optimized_t   name;
                PipelineInfo                info;
                result = des( OUT name, OUT info.uid );
                _pipelines.emplace( name, info );   // throw
            }
            CHECK_ERR( result );
            CHECK_ERR( _pipelines.size() == ppln_count );
        }

        // SBTs
        {
            uint    sbt_count = 0;
            CHECK_ERR( des( OUT sbt_count ));

            if ( sbt_count > 0 )
            {
                _rtSbtMap.reserve( sbt_count ); // throw

                bool    result = true;
                for (uint i = 0; result & (i < sbt_count); ++i)
                {
                    RTShaderBindingName::Optimized_t    name;
                    SBTInfo                             info;
                    result = des( OUT name, OUT info.uid );
                    _rtSbtMap.emplace( name, info );    // throw
                }
                CHECK_ERR( result );
                CHECK_ERR( _rtSbtMap.size() == sbt_count );
            }
        }

        return true;
    }

/*
=================================================
    LoadAsync
=================================================
*/
    auto  PPLNPACK::RenderTech::LoadAsync (ResMngr_t &resMngr, PipelineCacheID cacheid) __NE___ -> Promise<RenderTechPipelinesPtr>
    {
        class PreloadShadersTask final : public Threading::IAsyncTask
        {
        public:
            RC<RenderTech>  rtech;
            ResMngr_t &     resMngr;

            PreloadShadersTask (RC<RenderTech> rt, ResMngr_t& rm) __NE___ :
                IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm} {}

            void        Run ()      __Th_OV { CHECK_TE( rtech->_PreloadShaders( resMngr )); }
            StringView  DbgName ()  C_NE_OV { return "PreloadShadersTask"; }
        };

        class CompilePipelinesTask final : public Threading::IAsyncTask
        {
        public:
            RC<RenderTech>      rtech;
            ResMngr_t &         resMngr;
            PipelineCacheID     cacheId;
            PplnSpecIter_t      beginIt;
            PplnSpecIter_t      endIt;

            CompilePipelinesTask (RC<RenderTech> rt, ResMngr_t& rm, PipelineCacheID cache, PplnSpecIter_t begin, PplnSpecIter_t end) __NE___ :
                IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm}, cacheId{cache}, beginIt{begin}, endIt{end} {}

            void        Run ()      __Th_OV { CHECK_TE( rtech->_CompilePipelines( resMngr, cacheId, beginIt, endIt )); }
            StringView  DbgName ()  C_NE_OV { return "CompilePipelinesTask"; }
        };

        class CreateSBTsTask final : public Threading::IAsyncTask
        {
        public:
            RC<RenderTech>  rtech;
            ResMngr_t &     resMngr;

            CreateSBTsTask (RC<RenderTech> rt, ResMngr_t& rm) __NE___ :
                IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm} {}

            void        Run ()      __Th_OV { CHECK_TE( rtech->_PreloadShaders( resMngr )); }
            StringView  DbgName ()  C_NE_OV { return "CreateSBTsTask"; }
        };
        //-------------------------------------------------


        RC<RenderTech>  rt              = this->GetRC<RenderTech>();
        AsyncTask       preload         = Scheduler().Run<PreloadShadersTask>( Tuple{ rt, ArgRef(resMngr) });

        const usize     max_tasks       = 16;
        const usize     min_ppln_count  = 32;
        const usize     ppln_per_task   = Max( min_ppln_count, DivCeil( _pipelines.size(), max_tasks ));
        usize           task_count      = 0;
        AsyncTask       compile_tasks   [max_tasks];

        auto            begin_it        = _pipelines.begin();
        auto            it              = begin_it;

        // TODO:
        //  v1 - divide by same blocks and run in parallel
        //  v2 - use spinlock + single iterator, because pipeline compilation is slow but depends on complexity, one thread may be too slow

        for (; task_count < max_tasks; ++task_count)
        {
            begin_it = it;
            for (usize i = 0; (i < ppln_per_task) & (it != _pipelines.end()); ++i, ++it) {}

            compile_tasks[task_count] = Scheduler().Run<CompilePipelinesTask>(
                                            Tuple{ rt, ArgRef(resMngr), cacheid, begin_it, it },
                                            Tuple{ preload });
        }
        ASSERT( it == _pipelines.end() );
        ASSERT( task_count > 0 );

        if ( not _rtSbtMap.empty() )
        {
            auto    task = Scheduler().Run<CreateSBTsTask>(
                                Tuple{ rt, ArgRef(resMngr) },
                                Tuple{ArrayView<AsyncTask>{ compile_tasks, task_count }} );
            task_count       = 1;
            compile_tasks[0] = RVRef(task);
        }

        return MakePromiseFromValue(
                    RenderTechPipelinesPtr{ rt },
                    Tuple{ArrayView<AsyncTask>{ compile_tasks, task_count }} );
    }

/*
=================================================
    Load
=================================================
*/
    bool  PPLNPACK::RenderTech::Load (ResMngr_t &resMngr, PipelineCacheID cacheid) __NE___
    {
        {
            DRC_SHAREDLOCK( _drCheck );

            if ( _isLoaded )
                return true;

            if ( _wasAttampToLoad )
                return false;
        }

        DRC_EXLOCK( _drCheck );     // TODO

        if ( _wasAttampToLoad )
            return false;

        _wasAttampToLoad = true;


        CHECK_ERR( _PreloadShaders( resMngr ));
        CHECK_ERR( _CompilePipelines( resMngr, cacheid, _pipelines.begin(), _pipelines.end() ));
        CHECK_ERR( _CreateSBTs( resMngr ));

        _isLoaded = true;
        return true;
    }

/*
=================================================
    _CompilePipelines
----
    May be slow because of pipeline optimization in driver.
=================================================
*/
    bool  PPLNPACK::RenderTech::_CompilePipelines (ResMngr_t &resMngr, PipelineCacheID cacheid,
                                                   const PplnSpecIter_t beginIt, const PplnSpecIter_t endIt) __NE___
    {
        for (auto it = beginIt; it != endIt; ++it)
        {
            #ifdef AE_DEBUG
            String      dbg_name = resMngr.HashToName( it->first );
            #else
            StringView  dbg_name;
            #endif
            auto&       info    = it->second;

            BEGIN_ENUM_CHECKS();
            switch ( info.uid & PipelineSpecUID::_Mask )
            {
                case PipelineSpecUID::Graphics :
                {
                    auto [spec, tmpl] = _Extract< PipelineSpecUID::Graphics, PipelineTemplUID::Graphics >(
                                                resMngr, info.uid, _pack._serGPplnSpec, _pack._serGPplnTempl,
                                                *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets ));
                    CHECK_ERR( spec != null and tmpl != null );
                    info.pplnId = _CompileGraphicsPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
                    break;
                }

                case PipelineSpecUID::Mesh :
                {
                    auto [spec, tmpl] = _Extract< PipelineSpecUID::Mesh, PipelineTemplUID::Mesh >(
                                                resMngr, info.uid, _pack._serMPplnSpec, _pack._serMPplnTempl,
                                                *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets ));
                    CHECK_ERR( spec != null and tmpl != null );
                    info.pplnId = _CompileMeshPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
                    break;
                }

                case PipelineSpecUID::Compute :
                {
                    auto [spec, tmpl] = _Extract< PipelineSpecUID::Compute, PipelineTemplUID::Compute >(
                                                resMngr, info.uid, _pack._serCPplnSpec, _pack._serCPplnTempl,
                                                *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets ));
                    CHECK_ERR( spec != null and tmpl != null );
                    info.pplnId = _CompileComputePipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
                    break;
                }

                case PipelineSpecUID::RayTracing :
                {
                    auto [spec, tmpl] = _Extract< PipelineSpecUID::RayTracing, PipelineTemplUID::RayTracing >(
                                                resMngr, info.uid, _pack._serRTPplnSpec, _pack._serRTPplnTempl,
                                                *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets ));
                    CHECK_ERR( spec != null and tmpl != null );
                    info.pplnId = _CompileRayTracingPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
                    break;
                }

                case PipelineSpecUID::Tile :
                {
                    auto [spec, tmpl] = _Extract< PipelineSpecUID::Tile, PipelineTemplUID::Tile >(
                                                resMngr, info.uid, _pack._serTPplnSpec, _pack._serTPplnTempl,
                                                *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets ));
                    CHECK_ERR( spec != null and tmpl != null );
                    info.pplnId = _CompileTilePipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
                    break;
                }

                case PipelineSpecUID::_Mask :
                case PipelineSpecUID::Unknown :
                default_unlikely :                  break;
            }
            END_ENUM_CHECKS();
            CHECK_ERR( info.pplnId );
        }
        return true;
    }

/*
=================================================
    _CreateSBTs
----
    Internally use RT pipeline, so should not run in parallel with '_CompilePipelines()'.
=================================================
*/
    bool  PPLNPACK::RenderTech::_CreateSBTs (ResMngr_t &resMngr) __NE___
    {
        for (auto& [name, info] : _rtSbtMap)
        {
            CHECK_ERR( info.uid < _pack._serRTSBTs.Get<0>() );

            #ifdef AE_DEBUG
            String      dbg_name = resMngr.HashToName( name );
            #else
            StringView  dbg_name;
            #endif

            info.sbtId = _CreateRTShaderBinding( resMngr, _pack._serRTSBTs.Get<1>()[ info.uid ], dbg_name );
            CHECK_ERR( info.sbtId );
        }
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  PPLNPACK::RenderTech::Destroy (ResMngr_t &resMngr) __NE___
    {
        for (auto& [name, info] : _pipelines)
        {
            BEGIN_ENUM_CHECKS();
            switch ( info.uid & PipelineSpecUID::_Mask )
            {
                case PipelineSpecUID::Graphics :    { Strong<GraphicsPipelineID>    id{ info.pplnId.Index(), info.pplnId.Generation() };    resMngr.ImmediatelyRelease( id );   break; }
                case PipelineSpecUID::Mesh :        { Strong<MeshPipelineID>        id{ info.pplnId.Index(), info.pplnId.Generation() };    resMngr.ImmediatelyRelease( id );   break; }
                case PipelineSpecUID::Compute :     { Strong<ComputePipelineID>     id{ info.pplnId.Index(), info.pplnId.Generation() };    resMngr.ImmediatelyRelease( id );   break; }
                case PipelineSpecUID::RayTracing :  { Strong<RayTracingPipelineID>  id{ info.pplnId.Index(), info.pplnId.Generation() };    resMngr.ImmediatelyRelease( id );   break; }
                case PipelineSpecUID::Tile :        { Strong<TilePipelineID>        id{ info.pplnId.Index(), info.pplnId.Generation() };    resMngr.ImmediatelyRelease( id );   break; }
                case PipelineSpecUID::_Mask :
                case PipelineSpecUID::Unknown :
                default_unlikely :                  DBG_WARNING( "unknown pipeline type" ); break;
            }
            END_ENUM_CHECKS();
        }
        _pipelines.clear();

        for (auto& [name, sbt] : _rtSbtMap)
        {
            Strong<RTShaderBindingID>   id{ sbt.sbtId };
            CHECK( resMngr.ImmediatelyRelease( id ) == 0 );
        }
        _rtSbtMap.clear();
    }

/*
=================================================
    _Extract
=================================================
*/
    template <PipelineSpecUID SpecMask, PipelineTemplUID TemplMask, typename SpecType, typename TemplType>
    Pair< const typename TypeList<SpecType>::template Get<1>, const typename TypeList<TemplType>::template Get<1> >
        PPLNPACK::RenderTech::_Extract (const ResMngr_t &resMngr, PipelineSpecUID specUID, SpecType &specArr, TemplType &templArr,
                                        const FeatureNames_t &unsupportedFS
                                        DEBUG_ONLY(, const FeatureNames_t &allFeatureSets )) __NE___
    {
        Unused( resMngr );

        CHECK_ERR( AllBits( specUID, SpecMask ));
        const uint  spec_idx    = uint(specUID & ~PipelineSpecUID::_Mask);

        CHECK_ERR( spec_idx < specArr.template Get<0>() );
        auto&       spec        = specArr.template Get<1>()[ spec_idx ];

        CHECK_ERR( AllBits( spec.templUID, TemplMask ));
        const uint  tmpl_idx    = uint(spec.templUID & ~PipelineTemplUID::_Mask);

        CHECK_ERR( tmpl_idx < templArr.template Get<0>() );
        auto&       tmpl        = templArr.template Get<1>()[ tmpl_idx ];

        bool        supported   = true;
        for (auto& fs_name : tmpl.features)
        {
            supported &= (not unsupportedFS.contains( fs_name ));
            DEBUG_ONLY(
                if ( not allFeatureSets.contains( fs_name ))
                    AE_LOG_SE( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
            )
        }
        CHECK_ERR( supported );

        return { &spec, &tmpl };
    }

/*
=================================================
    _PreloadShaders
----
    Preload shaders to avoid stalls on file IO.

    Possible optimizations:
    * batching:
        - use fixed buffer to load shader bytecode
        - use async read file
        - create shaders
    * sort shaders to load them in single read op
=================================================
*/
    bool  PPLNPACK::RenderTech::_PreloadShaders (const ResMngr_t &resMngr) __NE___
    {
        FlatHashSet< ShaderUID >    shaders;
        shaders.reserve( _pipelines.size() * 2 );

        for (auto& [name, info] : _pipelines)
        {
            BEGIN_ENUM_CHECKS();
            switch ( info.uid & PipelineSpecUID::_Mask )
            {
                case PipelineSpecUID::Graphics :
                {
                    auto*   tmpl = _Extract< PipelineSpecUID::Graphics, PipelineTemplUID::Graphics >(
                                            resMngr, info.uid, _pack._serGPplnSpec, _pack._serGPplnTempl,
                                            *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets )).second;
                    CHECK_ERR( tmpl != null );
                    for (auto sh : tmpl->shaders)
                        shaders.insert( sh.second );
                    break;
                }
                case PipelineSpecUID::Mesh :
                {
                    auto*   tmpl = _Extract< PipelineSpecUID::Mesh, PipelineTemplUID::Mesh >(
                                            resMngr, info.uid, _pack._serMPplnSpec, _pack._serMPplnTempl,
                                            *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets )).second;
                    CHECK_ERR( tmpl != null );
                    for (auto sh : tmpl->shaders)
                        shaders.insert( sh.second );
                    break;
                }
                case PipelineSpecUID::Compute :
                {
                    auto*   tmpl = _Extract< PipelineSpecUID::Compute, PipelineTemplUID::Compute >(
                                            resMngr, info.uid, _pack._serCPplnSpec, _pack._serCPplnTempl,
                                            *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets )).second;
                    CHECK_ERR( tmpl != null );
                    shaders.insert( tmpl->shader );
                    break;
                }
                case PipelineSpecUID::RayTracing :
                {
                    auto*   tmpl = _Extract< PipelineSpecUID::RayTracing, PipelineTemplUID::RayTracing >(
                                            resMngr, info.uid, _pack._serRTPplnSpec, _pack._serRTPplnTempl,
                                            *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets )).second;
                    CHECK_ERR( tmpl != null );
                    for (auto& sh : tmpl->shaderArr)
                        shaders.insert( sh.Get<ShaderUID>() );
                    break;
                }
                case PipelineSpecUID::Tile :
                {
                    auto*   tmpl = _Extract< PipelineSpecUID::Tile, PipelineTemplUID::Tile >(
                                            resMngr, info.uid, _pack._serTPplnSpec, _pack._serTPplnTempl,
                                            *_pack._unsupportedFS DEBUG_ONLY(, *_pack._allFeatureSets )).second;
                    CHECK_ERR( tmpl != null );
                    shaders.insert( tmpl->shader );
                    break;
                }
                case PipelineSpecUID::_Mask :
                case PipelineSpecUID::Unknown :
                default_unlikely :                  break;
            }
            END_ENUM_CHECKS();
        }

        for (auto uid : shaders)
        {
            auto    ref = _pack._GetShader( resMngr, uid, EShader::Unknown );
            CHECK_ERR( ref.IsValid() );
        }
        return true;
    }

/*
=================================================
    _CompileGraphicsPipeline
=================================================
*/
    PPLNPACK::RenderTech::PipelineID
        PPLNPACK::RenderTech::_CompileGraphicsPipeline (ResMngr_t &resMngr,
                                                        const PipelineCompiler::SerializableGraphicsPipelineSpec &inSpec,
                                                        const PipelineCompiler::SerializableGraphicsPipeline &tmpl,
                                                        PipelineCacheID cacheId,
                                                        StringView dbgName) __NE___
    {
        SerializableGraphicsPipelineSpec    spec = inSpec;
        spec.desc.renderStatePtr    = _pack._GetRenderState( spec.rStateUID );
        spec.desc.dbgName           = dbgName;

        FixedArray< ShaderModuleRef, 5 >    shaders;
        for (auto [type, uid] : tmpl.shaders) {
            shaders.push_back( _pack._GetShader( resMngr, uid, type ));
        }

        auto    id = resMngr.CreatePipeline( GraphicsPipeline_t::CreateInfo{
                                                _pack,
                                                tmpl, spec.desc,
                                                _pack._GetPipelineLayout( tmpl.layout ),
                                                shaders, cacheId,
                                                _pack._allocator.get()
                                            }).Release();
        return PipelineID{ id.Index(), id.Generation() };
    }

/*
=================================================
    _CompileMeshPipeline
=================================================
*/
    PPLNPACK::RenderTech::PipelineID
        PPLNPACK::RenderTech::_CompileMeshPipeline (ResMngr_t &resMngr,
                                                    const PipelineCompiler::SerializableMeshPipelineSpec &inSpec,
                                                    const PipelineCompiler::SerializableMeshPipeline &tmpl,
                                                    PipelineCacheID cacheId,
                                                    StringView dbgName) __NE___
    {
        SerializableMeshPipelineSpec    spec = inSpec;
        spec.desc.renderStatePtr    = _pack._GetRenderState( spec.rStateUID );
        spec.desc.dbgName           = dbgName;

        FixedArray< ShaderModuleRef, 3 >    shaders;
        for (auto [type, uid] : tmpl.shaders) {
            shaders.push_back( _pack._GetShader( resMngr, uid, type ));
        }

        auto    id = resMngr.CreatePipeline( MeshPipeline_t::CreateInfo{
                                                _pack,
                                                tmpl, spec.desc,
                                                _pack._GetPipelineLayout( tmpl.layout ),
                                                shaders, cacheId,
                                                _pack._allocator.get()
                                            }).Release();
        return PipelineID{ id.Index(), id.Generation() };
    }

/*
=================================================
    _CompileComputePipeline
=================================================
*/
    PPLNPACK::RenderTech::PipelineID
        PPLNPACK::RenderTech::_CompileComputePipeline (ResMngr_t &resMngr,
                                                       const PipelineCompiler::SerializableComputePipelineSpec &inSpec,
                                                       const PipelineCompiler::SerializableComputePipeline &tmpl,
                                                       PipelineCacheID cacheId,
                                                       StringView dbgName) __NE___
    {
        SerializableComputePipelineSpec spec = inSpec;
        spec.desc.dbgName   = dbgName;

        auto    id = resMngr.CreatePipeline( ComputePipeline_t::CreateInfo{
                                                tmpl, spec.desc,
                                                _pack._GetPipelineLayout( tmpl.layout ),
                                                _pack._GetShader( resMngr, tmpl.shader, EShader::Compute ), cacheId
                                            }).Release();
        return PipelineID{ id.Index(), id.Generation() };
    }

/*
=================================================
    _CompileTilePipeline
=================================================
*/
    PPLNPACK::RenderTech::PipelineID
        PPLNPACK::RenderTech::_CompileTilePipeline (ResMngr_t &resMngr,
                                                    const PipelineCompiler::SerializableTilePipelineSpec &inSpec,
                                                    const PipelineCompiler::SerializableTilePipeline &tmpl,
                                                    PipelineCacheID cacheId,
                                                    StringView dbgName) __NE___
    {
        SerializableTilePipelineSpec    spec = inSpec;
        spec.desc.dbgName   = dbgName;

        auto    id = resMngr.CreatePipeline( TilePipeline_t::CreateInfo{
                                                _pack,
                                                tmpl, spec.desc,
                                                _pack._GetPipelineLayout( tmpl.layout ),
                                                _pack._GetShader( resMngr, tmpl.shader, EShader::Tile ), cacheId
                                            }).Release();
        return PipelineID{ id.Index(), id.Generation() };
    }

/*
=================================================
    _CompileRayTracingPipeline
=================================================
*/
    PPLNPACK::RenderTech::PipelineID
        PPLNPACK::RenderTech::_CompileRayTracingPipeline (ResMngr_t &resMngr,
                                                          const PipelineCompiler::SerializableRayTracingPipelineSpec &inSpec,
                                                          const PipelineCompiler::SerializableRayTracingPipeline &tmpl,
                                                          PipelineCacheID cacheId,
                                                          StringView dbgName) __NE___
    {
        SerializableRayTracingPipelineSpec  spec = inSpec;
        spec.desc.dbgName   = dbgName;

        TempLinearAllocator_t   allocator;

        auto*   shaders = allocator.Allocate< ShaderModuleRef >( tmpl.shaderArr.size() );
        CHECK_ERR( shaders != null );

        for (usize i = 0; i < tmpl.shaderArr.size(); ++i)
        {
            auto&   sh = tmpl.shaderArr[i];
            shaders[i] = _pack._GetShader( resMngr, sh.Get<ShaderUID>(), sh.Get<EShader>() );
        }

        auto    id = resMngr.CreatePipeline( RayTracingPipeline_t::CreateInfo{
                                                tmpl, spec.desc,
                                                _pack._GetPipelineLayout( tmpl.layout ),
                                                ArrayView< ShaderModuleRef >{ shaders, tmpl.shaderArr.size() }, cacheId,
                                                _pack._allocator.get(),
                                                &allocator
                                            }).Release();
        return PipelineID{ id.Index(), id.Generation() };
    }

/*
=================================================
    _PrintPipelines
=================================================
*/
    void  PPLNPACK::RenderTech::_PrintPipelines (const PipelineName &reqName, PipelineSpecUID mask) C_NE___
    {
    #ifdef AE_DEBUG
        const auto& res_mngr = GraphicsScheduler().GetResourceManager();
        String      str;

        for (auto& [name, ppln] : _pipelines)
        {
            if ( AllBits( ppln.uid, mask ))
                str << "'" << res_mngr.HashToName( name ) << "', ";
        }

        if ( str.empty() )
            return;

        str.pop_back();
        str.pop_back();

        BEGIN_ENUM_CHECKS();
        switch ( mask )
        {
            case PipelineSpecUID::Graphics :    "Available graphics pipelines:\n"    >> str;    break;
            case PipelineSpecUID::Mesh :        "Available mesh pipelines:\n"        >> str;    break;
            case PipelineSpecUID::Compute :     "Available compute pipelines:\n"     >> str;    break;
            case PipelineSpecUID::RayTracing :  "Available ray tracing pipelines:\n" >> str;    break;
            case PipelineSpecUID::Tile :        "Available tile pipelines:\n"        >> str;    break;
            case PipelineSpecUID::_Mask :       break;
            case PipelineSpecUID::Unknown :     break;
        }
        END_ENUM_CHECKS();

        ("Can't find pipeline '"s << res_mngr.HashToName( reqName ) << "'\n") >> str;

        AE_LOGI( str );
    #else
        Unused( mask, reqName );
    #endif
    }

/*
=================================================
    GetGraphicsPipeline
=================================================
*/
    GraphicsPipelineID  PPLNPACK::RenderTech::GetGraphicsPipeline (const PipelineName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _pipelines.find( name );
        if_unlikely( iter == _pipelines.end() )
        {
            _PrintPipelines( name, PipelineSpecUID::Graphics );
            return Default;
        }

        CHECK_ERR( AllBits( iter->second.uid, PipelineSpecUID::Graphics ));

        return iter->second.Cast<GraphicsPipelineID>();
    }

/*
=================================================
    GetMeshPipeline
=================================================
*/
    MeshPipelineID  PPLNPACK::RenderTech::GetMeshPipeline (const PipelineName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _pipelines.find( name );
        if_unlikely( iter == _pipelines.end() )
        {
            _PrintPipelines( name, PipelineSpecUID::Mesh );
            return Default;
        }

        CHECK_ERR( AllBits( iter->second.uid, PipelineSpecUID::Mesh ));

        return iter->second.Cast<MeshPipelineID>();
    }

/*
=================================================
    GetTilePipeline
=================================================
*/
    TilePipelineID  PPLNPACK::RenderTech::GetTilePipeline (const PipelineName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _pipelines.find( name );
        if_unlikely( iter == _pipelines.end() )
        {
            _PrintPipelines( name, PipelineSpecUID::Tile );
            return Default;
        }

        CHECK_ERR( AllBits( iter->second.uid, PipelineSpecUID::Tile ));

        return iter->second.Cast<TilePipelineID>();
    }

/*
=================================================
    GetComputePipeline
=================================================
*/
    ComputePipelineID  PPLNPACK::RenderTech::GetComputePipeline (const PipelineName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _pipelines.find( name );
        if_unlikely( iter == _pipelines.end() )
        {
            _PrintPipelines( name, PipelineSpecUID::Compute );
            return Default;
        }

        CHECK_ERR( AllBits( iter->second.uid, PipelineSpecUID::Compute ));

        return iter->second.Cast<ComputePipelineID>();
    }

/*
=================================================
    GetRayTracingPipeline
=================================================
*/
    RayTracingPipelineID  PPLNPACK::RenderTech::GetRayTracingPipeline (const PipelineName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _pipelines.find( name );
        if_unlikely( iter == _pipelines.end() )
        {
            _PrintPipelines( name, PipelineSpecUID::RayTracing );
            return Default;
        }

        CHECK_ERR( AllBits( iter->second.uid, PipelineSpecUID::RayTracing ));

        return iter->second.Cast<RayTracingPipelineID>();
    }

/*
=================================================
    _PrintSBTs
=================================================
*/
    void  PPLNPACK::RenderTech::_PrintSBTs (const RTShaderBindingName &reqName) C_NE___
    {
    #ifdef AE_DEBUG
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        String  str;

        for (auto& [name, info] : _rtSbtMap) {
            str << "'" << res_mngr.HashToName( name ) << "', ";
        }

        if ( str.empty() )
            return;

        ("Can't find SBT '"s << res_mngr.HashToName( reqName ) << "'\nAvailable:\n") >> str;

        AE_LOGI( str );
    #else
        Unused( reqName );
    #endif
    }

/*
=================================================
    GetRTShaderBinding
=================================================
*/
    RTShaderBindingID  PPLNPACK::RenderTech::GetRTShaderBinding (const RTShaderBindingName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _rtSbtMap.find( name );
        if_unlikely( iter == _rtSbtMap.end() )
        {
            _PrintSBTs( name );
            return Default;
        }

        return iter->second.sbtId;
    }

/*
=================================================
    FeatureSetSupported
=================================================
*/
    bool  PPLNPACK::RenderTech::FeatureSetSupported (const FeatureSetName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _pack._allFeatureSets->contains( name ));

        return not _pack._unsupportedFS->contains( name );
    }

/*
=================================================
    GetAttachmentFormat
=================================================
*/
    EPixelFormat  PPLNPACK::RenderTech::GetAttachmentFormat (const RenderTechPassName &passName, const AttachmentName &attName) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        const auto  pass = GetPass( passName );
        if ( pass.IsDefined() )
        {
            auto&           res_mngr = GraphicsScheduler().GetResourceManager();
            RenderPassID    rp_id    = res_mngr.GetRenderPass( pass.packId, RenderPassName{pass.renderPass} );

            if ( rp_id != Default )
                return res_mngr.GetResource( rp_id )->GetPixelFormat( attName );
        }
        return Default;
    }

/*
=================================================
    Begin
=================================================
*
    bool  PPLNPACK::RenderTech::Begin ()
    {
        DRC_SHAREDLOCK( _drCheck );

        uint    old_idx = _passIdx.exchange( 0 );
        CHECK_ERR( old_idx >= _passes.Get<0>() );

        return true;
    }

/*
=================================================
    NextPass
=================================================
*
    bool  PPLNPACK::RenderTech::NextPass (const RenderTechPassName &pass)
    {
        DRC_SHAREDLOCK( _drCheck );

        const uint  idx = _passIdx.fetch_add( 1 );
        CHECK_ERR( idx < _passes.Get<0>() );

        const auto  req_name = _passes.Get<1>()[ idx ].name;
        if_unlikely( req_name != pass )
        {
            auto&   res_mngr = GraphicsScheduler().GetResourceManager();
            AE_LOG_DBG( "Render technique pass '"s << res_mngr.HashToName( pass ) << "' is not the (" << ToString(idx) << ") pass, '" <<
                        res_mngr.HashToName( req_name ) << "' is required" );
            return false;
        }
        return true;
    }

/*
=================================================
    End
=================================================
*
    bool  PPLNPACK::RenderTech::End ()
    {
        DRC_SHAREDLOCK( _drCheck );

        const uint  idx = _passIdx.fetch_add( 1 );
        CHECK_ERR( idx == _passes.Get<0>() );

        return true;
    }

/*
=================================================
    GetPass
=================================================
*/
    IRenderTechPipelines::PassInfo  PPLNPACK::RenderTech::GetPass (const RenderTechPassName &passName) C_NE___
    {
        // TODO: optimize
        for (uint i = 0, cnt = _passes.Get<0>(); i < cnt; ++i)
        {
            auto&   pass = _passes.Get<1>()[i];

            if ( pass.name == passName )
            {
                PassInfo    info;
                info.dsIndex    = DescSetBinding{0};    // TODO
                info.type       = pass.IsGraphics() ? EPassType::Graphics : EPassType::Compute;
                info.renderPass = pass.renderPass;
                info.subpass    = pass.subpass;
                info.packId     = _pack._parentPackId;

                if ( uint(pass.dsLayout) < _pack._dsLayouts.Get<0>() )
                {
                    info.dsLayoutId = _pack._dsLayouts.Get<1>()[ uint(pass.dsLayout) ];
                }
                return info;
            }
        }

        RETURN_ERR( "unknown render technique pass" );
    }


} // AE::Graphics
