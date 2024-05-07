// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ResourceValidation.h"
# include "graphics/Remote/Resources/RRTScene.h"
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/REnumCast.h"
# include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RRTScene::~RRTScene () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _sceneId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRTScene::Create (RResourceManager &resMngr, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _sceneId );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( RTScene_IsSupported( resMngr, desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateRTScene					msg;
		RC<Msg::ResMngr_CreateRTScene_Response>		res;

		msg.desc		= _desc;
		msg.gfxAlloc	= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_sceneId	= res->sceneId;
		_memoryId	= res->memoryId;
		_address	= res->addr;
		_desc		= res->desc;
		CHECK_ERR( _sceneId );

		_allocator = RVRef(allocator);
		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRTScene::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto	id	= _sceneId;
		_sceneId	= Default;
		_memoryId	= Default;
		_address	= Default;
		_desc		= Default;
		_allocator	= null;

		GFX_DBG_ONLY( _debugName.clear() );

		if ( id )
		{
			Msg::ResMngr_ReleaseResource	msg {id};
			CHECK( resMngr.GetDevice().Send( msg ));
		}
	}

/*
=================================================
	GetBuildSizes
=================================================
*/
	RTASBuildSizes  RRTScene::GetBuildSizes (const RResourceManager &resMngr, const RTSceneBuild &build) __NE___
	{
		Msg::ResMngr_GetRTSceneSizes				msg;
		RC<Msg::ResMngr_GetRTSceneSizes_Response>	res;

		msg.desc = build;
		CHECK_ERR( ConvertBuildInfo( resMngr, INOUT msg.desc ));

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->sizes;
	}

/*
=================================================
	IsSerializedMemoryCompatible
=================================================
*/
	bool  RRTScene::IsSerializedMemoryCompatible (const RDevice &dev, const void* ptr, Bytes size) __NE___
	{
		UNTESTED
		return false;
	}

/*
=================================================
	IsSupported (RTSceneDesc)
=================================================
*/
	bool  RRTScene::IsSupported (const RResourceManager &resMngr, const RTSceneDesc &desc) __NE___
	{
		return RTScene_IsSupported( resMngr, desc );
	}

/*
=================================================
	IsSupported (RTSceneBuild)
=================================================
*/
	bool  RRTScene::IsSupported (const RResourceManager &resMngr, const RTSceneBuild &build) __NE___
	{
		Msg::ResMngr_IsSupported_RTSceneBuild	msg;
		RC<Msg::ResMngr_IsSupported_Response>	res;

		msg.desc = build;
		CHECK_ERR( ConvertBuildInfo( resMngr, INOUT msg.desc ));

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  RRTScene::ConvertBuildInfo (const RResourceManager &resMngr, INOUT RTSceneBuild &build) __NE___
	{
		if ( build.scratch.id )
		{
			auto*	scratch = resMngr.GetResource( build.scratch.id );
			CHECK_ERR( scratch != null );
			build.scratch.id = RmCast( scratch->Handle() );
		}

		if ( build.instanceData.id )
		{
			auto*	inst_buf = resMngr.GetResource( build.instanceData.id );
			CHECK_ERR( inst_buf != null );
			build.instanceData.id = RmCast( inst_buf->Handle() );
		}

		RTSceneBuild::GeometrySet_t	unique_geoms = build.uniqueGeoms;
		build.uniqueGeoms.clear();

		for (auto id : unique_geoms)
		{
			auto*	geom = resMngr.GetResource( id );
			CHECK_ERR( geom != null );
			build.uniqueGeoms.insert( RmCast( geom->Handle() ));
		}
		CHECK_Eq( unique_geoms.size(), build.uniqueGeoms.size() );

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
