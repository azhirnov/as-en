// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

/*
=================================================
	GetResource
=================================================
*/
	template <typename ID>
	auto const*  RESMNGR::GetResource (ID id, Bool incRef, Bool quiet) C_NE___
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
	auto const*  RESMNGR::GetResource (const Strong<ID> &id, Bool incRef, Bool quiet) C_NE___
	{
		return GetResource( *id, incRef, quiet );
	}
	
/*
=================================================
	GetDescription
=================================================
*/
	template <typename ID>
	auto const&  RESMNGR::_GetDescription (ID id) C_NE___
	{
		auto*	res = GetResource( id );

		using T = RemoveAllQualifiers< decltype(*res) >;
		static const typename FunctionInfo< decltype(&T::Description) >::result	defaultDesc{};

		return res ? res->Description() : defaultDesc;
	}
	
	inline BufferDesc const&  RESMNGR::GetDescription (BufferID id) C_NE___ {
		return _GetDescription( id );
	}

	inline ImageDesc const&  RESMNGR::GetDescription (ImageID id) C_NE___ {
		return _GetDescription( id );
	}

	inline BufferViewDesc const&  RESMNGR::GetDescription (BufferViewID id) C_NE___ {
		return _GetDescription( id );
	}

	inline ImageViewDesc const&  RESMNGR::GetDescription (ImageViewID id) C_NE___ {
		return _GetDescription( id );
	}

/*
=================================================
	IsAlive
=================================================
*/
	template <usize IS, usize GS, uint UID>
	bool  RESMNGR::IsAlive (HandleTmpl<IS,GS,UID> id) C_NE___
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
	auto  RESMNGR::AcquireResource (HandleTmpl<IS, GS, UID> id) __NE___
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
	if cache empty then acquire new indices from main pool (internally synchronized),
	if pool is full then error (false) will be returned.
=================================================
*/
	template <typename ID>
	bool  RESMNGR::_Assign (OUT ID &id) __NE___
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
	void  RESMNGR::_Unassign (ID id) __NE___
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
	int  RESMNGR::_ReleaseResource (ID id, uint refCount) __NE___
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
	bool  RESMNGR::ReleaseResources (Arg0 &arg0, Args& ...args) __NE___
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
	String  RESMNGR::HashToName (const NamedID< Size, UID, Opt, Seed > &name) C_NE___
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
	inline RESMNGR::NativeBuffer_t  RESMNGR::GetBufferHandle (BufferID id) C_NE___
	{
		auto*	buf = GetResource( id );
		CHECK_ERR( buf );

		return buf->Handle();
	}

	inline RESMNGR::NativeImage_t  RESMNGR::GetImageHandle (ImageID id) C_NE___
	{
		auto*	img = GetResource( id );
		CHECK_ERR( img );

		return img->Handle();
	}
		
	inline RESMNGR::NativeBufferView_t  RESMNGR::GetBufferViewHandle (BufferViewID id) C_NE___
	{
		auto*	view = GetResource( id );
		CHECK_ERR( view );

		return view->Handle();
	}

	inline RESMNGR::NativeImageView_t  RESMNGR::GetImageViewHandle (ImageViewID id) C_NE___
	{
		auto*	view = GetResource( id );
		CHECK_ERR( view );

		return view->Handle();
	}
	
/*
=================================================
	GetResources
=================================================
*/
	template <typename ID>
	auto*  RESMNGR::GetResources (ID id) C_NE___
	{
		return GetResource( id );
	}
		
	template <typename ID0, typename ID1, typename ...IDs>
	auto  RESMNGR::GetResources (ID0 id0, ID1 id1, IDs ...ids) C_NE___
	{
		return Tuple{ GetResource( id0 ), GetResource( id1 ), GetResource( ids ) ... };
	}
		
	template <typename ID>
	auto&  RESMNGR::GetResourcesOrThrow (ID id) C_Th___
	{
		auto*	result = GetResource( id );
		CHECK_THROW( result != null );
		return *result;
	}
		
	template <typename ID0, typename ID1, typename ...IDs>
	auto  RESMNGR::GetResourcesOrThrow (ID0 id0, ID1 id1, IDs ...ids) C_Th___
	{
		auto	result = TupleRef{ GetResource( id0 ), GetResource( id1 ), GetResource( ids ) ... };
		CHECK_THROW( result.AllNonNull() );
		return result;
	}
	

#undef RESMNGR
