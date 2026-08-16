// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Remote/Resources/RRTMicromap.h"
# include "graphics_rhi/Remote/RResourceManager.h"
# include "graphics_rhi/Remote/REnumCast.h"
# include "graphics_rhi/Private/EnumUtils.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RRTMicromap::~RRTMicromap () __NE___
	{
		ASSERT( not _micromap );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRTMicromap::Create (ResourceManager &resMngr, const RTMicromapDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		CHECK_ERR( not _micromap );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( IsSupported( resMngr, desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateRTMicromap				msg;
		RC<Msg::ResMngr_CreateRTMicromap_Response>	res;

		msg.desc		= _desc;
		msg.gfxAlloc	= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_micromap	= res->micromapId;
		_memoryId	= res->memoryId;
		_desc		= res->desc;
		CHECK_ERR( _micromap );

		_allocator = RVRef(allocator);
		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRTMicromap::Destroy (ResourceManager &resMngr) __NE___
	{
		auto	id	= _micromap;
		_micromap	= Default;
		_memoryId	= Default;
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
	RTMicromapBuildSizes  RRTMicromap::GetBuildSizes (const ResourceManager &resMngr, const RTMicromapInfo &desc) __NE___
	{
		Msg::ResMngr_GetRTMicromapBuildSizes				msg;
		RC<Msg::ResMngr_GetRTMicromapBuildSizes_Response>	res;

		msg.desc = desc;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->sizes;
	}

/*
=================================================
	IsSupported (RTGeometryDesc)
=================================================
*/
	bool  RRTMicromap::IsSupported (const ResourceManager &resMngr, const RTMicromapInfo &desc) __NE___
	{
		return RTMicromapInfo_IsSupported( resMngr, desc );
	}

	bool  RRTMicromap::IsSupported (const ResourceManager &resMngr, const RTMicromapDesc &desc) __NE___
	{
		return RTMicromapDesc_IsSupported( resMngr, desc );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
