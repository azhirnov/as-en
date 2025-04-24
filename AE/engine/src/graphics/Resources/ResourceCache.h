// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  yes
*/

#pragma once

#include "graphics/Graphics.pch.h"

namespace AE::Graphics
{
	class ResourceUploadManager;


	//
	// Cached Resource
	//
	class CachedResource : public EnableRC<CachedResource>
	{
	// types
	public:
		enum class ELoadingStatus : uint
		{
			Failed,
			Initial,
			Created,		// internal resource is created, memory allocated, but not initialized
			Loading,		// from Disk
			Uploading,		// to VRAM
			Complete,
		};


	// methods
	public:
		CachedResource ()						__NE___ { _SetLoadingStatus( ELoadingStatus::Initial ); }

		ND_ ELoadingStatus	LoadingStatus ()	C_NE___	{ return RefCast< Atomic<ELoadingStatus> >(_unused).load(); }

	protected:
		ELoadingStatus  _SetLoadingStatus (ELoadingStatus value) __NE___ { return RefCast< Atomic<ELoadingStatus> >(_unused).exchange( value ); }
	};



	//
	// Resource Cache
	//
	class ResourceCache
	{
	// types
	private:
		using ResPtr			= RC<CachedResource>;
		using ResMap_t			= FlatHashMap< CachedResourceName::Optimized_t, RC<CachedResource> >;
		using ELoadingStatus	= CachedResource::ELoadingStatus;


	// variables
	private:
		mutable RWSpinLock		_guard;
		ResMap_t				_cache;


	// methods
	public:
		ResourceCache ()														__NE___;
		~ResourceCache ()														__NE___;

		template <typename T>
		ND_ RC<T>  GetResource (CachedResourceName::Ref name)					C_NE___;


		// Returns 'null' if 'res' was inserted to cache,
		// otherwise returns previously inserted resource.
		//
		template <typename T>
		ND_ RC<T>  InsertResource (CachedResourceName::Ref name, RC<T> res)		__NE___;


		// Returns 'true' if 'res' was inserted to cache, otherwise returns 'false'.
		//
		template <typename T>
		ND_ bool  AddResource (CachedResourceName::Ref name, RC<T> res)			__NE___;


			bool  RemoveResource (CachedResourceName::Ref name)					__NE___;

		// Returns number of removed resources.
			usize ClearUnused ()												__NE___;

			void  Clear ()														__NE___;

	private:
		ND_ ResPtr	_GetResource (CachedResourceName::Ref)						C_NE___;
			bool	_RemoveResource (CachedResourceName::Ref)					__NE___;

	};
//-----------------------------------------------------------------------------



/*
=================================================
	GetResource
=================================================
*/
	template <typename T>
	RC<T>  ResourceCache::GetResource (CachedResourceName::Ref name) C_NE___
	{
		ResPtr	res = _GetResource( name );
		ASSERT( Base::CastAllowed<T>( res.get() ));
		return RC<T>{RVRef(res)};
	}

/*
=================================================
	InsertResource
=================================================
*/
	template <typename T>
	RC<T>  ResourceCache::InsertResource (CachedResourceName::Ref name, RC<T> res) __NE___
	{
		StaticAssert( IsBaseOf< CachedResource, T >);
		CHECK_ERR( name.IsDefined() );
		CHECK_ERR( res );
		CHECK_ERR( res->LoadingStatus() >= ELoadingStatus::Created );

		TRY{
			EXLOCK( _guard );

			auto&	current = _cache[ name ];  // throw

			if ( current == null )
			{
				current = RVRef(res);
				return null;
			}
			else
			{
				ASSERT( Base::CastAllowed<T>( current.get() ));
				ASSERT( current->LoadingStatus() >= ELoadingStatus::Created );

				return RC<T>{ current };
			}
		}
		CATCH_ALL(
			return null;
		)
	}

/*
=================================================
	AddResource
=================================================
*/
	template <typename T>
	bool  ResourceCache::AddResource (CachedResourceName::Ref name, RC<T> res) __NE___
	{
		return InsertResource( name, RVRef(res) ) == null;
	}


} // AE::Graphics
