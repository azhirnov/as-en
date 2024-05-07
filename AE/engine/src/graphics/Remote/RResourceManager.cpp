// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

# include "graphics/Remote/Allocators/RGfxMemAllocator.h"
# include "graphics/Remote/Descriptors/RDescriptorAllocator.h"

namespace AE::Graphics
{
	using RDefaultDescriptorAllocator = RDescriptorAllocator;

#	include "graphics/Private/ResourceManager.cpp.h"

	using namespace AE::RemoteGraphics;

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  RResourceManager::GetMemoryInfo (ImageID id, OUT RemoteMemoryObjInfo &info) C_NE___
	{
		auto*	image = GetResource( id );
		CHECK_ERR( image != null );
		return image->GetMemoryInfo( OUT info );
	}

	bool  RResourceManager::GetMemoryInfo (BufferID id, OUT RemoteMemoryObjInfo &info) C_NE___
	{
		auto*	buffer = GetResource( id );
		CHECK_ERR( buffer != null );
		return buffer->GetMemoryInfo( OUT info );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_CreateDescriptorSets
=================================================
*/
	template <typename PplnID>
	bool  RResourceManager::_CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, const usize count,
												   PplnID pplnId, DescriptorSetName::Ref dsName,
												   DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		auto*	ppln = GetResource( pplnId );
		CHECK_ERR( ppln != null );

		auto*	ppln_layout = GetResource( ppln->LayoutId() );
		CHECK_ERR( ppln_layout );

		DescriptorSetLayoutID	layoutId;
		CHECK_ERR( ppln_layout->GetDescriptorSetLayout( dsName, OUT layoutId, OUT binding ));

		return CreateDescriptorSets( OUT dst, count, layoutId, RVRef(allocator), dbgName );
	}

/*
=================================================
	CreateDescriptorSets
=================================================
*/
	bool  RResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
												  PipelinePackID packId, DSLayoutName::Ref dslName,
												  DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		auto*	pack = GetResource( packId );
		CHECK_ERR( pack != null );

		auto	layout_id = pack->GetDSLayout( dslName );
		CHECK_ERR( layout_id );

		return CreateDescriptorSets( OUT dst, count, layout_id, RVRef(allocator), dbgName );
	}

/*
=================================================
	CreateDescriptorSets
=================================================
*/
	bool  RResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, const usize count,
												  DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
	{
		Msg::ResMngr_CreateDescriptorSets3				msg;
		RC<Msg::ResMngr_CreateDescriptorSets_Response>	res;

		auto&	dev	= GetDevice();
		auto*	dsl	= GetResource( layoutId );
		CHECK_ERR( dsl != null );

		msg.layoutId	= dsl->Handle();
		msg.count		= uint(count);
		msg.dsAlloc		= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));
		CHECK_ERR( not res->ds.empty() );

		usize	i		= 0;
		bool	created	= true;

		for (; created and (i < count); ++i)
		{
			dst[i]  = _CreateResource<DescriptorSetID>( "failed when creating descriptor set", *this, res->ds[i], layoutId, allocator, dbgName );
			created = (dst[i].IsValid());
		}

		if ( not created ) {
			for (usize j = 0; j < i; ++j) {
				ImmediatelyRelease( INOUT dst[j] );
			}
		}

		return created;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CreateSampler
=================================================
*/
	Strong<SamplerID>  RResourceManager::CreateSampler (RmSamplerID id) __NE___
	{
		return _CreateResource<SamplerID>( "failed when creating sampler", *this, id );
	}

/*
=================================================
	CreateRenderPass
=================================================
*/
	Strong<RenderPassID>  RResourceManager::CreateRenderPass (const RRenderPass::CreateInfo &ci) __NE___
	{
		return _CreateResource<RenderPassID>( "failed when creating render pass", ci );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_ForceReleaseResources
=================================================
*/
	bool  RResourceManager::_ForceReleaseResources () __NE___
	{
		Msg::ResMngr_ForceReleaseResources	msg;
		RC<Msg::DefaultResponse>			res;

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		return res->ok;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Create***Allocator
=================================================
*/
	GfxMemAllocatorPtr  RResourceManager::CreateLinearGfxMemAllocator (Bytes pageSize) C_NE___
	{
		Msg::ResMngr_CreateLinearGfxMemAllocator			msg;
		RC<Msg::ResMngr_CreateGfxMemAllocator_Response>		res;

		msg.pageSize = pageSize;

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		CHECK_ERR( res->id );

		return MakeRC<RGfxMemAllocator>( res->id, res->minAlign, res->maxSize );
	}

	GfxMemAllocatorPtr  RResourceManager::CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize) C_NE___
	{
		Msg::ResMngr_CreateBlockGfxMemAllocator				msg;
		RC<Msg::ResMngr_CreateGfxMemAllocator_Response>		res;

		msg.blockSize	= blockSize;
		msg.pageSize	= pageSize;

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		CHECK_ERR( res->id );

		return MakeRC<RGfxMemAllocator>( res->id, res->minAlign, res->maxSize );
	}

	GfxMemAllocatorPtr  RResourceManager::CreateUnifiedGfxMemAllocator (Bytes pageSize) C_NE___
	{
		Msg::ResMngr_CreateUnifiedGfxMemAllocator			msg;
		RC<Msg::ResMngr_CreateGfxMemAllocator_Response>		res;

		msg.pageSize = pageSize;

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		CHECK_ERR( res->id );

		return MakeRC<RGfxMemAllocator>( res->id, res->minAlign, res->maxSize );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	LoadRenderTech
=================================================
*/
	RenderTechPipelinesPtr  RResourceManager::LoadRenderTech (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_LoadRenderTech					msg;
		RC<Msg::ResMngr_LoadRenderTech_Response>	res;

		msg.packId	= pack->Handle();
		msg.cacheId	= (cache != null ? cache->Handle() : Default);
		msg.name	= name;

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		if ( res->id == Default )
			return null;	// not supported

		auto	rtech = MakeRC<RPipelinePack::RenderTech>( *pack );
		CHECK_ERR( rtech->Create( *this, *res ));

		return rtech;
	}

/*
=================================================
	LoadRenderTechAsync
=================================================
*/
	Promise<RenderTechPipelinesPtr>  RResourceManager::LoadRenderTechAsync (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cacheId) __NE___
	{
		return MakePromise(	[this, packId, name, cacheId] () {
								return LoadRenderTech( packId, name, cacheId );
							},
							Tuple{},
							"RResourceManager::LoadRenderTechAsync",
							ETaskQueue::Background );
	}

/*
=================================================
	CreateGraphicsPipeline
=================================================
*/
	Strong<GraphicsPipelineID>  RResourceManager::CreateGraphicsPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const GraphicsPipelineDesc &desc, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_CreateGraphicsPipeline					msg;
		RC<Msg::ResMngr_CreateGraphicsPipeline_Response>	res;

		msg.packId	= pack->Handle();
		msg.name	= name;
		msg.desc	= desc;
		msg.cacheId	= (cache != null ? cache->Handle() : Default);

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		return CreatePipeline( RGraphicsPipeline::CreateInfo{ *res, *pack, true });
	}

/*
=================================================
	CreateMeshPipeline
=================================================
*/
	Strong<MeshPipelineID>  RResourceManager::CreateMeshPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const MeshPipelineDesc &desc, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_CreateMeshPipeline					msg;
		RC<Msg::ResMngr_CreateMeshPipeline_Response>	res;

		msg.packId	= pack->Handle();
		msg.name	= name;
		msg.desc	= desc;
		msg.cacheId	= (cache != null ? cache->Handle() : Default);

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		return CreatePipeline( RMeshPipeline::CreateInfo{ *res, *pack, true });
	}

/*
=================================================
	CreateComputePipeline
=================================================
*/
	Strong<ComputePipelineID>  RResourceManager::CreateComputePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const ComputePipelineDesc &desc, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_CreateComputePipeline					msg;
		RC<Msg::ResMngr_CreateComputePipeline_Response>		res;

		msg.packId	= pack->Handle();
		msg.name	= name;
		msg.desc	= desc;
		msg.cacheId	= (cache != null ? cache->Handle() : Default);

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		return CreatePipeline( RComputePipeline::CreateInfo{ *res, *pack, true });
	}

/*
=================================================
	CreateRayTracingPipeline
=================================================
*/
	Strong<RayTracingPipelineID>  RResourceManager::CreateRayTracingPipeline (PipelinePackID packId, PipelineTmplName::Ref name, const RayTracingPipelineDesc &desc, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_CreateRayTracingPipeline					msg;
		RC<Msg::ResMngr_CreateRayTracingPipeline_Response>		res;

		msg.packId	= pack->Handle();
		msg.name	= name;
		msg.desc	= desc;
		msg.cacheId	= (cache != null ? cache->Handle() : Default);

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		return CreatePipeline( RRayTracingPipeline::CreateInfo{ *res, *pack, true });
	}

/*
=================================================
	CreateTilePipeline
=================================================
*/
	Strong<TilePipelineID>  RResourceManager::CreateTilePipeline (PipelinePackID packId, PipelineTmplName::Ref name, const TilePipelineDesc &desc, PipelineCacheID cacheId) __NE___
	{
		auto*	cache	= GetResource( cacheId, False{"don't inc ref"}, True{"quiet"} );
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_CreateTilePipeline					msg;
		RC<Msg::ResMngr_CreateTilePipeline_Response>	res;

		msg.packId	= pack->Handle();
		msg.name	= name;
		msg.desc	= desc;
		msg.cacheId	= (cache != null ? cache->Handle() : Default);

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));

		return CreatePipeline( RTilePipeline::CreateInfo{ *res, *pack, true });
	}

/*
=================================================
	GetSupportedRenderTechs
=================================================
*/
	Array<RenderTechName>  RResourceManager::GetSupportedRenderTechs (PipelinePackID packId) C_NE___
	{
		auto*	pack	= GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		Msg::ResMngr_GetSupportedRenderTechs				msg;
		RC<Msg::ResMngr_GetSupportedRenderTechs_Response>	res;

		msg.id = pack->Handle();

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		return res->result;
	}

/*
=================================================
	InitializeResources
=================================================
*/
	bool  RResourceManager::InitializeResources (Strong<PipelinePackID> defaultPackId) __NE___
	{
		CHECK_ERR( defaultPackId );

		auto	old = _defaultPack.Attach( RVRef(defaultPackId) );

		CHECK_MSG( not old, "already initialized" );
		ReleaseResource( old );

		Msg::ResMngr_InitializeResources	msg;
		RC<Msg::DefaultResponse>			res;

		auto*	pack = GetResource( _defaultPack.Get() );
		CHECK_ERR( pack != null );

		msg.packId = pack->Handle();

		CHECK_ERR( GetDevice().SendAndWait( msg, OUT res ));
		CHECK_ERR( res->ok );

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
