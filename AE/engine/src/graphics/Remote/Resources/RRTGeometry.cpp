// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ResourceValidation.h"
# include "graphics/Remote/Resources/RRTGeometry.h"
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
	RRTGeometry::~RRTGeometry () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _geomId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RRTGeometry::Create (RResourceManager &resMngr, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _geomId );
		CHECK_ERR( desc.size > 0 );

		_desc = desc;
		GRES_CHECK( RTGeometry_IsSupported( resMngr, desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateRTGeometry				msg;
		RC<Msg::ResMngr_CreateRTGeometry_Response>	res;

		msg.desc		= _desc;
		msg.gfxAlloc	= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_geomId		= res->geomId;
		_memoryId	= res->memoryId;
		_address	= res->addr;
		_desc		= res->desc;
		CHECK_ERR( _geomId );

		_allocator = RVRef(allocator);
		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RRTGeometry::Destroy (RResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto	id	= _geomId;
		_geomId		= Default;
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
	RTASBuildSizes  RRTGeometry::GetBuildSizes (const RResourceManager &resMngr, const RTGeometryBuild &build) __NE___
	{
		Msg::ResMngr_GetRTGeometrySizes					msg;
		RC<Msg::ResMngr_GetRTGeometrySizes_Response>	res;
		RTempLinearAllocator							alloc;

		msg.desc = build;
		msg.desc.triangles.set< RTGeometryBuild::TrianglesData >( null );
		msg.desc.aabbs.set< RTGeometryBuild::AABBsData >( null );
		msg.desc.scratch.id = Default;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->sizes;
	}

/*
=================================================
	IsSerializedMemoryCompatible
=================================================
*/
	bool  RRTGeometry::IsSerializedMemoryCompatible (const RDevice &dev, const void* ptr, Bytes size) __NE___
	{
		UNTESTED
		return false;
	}

/*
=================================================
	IsSupported (RTGeometryDesc)
=================================================
*/
	bool  RRTGeometry::IsSupported (const RResourceManager &resMngr, const RTGeometryDesc &desc) __NE___
	{
		return RTGeometry_IsSupported( resMngr, desc );
	}

/*
=================================================
	IsSupported (RTGeometryBuild)
=================================================
*/
	bool  RRTGeometry::IsSupported (const RResourceManager &resMngr, const RTGeometryBuild &build) __NE___
	{
		Msg::ResMngr_IsSupported_RTGeometryBuild	msg;
		RC<Msg::ResMngr_IsSupported_Response>		res;
		RTempLinearAllocator						alloc;

		msg.desc = build;
		CHECK_ERR( ConvertBuildInfo( resMngr, INOUT msg.desc, alloc ));

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}

/*
=================================================
	ConvertBuildInfo
=================================================
*/
	bool  RRTGeometry::ConvertBuildInfo (const RResourceManager &resMngr, INOUT RTGeometryBuild &build, RTempLinearAllocator &alloc) __NE___
	{
		if ( not build.triangles.empty() )
		{
			const usize	count	= build.triangles.size();
			auto*		src		= build.triangles.data< RTGeometryBuild::TrianglesData >();
			auto*		dst		= alloc.Allocate< RTGeometryBuild::TrianglesData >( count );
			CHECK_ERR( dst != null );

			MemCopy( OUT dst, src, SizeOf<RTGeometryBuild::TrianglesData> * count );
			build.triangles.set( dst );

			for (usize i = 0; i < count; ++i)
			{
				auto*	vb	= resMngr.GetResource( dst[i].vertexData,	False{"don't inc ref"}, True{"quiet"} );
				auto*	ib	= resMngr.GetResource( dst[i].indexData,	False{"don't inc ref"}, True{"quiet"} );
				auto*	tb	= resMngr.GetResource( dst[i].transformData,False{"don't inc ref"}, True{"quiet"} );
				CHECK_ERR( vb != null );

				dst[i].vertexData	= RmCast( vb->Handle() );
				dst[i].indexData	= (ib != null ? RmCast( ib->Handle() ) : Default);
				dst[i].transformData= (tb != null ? RmCast( tb->Handle() ) : Default);
			}
		}

		if ( not build.aabbs.empty() )
		{
			const usize	count	= build.aabbs.size();
			auto*		src		= build.aabbs.data< RTGeometryBuild::AABBsData >();
			auto*		dst		= alloc.Allocate< RTGeometryBuild::AABBsData >( count );
			CHECK_ERR( dst != null );

			MemCopy( OUT dst, src, SizeOf<RTGeometryBuild::AABBsData> * count );
			build.aabbs.set( dst );

			for (usize i = 0; i < count; ++i)
			{
				auto*	data = resMngr.GetResource( dst[i].data );
				CHECK_ERR( data != null );

				dst[i].data = RmCast( data->Handle() );
			}
		}

		if ( build.scratch.id )
		{
			auto*	scratch = resMngr.GetResource( build.scratch.id );
			CHECK_ERR( scratch != null );
			build.scratch.id = RmCast( scratch->Handle() );
		}

		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
