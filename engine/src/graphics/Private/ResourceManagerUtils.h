// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef RESMNGR_HEADER
#	undef RESMNGR_HEADER
/*
=================================================
	GetResource
=================================================
*/
	template <typename ID>
	auto const*  RESMNGR::GetResource (ID id, Bool incRef, Bool quiet) const
	{
		auto&	pool = _GetResourceCPool( id );

		using Result_t = typename RemoveReference<decltype(pool)>::Value_t::Resource_t const*;

		if_likely( auto* res = pool.At( id.Index() ))
		{
			if_likely( res->IsCreated() & (res->GetGeneration() == id.Generation()) )
			{
				if_unlikely( incRef )
					res->AddRef();

				return &res->Data();
			}

			Unused( quiet );
			ASSERT( quiet or res->IsCreated() );
			ASSERT( quiet or res->GetGeneration() == id.Generation() );
		}

		ASSERT( quiet and "resource index is out of range" );
		return static_cast< Result_t >(null);
	}
	
	template <typename ID>
	auto const*  RESMNGR::GetResource (const Strong<ID> &id, Bool incRef, Bool quiet) const
	{
		return GetResource( *id, incRef, quiet );
	}
	
/*
=================================================
	GetDescription
=================================================
*/
	template <typename ID>
	auto const&  RESMNGR::_GetDescription (ID id) const
	{
		auto*	res = GetResource( id );

		using T = RemoveAllQualifiers< decltype(*res) >;
		static const typename FunctionInfo< decltype(&T::Description) >::result	defaultDesc{};

		return res ? res->Description() : defaultDesc;
	}
	
	inline BufferDesc const&  RESMNGR::GetDescription (BufferID id) const {
		return _GetDescription( id );
	}

	inline ImageDesc const&  RESMNGR::GetDescription (ImageID id) const {
		return _GetDescription( id );
	}

	inline BufferViewDesc const&  RESMNGR::GetDescription (BufferViewID id) const {
		return _GetDescription( id );
	}

	inline ImageViewDesc const&  RESMNGR::GetDescription (ImageViewID id) const {
		return _GetDescription( id );
	}

/*
=================================================
	IsAlive
=================================================
*/
	template <usize IS, usize GS, uint UID>
	bool  RESMNGR::IsAlive (HandleTmpl<IS,GS,UID> id) const
	{
		ASSERT( id );
		auto&	pool = _GetResourceCPool( id );
		
		if_likely( auto* res = pool.At( id.Index() ))
			return res->GetGeneration() == id.Generation();

		return false;
	}

/*
=================================================
	AcquireResource
=================================================
*/
	template <usize IS, usize GS, uint UID>
	auto  RESMNGR::AcquireResource (HandleTmpl<IS, GS, UID> id)
	{
		using Unique_t = Strong< HandleTmpl< IS, GS, UID >>;

		ASSERT( id );
		
		auto&	pool = _GetResourcePool( id );
		
		if_likely( auto* res = pool.At( id.Index() ))
		{
			if_unlikely( not res->IsCreated() or res->GetGeneration() != id.Generation() )
				return Unique_t{};

			res->AddRef();
			return Unique_t{ id };
		}

		return Unique_t{};
	}

/*
=================================================
	_Assign
----
	acquire free index from cache (cache is local in thread),
	if cache empty then acquire new indices from main pool (internaly synchronized),
	if pool is full then error (false) will be returned.
=================================================
*/
	template <typename ID>
	bool  RESMNGR::_Assign (OUT ID &id)
	{
		auto&	pool	= _GetResourcePool( id );
		auto	index	= pool.Assign();
		CHECK_ERR( index != UMax );

		id = ID{ index, pool[index].GetGeneration() };
		return true;
	}
	
/*
=================================================
	_Unassign
=================================================
*/
	template <typename ID>
	void  RESMNGR::_Unassign (ID id)
	{
		ASSERT( id );
		auto&	pool = _GetResourcePool( id );

		pool.Unassign( id.Index() );
	}

/*
=================================================
	_ReleaseResource
=================================================
*/
	template <typename ID>
	int  RESMNGR::_ReleaseResource (ID id, uint refCount)
	{
		if_unlikely( not id )
			return -1;

		auto&	pool = _GetResourcePool( id );
		
		if_likely( auto* res = pool.At( id.Index() ))
		{
			if_likely( res->GetGeneration() == id.Generation() )
			{
				int	count = res->ReleaseRef( refCount );

				if_unlikely( count == 0 and res->IsCreated() )
				{
					res->Destroy( *this );
					pool.Unassign( id.Index() );
				}
				return count;
			}
		}
		return -1;
	}
	
/*
=================================================
	ReleaseResources
=================================================
*/
	template <typename Arg0, typename ...Args>
	bool  RESMNGR::ReleaseResources (Arg0 &arg0, Args& ...args)
	{
		bool	res = ReleaseResource( INOUT arg0 );
		
		if constexpr( CountOf<Args...>() > 0 )
			return res & ReleaseResources( FwdArg<Args&>( args )... );
		else
			return res;
	}
	
/*
=================================================
	HashToName
=================================================
*/
	template <usize Size, uint UID, bool Opt, uint Seed>
	String  RESMNGR::HashToName (const NamedID< Size, UID, Opt, Seed > &name) const
	{
		#ifdef AE_DBG_OR_DEV
			if constexpr( Opt )
			{
				SHAREDLOCK( _hashToNameGuard );
				return _hashToName( name );
			}
			else
				return String{name.GetName()};
		#else
			Unused( name );
			return {};
		#endif
	}

/*
=================================================
	Get***Handle
=================================================
*/
	inline RESMNGR::NativeBuffer_t  RESMNGR::GetBufferHandle (BufferID id) const
	{
		auto*	buf = GetResource( id );
		CHECK_ERR( buf );

		return buf->Handle();
	}

	inline RESMNGR::NativeImage_t  RESMNGR::GetImageHandle (ImageID id) const
	{
		auto*	img = GetResource( id );
		CHECK_ERR( img );

		return img->Handle();
	}
		
	inline RESMNGR::NativeBufferView_t  RESMNGR::GetBufferViewHandle (BufferViewID id) const
	{
		auto*	view = GetResource( id );
		CHECK_ERR( view );

		return view->Handle();
	}

	inline RESMNGR::NativeImageView_t  RESMNGR::GetImageViewHandle (ImageViewID id) const
	{
		auto*	view = GetResource( id );
		CHECK_ERR( view );

		return view->Handle();
	}
#endif
//-----------------------------------------------------------------------------

	

#ifdef RESMNGR_IMPL
#	undef RESMNGR_IMPL
	
namespace {
/*
=================================================
	Replace
----
	destroy previous resource instance and construct new instance
=================================================
*/
	template <typename ResType, typename ...Args>
	inline void  Replace (INOUT ResourceBase<ResType> &target, Args&& ...args)
	{
		Reconstruct<ResType>( target.Data(), FwdArg<Args>( args )... );
	}
	
/*
=================================================
	DestroyResources
=================================================
*/
	template <typename PoolT>
	inline void  DestroyResources (RESMNGR* resMngr, INOUT PoolT &pool)
	{
		pool.UnassignAll( [resMngr] (auto& res)
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
	inline void  LogAssignedResourcesAndDestroy (RESMNGR* resMngr, INOUT PoolT &pool)
	{
		pool.UnassignAll( [resMngr] (auto& res)
			{
				ASSERT( res.IsCreated() );	// all assigned resources should be created

				if_unlikely( res.IsCreated() )
				{
					AE_LOG_DBG( "Resource "s << res.Data().GetDebugName() << " is not released" );
					res.Destroy( *resMngr );
				}
			});
	}
}
//-----------------------------------------------------------------------------



/*
=================================================
	ResourceDestructor
=================================================
*/
	struct RESMNGR::ResourceDestructor
	{
		RESMNGR&	resMngr;

		template <typename T, uint I>
		void operator () ()
		{
			auto	name = resMngr._GetResourcePoolName( T{} );
			Unused( name );

			auto&	pool = resMngr._GetResourcePool( T{} );
			pool.Release();
		}
	};
	
/*
=================================================
	AddHashToName
=================================================
*/
# ifdef AE_DBG_OR_DEV
	void  RESMNGR::AddHashToName (const PipelineCompiler::HashToName &value)
	{
		EXLOCK( _hashToNameGuard );
		_hashToName.Merge( value );
	}
# endif
	
/*
=================================================
	_CreateResource
=================================================
*/
	template <typename ID, typename ...Args>
	forceinline Strong<ID>  RESMNGR::_CreateResource (const char* msg, Args&& ...args)
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
	
/*
=================================================
	IsAlive
=================================================
*
	bool  RESMNGR::IsAlive (const SamplerName &name) const
	{
		auto	id = GetSampler( name );

		if ( id == Default )
			return false;

		return IsAlive( id );
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	LoadRenderTech
=================================================
*/
	RenderTechPipelinesPtr  RESMNGR::LoadRenderTech (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)
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
	Promise<RenderTechPipelinesPtr>  RESMNGR::LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)
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
	Strong<GraphicsPipelineID>  RESMNGR::CreateGraphicsPipeline (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc &desc, PipelineCacheID cache)
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
	Strong<MeshPipelineID>  RESMNGR::CreateMeshPipeline (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc &desc, PipelineCacheID cache)
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
	Strong<ComputePipelineID>  RESMNGR::CreateComputePipeline (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc &desc, PipelineCacheID cache)
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
	Strong<RayTracingPipelineID>  RESMNGR::CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache)
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
	Strong<TilePipelineID>  RESMNGR::CreateTilePipeline (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc &desc, PipelineCacheID cache)
	{
		auto*	pack = GetResource( packId ? packId : _defaultPack.Get() );
		CHECK_ERR( pack != null );

		return pack->CreatePipeline( *this, name, desc, cache );
	}

#endif
//-----------------------------------------------------------------------------


#undef RESMNGR
