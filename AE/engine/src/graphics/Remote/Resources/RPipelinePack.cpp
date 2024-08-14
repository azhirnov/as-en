// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/REnumCast.h"
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"
# include "graphics/Remote/Resources/RPipelinePack.h"

# ifdef AE_ENABLE_GLSL_TRACE
#	include "ShaderTrace.h"
# else
#	include "Packer/ShaderTraceDummy.h"
# endif

namespace AE::PipelineCompiler
{
	ND_ bool  Deserialize_Uniform (Serializing::Deserializer& des, uint samplerStorageSize, OUT DescriptorSetLayoutDesc::Uniform &un);
}

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;
	using namespace AE::PipelineCompiler;


/*
=================================================
	constructor / destructor
=================================================
*/
	RPipelinePack::RenderTech::RenderTech (const RPipelinePack &pack) __NE___ :
		_pack{ pack }
	{
		_pack._renTechs->push_back( GetRC<RenderTech>() );
	}

	RPipelinePack::RenderTech::~RenderTech () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( _rtechId == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RPipelinePack::RenderTech::Create (RResourceManager &resMngr, const RemoteGraphics::Msg::ResMngr_LoadRenderTech_Response &inMsg) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _rtechId );
		CHECK_ERR( inMsg.id );

		_rtechId	= inMsg.id;
		_name		= inMsg.name;

		_passes.reserve( inMsg.passes.size() );
		for (auto& [name, src] : inMsg.passes)
		{
			IRenderTechPipelines::PassInfo	pass = src;
			pass.dsLayoutId	= _pack.Cast( BitCast<RmDescriptorSetLayoutID>( src.dsLayoutId ));
			pass.packId		= _pack._parentPackId;	// TODO ?

			_passes.emplace( name, pass );
		}

		_pipelines.reserve( inMsg.pipelines.size() );
		for (auto& [name, uid] : inMsg.pipelines)
		{
			Msg::ResMngr_GetRTechPipeline	msg;

			msg.rtechId	= _rtechId;
			msg.name	= name;

			switch_enum( uid & PipelineSpecUID::_Mask )
			{
				case PipelineSpecUID::Graphics :
				{
					msg.index = Msg::ResMngr_GetRTechPipeline::Types::Index<RmGraphicsPipelineID>;

					RC<Msg::ResMngr_CreateGraphicsPipeline_Response>	res;
					CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

					auto	id = resMngr.CreatePipeline( RGraphicsPipeline::CreateInfo{ *res, _pack, false });
					CHECK_ERR( id );

					_pipelines.emplace( name, PipelineInfo{ uid, BitCast<PipelineID>( id.Release() )});
					break;
				}
				case PipelineSpecUID::Mesh :
				{
					msg.index = Msg::ResMngr_GetRTechPipeline::Types::Index<RmMeshPipelineID>;

					RC<Msg::ResMngr_CreateMeshPipeline_Response>	res;
					CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

					auto	id = resMngr.CreatePipeline( RMeshPipeline::CreateInfo{ *res, _pack, false });
					CHECK_ERR( id );

					_pipelines.emplace( name, PipelineInfo{ uid, BitCast<PipelineID>( id.Release() )});
					break;
				}
				case PipelineSpecUID::Compute :
				{
					msg.index = Msg::ResMngr_GetRTechPipeline::Types::Index<RmComputePipelineID>;

					RC<Msg::ResMngr_CreateComputePipeline_Response>	res;
					CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

					auto	id = resMngr.CreatePipeline( RComputePipeline::CreateInfo{ *res, _pack, false });
					CHECK_ERR( id );

					_pipelines.emplace( name, PipelineInfo{ uid, BitCast<PipelineID>( id.Release() )});
					break;
				}
				case PipelineSpecUID::RayTracing :
				{
					msg.index = Msg::ResMngr_GetRTechPipeline::Types::Index<RmRayTracingPipelineID>;

					RC<Msg::ResMngr_CreateRayTracingPipeline_Response>	res;
					CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

					auto	id = resMngr.CreatePipeline( RRayTracingPipeline::CreateInfo{ *res, _pack, false });
					CHECK_ERR( id );

					_pipelines.emplace( name, PipelineInfo{ uid, BitCast<PipelineID>( id.Release() )});
					break;
				}
				case PipelineSpecUID::Tile :
				{
					msg.index = Msg::ResMngr_GetRTechPipeline::Types::Index<RmTilePipelineID>;

					RC<Msg::ResMngr_CreateTilePipeline_Response>	res;
					CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

					auto	id = resMngr.CreatePipeline( RTilePipeline::CreateInfo{ *res, _pack, false });
					CHECK_ERR( id );

					_pipelines.emplace( name, PipelineInfo{ uid, BitCast<PipelineID>( id.Release() )});
					break;
				}

				case PipelineSpecUID::_Mask :
				case PipelineSpecUID::Unknown :
				default_unlikely :					break;
			}
			switch_end
		}

		_rtSbtMap.reserve( inMsg.rtSbtMap.size() );
		for (auto& [name, sbt] : inMsg.rtSbtMap)
		{
			RRTShaderBindingTable::CreateInfo	ci;
			ci.sbt.id = sbt;

			auto	id = resMngr.CreateRTShaderBinding( ci );
			CHECK_ERR( id );

			_rtSbtMap.emplace( name, RVRef(id) );
		}

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RPipelinePack::RenderTech::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto& [name, info] : _pipelines)
		{
			switch_enum( info.uid & PipelineSpecUID::_Mask )
			{
				case PipelineSpecUID::Graphics :	{ Strong<GraphicsPipelineID>	id {info.Cast<GraphicsPipelineID>()};	resMngr.ImmediatelyRelease( INOUT id );		break; }
				case PipelineSpecUID::Mesh :		{ Strong<MeshPipelineID>		id {info.Cast<MeshPipelineID>()};		resMngr.ImmediatelyRelease( INOUT id );		break; }
				case PipelineSpecUID::Compute :		{ Strong<ComputePipelineID>		id {info.Cast<ComputePipelineID>()};	resMngr.ImmediatelyRelease( INOUT id );		break; }
				case PipelineSpecUID::RayTracing :	{ Strong<RayTracingPipelineID>	id {info.Cast<RayTracingPipelineID>()};	resMngr.ImmediatelyRelease( INOUT id );		break; }
				case PipelineSpecUID::Tile :		{ Strong<TilePipelineID>		id {info.Cast<TilePipelineID>()};		resMngr.ImmediatelyRelease( INOUT id );		break; }
				case PipelineSpecUID::_Mask :
				case PipelineSpecUID::Unknown :
				default_unlikely :					DBG_WARNING( "unknown pipeline type" ); break;
			}
			switch_end
		}
		_pipelines.clear();

		for (auto& [name, sbt] : _rtSbtMap) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT sbt ));
		}
		_rtSbtMap.clear();

		if ( _rtechId )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_rtechId} ));

		_rtechId	= Default;
		_name		= Default;
	}

/*
=================================================
	_PrintPipelines
=================================================
*/
	void  RPipelinePack::RenderTech::_PrintPipelines (PipelineName::Ref reqName, PipelineSpecUID mask) C_NE___
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
	GraphicsPipelineID  RPipelinePack::RenderTech::GetGraphicsPipeline (PipelineName::Ref name) C_NE___
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
	MeshPipelineID  RPipelinePack::RenderTech::GetMeshPipeline (PipelineName::Ref name) C_NE___
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
	TilePipelineID  RPipelinePack::RenderTech::GetTilePipeline (PipelineName::Ref name) C_NE___
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
	ComputePipelineID  RPipelinePack::RenderTech::GetComputePipeline (PipelineName::Ref name) C_NE___
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
	RayTracingPipelineID  RPipelinePack::RenderTech::GetRayTracingPipeline (PipelineName::Ref name) C_NE___
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
	void  RPipelinePack::RenderTech::_PrintSBTs (RTShaderBindingName::Ref reqName) C_NE___
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
	RTShaderBindingID  RPipelinePack::RenderTech::GetRTShaderBinding (RTShaderBindingName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _rtSbtMap.find( name );
		if_unlikely( it == _rtSbtMap.end() )
		{
			_PrintSBTs( name );
			return Default;
		}

		return it->second;
	}

/*
=================================================
	FeatureSetSupported
=================================================
*/
	bool  RPipelinePack::RenderTech::FeatureSetSupported (FeatureSetName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		return not _pack._unsupportedFS.contains( name );
	}

/*
=================================================
	GetPass
=================================================
*/
	RPipelinePack::RenderTech::PassInfo  RPipelinePack::RenderTech::GetPass (RenderTechPassName::Ref pass) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _passes.find( pass );
		if ( it != _passes.end() )
			return it->second;

		return Default;
	}

/*
=================================================
	GetAttachmentFormat
=================================================
*/
	EPixelFormat  RPipelinePack::RenderTech::GetAttachmentFormat (RenderTechPassName::Ref passName, AttachmentName::Ref attName) C_NE___
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
//-----------------------------------------------------------------------------



/*
=================================================
	Create
=================================================
*/
	bool  RPipelinePack::Create (RResourceManager &resMngr, const PipelinePackDesc &desc) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( desc.stream );

		ASSERT( not _allocator );
		_allocator = MakeRC< LinearAllocator_t >();

		auto&		dev			= resMngr.GetDevice();
		const Bytes	base_offset = desc.stream->Position();

		RC<Msg::ResMngr_LoadPipelinePack_Response>	res;
		{
			Msg::UploadData		msg1;
			msg1.size		= desc.stream->RemainingSize();
			msg1.stream		= desc.stream;

			Msg::ResMngr_LoadPipelinePack	msg2;

			msg2.desc			= desc;
			msg2.desc.stream	= null;
			msg2.sbtAllocatorId	= dev.Cast( desc.sbtAllocator );

			CHECK_ERR( dev.SendAndWait( List<Msg::BaseMsg*>{&msg1, &msg2}, OUT res ));
			CHECK_ERR( res->packId );
		}

		_packId			= res->packId;
		_unsupportedFS	= RVRef(res->unsupportedFS);

		// create samplers
		if ( AllBits( desc.options, EPipelinePackOpt::Samplers ))
		{
			_samplerRefs.reserve( res->samplerRefs.size() );
			for (auto& [name, id] : res->samplerRefs)
			{
				auto	samp = resMngr.CreateSampler( id );
				CHECK_ERR( samp );
				CHECK_ERR( _samplerRefs.emplace( name, RVRef(samp) ).second );
			}
		}else{
			CHECK( res->samplerRefs.empty() );
		}

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
			}
		}

		CHECK_ERR( _LoadPipelineBlock( resMngr, desc, res->dsLayouts, res->pplnLayouts, base_offset ));
		CHECK_ERR( _CreateRenderPasses( resMngr, res->renderPasses ));

		GFX_DBG_ONLY( _debugName = desc.dbgName;)
		return true;
	}

/*
=================================================
	_LoadPipelineBlock
=================================================
*/
	bool  RPipelinePack::_LoadPipelineBlock (RResourceManager &resMngr, const PipelinePackDesc &desc,
											 ArrayView<RmDescriptorSetLayoutID> rmDSLayouts,
											 ArrayView<RmPipelineLayoutID> pplnLayouts,
											 const Bytes baseOffset) __NE___
	{
		CHECK_ERR( desc.stream->SeekSet( baseOffset ));

		uint	hdr_name = 0;
		CHECK_ERR( desc.stream->Read( OUT hdr_name ) and hdr_name == PackOffsets_Name );

		PipelinePackOffsets		offsets;
		CHECK_ERR( desc.stream->Read( OUT offsets ));

		if ( AllBits( desc.options, EPipelinePackOpt::Pipelines )		and
			 (baseOffset + offsets.pipelineOffset < desc.stream->Size()) )
		{
			CHECK_ERR( desc.stream->SeekSet( baseOffset + offsets.pipelineOffset ));

			auto	mem_stream = MakeRC<ArrayRStream>();
			CHECK_ERR( mem_stream->LoadRemainingFrom( *desc.stream, offsets.pipelineDataSize ));

			// check header & version
			{
				DefaultPackHeader	hdr;
				CHECK_ERR( mem_stream->Read( OUT hdr ));
				CHECK_ERR( hdr.name == PipelinePack_Name );
				CHECK_ERR( hdr.ver == PipelinePack_Version );
			}

			PipelineStorage::BlockOffsets_t		block_offsets;
			CHECK_ERR( mem_stream->Read( OUT block_offsets ));

			// create descriptor set layouts
			if ( not rmDSLayouts.empty() )
			{
				const auto	block_off = block_offsets[ uint(EMarker::DescrSetLayouts) ];
				CHECK_ERR( block_off != UMax );
				CHECK_ERR( _LoadDescrSetLayouts( resMngr, *mem_stream, rmDSLayouts, block_off ));
			}

			// create pipeline layouts
			if ( not pplnLayouts.empty() )
			{
				const auto	block_off = block_offsets[ uint(EMarker::PipelineLayouts) ];
				CHECK_ERR( block_off != UMax );
				CHECK_ERR( _LoadPipelineLayouts( resMngr, *mem_stream, rmDSLayouts, pplnLayouts, block_off ));
			}
		}
		return true;
	}

/*
=================================================
	_LoadDescrSetLayouts
=================================================
*/
	bool  RPipelinePack::_LoadDescrSetLayouts (RResourceManager &resMngr, ArrayRStream &memStream,
											   ArrayView<RmDescriptorSetLayoutID> rmDSLayouts, const Bytes blockOffset) __NE___
	{
		using StackAllocator_t	= StackAllocator< UntypedAllocator, 16, false >;
		using UniformOffsets_t	= DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniform_t			= DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t		= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		Serializing::Deserializer	des{ memStream.ToSubStream( blockOffset ), _allocator.get() };
		StackAllocator_t			stack_alloc;

		EMarker	marker;
		uint	count = 0;

		CHECK_ERR( des( OUT marker, OUT count ));
		CHECK_ERR( marker == EMarker::DescrSetLayouts and count <= PipelineStorage::MaxDSLayoutCount );
		CHECK_ERR( usize{count} == rmDSLayouts.size() );

		_rmToDSLayout.reserve( count );
		_nameToDSLayout.reserve( count );

		for (uint i = 0; i < count; ++i)
		{
			// same as 'PipelinePack::_LoadDescrSetLayouts()'
			const auto					bm				= stack_alloc.PushAuto();
			DSLayoutName::Optimized_t	dsl_name;
			EDescSetUsage				usage			= Default;
			EShaderStages				stages			= Default;
			FSNameArr_t					features;
			UniformOffsets_t			desc_offsets	= {};
			uint						sampler_count	= 0;
			SamplerID *					samplers		= null;
			const bool					is_supported	= (rmDSLayouts[i] != Default);

			CHECK_ERR( des( OUT dsl_name, OUT usage, OUT stages, OUT features, OUT desc_offsets, OUT sampler_count ));

			const uint	uniform_count = desc_offsets.back();
			CHECK_ERR( sampler_count <= DescriptorSetLayoutDesc::MaxSamplers );
			CHECK_ERR( uniform_count <= DescriptorSetLayoutDesc::MaxUniforms );

			if ( sampler_count > 0 )
			{
				samplers = stack_alloc.Allocate< SamplerID >( sampler_count );
				CHECK_ERR( samplers != null );

				for (uint j = 0; j < sampler_count; ++j)
				{
					SamplerName::Optimized_t	samp_name;
					CHECK_ERR( des( OUT samp_name ));

					if ( is_supported )
					{
						samplers[j] = GetSampler( SamplerName{samp_name} );
						CHECK_ERR( samplers[j] );
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

			// if not supported
			if ( not is_supported )
			{
				_rmToDSLayout.emplace( rmDSLayouts[i], Default );
				continue;
			}

			RDescriptorSetLayout::CreateInfo	ci;
			ci.id			= rmDSLayouts[i];
			ci.uniforms		= Uniforms_t{ uniform_count, un_names, un_data, un_offsets };
			ci.unOffsets	= desc_offsets;
			ci.usage		= usage;

			auto	id = resMngr.CreateDescriptorSetLayout( ci );
			CHECK_ERR( id );

			_nameToDSLayout.emplace( dsl_name, id.Get() );		// throw
			_rmToDSLayout.emplace( rmDSLayouts[i], RVRef(id) );	// throw
		}

		return true;
	}

/*
=================================================
	_LoadPipelineLayouts
=================================================
*/
	bool  RPipelinePack::_LoadPipelineLayouts (RResourceManager &resMngr, ArrayRStream &memStream,
												ArrayView<RmDescriptorSetLayoutID> rmDSLayouts,
												ArrayView<RmPipelineLayoutID> rmPplnLayouts, Bytes blockOffset) __NE___
	{
		Serializing::Deserializer	des{ memStream.ToSubStream( blockOffset ), _allocator.get() };

		const auto	api		= resMngr.GetDevice().GetGraphicsAPI();
		uint		count	= 0;
		EMarker		marker;

		CHECK_ERR( des( OUT marker, OUT count ));
		CHECK_ERR( marker == EMarker::PipelineLayouts and count <= PipelineStorage::MaxPplnLayoutCount );
		CHECK_ERR( usize{count} == rmPplnLayouts.size() );

		for (uint i = 0; i < count; ++i)
		{
			PipelineLayoutDesc	desc;
			CHECK_ERR( des( OUT desc ));

			const bool	is_supported = (rmPplnLayouts[i] != Default);
			DSLMap_t	desc_sets;
			RmDSLMap_t	rm_ds;

			if ( not is_supported )
				continue;

			for (usize j = 0; j < desc.descrSets.size(); ++j)
			{
				auto	src = desc.descrSets[j];
				CHECK_ERR( usize(src.second.uid) < rmDSLayouts.size() );

				RmDescriptorSetLayoutID	rm = rmDSLayouts[ usize(src.second.uid) ];
				DescSetBinding			binding;

				CHECK_ERR( rm != Default );

				switch_enum( api )
				{
					case EGraphicsAPI::Vulkan :		binding.vkIndex  = src.second.vkIndex;		break;
					case EGraphicsAPI::Metal :		binding.mtlIndex = src.second.mtlIndex;		break;
				}
				switch_end

				rm_ds.emplace( src.first, MakePair( binding, rm ));
			}

			CHECK_ERR( ConvertDescSetsLayouts( rm_ds, OUT desc_sets ));

			auto	id = resMngr.CreatePipelineLayout( RPipelineLayout::CreateInfo{
															desc_sets,
															desc.pushConstants.items
															GFX_DBG_ONLY(, "pl-"s+ToString(i))
														});
			CHECK_ERR( id );

			_rmToPplnLayout.emplace( rmPplnLayouts[i], RVRef(id) );
		}
		return true;
	}

/*
=================================================
	_CreateRenderPasses
=================================================
*/
	bool  RPipelinePack::_CreateRenderPasses (RResourceManager &resMngr, ArrayView<RenderPassName::Optimized_t> rpNames) __NE___
	{
		for (auto name : rpNames)
		{
			Msg::ResMngr_GetRenderPass				msg;
			RC<Msg::ResMngr_GetRenderPass_Response>	res;

			msg.packId	= _packId;
			msg.name	= name;

			CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

			auto	id = resMngr.CreateRenderPass( *res );
			CHECK_ERR( id );

			_renderPassMap.emplace( name, RVRef(id) );
		}
		CHECK_Eq( _renderPassMap.size(), rpNames.size() );
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RPipelinePack::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		resMngr.ImmediatelyRelease( INOUT _parentPackId );
		_unsupportedFS.clear();

		for (auto& [name, id] : _samplerRefs) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( id ));
		}
		_samplerRefs.clear();

		{
			auto	rtechs = _renTechs.WriteLock();

			for (auto& rt : *rtechs)
			{
				CHECK( rt.use_count() == 1 );
				rt->Destroy( resMngr );
			}
			rtechs->clear();
		}

		for (auto& [rm, id] : _rmToDSLayout) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}
		_rmToDSLayout.clear();

		for (auto& [rm, id] : _rmToPplnLayout) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}
		_rmToPplnLayout.clear();

		for (auto& [name, id] : _renderPassMap) {
			DEV_CHECK( resMngr.ImmediatelyRelease2( INOUT id ));
		}
		_renderPassMap.clear();

		ASSERT( _allocator.use_count() == 1 );
		_allocator = null;

		GFX_DBG_ONLY( _debugName.clear();)

		if ( _packId )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_packId} ));

		_packId			= Default;
		_parentPack		= null;
		_parentPackId	= Default;
	}

/*
=================================================
	GetSampler
=================================================
*/
	SamplerID  RPipelinePack::GetSampler (SamplerName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _samplerRefs.find( name );

		if_likely( it != _samplerRefs.end() )
			return it->second;

		// search in parent pack
		if_likely( _parentPack != null )
			return _parentPack->GetSampler( name );

		return Default;
	}

/*
=================================================
	GetDSLayout
=================================================
*/
	DescriptorSetLayoutID  RPipelinePack::GetDSLayout (DSLayoutName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _nameToDSLayout.find( name );

		if_likely( it != _nameToDSLayout.end() )
			return it->second;

		return Default;
	}

/*
=================================================
	Cast
=================================================
*/
	auto  RPipelinePack::Cast (RmDescriptorSetLayoutID srcId) C_NE___ -> DescriptorSetLayoutID
	{
		DRC_SHAREDLOCK( _drCheck );

		if ( srcId == Default )
			return Default;

		auto	it = _rmToDSLayout.find( srcId );
		CHECK_ERR( it != _rmToDSLayout.end() );

		return it->second;
	}

/*
=================================================
	Cast
=================================================
*/
	auto  RPipelinePack::Cast (RmPipelineLayoutID srcId) C_NE___ -> PipelineLayoutID
	{
		DRC_SHAREDLOCK( _drCheck );

		if ( srcId == Default )
			return Default;

		auto	it = _rmToPplnLayout.find( srcId );
		CHECK_ERR( it != _rmToPplnLayout.end() );

		return it->second;
	}

/*
=================================================
	ConvertDescSetsLayouts
=================================================
*/
	bool  RPipelinePack::ConvertDescSetsLayouts (const RmDSLMap_t &src, OUT DSLMap_t &dst) C_NE___
	{
		for (auto [key, info] : src)
		{
			auto	id = Cast( info.second );
			CHECK_ERR( id );
			CHECK_ERR( info.first );
			dst.emplace( key, MakePair( info.first, id ));
		}
		return src.size() == dst.size();
	}

/*
=================================================
	GetRenderPass
=================================================
*/
	auto  RPipelinePack::GetRenderPass (RenderPassName::Ref name) C_NE___ -> RenderPassID
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _renderPassMap.find( name );

		if ( it != _renderPassMap.end() )
			return it->second;

		return Default;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
