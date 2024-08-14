// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define PPLNPACK			VPipelinePack

#elif defined(AE_ENABLE_METAL)
#	define PPLNPACK			MPipelinePack

#else
#	error not implemented
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

		_parentPack		= null;
		_parentPackId	= Default;
		_surfaceFormat	= Default;
		_shaders		= Default;
		_shaderOffset	= Default;
		_shaderDataSize	= Default;

		// none of the resources has strong reference on sampler
		for (auto& [name, id] : *_samplerRefs) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}
		_samplerRefs.Destroy();

		// none of the resources has strong reference on render pass
		for (auto& [name, id] : _renderPassRefs->specMap) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}

		// release pipelines
		for (auto& rt : _renTechs)
		{
			rt->Destroy( resMngr );
			PlacementDelete( INOUT rt );
		}
		_renTechs = Default;

		// pipelines has strong reference on compatible render pass
		for (auto& [name, id] : _renderPassRefs->compatMap) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}
		_renderPassRefs.Destroy();

		// pipelines has strong reference on pipeline layout
		for (auto& pl : _pplnLayouts) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT pl ));
		}
		_pplnLayouts = Default;

		// pipeline layout has strong reference on descriptor set layout
		for (auto& dsl : _dsLayouts) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT dsl ));
		}
		_dsLayouts = Default;

		GFX_DBG_ONLY(
			_allFeatureSets.Destroy();
		)
		_unsupportedFS.Destroy();
		_pplnTemplMap.Destroy();
		_dsLayoutMap.Destroy();
		_renTechMap.Destroy();

		_serGPplnTempl	= Default;
		_serMPplnTempl	= Default;
		_serCPplnTempl	= Default;
		_serRTPplnTempl	= Default;

		_serGPplnSpec	= Default;
		_serMPplnSpec	= Default;
		_serCPplnSpec	= Default;
		_serRTPplnSpec	= Default;

		ASSERT( _allocator.use_count() == 1 );
		_allocator = null;

		GFX_DBG_ONLY( _debugName.clear() );
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

		_unsupportedFS	.CreateTh( StdAlloc_t< FeatureSetName::Optimized_t											 >{ _allocator.get() });
		_renTechMap		.CreateTh( StdAlloc_t< Pair< const RenderTechName::Optimized_t,		ushort					>>{ _allocator.get() });
		_pplnTemplMap	.CreateTh( StdAlloc_t< Pair< const PipelineTmplName::Optimized_t,	PipelineTemplUID		>>{ _allocator.get() });
		_dsLayoutMap	.CreateTh( StdAlloc_t< Pair< const DescriptorSetName::Optimized_t,	DescriptorSetLayoutID	>>{ _allocator.get() });
		_samplerRefs	.CreateTh( StdAlloc_t< Pair< const SamplerName::Optimized_t,		SamplerID				>>{ _allocator.get() });
		_renderPassRefs	.CreateTh( _allocator.get() );
		GFX_DBG_ONLY( _allFeatureSets.CreateTh( StdAlloc_t< FeatureSetName::Optimized_t >{ _allocator.get() });)

		EXLOCK( _fileGuard );

		_file			= desc.stream;
		_surfaceFormat	= desc.surfaceFormat;

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
				_parentPack = resMngr.GetResource( _parentPackId );
				const auto	parent_surf_fmt	= _parentPack->GetSurfaceFormat();

				CHECK_ERR( _surfaceFormat == parent_surf_fmt	or
						   _surfaceFormat == Default			or
						   parent_surf_fmt == Default );
			}
		}

		const Bytes	base_offset = _file->Position();

		uint	hdr_name = 0;
		CHECK_ERR( _file->Read( OUT hdr_name ) and hdr_name == PackOffsets_Name );

		PipelinePackOffsets		offsets;
		CHECK_ERR( _file->Read( OUT offsets ));

		if ( AllBits( desc.options, EPipelinePackOpt::Pipelines ) and offsets.shaderDataSize > 8 )
		{
			_shaderOffset	= offsets.shaderOffset   + 8_b + base_offset;	// skip block name and version
			_shaderDataSize	= offsets.shaderDataSize - 8_b;
			CHECK_ERR( offsets.shaderOffset + offsets.shaderDataSize <= _file->Size() );
		}

		if ( base_offset + offsets.nameMappingOffset < _file->Size() )
			CHECK_ERR( _LoadNameMapping( resMngr, base_offset + offsets.nameMappingOffset, offsets.nameMappingDataSize ));

		if ( AllBits( desc.options, EPipelinePackOpt::FeatureSets ) and (base_offset + offsets.featureSetOffset < _file->Size()) ) {
			CHECK_ERR( _LoadFeatureSets( resMngr, base_offset + offsets.featureSetOffset, offsets.featureSetDataSize ));	// throw
		}else{
			CHECK_ERR( _CopyFeatureSets() );
		}

		if ( AllBits( desc.options, EPipelinePackOpt::Samplers ) and (base_offset + offsets.samplerOffset < _file->Size()) )
			CHECK_ERR( _LoadSamplers( resMngr, base_offset + offsets.samplerOffset, offsets.samplerDataSize ));				// throw

		if ( AllBits( desc.options, EPipelinePackOpt::RenderPasses ) and (base_offset + offsets.renderPassOffset < _file->Size()) )
			CHECK_ERR( _LoadRenderPasses( resMngr, base_offset + offsets.renderPassOffset, offsets.renderPassDataSize ));	// throw

		if ( AllBits( desc.options, EPipelinePackOpt::Pipelines ) and (base_offset + offsets.pipelineOffset < _file->Size()) )
			CHECK_ERR( _LoadPipelineBlock( resMngr, base_offset + offsets.pipelineOffset, offsets.pipelineDataSize ));		// throw

		GFX_DBG_ONLY( _debugName = desc.dbgName; )
		return true;
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	RenderPassID  PPLNPACK::GetRenderPass (RenderPassName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _renderPassRefs->specMap.find( name );

		if_likely( it != _renderPassRefs->specMap.end() )
			return it->second;

		// search in parent pack
		if_likely( _parentPack != null )
			return _parentPack->GetRenderPass( name );

		return Default;
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	RenderPassID  PPLNPACK::GetRenderPass (CompatRenderPassName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _renderPassRefs->compatMap.find( name );

		if_likely( it != _renderPassRefs->compatMap.end() )
			return it->second;

		// search in parent pack
		if_likely( _parentPack != null )
			return _parentPack->GetRenderPass( name );

		return Default;
	}

/*
=================================================
	GetSampler
=================================================
*/
	SamplerID  PPLNPACK::GetSampler (SamplerName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _samplerRefs->find( SamplerName::Optimized_t{name} );

		if_likely( it != _samplerRefs->end() )
			return it->second;

		// search in parent pack
		if_likely( _parentPack != null )
			return _parentPack->GetSampler( name );

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

		Array<RenderTechName>	supported;
		supported.reserve( _renTechs.size() / 2 );	// throw

		for (auto& [name, idx] : *_renTechMap)
		{
			auto&	rt = *_renTechs[ idx ];
			if ( rt.IsSupported() )
				supported.push_back( RenderTechName{ name });	// throw
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
		CHECK_ERR( usize(uid) < _pplnLayouts.size() );

		return _pplnLayouts[ usize(uid) ];
	}

/*
=================================================
	_Extract
=================================================
*/
	template <PipelineCompiler::PipelineTemplUID TemplMask, typename TemplType>
	const typename TemplType::value_type*
		PPLNPACK::_Extract (ResMngr_t &resMngr, PipelineTmplName::Ref name, const TemplType &templArr) C_NE___
	{
		auto	it = _pplnTemplMap->find( name );
		CHECK_ERR( it != _pplnTemplMap->end() );

		CHECK_ERR( AllBits( it->second, TemplMask ));

		const uint	idx = uint(it->second) & ~uint(PipelineTemplUID::_Mask);
		CHECK_ERR( idx < templArr.size() );

		auto&	ppln		= templArr[ idx ];
		bool	supported	= true;

		for (auto& fs_name : ppln.features)
		{
			supported &= (not _unsupportedFS->contains( fs_name ));
			GFX_DBG_ONLY(
				if ( not _allFeatureSets->contains( fs_name ))
					AE_LOGW( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
			)
		}

		if_unlikely( not supported )
		{
			GFX_DBG_ONLY( AE_LOGW( "Pipeline template '"s << resMngr.HashToName( name ) << "' is NOT supported" ));
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
	Strong<ComputePipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, PipelineTmplName::Ref name,
														 const ComputePipelineDesc &desc, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	cppln = _Extract< PipelineTemplUID::Compute >( resMngr, name, _serCPplnTempl );
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
	Strong<GraphicsPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, PipelineTmplName::Ref name,
														  const GraphicsPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	gppln = _Extract< PipelineTemplUID::Graphics >( resMngr, name, _serGPplnTempl );
		CHECK_ERR( gppln != null );

		FixedArray< ShaderModuleRef, 5 >	shaders;
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
	Strong<MeshPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, PipelineTmplName::Ref name,
													  const MeshPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	mppln = _Extract< PipelineTemplUID::Mesh >( resMngr, name, _serMPplnTempl );
		CHECK_ERR( mppln != null );

		FixedArray< ShaderModuleRef, 3 >	shaders;
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
	Strong<RayTracingPipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, PipelineTmplName::Ref name,
															const RayTracingPipelineDesc &desc, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	rtppln = _Extract< PipelineTemplUID::RayTracing >( resMngr, name, _serRTPplnTempl );
		CHECK_ERR( rtppln != null );

		TempLinearAllocator_t	allocator;

		auto*	shaders = allocator.Allocate< ShaderModuleRef >( rtppln->shaderArr.size() );
		CHECK_ERR( shaders != null );

		for (usize i = 0; i < rtppln->shaderArr.size(); ++i)
		{
			auto&	sh = rtppln->shaderArr[i];
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
	Strong<TilePipelineID>  PPLNPACK::CreatePipeline (ResMngr_t &resMngr, PipelineTmplName::Ref name,
													  const TilePipelineDesc &desc, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	tppln = _Extract< PipelineTemplUID::Tile >( resMngr, name, _serTPplnTempl );
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
	DescriptorSetLayoutID  PPLNPACK::GetDSLayout (DSLayoutName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _dsLayoutMap->find( name );

		if_likely( it != _dsLayoutMap->end() )
			return it->second;

		return Default;
	}

/*
=================================================
	LoadRenderTechAsync
=================================================
*/
	Promise<RenderTechPipelinesPtr>  PPLNPACK::LoadRenderTechAsync (ResMngr_t &resMngr, RenderTechName::Ref name, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		ASSERT( not _renTechMap->empty() );

		auto	it = _renTechMap->find( RenderTechName::Optimized_t{name} );
		if_unlikely( it == _renTechMap->end() )
			return Default;

		CHECK_ERR( usize(it->second) < _renTechs.size() );

		auto&	rt = ConstCast(*_renTechs[ usize(it->second) ]);
		return rt.LoadAsync( resMngr, cacheId );
	}

/*
=================================================
	LoadRenderTech
=================================================
*/
	RenderTechPipelinesPtr  PPLNPACK::LoadRenderTech (ResMngr_t &resMngr, RenderTechName::Ref name, PipelineCacheID cacheId) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		ASSERT( not _renTechMap->empty() );

		auto	it = _renTechMap->find( RenderTechName::Optimized_t{name} );
		if_unlikely( it == _renTechMap->end() )
			return Default;

		CHECK_ERR( usize(it->second) < _renTechs.size() );

		auto&	rt = ConstCast(*_renTechs[ usize(it->second) ]);
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
	#if AE_DBG_GRAPHICS
		CHECK_ERR( _file->SeekSet( offset ));

		auto	mem_stream = MakeRC<ArrayRStream>();
		CHECK_ERR( mem_stream->LoadRemainingFrom( *_file, size ));

		Serializing::Deserializer	des{ RVRef(mem_stream) };
		CHECK_ERR( not des.stream.Empty() );

		uint	hdr_name	= 0;
		CHECK_ERR( des( OUT hdr_name ));
		CHECK_ERR( hdr_name == NameMapping_Name );

		PipelineCompiler::HashToName	hash_to_name;
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

		auto	mem_stream = MakeRC<ArrayRStream>();
		CHECK_ERR( mem_stream->LoadRemainingFrom( *_file, size ));

		Serializing::Deserializer	des{ RVRef(mem_stream) };
		CHECK_ERR( not des.stream.Empty() );

		uint	hdr_name	= 0;
		uint	version		= 0;
		CHECK_ERR( des( OUT hdr_name, OUT version ));
		CHECK_ERR( hdr_name == FeatureSetPack_Name and version == FeatureSetPack_Version );

		ulong	fs_hash	= 0;
		uint	count	= 0;
		CHECK_ERR( des( OUT fs_hash, OUT count ));
		CHECK_ERR( count <= FeatureSetSerializer::MaxCount );
		CHECK_ERR( fs_hash == ulong(FeatureSet::GetHashOfFS_Precalculated()) );

		size -= 8_b;	// header & version
		size -= 12_b;	// fs_hash & count
		CHECK_ERR( IsMultipleOf( size, count ));
		CHECK_ERR( size == (SizeOf<FeatureSet> + 4_b) * count );

		if_likely( _parentPack != null )
		{
			*_unsupportedFS = _parentPack->_GetUnsupportedFS();			// throw
			_unsupportedFS->reserve( _unsupportedFS->size() + count );	// throw

			GFX_DBG_ONLY(
				*_allFeatureSets = _parentPack->_GetAllFS();					// throw
				_allFeatureSets->reserve( _allFeatureSets->size() + count );	// throw
			)
		}
		else
		{
			_unsupportedFS->reserve( count );					// throw
			GFX_DBG_ONLY( _allFeatureSets->reserve( count ));	// throw
		}

		const auto&	current_fs = resMngr.GetFeatureSet();

		for (uint i = 0; i < count; ++i)
		{
			FeatureSetName::Optimized_t	fs_name;
			FeatureSet					fs;

			CHECK_ERR( des( OUT fs_name, OUT fs ));
			CHECK( fs_name.IsDefined() );
			CHECK( fs.IsValid() );

			if ( not current_fs.IsCompatible( fs ))
			{
				GFX_DBG_ONLY( AE_LOGW( "Feature set '"s << resMngr.HashToName( fs_name ) << "' is NOT supported" ));

				CHECK( _unsupportedFS->insert( fs_name ).second );	// throw
			}
			else
			{
				GFX_DBG_ONLY( AE_LOGI( "Feature set '"s << resMngr.HashToName( fs_name ) << "' is supported" ));
			}

			GFX_DBG_ONLY( CHECK( _allFeatureSets->insert( fs_name ).second );)	// throw
		}

		CHECK( des.IsEnd() );
		return true;
	}

/*
=================================================
	_CopyFeatureSets
=================================================
*/
	bool  PPLNPACK::_CopyFeatureSets () __Th___
	{
		if ( _parentPack != null )
		{
			*_unsupportedFS = _parentPack->_GetUnsupportedFS();			// throw
			GFX_DBG_ONLY( *_allFeatureSets = _parentPack->_GetAllFS();)	// throw
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

		auto	mem_stream = MakeRC<ArrayRStream>();
		CHECK_ERR( mem_stream->LoadRemainingFrom( *_file, size ));

		Serializing::Deserializer	des{ RVRef(mem_stream) };
		CHECK_ERR( not des.stream.Empty() );

		// for feature set array
		des.allocator = _allocator.get();

		uint	hdr_name	= 0;
		uint	version		= 0;
		CHECK_ERR( des( OUT hdr_name, OUT version ));
		CHECK_ERR( hdr_name == SamplerPack_Name and version == SamplerPack_Version );

		Array<Pair< SamplerName::Optimized_t, uint >>	samp_names;
		CHECK_ERR( des( OUT samp_names ));

		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= SamplerSerializer::MaxCount );

		if ( count == 0 )
			return true;

		using SamplerYcbcrArr_t = Array< Optional< SamplerYcbcrConversionDesc >>;

		Array< SamplerID >		samplers;			samplers.resize( count );
		Array< SamplerDesc >	samp_desc;			samp_desc.resize( count );
		SamplerYcbcrArr_t		ycbcr_conv;			ycbcr_conv.resize( count );
		Array< bool >			samp_supported;		samp_supported.resize( count );

		for (uint i = 0; i < count; ++i)
		{
			SamplerSerializer	samp;
			CHECK_ERR( samp.Deserialize( des ));

			bool	supported = true;
			for (auto fs_name : samp.GetFeatures())
			{
				supported &= (not _unsupportedFS->contains( fs_name ));
				GFX_DBG_ONLY(
					if ( not _allFeatureSets->contains( fs_name ))
						AE_LOGW( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
				)
			}

			samp_desc[i]		= samp.GetDesc();
			ycbcr_conv[i]		= samp.YcbcrDescOpt();
			samp_supported[i]	= supported;
		}

		_samplerRefs->reserve( count );		// throw

		for (auto& [name, uid] : samp_names)
		{
			const usize	i = usize(uid);
			CHECK_ERR( i < samplers.size() );

			if_likely( samp_supported[i] )
			{
				// if not created
				if_unlikely( samplers[i] == Default )
				{
					String	samp_name;
					GFX_DBG_ONLY( samp_name = resMngr.HashToName( name ));

					samplers[i] = _CreateSampler( resMngr, samp_desc[i], ycbcr_conv[i], samp_name );
					if ( samplers[i] == Default )
					{
						AE_LOG_DBG( "Failed to create sampler '"s << samp_name << "'" );
						samp_supported[i] = false;
						continue;
					}
				}

				if ( not _samplerRefs->insert_or_assign( name, Strong<SamplerID>{samplers[i]} ).second )  // throw
				{
					GFX_DBG_ONLY( AE_LOGW( "Sampler '"s << resMngr.HashToName( name ) << "' is already assigned" ));
				}
			}else{
				GFX_DBG_ONLY( AE_LOGW( "Sampler '"s << resMngr.HashToName( name ) << "' is NOT supported" ));
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

		auto	mem_stream = MakeRC<ArrayRStream>();
		CHECK_ERR( mem_stream->LoadRemainingFrom( *_file, size ));

		// check header & version
		{
			DefaultPackHeader	hdr;
			CHECK_ERR( mem_stream->Read( OUT hdr ));
			CHECK_ERR( hdr.name == PipelinePack_Name );
			CHECK_ERR( hdr.ver == PipelinePack_Version );
		}

		StackAllocator_t					stack_alloc;
		PipelineStorage::BlockOffsets_t		block_offsets;

		CHECK_ERR( mem_stream->Read( OUT block_offsets ));

		for (usize i = 0; i < block_offsets.size(); ++i)
		{
			const auto	base_off = block_offsets[i];
			if ( base_off == UMax )
				continue;

			Serializing::Deserializer	des{ mem_stream->ToSubStream( base_off ), _allocator.get() };
			EMarker						marker;

			CHECK_ERR( des( OUT marker ));
			CHECK_ERR( usize(marker) == i );

			switch_enum( marker )
			{
				case EMarker::RenderStates :			CHECK_ERR( _LoadRenderStates( des ));					break;
				case EMarker::DepthStencilStates :		CHECK_ERR( _LoadDepthStencilStates( resMngr, des ));	break;

				case EMarker::DescrSetLayouts :			CHECK_ERR( _LoadDescrSetLayouts( resMngr, des, stack_alloc, OUT *_dsLayoutMap ));	break;	// throw
				case EMarker::PipelineLayouts :			CHECK_ERR( _LoadPipelineLayouts( resMngr, des ));				break;

				case EMarker::PipelineTemplNames :		CHECK_ERR( _LoadPipelineNames( des, OUT *_pplnTemplMap ));		break;	// throw

				case EMarker::GraphicsPipelineTempl :	CHECK_ERR( _LoadPipelineArray( des, OUT _serGPplnTempl ));		break;
				case EMarker::MeshPipelineTempl :		CHECK_ERR( _LoadPipelineArray( des, OUT _serMPplnTempl ));		break;
				case EMarker::ComputePipelineTempl :	CHECK_ERR( _LoadPipelineArray( des, OUT _serCPplnTempl ));		break;
				case EMarker::RayTracingPipelineTempl :	CHECK_ERR( _LoadPipelineArray( des, OUT _serRTPplnTempl ));		break;
				case EMarker::TilePipelineTempl :		CHECK_ERR( _LoadPipelineArray( des, OUT _serTPplnTempl ));		break;

				case EMarker::GraphicsPipelineSpec :	CHECK_ERR( _LoadPipelineArray( des, OUT _serGPplnSpec ));		break;
				case EMarker::MeshPipelineSpec :		CHECK_ERR( _LoadPipelineArray( des, OUT _serMPplnSpec ));		break;
				case EMarker::ComputePipelineSpec :		CHECK_ERR( _LoadPipelineArray( des, OUT _serCPplnSpec ));		break;
				case EMarker::RayTracingPipelineSpec :	CHECK_ERR( _LoadPipelineArray( des, OUT _serRTPplnSpec ));		break;
				case EMarker::TilePipelineSpec :		CHECK_ERR( _LoadPipelineArray( des, OUT _serTPplnSpec ));		break;

				case EMarker::RenderTechniques :		CHECK_ERR( _LoadRenderTechniques( resMngr, des ));				break;	// throw

				case EMarker::RTShaderBindingTable :	CHECK_ERR( _LoadRTShaderBindingTable( des ));					break;

			  #ifdef AE_ENABLE_VULKAN
				case EMarker::SpirvShaders :			CHECK_ERR( _LoadShaders( des ));								break;
			  #else
				case EMarker::SpirvShaders :
			  #endif

			  #ifdef AE_PLATFORM_IOS
				case EMarker::MetaliOSShaders :			CHECK_ERR( _LoadShaders( des ));								break;
			  #else
				case EMarker::MetaliOSShaders :
			  #endif

			  #ifdef AE_PLATFORM_MACOS
				case EMarker::MetalMacShaders :			CHECK_ERR( _LoadShaders( des ));								break;
			  #else
				case EMarker::MetalMacShaders :
			  #endif

				case EMarker::Unknown :
				case EMarker::_Count :
				default_unlikely :						DBG_WARNING( "unknown marker" );								break;
			}
			switch_end
		}
		return true;
	}

/*
=================================================
	_LoadRenderStates
=================================================
*/
	bool  PPLNPACK::_LoadRenderStates (Serializing::Deserializer &des) __NE___
	{
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxStateCount );

		if ( count == 0 )
			return true;

		auto	rs_ptr = _allocator->Allocate< PipelineCompiler::SerializableRenderState >( count );
		CHECK_ERR( rs_ptr != null );
		_renderStates = ArrayView{ rs_ptr, count };

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
		const auto	idx = usize(uid);

		return	idx < _renderStates.size()  ?
					&_renderStates[idx].rs :
					null;
	}

/*
=================================================
	_LoadDepthStencilStatesImpl
=================================================
*/
	bool  PPLNPACK::_LoadDepthStencilStatesImpl (Serializing::Deserializer &des) __NE___
	{
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxStateCount );

		if ( count == 0 )
			return true;

		auto	ds_ptr = _allocator->Allocate< PipelineCompiler::SerializableDepthStencilState >( count );
		CHECK_ERR( ds_ptr != null );
		_depthStencilStates = ArrayView{ ds_ptr, count };

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
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxDSLayoutCount );

		if ( count == 0 )
			return true;

		layoutMap.reserve( count );  // throw

		_dsLayouts = MutableArrayView{ _allocator->Allocate< Strong<DescriptorSetLayoutID> >( count ), count };
		CHECK_ERR( _dsLayouts.data() != null );

		for (uint i = 0; i < count; ++i)
		{
			const auto					bm				= stackAlloc.PushAuto();
			DSLayoutName::Optimized_t	dsl_name;
			EDescSetUsage				usage			= Default;
			EShaderStages				stages			= Default;
			FSNameArr_t					features;
			UniformOffsets_t			desc_offsets	= {};
			uint						sampler_count	= 0;
			SamplerID *					samplers		= null;

			CHECK_ERR( des( OUT dsl_name, OUT usage, OUT stages, OUT features, OUT desc_offsets, OUT sampler_count ));

			const uint	uniform_count = desc_offsets.back();
			CHECK_ERR( sampler_count <= DescriptorSetLayoutDesc::MaxSamplers );
			CHECK_ERR( uniform_count <= DescriptorSetLayoutDesc::MaxUniforms );

			bool	supported = true;
			for (auto fs_name : features)
			{
				supported &= (not _unsupportedFS->contains( fs_name ));
				GFX_DBG_ONLY(
					if ( not _allFeatureSets->contains( fs_name ))
						AE_LOGW( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
				)
			}

			if ( sampler_count > 0 )
			{
				samplers = stackAlloc.Allocate< SamplerID >( sampler_count );
				CHECK_ERR( samplers != null );

				for (uint j = 0; j < sampler_count; ++j)
				{
					SamplerName::Optimized_t	samp_name;
					CHECK_ERR( des( OUT samp_name ));

					if ( not supported )
					{
						samplers[j] = Default;
						continue;
					}

					samplers[j] = GetSampler( SamplerName{samp_name} );
					if_unlikely( not samplers[j] )
					{
						GFX_DBG_ONLY(
							AE_LOGW( "Sampler '"s << resMngr.HashToName( samp_name ) << "' is used by DescriptorSetLayout '" <<
									 resMngr.HashToName( dsl_name ) << "' but not created (not supported?)" );
						)
						if ( AllBits( usage, EDescSetUsage::MaybeUnsupported ))
						{
							supported = false;
							continue;
						}
						return false;
					}
				}
			}

			const bool	is_upd_tmpl	= AllBits( usage, EDescSetUsage::UpdateTemplate );	// Vulkan only
			auto*		un_names	= _allocator->Allocate< UniformName::Optimized_t >( uniform_count );
			auto*		un_data		= _allocator->Allocate< Uniform_t >( uniform_count );
			auto*		un_offsets	= is_upd_tmpl ? _allocator->Allocate< Bytes16u >( uniform_count ) : null;
			bool		result		= true;
			CHECK_ERR( un_names != null and un_data != null );

			for (uint j = 0; result and (j < uniform_count); ++j) {
				result = des( OUT un_names[j] );
			}
			CHECK_ERR( result );

			for (uint j = 0; result and (j < uniform_count); ++j)
			{
				PlacementNew<Uniform_t>( OUT un_data + j );
				result = Deserialize_Uniform( des, sampler_count, OUT un_data[j] );
			}
			CHECK_ERR( result );

			if ( not supported )
			{
				PlacementNew< Strong<DescriptorSetLayoutID> >( OUT _dsLayouts.data() + i );

				GFX_DBG_ONLY( AE_LOGW( "DescriptorSetLayout '"s << resMngr.HashToName( dsl_name ) << "' is NOT supported" ));
				continue;
			}

			const auto	usage_mask = ~EDescSetUsage::_PrivateMask;

			Strong<DescriptorSetLayoutID>	item = _CreateDescriptorSetLayout( resMngr,
																			   Uniforms_t{ uniform_count, un_names, un_data, un_offsets },
																			   ArrayView{ samplers, sampler_count },
																			   desc_offsets,
																			   (usage & usage_mask), stages, stackAlloc );
			CHECK_ERR( item );

			layoutMap.emplace( dsl_name, DescriptorSetLayoutID{item} );  // throw

			PlacementNew< Strong<DescriptorSetLayoutID> >( OUT _dsLayouts.data() + i, RVRef(item) );
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
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxPplnLayoutCount );

		if ( count == 0 )
			return true;

		auto*	empty_ds = resMngr.GetResource( resMngr.GetEmptyDescriptorSetLayout() );
		CHECK_ERR( empty_ds != null );

		_pplnLayouts = MutableArrayView{ _allocator->Allocate< Strong<PipelineLayoutID> >( count ), count };
		CHECK_ERR( _pplnLayouts.data() != null );

		for (uint i = 0; i < count; ++i)
		{
			PipelineLayoutDesc	desc;
			CHECK_ERR( des( OUT desc ));

			PipelineLayout_t::DescriptorSets_t		desc_sets;
			bool									supported = true;

			for (usize j = 0; j < desc.descrSets.size(); ++j)
			{
				auto	src = desc.descrSets[j];
				CHECK_ERR( usize(src.second.uid) < _dsLayouts.size() );

				DescriptorSetLayoutID	id = _dsLayouts[ usize(src.second.uid) ];
				DescSetBinding			binding;

				supported &= (id != Default);

				#if defined(AE_ENABLE_VULKAN)
					binding.vkIndex = src.second.vkIndex;
				#elif defined(AE_ENABLE_METAL)
					binding.mtlIndex = src.second.mtlIndex;
				#else
				#	error not implemented
				#endif

				desc_sets.insert_or_assign( src.first, PipelineLayout_t::DescSetLayout{ id, binding });
			}

			if ( not supported )
			{
				PlacementNew< Strong<PipelineLayoutID> >( OUT _pplnLayouts.data() + i );
				GFX_DBG_ONLY( AE_LOGW( "PipelineLayout ("s << ToString(i) << ") is NOT supported" ));
				continue;
			}

			auto	item = resMngr.CreatePipelineLayout( PipelineLayout_t::CreateInfo{
															desc_sets,
															desc.pushConstants.items,
															empty_ds->Handle()
															GFX_DBG_ONLY(, "pl-"s+ToString(i))
														});
			CHECK_ERR( item );

			PlacementNew< Strong<PipelineLayoutID> >( OUT _pplnLayouts.data() + i, RVRef(item) );
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

		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxPipelineNameCount );

		if ( count == 0 )
			return true;

		nameMap.reserve( count );	// throw

		bool	result = true;
		for (uint i = 0; result and (i < count); ++i)
		{
			TName	name;
			TUID	uid;
			result = des( OUT name, OUT uid );

			auto [it, inserted] = nameMap.emplace( name, uid );	// throw
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
	bool  PPLNPACK::_LoadPipelineArray (Serializing::Deserializer &des, OUT ArrayView<T> &arr) __NE___
	{
		CHECK_ERR( arr.empty() );

		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxPipelineCount );

		if ( count == 0 )
			return true;

		auto	ptr = _allocator->Allocate<T>( count );
		CHECK_ERR( ptr != null );
		arr = ArrayView{ ptr, count };

		bool	result = true;
		for (uint i = 0; result and (i < count); ++i)
		{
			auto&	ppln = ptr[i];
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
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxRenTechCount );

		if ( count == 0 )
			return true;

		_renTechMap->reserve( count );	// throw

		_renTechs = MutableArrayView{ _allocator->Allocate< StaticRC<RenderTech> >( count ), count };
		CHECK_ERR( _renTechs.data() != null );

		bool	result = true;
		for (uint i = 0; result and (i < count); ++i)
		{
			auto&	rt = _renTechs[i];
			PlacementNew< StaticRC<RenderTech> >( &rt, *this );

			result = rt->Deserialize( resMngr, des );	// throw

			if ( rt->IsSupported() )
				_renTechMap->emplace( rt->Name(), ushort(i) );	// throw
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
		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxSBTCount );

		if ( count == 0 )
			return true;

		auto	ptr = _allocator->Allocate< PipelineCompiler::SerializableRTShaderBindingTable >( count );
		CHECK_ERR( ptr != null );
		_serRTSBTs = SerRTSBTs_t{ ptr, count };

		bool	result = true;
		for (uint i = 0; result and (i < count); ++i)
		{
			auto&	sbt = ptr[i];
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
		using Types_t = TypeList< ShaderBytecode::Bytecode_t >;	// union to typelist

		uint	count = 0;
		CHECK_ERR( des( OUT count ));
		CHECK_ERR( count <= PipelineStorage::MaxShaderCount );

		if ( count == 0 )
			return true;

		_shaders = MutableArrayView{ _allocator->Allocate< ShaderModule >( count ), count };
		CHECK_ERR( _shaders.data() != null );

		const Bytes32u	total_size {_shaderDataSize};

		bool	result = true;
		for (uint i = 0; result and (i < count); ++i)
		{
			auto&	sh = _shaders[i];
			PlacementNew< ShaderModule >( OUT &sh );

			result  = des( OUT sh.offset, OUT sh.dataSize, OUT sh.data2Size, OUT sh.shaderTypeIdx );
			result &= (sh.offset < total_size) and (sh.offset + sh.dataSize + sh.data2Size <= total_size);

			CHECK_ERR( sh.offset < total_size );
			CHECK_ERR( sh.offset + sh.dataSize + sh.data2Size <= total_size );

			#if defined(AE_ENABLE_VULKAN)
				CHECK_ERR(	sh.shaderTypeIdx == Types_t::Index<SpirvBytecode_t> or
							sh.shaderTypeIdx == Types_t::Index<SpirvWithTrace> );
			#elif defined(AE_ENABLE_METAL)
				CHECK_ERR(	sh.shaderTypeIdx == Types_t::Index<MetalBytecode_t> );
			#else
			#	error not implemented
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
		_isSupported{ false },	_isLoaded{ false },		_wasAttemptToLoad{ false },
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

		uint	fs_count = 0;
		CHECK_ERR( des( OUT _name, OUT fs_count ));
		CHECK_ERR( fs_count <= FeatureSetSerializer::MaxCount );

		// feature sets
		{
			bool	result		= true;
			bool	supported	= true;

			for (uint i = 0; result and (i < fs_count); ++i)
			{
				FeatureSetName::Optimized_t		fs_name;
				result = des( OUT fs_name );

				supported &= (not _pack._unsupportedFS->contains( fs_name ));
				GFX_DBG_ONLY(
					if ( _pack._unsupportedFS->contains( fs_name ))
						AE_LOG_DBG( "Render technique '"s << resMngr.HashToName( _name ) << "' requires '" << resMngr.HashToName( fs_name ) << "' FeatureSet" );

					if ( not _pack._allFeatureSets->contains( fs_name ))
						AE_LOGW( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
				)
			}

			CHECK_ERR( result );
			_isSupported = supported;

			if ( not _isSupported ) {
				GFX_DBG_ONLY( AE_LOGW( "Render technique '"s << resMngr.HashToName( _name ) << "' is NOT supported" ));
			}
		}

		// passes
		{
			uint	pass_count = 0;
			CHECK_ERR( des( OUT pass_count ) and pass_count > 0 );
			CHECK_ERR( pass_count <= SerializableRenderTechnique::MaxPassCount );

			_passes = MutableArrayView{ _pack._allocator->Allocate< SerializableRenderTechnique::Pass >( pass_count ), pass_count };

			bool	result = true;
			for (uint i = 0; result and (i < pass_count); ++i)
			{
				auto&	pass = _passes[i];
				PlacementNew< SerializableRenderTechnique::Pass >( OUT &pass );
				result = pass.Deserialize( des );
			}
			CHECK_ERR( result );
		}

		// pipelines
		{
			uint	ppln_count = 0;
			CHECK_ERR( des( OUT ppln_count ) and ppln_count > 0 );
			CHECK_ERR( ppln_count <= SerializableRenderTechnique::MaxPipelineCount );

			_pipelines.reserve( ppln_count );	// throw

			bool	result = true;
			for (uint i = 0; result and (i < ppln_count); ++i)
			{
				PipelineName::Optimized_t	name;
				PipelineInfo				info;
				result = des( OUT name, OUT info.uid );
				_pipelines.emplace( name, info );	// throw
			}
			CHECK_ERR( result );
			CHECK_ERR( _pipelines.size() == ppln_count );
		}

		// SBTs
		{
			uint	sbt_count = 0;
			CHECK_ERR( des( OUT sbt_count ));

			if ( sbt_count > 0 )
			{
				_rtSbtMap.reserve( sbt_count );	// throw

				bool	result = true;
				for (uint i = 0; result and (i < sbt_count); ++i)
				{
					RTShaderBindingName::Optimized_t	name;
					SBTInfo								info;
					result = des( OUT name, OUT info.uid );
					_rtSbtMap.emplace( name, info );	// throw
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
			RC<RenderTech>	rtech;
			ResMngr_t &		resMngr;

			PreloadShadersTask (RC<RenderTech> rt, ResMngr_t& rm) __NE___ :
				IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm} {}

			void		Run ()		__Th_OV { CHECK_TE( rtech->_PreloadShaders( resMngr )); }
			StringView  DbgName ()	C_NE_OV	{ return "PreloadShadersTask"; }
		};

		class CompilePipelinesTask final : public Threading::IAsyncTask
		{
		public:
			RC<RenderTech>		rtech;
			ResMngr_t &			resMngr;
			PipelineCacheID		cacheId;
			PplnSpecIter_t		beginIt;
			PplnSpecIter_t		endIt;

			CompilePipelinesTask (RC<RenderTech> rt, ResMngr_t& rm, PipelineCacheID cache, PplnSpecIter_t begin, PplnSpecIter_t end) __NE___ :
				IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm}, cacheId{cache}, beginIt{begin}, endIt{end} {}

			void		Run ()		__Th_OV { CHECK_TE( rtech->_CompilePipelines( resMngr, cacheId, beginIt, endIt )); }
			StringView  DbgName ()	C_NE_OV	{ return "CompilePipelinesTask"; }
		};

		class CreateSBTsTask final : public Threading::IAsyncTask
		{
		public:
			RC<RenderTech>	rtech;
			ResMngr_t &		resMngr;

			CreateSBTsTask (RC<RenderTech> rt, ResMngr_t& rm) __NE___ :
				IAsyncTask{ ETaskQueue::Background }, rtech{rt}, resMngr{rm} {}

			void		Run ()		__Th_OV { CHECK_TE( rtech->_PreloadShaders( resMngr )); }
			StringView  DbgName ()	C_NE_OV	{ return "CreateSBTsTask"; }
		};
		//-------------------------------------------------


		RC<RenderTech>	rt				= this->GetRC<RenderTech>();
		AsyncTask		preload			= Scheduler().Run<PreloadShadersTask>( Tuple{ rt, ArgRef(resMngr) });

		const usize		max_tasks		= 16;
		const usize		min_ppln_count	= 32;
		const usize		ppln_per_task	= Max( min_ppln_count, DivCeil( _pipelines.size(), max_tasks ));
		usize			task_count		= 0;
		AsyncTask		compile_tasks	[max_tasks];

		auto			begin_it		= _pipelines.begin();
		auto			it				= begin_it;

		// TODO:
		//	v1 - divide by same blocks and run in parallel
		//	v2 - use spinlock + single iterator, because pipeline compilation is slow but depends on complexity, one thread may be too slow

		for (; task_count < max_tasks; ++task_count)
		{
			begin_it = it;
			for (usize i = 0; (i < ppln_per_task) and (it != _pipelines.end()); ++i, ++it) {}

			compile_tasks[task_count] = Scheduler().Run<CompilePipelinesTask>(
											Tuple{ rt, ArgRef(resMngr), cacheid, begin_it, it },
											Tuple{ preload });
		}
		ASSERT( it == _pipelines.end() );
		ASSERT( task_count > 0 );

		if ( not _rtSbtMap.empty() )
		{
			auto	task = Scheduler().Run<CreateSBTsTask>(
								Tuple{ rt, ArgRef(resMngr) },
								Tuple{ArrayView<AsyncTask>{ compile_tasks, task_count }} );
			task_count		 = 1;
			compile_tasks[0] = RVRef(task);
		}

		return MakePromiseFromValue(
					RenderTechPipelinesPtr{ rt },
					Tuple{ArrayView<AsyncTask>{ compile_tasks, task_count }},
					"RenderTech::LoadAsync",
					ETaskQueue::Background );
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

			if ( _wasAttemptToLoad )
				return false;
		}

		DRC_EXLOCK( _drCheck );		// TODO

		if ( _wasAttemptToLoad )
			return false;

		_wasAttemptToLoad = true;

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
		  #if AE_DBG_GRAPHICS
			String		dbg_name = resMngr.HashToName( it->first );
		  #else
			StringView	dbg_name;
		  #endif
			auto&		info	= it->second;

			switch_enum( info.uid & PipelineSpecUID::_Mask )
			{
				case PipelineSpecUID::Graphics :
				{
					auto [spec, tmpl] = _Extract< PipelineSpecUID::Graphics, PipelineTemplUID::Graphics >(
												resMngr, info.uid, _pack._serGPplnSpec, _pack._serGPplnTempl,
												*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets ));
					CHECK_ERR( spec != null and tmpl != null );
					info.pplnId = _CompileGraphicsPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
					break;
				}

				case PipelineSpecUID::Mesh :
				{
					auto [spec, tmpl] = _Extract< PipelineSpecUID::Mesh, PipelineTemplUID::Mesh >(
												resMngr, info.uid, _pack._serMPplnSpec, _pack._serMPplnTempl,
												*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets ));
					CHECK_ERR( spec != null and tmpl != null );
					info.pplnId = _CompileMeshPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
					break;
				}

				case PipelineSpecUID::Compute :
				{
					auto [spec, tmpl] = _Extract< PipelineSpecUID::Compute, PipelineTemplUID::Compute >(
												resMngr, info.uid, _pack._serCPplnSpec, _pack._serCPplnTempl,
												*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets ));
					CHECK_ERR( spec != null and tmpl != null );
					info.pplnId = _CompileComputePipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
					break;
				}

				case PipelineSpecUID::RayTracing :
				{
					auto [spec, tmpl] = _Extract< PipelineSpecUID::RayTracing, PipelineTemplUID::RayTracing >(
												resMngr, info.uid, _pack._serRTPplnSpec, _pack._serRTPplnTempl,
												*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets ));
					CHECK_ERR( spec != null and tmpl != null );
					info.pplnId = _CompileRayTracingPipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
					break;
				}

				case PipelineSpecUID::Tile :
				{
					auto [spec, tmpl] = _Extract< PipelineSpecUID::Tile, PipelineTemplUID::Tile >(
												resMngr, info.uid, _pack._serTPplnSpec, _pack._serTPplnTempl,
												*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets ));
					CHECK_ERR( spec != null and tmpl != null );
					info.pplnId = _CompileTilePipeline( resMngr, *spec, *tmpl, cacheid, dbg_name );
					break;
				}

				case PipelineSpecUID::_Mask :
				case PipelineSpecUID::Unknown :
				default_unlikely :					break;
			}
			switch_end
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
			CHECK_ERR( info.uid < _pack._serRTSBTs.size() );

		  #if AE_DBG_GRAPHICS
			String		dbg_name = resMngr.HashToName( name );
		  #else
			StringView	dbg_name;
		  #endif

			info.sbtId = _CreateRTShaderBinding( resMngr, _pack._serRTSBTs[ info.uid ], dbg_name );
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
		DRC_EXLOCK( _drCheck );

		if ( _isLoaded or _wasAttemptToLoad )
		{
			for (auto& [name, sbt] : _rtSbtMap)
			{
				Strong<RTShaderBindingID>	id{ sbt.sbtId };
				DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
			}

			for (auto& [name, info] : _pipelines)
			{
				switch_enum( info.uid & PipelineSpecUID::_Mask )
				{
					case PipelineSpecUID::Graphics :	{ Strong<GraphicsPipelineID>	id {info.Cast<GraphicsPipelineID>()};	DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));  break; }
					case PipelineSpecUID::Mesh :		{ Strong<MeshPipelineID>		id {info.Cast<MeshPipelineID>()};		DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));  break; }
					case PipelineSpecUID::Compute :		{ Strong<ComputePipelineID>		id {info.Cast<ComputePipelineID>()};	DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));  break; }
					case PipelineSpecUID::RayTracing :	{ Strong<RayTracingPipelineID>	id {info.Cast<RayTracingPipelineID>()};	DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));  break; }
					case PipelineSpecUID::Tile :		{ Strong<TilePipelineID>		id {info.Cast<TilePipelineID>()};		DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));  break; }
					case PipelineSpecUID::_Mask :
					case PipelineSpecUID::Unknown :
					default_unlikely :					DBG_WARNING( "unknown pipeline type" ); break;
				}
				switch_end
			}
		}

		_rtSbtMap.clear();
		_pipelines.clear();
		_passes				= Default;
		_isLoaded			= false;
		_wasAttemptToLoad	= false;
	}

/*
=================================================
	_Extract
=================================================
*/
	template <PipelineSpecUID SpecMask, PipelineTemplUID TemplMask, typename SpecType, typename TemplType>
	Pair< const typename SpecType::value_type*, const typename TemplType::value_type* >
		PPLNPACK::RenderTech::_Extract (const ResMngr_t &resMngr, PipelineSpecUID specUID, SpecType &specArr, TemplType &templArr,
										const FeatureNames_t &unsupportedFS
										GFX_DBG_ONLY(, const FeatureNames_t &allFeatureSets )) __NE___
	{
		Unused( resMngr );

		CHECK_ERR( AllBits( specUID, SpecMask ));
		const uint	spec_idx	= uint(specUID & ~PipelineSpecUID::_Mask);

		CHECK_ERR( spec_idx < specArr.size() );
		auto&		spec		= specArr[ spec_idx ];

		CHECK_ERR( AllBits( spec.templUID, TemplMask ));
		const uint	tmpl_idx	= uint(spec.templUID & ~PipelineTemplUID::_Mask);

		CHECK_ERR( tmpl_idx < templArr.size() );
		auto&		tmpl		= templArr[ tmpl_idx ];

		bool		supported	= true;
		for (auto& fs_name : tmpl.features)
		{
			supported &= (not unsupportedFS.contains( fs_name ));
			GFX_DBG_ONLY(
				if ( not allFeatureSets.contains( fs_name ))
					AE_LOGW( "FeatureSet '"s << resMngr.HashToName( fs_name ) << "' is not exists" );
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
		FlatHashSet< ShaderUID >	shaders;
		shaders.reserve( _pipelines.size() * 2 );

		for (auto& [name, info] : _pipelines)
		{
			switch_enum( info.uid & PipelineSpecUID::_Mask )
			{
				case PipelineSpecUID::Graphics :
				{
					auto*	tmpl = _Extract< PipelineSpecUID::Graphics, PipelineTemplUID::Graphics >(
											resMngr, info.uid, _pack._serGPplnSpec, _pack._serGPplnTempl,
											*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets )).second;
					CHECK_ERR( tmpl != null );
					for (auto sh : tmpl->shaders)
						shaders.insert( sh.second );
					break;
				}
				case PipelineSpecUID::Mesh :
				{
					auto*	tmpl = _Extract< PipelineSpecUID::Mesh, PipelineTemplUID::Mesh >(
											resMngr, info.uid, _pack._serMPplnSpec, _pack._serMPplnTempl,
											*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets )).second;
					CHECK_ERR( tmpl != null );
					for (auto sh : tmpl->shaders)
						shaders.insert( sh.second );
					break;
				}
				case PipelineSpecUID::Compute :
				{
					auto*	tmpl = _Extract< PipelineSpecUID::Compute, PipelineTemplUID::Compute >(
											resMngr, info.uid, _pack._serCPplnSpec, _pack._serCPplnTempl,
											*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets )).second;
					CHECK_ERR( tmpl != null );
					shaders.insert( tmpl->shader );
					break;
				}
				case PipelineSpecUID::RayTracing :
				{
					auto*	tmpl = _Extract< PipelineSpecUID::RayTracing, PipelineTemplUID::RayTracing >(
											resMngr, info.uid, _pack._serRTPplnSpec, _pack._serRTPplnTempl,
											*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets )).second;
					CHECK_ERR( tmpl != null );
					for (auto& sh : tmpl->shaderArr)
						shaders.insert( sh.Get<ShaderUID>() );
					break;
				}
				case PipelineSpecUID::Tile :
				{
					auto*	tmpl = _Extract< PipelineSpecUID::Tile, PipelineTemplUID::Tile >(
											resMngr, info.uid, _pack._serTPplnSpec, _pack._serTPplnTempl,
											*_pack._unsupportedFS GFX_DBG_ONLY(, *_pack._allFeatureSets )).second;
					CHECK_ERR( tmpl != null );
					shaders.insert( tmpl->shader );
					break;
				}
				case PipelineSpecUID::_Mask :
				case PipelineSpecUID::Unknown :
				default_unlikely :					break;
			}
			switch_end
		}

		for (auto uid : shaders)
		{
			auto	ref = _pack._GetShader( resMngr, uid, EShader::Unknown );
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
		SerializableGraphicsPipelineSpec	spec = inSpec;
		spec.desc.renderStatePtr	= _pack._GetRenderState( spec.rStateUID );
		spec.desc.dbgName			= dbgName;

		FixedArray< ShaderModuleRef, 5 >	shaders;
		for (auto [type, uid] : tmpl.shaders) {
			shaders.push_back( _pack._GetShader( resMngr, uid, type ));
		}

		auto	id = resMngr.CreatePipeline( GraphicsPipeline_t::CreateInfo{
												_pack,
												tmpl, spec.desc,
												_pack._GetPipelineLayout( tmpl.layout ),
												shaders, cacheId,
												_pack._allocator.get()
											}).Release();
		return BitCast<PipelineID>( id );
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
		SerializableMeshPipelineSpec	spec = inSpec;
		spec.desc.renderStatePtr	= _pack._GetRenderState( spec.rStateUID );
		spec.desc.dbgName			= dbgName;

		FixedArray< ShaderModuleRef, 3 >	shaders;
		for (auto [type, uid] : tmpl.shaders) {
			shaders.push_back( _pack._GetShader( resMngr, uid, type ));
		}

		auto	id = resMngr.CreatePipeline( MeshPipeline_t::CreateInfo{
												_pack,
												tmpl, spec.desc,
												_pack._GetPipelineLayout( tmpl.layout ),
												shaders, cacheId,
												_pack._allocator.get()
											}).Release();
		return BitCast<PipelineID>( id );
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
		SerializableComputePipelineSpec	spec = inSpec;
		spec.desc.dbgName	= dbgName;

		auto	id = resMngr.CreatePipeline( ComputePipeline_t::CreateInfo{
												tmpl, spec.desc,
												_pack._GetPipelineLayout( tmpl.layout ),
												_pack._GetShader( resMngr, tmpl.shader, EShader::Compute ), cacheId
											}).Release();
		return BitCast<PipelineID>( id );
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
		SerializableTilePipelineSpec	spec = inSpec;
		spec.desc.dbgName	= dbgName;

		auto	id = resMngr.CreatePipeline( TilePipeline_t::CreateInfo{
												_pack,
												tmpl, spec.desc,
												_pack._GetPipelineLayout( tmpl.layout ),
												_pack._GetShader( resMngr, tmpl.shader, EShader::Tile ), cacheId
											}).Release();
		return BitCast<PipelineID>( id );
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
		SerializableRayTracingPipelineSpec	spec = inSpec;
		spec.desc.dbgName	= dbgName;

		TempLinearAllocator_t	allocator;

		auto*	shaders = allocator.Allocate< ShaderModuleRef >( tmpl.shaderArr.size() );
		CHECK_ERR( shaders != null );

		for (usize i = 0; i < tmpl.shaderArr.size(); ++i)
		{
			auto&	sh = tmpl.shaderArr[i];
			shaders[i] = _pack._GetShader( resMngr, sh.Get<ShaderUID>(), sh.Get<EShader>() );
		}

		auto	id = resMngr.CreatePipeline( RayTracingPipeline_t::CreateInfo{
												tmpl, spec.desc,
												_pack._GetPipelineLayout( tmpl.layout ),
												ArrayView< ShaderModuleRef >{ shaders, tmpl.shaderArr.size() }, cacheId,
												_pack._allocator.get(),
												&allocator
											}).Release();
		return BitCast<PipelineID>( id );
	}

/*
=================================================
	_PrintPipelines
=================================================
*/
	void  PPLNPACK::RenderTech::_PrintPipelines (PipelineName::Ref reqName, PipelineSpecUID mask) C_NE___
	{
	#if AE_DBG_GRAPHICS
		const auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		String		str;

		for (auto& [name, ppln] : _pipelines)
		{
			if ( AllBits( ppln.uid, mask ))
				str << "'" << res_mngr.HashToName( name ) << "', ";
		}

		if ( str.empty() )
			return;

		str.pop_back();
		str.pop_back();

		switch_enum( mask )
		{
			case PipelineSpecUID::Graphics :	"Available graphics pipelines:\n"	 >> str;	break;
			case PipelineSpecUID::Mesh :		"Available mesh pipelines:\n"		 >> str;	break;
			case PipelineSpecUID::Compute :		"Available compute pipelines:\n"	 >> str;	break;
			case PipelineSpecUID::RayTracing :	"Available ray tracing pipelines:\n" >> str;	break;
			case PipelineSpecUID::Tile :		"Available tile pipelines:\n"		 >> str;	break;
			case PipelineSpecUID::_Mask :		break;
			case PipelineSpecUID::Unknown :		break;
		}
		switch_end

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
	GraphicsPipelineID  PPLNPACK::RenderTech::GetGraphicsPipeline (PipelineName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pipelines.find( name );
		if_unlikely( it == _pipelines.end() )
		{
			_PrintPipelines( name, PipelineSpecUID::Graphics );
			return Default;
		}

		CHECK_ERR( AllBits( it->second.uid, PipelineSpecUID::Graphics ));

		return it->second.Cast<GraphicsPipelineID>();
	}

/*
=================================================
	GetMeshPipeline
=================================================
*/
	MeshPipelineID  PPLNPACK::RenderTech::GetMeshPipeline (PipelineName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pipelines.find( name );
		if_unlikely( it == _pipelines.end() )
		{
			_PrintPipelines( name, PipelineSpecUID::Mesh );
			return Default;
		}

		CHECK_ERR( AllBits( it->second.uid, PipelineSpecUID::Mesh ));

		return it->second.Cast<MeshPipelineID>();
	}

/*
=================================================
	GetTilePipeline
=================================================
*/
	TilePipelineID  PPLNPACK::RenderTech::GetTilePipeline (PipelineName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pipelines.find( name );
		if_unlikely( it == _pipelines.end() )
		{
			_PrintPipelines( name, PipelineSpecUID::Tile );
			return Default;
		}

		CHECK_ERR( AllBits( it->second.uid, PipelineSpecUID::Tile ));

		return it->second.Cast<TilePipelineID>();
	}

/*
=================================================
	GetComputePipeline
=================================================
*/
	ComputePipelineID  PPLNPACK::RenderTech::GetComputePipeline (PipelineName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pipelines.find( name );
		if_unlikely( it == _pipelines.end() )
		{
			_PrintPipelines( name, PipelineSpecUID::Compute );
			return Default;
		}

		CHECK_ERR( AllBits( it->second.uid, PipelineSpecUID::Compute ));

		return it->second.Cast<ComputePipelineID>();
	}

/*
=================================================
	GetRayTracingPipeline
=================================================
*/
	RayTracingPipelineID  PPLNPACK::RenderTech::GetRayTracingPipeline (PipelineName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _pipelines.find( name );
		if_unlikely( it == _pipelines.end() )
		{
			_PrintPipelines( name, PipelineSpecUID::RayTracing );
			return Default;
		}

		CHECK_ERR( AllBits( it->second.uid, PipelineSpecUID::RayTracing ));

		return it->second.Cast<RayTracingPipelineID>();
	}

/*
=================================================
	_PrintSBTs
=================================================
*/
	void  PPLNPACK::RenderTech::_PrintSBTs (RTShaderBindingName::Ref reqName) C_NE___
	{
	#if AE_DBG_GRAPHICS
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		String	str;

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
	RTShaderBindingID  PPLNPACK::RenderTech::GetRTShaderBinding (RTShaderBindingName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _rtSbtMap.find( name );
		if_unlikely( it == _rtSbtMap.end() )
		{
			_PrintSBTs( name );
			return Default;
		}

		return it->second.sbtId;
	}

/*
=================================================
	FeatureSetSupported
=================================================
*/
	bool  PPLNPACK::RenderTech::FeatureSetSupported (FeatureSetName::Ref name) C_NE___
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
	EPixelFormat  PPLNPACK::RenderTech::GetAttachmentFormat (RenderTechPassName::Ref passName, AttachmentName::Ref attName) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const auto	pass = GetPass( passName );
		if ( pass.IsDefined() )
		{
			auto&			res_mngr = GraphicsScheduler().GetResourceManager();
			RenderPassID	rp_id	 = _pack.GetRenderPass( RenderPassName{pass.renderPass} );

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

		uint	old_idx = _passIdx.exchange( 0 );
		CHECK_ERR( old_idx >= _passes.Get<0>() );

		return true;
	}

/*
=================================================
	NextPass
=================================================
*
	bool  PPLNPACK::RenderTech::NextPass (RenderTechPassName::Ref pass)
	{
		DRC_SHAREDLOCK( _drCheck );

		const uint	idx = _passIdx.fetch_add( 1 );
		CHECK_ERR( idx < _passes.Get<0>() );

		const auto	req_name = _passes.Get<1>()[ idx ].name;
		if_unlikely( req_name != pass )
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();
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

		const uint	idx = _passIdx.fetch_add( 1 );
		CHECK_ERR( idx == _passes.Get<0>() );

		return true;
	}

/*
=================================================
	GetPass
=================================================
*/
	IRenderTechPipelines::PassInfo  PPLNPACK::RenderTech::GetPass (RenderTechPassName::Ref passName) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		// TODO: optimize
		for (usize i = 0, cnt = _passes.size(); i < cnt; ++i)
		{
			auto&	pass = _passes[i];

			if ( pass.name == passName )
			{
				PassInfo	info;
				info.dsIndex	= DescSetBinding{0};	// TODO
				info.type		= pass.IsGraphics() ? EPassType::Graphics : EPassType::Compute;
				info.renderPass	= pass.renderPass;
				info.subpass	= pass.subpass;
				info.packId		= _pack._parentPackId;	// TODO ?

				if ( usize(pass.dsLayout) < _pack._dsLayouts.size() )
				{
					info.dsLayoutId	= _pack._dsLayouts[ usize(pass.dsLayout) ];
				}
				return info;
			}
		}

		RETURN_ERR( "unknown render technique pass" );
	}


} // AE::Graphics
