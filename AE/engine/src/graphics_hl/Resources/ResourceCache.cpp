// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/ResourceCache.h"

namespace AE::Graphics
{

/*
=================================================
	constructor / destructor
=================================================
*/
	ResourceCache::ResourceCache () __NE___
	{
		EXLOCK( _guard );
		_cache.reserve( 1024 );
	}

	ResourceCache::~ResourceCache () __NE___
	{
		EXLOCK( _guard );
		_cache.clear();
	}

/*
=================================================
	_GetResource
=================================================
*/
	RC<CachedResource>  ResourceCache::_GetResource (CachedResourceName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );

		auto	it = _cache.find( name );

		if_likely( it != _cache.end() )
		{
			ASSERT( it->second->LoadingStatus() >= ELoadingStatus::Created );
			return it->second;
		}
		return null;
	}

/*
=================================================
	_RemoveResource
=================================================
*/
	bool  ResourceCache::_RemoveResource (CachedResourceName::Ref name) __NE___
	{
		EXLOCK( _guard );

		auto	it = _cache.find( name );

		if ( it != _cache.end() )
		{
			_cache.erase( it );
			return true;
		}
		return false;
	}

/*
=================================================
	ClearUnused
=================================================
*/
	usize  ResourceCache::ClearUnused () __NE___
	{
		EXLOCK( _guard );

		usize	count = 0;
		for (auto it = _cache.begin(), end = _cache.end(); it != end;)
		{
			if_unlikely( it->second.use_count() == 1 )
			{
				auto	copy = it++;
				_cache.erase( copy );
				++count;
			}
			else
				++it;
		}
		return count;
	}

/*
=================================================
	Clear
=================================================
*/
	void  ResourceCache::Clear () __NE___
	{
		EXLOCK( _guard );
		_cache.clear();
	}


} // AE::Graphics
