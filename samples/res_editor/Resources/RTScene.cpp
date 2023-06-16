// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Passes/FrameGraph.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{

	
/*
=================================================
	constructor
=================================================
*/
	RTGeometry::RTGeometry (TriangleMeshes_t	triangleMeshes,
							Renderer &			renderer,
							StringView			dbgName) __Th___ :
		IResource{ renderer },
		_triangleMeshes{ RVRef(triangleMeshes) },
		_uploadStatus{ EUploadStatus::InProgress },
		_dbgName{ dbgName }
	{
		_ResQueue().EnqueueForUpload( GetRC() );

		for (auto& tri_mesh : _triangleMeshes) {
			_isMutable |= tri_mesh.isMutable;
		}
		
		auto&				res_mngr	= FrameGraph().GetStateTracker();
		RTGeometryBuild		build;

		Array<RTGeometryBuild::TrianglesInfo>	tri_infos;
		tri_infos.reserve( _triangleMeshes.size() );

		for (auto& src : _triangleMeshes) {
			tri_infos.push_back( src );
		}

		build.options	= _options;
		build.triangles = RTGeometryBuild::Triangles{ tri_infos, Default };

		const auto	sizes			= res_mngr.GetRTGeometrySizes( build );
		const auto	CreateGeometry	= [&res_mngr, &sizes, this] ()
		{{
			return res_mngr.CreateRTGeometry( RTGeometryDesc{ sizes.rtasSize, _options }, _dbgName, _GfxAllocator() );
		}};

		_scratchBuffer = res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
												_dbgName + "-Scrach", _GfxAllocator() );
		_geomIds[0] = CreateGeometry();

		if ( _isMutable ) {
			_geomIds[1] = CreateGeometry();
		}else{
			_geomIds[1] = res_mngr.AcquireResource( _geomIds[0].Get() );
		}
		
		CHECK_THROW( _scratchBuffer );
		CHECK_THROW( _geomIds[0] and _geomIds[1] );

		_address[0] = res_mngr.GetDeviceAddress( _geomIds[0].Get() );
		_address[1] = res_mngr.GetDeviceAddress( _geomIds[1].Get() );
	}

/*
=================================================
	destructor
=================================================
*/
	RTGeometry::~RTGeometry ()
	{
		auto&	res_mngr = FrameGraph().GetStateTracker();
		res_mngr.ReleaseResourceArray( _geomIds );
		res_mngr.ReleaseResource( _scratchBuffer );
	}
	
/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  RTGeometry::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;
	
		EXLOCK( _loadOpGuard );
			
		bool	complete	= true;
		bool	failed		= false;

		for (auto& tri_mesh : _triangleMeshes)
		{
			EUploadStatus	v_status	= tri_mesh.vbuffer->GetStatus();
			EUploadStatus	i_status	= (tri_mesh.ibuffer ? tri_mesh.ibuffer->GetStatus() : EUploadStatus::Complete);

			complete &= (v_status == EUploadStatus::Complete) and (i_status == EUploadStatus::Complete);
			failed   |= (v_status == EUploadStatus::Canceled) or  (i_status == EUploadStatus::Canceled);
		}

		if ( failed )
			_uploadStatus.store( EUploadStatus::Canceled );
		else
		if ( complete )
		{
			DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

			if ( not _Build( as_ctx ))
				_uploadStatus.store( EUploadStatus::Canceled );

			Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );
			_uploadStatus.store( EUploadStatus::Complete );
		}

		return _uploadStatus.load();
	}
	
/*
=================================================
	_Build
=================================================
*/
	bool  RTGeometry::_Build (DirectCtx::ASBuild &ctx)
	{
		const uint			fid		= (ulong(ctx.GetFrameId().Unique()) + 1) & 1;
		Allocator_t			alloc;
		RTGeometryBuild		tris_geom;

		CHECK_ERR( _GetTriangles( OUT tris_geom, alloc ));

		tris_geom.SetScratchBuffer( _scratchBuffer );
		
		ctx.Build( tris_geom, _geomIds[fid] );

		// update version
		for (auto& item : _triangleMeshes)
		{
			item.vbufferVer = item.vbuffer->GetVersion();

			if ( item.ibuffer )
				item.ibufferVer = item.ibuffer->GetVersion();
		}
		_version.fetch_add(1);

		return true;
	}
	
/*
=================================================
	BuildIndirect
=================================================
*/
	bool  RTGeometry::BuildIndirect (DirectCtx::ASBuild &) __Th___
	{
		// TODO
		return false;
	}
	
/*
=================================================
	_GetTriangles
=================================================
*/
	bool  RTGeometry::_GetTriangles (OUT RTGeometryBuild &buildInfo, Allocator_t &alloc) const
	{
		auto*	triangle_info_arr	= alloc.Allocate<RTGeometryBuild::TrianglesInfo>( _triangleMeshes.size() );
		auto*	triangle_data_arr	= alloc.Allocate<RTGeometryBuild::TrianglesData>( _triangleMeshes.size() );
		CHECK_ERR( triangle_info_arr != null and triangle_data_arr != null );

		for (usize i = 0; i < _triangleMeshes.size(); ++i)
		{
			auto&	tri_mesh		= _triangleMeshes[i];
			auto&	info			= triangle_info_arr[i];
			auto&	data			= triangle_data_arr[i];
		
			info					= tri_mesh;
			data.vertexData			= tri_mesh.vbuffer->GetBufferId();
			data.vertexDataOffset	= tri_mesh.vertexDataOffset;
			data.indexData			= tri_mesh.ibuffer ? tri_mesh.ibuffer->GetBufferId() : Default;
			data.indexDataOffset	= tri_mesh.indexDataOffset;
			data.vertexStride		= tri_mesh.vertexStride;
			data.transformData		= Default;
			data.transformDataOffset= 0_b;

			ASSERT( info.maxPrimitives > 0 );
			ASSERT( info.maxVertex > 0 );
		}

		buildInfo = RTGeometryBuild{
						ArrayView<RTGeometryBuild::TrianglesInfo>{ triangle_info_arr, _triangleMeshes.size() },
						ArrayView<RTGeometryBuild::TrianglesData>{ triangle_data_arr, _triangleMeshes.size() },
						Default, Default,
						_options };
		return true;
	}
			
/*
=================================================
	Validate
=================================================
*/
	void  RTGeometry::Validate () const
	{
		for (auto& item : _triangleMeshes)
		{
			CHECK( item.vbuffer->GetVersion() == item.vbufferVer );

			if ( item.ibuffer )
				CHECK( item.ibuffer->GetVersion() == item.ibufferVer );
		}
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	RTScene::RTScene (Instances_t	instances,
					  Renderer &	renderer,
					  StringView	dbgName) __Th___ :
		IResource{ renderer },
		_instances{ RVRef(instances) },
		_uploadStatus{ EUploadStatus::InProgress },
		_dbgName{ dbgName }
	{
		_ResQueue().EnqueueForUpload( GetRC() );

		bool	is_mutable = false;

		_uniqueGeometries.reserve( _instances.size() );
		for (auto& inst : _instances)
		{
			is_mutable |= inst.geometry->IsMutable();
			_uniqueGeometries.emplace( inst.geometry, 0 );
		}
		
		auto&			res_mngr		= FrameGraph().GetStateTracker();
		const auto		sizes			= res_mngr.GetRTSceneSizes( RTSceneBuild{ uint(_instances.size()), _options });

		const auto	CreateInstanceBuf	= [&res_mngr, this] ()
		{{
			return res_mngr.CreateBuffer( BufferDesc{ SizeOf<RTSceneBuild::Instance> * _instances.size(),
													  EBufferUsage::ASBuild_ReadOnly | EBufferUsage::TransferDst },
										  _dbgName + "-Instances", _GfxAllocator() );
		}};
		const auto	CreateRTScene		= [&res_mngr, &sizes, this] ()
		{{
			return res_mngr.CreateRTScene( RTSceneDesc{ sizes.rtasSize, _options }, _dbgName, _GfxAllocator() );
		}};

		_sceneIds[0]		= CreateRTScene();
		_instanceBuffers[0] = CreateInstanceBuf();
		_scratchBuffer		= res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
													 _dbgName + "-Sratch", _GfxAllocator() );

		if ( is_mutable ) {
			_instanceBuffers[1] = CreateInstanceBuf();
			_sceneIds[1]		= CreateRTScene();
		}else{
			_sceneIds[1]		= res_mngr.AcquireResource( _sceneIds[0].Get() );
			_instanceBuffers[1] = res_mngr.AcquireResource( _instanceBuffers[0].Get() );
		}
		
		CHECK_THROW( _scratchBuffer );
		CHECK_THROW( _sceneIds[0] and _sceneIds[1] );
		CHECK_THROW( _instanceBuffers[0] and _instanceBuffers[1] );
	}

/*
=================================================
	destructor
=================================================
*/
	RTScene::~RTScene ()
	{
		auto&	res_mngr = FrameGraph().GetStateTracker();
		res_mngr.ReleaseResource( _scratchBuffer );
		res_mngr.ReleaseResourceArray( _sceneIds );
		res_mngr.ReleaseResourceArray( _instanceBuffers );
	}
	
/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  RTScene::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;
		
		EXLOCK( _loadOpGuard );
		
		bool	complete	= true;
		bool	failed		= false;

		for (auto& [geom, ver] : _uniqueGeometries)
		{
			EUploadStatus	status	= geom->GetStatus();

			complete &= (status == EUploadStatus::Complete);
			failed   |= (status == EUploadStatus::Canceled);
		}

		if ( failed )
			_uploadStatus.store( EUploadStatus::Canceled );
		else
		if ( complete )
		{
			if ( not _UploadInstances( ctx ))
				return EUploadStatus::NoMemory;

			DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

			if ( not _Build( as_ctx ))
				_uploadStatus.store( EUploadStatus::Canceled );

			Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );
			_uploadStatus.store( EUploadStatus::Complete );
		}

		return _uploadStatus.load();
	}
	
/*
=================================================
	_Build
=================================================
*/
	bool  RTScene::_UploadInstances (TransferCtx_t &ctx)
	{
		const Bytes		size	= SizeOf<RTSceneBuild::Instance> * _instances.size();
		const uint		fid		= (ulong(ctx.GetFrameId().Unique()) + 1) & 1;

		BufferMemView	mem_view;
		ctx.UploadBuffer( _instanceBuffers[fid], 0_b, size, OUT mem_view, EStagingHeapType::Dynamic );

		if ( mem_view.DataSize() < size )
			return false;

		Array<RTSceneBuild::Instance>	instances;

		for (const auto& src : _instances)
		{
			auto&	dst = instances.emplace_back();
			dst.transform			= src.transform;
			dst.instanceCustomIndex	= src.instanceCustomIndex;
			dst.mask				= src.mask;
			dst.instanceSBTOffset	= src.instanceSBTOffset;
			dst.flags				= VEnumCast( src.flags );
			dst.rtas				= src.geometry->GetDeviceAddress( ctx.GetFrameId().Inc() );

			ASSERT( dst.rtas != Default );
		}

		CHECK_ERR( mem_view.Copy( instances ) == size );
		return true;
	}
	
/*
=================================================
	_Build
=================================================
*/
	bool  RTScene::_Build (DirectCtx::ASBuild &ctx)
	{
		const uint		fid = (ulong(ctx.GetFrameId().Unique()) + 1) & 1;

		RTSceneBuild	scene_build{ uint(_instances.size()), _options };
		scene_build.SetInstanceData( _instanceBuffers[fid] );
		scene_build.SetScratchBuffer( _scratchBuffer );

		ctx.Build( scene_build, _sceneIds[fid] );

		for (auto& [geom, ver] : _uniqueGeometries) {
			ver = geom->GetVersion();
		}

		return true;
	}
	
/*
=================================================
	BuildIndirect
=================================================
*/
	bool  RTScene::BuildIndirect (DirectCtx::ASBuild &) __Th___
	{
		// TODO
		return false;
	}
	
/*
=================================================
	Validate
=================================================
*/
	void  RTScene::Validate () const
	{
		for (auto& [geom, ver] : _uniqueGeometries)
		{
			CHECK( geom->GetVersion() == ver );
			geom->Validate();
		}
	}

		
} // AE::ResEditor
