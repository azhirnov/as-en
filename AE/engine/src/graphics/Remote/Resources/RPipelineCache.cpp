// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RPipelineCache.h"
# include "graphics/Remote/RResourceManager.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RPipelineCache::~RPipelineCache () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _cacheId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RPipelineCache::Create (const RResourceManager &resMngr, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _cacheId );

		Msg::ResMngr_CreatePipelineCache				msg;
		RC<Msg::ResMngr_CreatePipelineCache_Response>	res;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));

		_cacheId = res->id;
		CHECK_ERR( _cacheId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RPipelineCache::Destroy (const RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _cacheId )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_cacheId} ));

		_cacheId = Default;
		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
