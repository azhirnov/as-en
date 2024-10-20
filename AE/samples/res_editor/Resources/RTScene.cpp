// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{
/*
=================================================
	constructor
=================================================
*/
	RTGeometry::RTGeometry (TriangleMeshes_t	triangleMeshes,
							RC<Buffer>			indirectBuffer,
							Renderer &			renderer,
							StringView			dbgName,
							Bool				isMutable) __Th___ :
		IResource{ renderer },
		_indirectBuffer{ RVRef(indirectBuffer) },
		_triangleMeshes{ RVRef(triangleMeshes) },
		_isMutable{ isMutable },
		_dbgName{ dbgName }
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		auto&				res_mngr	= RenderGraph().GetStateTracker();
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
			return res_mngr.CreateRTGeometry( RTGeometryDesc{ sizes.rtasSize, _options }, _dbgName,
											  _Renderer().ChooseAllocator( False{"static"}, sizes.rtasSize ));
		}};

		_scratchBuffer = res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
												_dbgName + "-Scratch",
												_Renderer().ChooseAllocator( Bool{_isMutable}, sizes.buildScratchSize ));
		Unused( _geomId.Attach( CreateGeometry() ));

		CHECK_THROW( _scratchBuffer and _geomId );

		if ( _indirectBuffer and
			 res_mngr.GetFeatureSet().accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
		{
			CHECK_THROW( _indirectBuffer->HasHistory() );

			for (auto& tri_mesh : _triangleMeshes)
			{
				CHECK_THROW( tri_mesh.vbuffer->HasHistory() );
				CHECK_THROW( tri_mesh.ibuffer == null or tri_mesh.ibuffer->HasHistory() );
			}

			// host-visible copy of indirect buffer
			const usize	count = _triangleMeshes.size() * GraphicsConfig::MaxFrames;

			_indirectBufferHostVis = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ASBuildIndirectCommand> * count,
																		EBufferUsage::TransferDst }
																.SetMemory( EMemoryType::HostCached ),
															_dbgName + "-InstHost",
															_Renderer().ChooseAllocator( False{"static"}, SizeOf<ASBuildIndirectCommand> * count ));
			CHECK_THROW( _indirectBufferHostVis );

			ResourceManager::NativeMemObjInfo_t	mem_obj;
			CHECK_THROW( res_mngr.GetMemoryInfo( _indirectBufferHostVis, OUT mem_obj ));

			_indirectBufferMem = Cast<ASBuildIndirectCommand>( mem_obj.mappedPtr );
			CHECK_THROW( _indirectBufferMem != null );
		}
	}

/*
=================================================
	constructor
=================================================
*/
	RTGeometry::RTGeometry (Renderer &	renderer,
							StringView	dbgName) __Th___ :
		IResource{ renderer },
		_dbgName{ dbgName }
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		auto	id = renderer.GetDummyRTGeometry();
		CHECK_THROW( id );

		Unused( _geomId.Attach( RVRef(id) ));
	}

/*
=================================================
	destructor
=================================================
*/
	RTGeometry::~RTGeometry () __NE___
	{
		auto&	res_mngr	= RenderGraph().GetStateTracker();
		auto	geom_id		= _geomId.Release();

		res_mngr.ReleaseResources( _scratchBuffer, _indirectBufferHostVis, geom_id );
	}

/*
=================================================
	Reset
=================================================
*/
	void  RTGeometry::Reset (Strong<RTGeometryID> geomId)
	{
		CHECK( geomId );
		CHECK( _uploadStatus.load() == EUploadStatus::InProgress );

		auto	prev = _geomId.Attach( RVRef(geomId) );

		auto&	res_mngr = RenderGraph().GetStateTracker();
		res_mngr.ReleaseResource( INOUT prev );

		_uploadStatus.store( EUploadStatus::Completed );
	}

/*
=================================================
	CompleteUploading
=================================================
*/
	void  RTGeometry::CompleteUploading ()
	{
		CHECK( _uploadStatus.load() == EUploadStatus::InProgress );

		_uploadStatus.store( EUploadStatus::Completed );
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

		bool	complete	= true;
		bool	failed		= false;

		for (auto& tri_mesh : _triangleMeshes)
		{
			EUploadStatus	v_status	= tri_mesh.vbuffer->GetStatus();
			EUploadStatus	i_status	= (tri_mesh.ibuffer ? tri_mesh.ibuffer->GetStatus() : EUploadStatus::Completed);

			complete &= (v_status == EUploadStatus::Completed) and (i_status == EUploadStatus::Completed);
			failed   |= (v_status == EUploadStatus::Canceled)  or  (i_status == EUploadStatus::Canceled);
		}

		if ( failed )
		{
			_SetUploadStatus( EUploadStatus::Canceled );
		}
		else
		if ( complete )
		{
			DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

			if ( not Build( as_ctx, EBuildMode::Direct ))
				_SetUploadStatus( EUploadStatus::Canceled );

			if ( not _isMutable )
				RenderGraph().GetStateTracker().ReleaseResource( INOUT _scratchBuffer );

			Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}

/*
=================================================
	Build
=================================================
*/
	bool  RTGeometry::Build (DirectCtx::ASBuild &ctx, EBuildMode mode) __Th___
	{
		CHECK_ERR( _scratchBuffer );

		Allocator_t		alloc;

		switch_enum( mode )
		{
			case EBuildMode::Direct :
			{
				RTGeometryBuild		tris_geom;
				CHECK_ERR( _GetTriangles( OUT tris_geom, ctx.GetFrameId(), alloc ));
				tris_geom.SetScratchBuffer( _scratchBuffer );

				ctx.Build( tris_geom, _geomId );
				break;
			}
			case EBuildMode::Indirect :
			{
				CHECK_ERR( _indirectBuffer );

				RTGeometryBuild		tris_geom;
				CHECK_ERR( _GetTriangles( OUT tris_geom, ctx.GetFrameId(), alloc ));
				tris_geom.SetScratchBuffer( _scratchBuffer );

				ctx.BuildIndirect( tris_geom, _geomId, _indirectBuffer->GetBufferId( ctx.GetFrameId() ));
				break;
			}
			case EBuildMode::IndirectEmulated :
				CHECK_ERR( _BuildIndirectEmulated( ctx, _geomId, alloc ));
				break;

			default :
				return false;
		}
		switch_end

		_version.Update( ctx.GetFrameId() );
		return true;
	}

/*
=================================================
	_BuildIndirectEmulated
=================================================
*/
	bool  RTGeometry::_BuildIndirectEmulated (DirectCtx::ASBuild &ctx, RTGeometryID geomId, Allocator_t &alloc) const
	{
		CHECK_ERR( _indirectBufferMem != null );

		const uint		fid					= ctx.GetFrameId().Index();
		const usize		geom_count			= _triangleMeshes.size();
		const auto*		indirect			= _indirectBufferMem + geom_count * fid;

		auto*			triangle_info_arr	= alloc.Allocate<RTGeometryBuild::TrianglesInfo>( _triangleMeshes.size() );
		auto*			triangle_data_arr	= alloc.Allocate<RTGeometryBuild::TrianglesData>( _triangleMeshes.size() );
		CHECK_ERR( triangle_info_arr != null and triangle_data_arr != null );

		for (usize i = 0; i < _triangleMeshes.size(); ++i)
		{
			auto&	tri_mesh		= _triangleMeshes[i];
			auto&	info			= triangle_info_arr[i];
			auto&	data			= triangle_data_arr[i];

			ASSERT( indirect->primitiveCount <= info.maxPrimitives );
			// not supported yet
			ASSERT( indirect->primitiveOffset == 0 );
			ASSERT( indirect->firstVertex == 0 );
			ASSERT( indirect->transformOffset == 0 );

			info					= tri_mesh;
			info.maxPrimitives		= Min( info.maxPrimitives, indirect->primitiveCount );

			data.vertexData			= tri_mesh.vbuffer->GetBufferId( fid );
			data.vertexDataOffset	= tri_mesh.vertexDataOffset;
			data.indexData			= tri_mesh.ibuffer ? tri_mesh.ibuffer->GetBufferId( fid ) : Default;
			data.indexDataOffset	= tri_mesh.indexDataOffset;
			data.vertexStride		= tri_mesh.vertexStride;
			data.transformData		= Default;
			data.transformDataOffset= 0_b;

			ASSERT( info.maxVertex > 0 );

			++indirect;
		}

		RTGeometryBuild		tris_geom{
								ArrayView<RTGeometryBuild::TrianglesInfo>{ triangle_info_arr, _triangleMeshes.size() },
								ArrayView<RTGeometryBuild::TrianglesData>{ triangle_data_arr, _triangleMeshes.size() },
								Default, Default,
								_options };
		tris_geom.SetScratchBuffer( _scratchBuffer );

		ctx.Build( tris_geom, geomId );


		// copy indirect commands
		DirectCtx::Transfer		tctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

		BufferCopy	copy;
		copy.srcOffset	= 0_b;
		copy.dstOffset	= SizeOf<ASBuildIndirectCommand> * geom_count * fid;
		copy.size		= SizeOf<ASBuildIndirectCommand> * geom_count;

		tctx.CopyBuffer( _indirectBuffer->GetBufferId( fid ), _indirectBufferHostVis, {copy} );

		Reconstruct( INOUT ctx, tctx.GetRenderTask(), tctx.ReleaseCommandBuffer() );
		return true;
	}

/*
=================================================
	_GetTriangles
=================================================
*/
	bool  RTGeometry::_GetTriangles (OUT RTGeometryBuild &buildInfo, FrameUID fid, Allocator_t &alloc) const
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
			data.vertexData			= tri_mesh.vbuffer->GetBufferId( fid );
			data.vertexDataOffset	= tri_mesh.vertexDataOffset;
			data.indexData			= tri_mesh.ibuffer ? tri_mesh.ibuffer->GetBufferId( fid ) : Default;
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
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RTScene::RTScene (Instances_t	instances,
					  RC<Buffer>	instanceBuffer,
					  RC<Buffer>	indirectBuffer,
					  Renderer &	renderer,
					  StringView	dbgName,
					  Bool			allowUpdate) __Th___ :
		IResource{ renderer },
		_instanceBuffer{ RVRef(instanceBuffer) },
		_indirectBuffer{ RVRef(indirectBuffer) },
		_instances{ RVRef(instances) },
		_isMutable{ allowUpdate or	[this]() {
										bool  is_mutable = false;
										for (auto& inst : _instances)  is_mutable |= inst.geometry->IsMutable();
										return is_mutable;
									}() },
		_dbgName{ dbgName }
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		_uniqueGeometries.reserve( _instances.size() );
		for (auto& inst : _instances) {
			_uniqueGeometries.emplace( inst.geometry, 0 );
		}

		auto&		res_mngr		= RenderGraph().GetStateTracker();
		const auto	sizes			= res_mngr.GetRTSceneSizes( RTSceneBuild{ uint(_instances.size()), _options });
		const auto	CreateRTScene	= [&res_mngr, &sizes, this] ()
		{{
			return res_mngr.CreateRTScene( RTSceneDesc{ sizes.rtasSize, _options }, _dbgName,
										   _Renderer().ChooseAllocator( False{"static"}, sizes.rtasSize ));
		}};

		_sceneId		= CreateRTScene();
		_scratchBuffer	= res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
												 _dbgName + "-Scratch",
												 _Renderer().ChooseAllocator( Bool{_isMutable}, sizes.buildScratchSize ));

		CHECK_THROW( _scratchBuffer and _sceneId );

		if ( _indirectBuffer and
			 res_mngr.GetFeatureSet().accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
		{
			CHECK_THROW( _indirectBuffer->HasHistory() );
			CHECK_THROW( _instanceBuffer->HasHistory() );

			_indirectBufferHostVis = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ASBuildIndirectCommand> * GraphicsConfig::MaxFrames,
																		EBufferUsage::TransferDst }
																.SetMemory( EMemoryType::HostCached ),
															_dbgName + "-InstHost", _Renderer().GetStaticAllocator() );
			CHECK_THROW( _indirectBufferHostVis );

			ResourceManager::NativeMemObjInfo_t	mem_obj;
			CHECK_THROW( res_mngr.GetMemoryInfo( _indirectBufferHostVis, OUT mem_obj ));

			_indirectBufferMem = Cast<ASBuildIndirectCommand>( mem_obj.mappedPtr );
			CHECK_THROW( _indirectBufferMem != null );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	RTScene::~RTScene () __NE___
	{
		auto&	res_mngr = RenderGraph().GetStateTracker();
		res_mngr.ReleaseResources( _scratchBuffer, _indirectBufferHostVis, _sceneId );
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

		bool	complete	= true;
		bool	failed		= false;

		for (auto& [geom, ver] : _uniqueGeometries)
		{
			EUploadStatus	status	= geom->GetStatus();

			complete &= (status == EUploadStatus::Completed);
			failed   |= (status == EUploadStatus::Canceled);
		}

		if ( failed )
			_SetUploadStatus( EUploadStatus::Canceled );
		else
		if ( complete )
		{
			if ( not _UploadInstances( ctx ))
				return EUploadStatus::NoMemory;

			DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

			if ( not Build( as_ctx, EBuildMode::Direct ))
				_SetUploadStatus( EUploadStatus::Canceled );

			if ( not _isMutable )
				RenderGraph().GetStateTracker().ReleaseResource( INOUT _scratchBuffer );

			Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );
			_SetUploadStatus( EUploadStatus::Completed );
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
		const Bytes		size	= RTSceneBuild::InstanceSize * _instances.size();

		BufferMemView	mem_view;
		ctx.UploadBuffer( _instanceBuffer->GetBufferId(0), UploadBufferDesc{ 0_b, size }.DynamicHeap().MaxBlockSize(), OUT mem_view );

		if ( mem_view.DataSize() < size )
			return false;

		// copy instance data
		{
			RTSceneBuild					build;
			Array<RTSceneBuild::Instance>	instances;

			switch_enum( GraphicsScheduler().GetDevice().GetGraphicsAPI() )
			{
				case EGraphicsAPI::Vulkan :
				{
					for (const auto& src : _instances)
					{
						auto&	dst = instances.emplace_back().vk;
						dst.SetTransform( src.transform );
						dst.SetInstanceCustomIndex( src.instanceCustomIndex );
						dst.SetInstanceOffset( src.instanceSBTOffset );
						dst.SetMask( src.mask );
						dst.SetFlags( src.flags );
						CHECK_ERR( build.SetGeometry( src.geometry->GetGeometryId( ctx.GetFrameId() ), INOUT dst ));
					}
					break;
				}
				case EGraphicsAPI::Metal :
				{
					for (const auto& src : _instances)
					{
						auto&	dst = instances.emplace_back().mtl;
						dst.SetTransform( src.transform );
						//dst.SetInstanceCustomIndex( src.instanceCustomIndex );
						dst.SetInstanceOffset( src.instanceSBTOffset );
						dst.SetMask( src.mask );
						dst.SetFlags( src.flags );
						CHECK_ERR( build.SetGeometry( src.geometry->GetGeometryId( ctx.GetFrameId() ), INOUT dst ));
					}
					break;
				}
			}
			switch_end

			CHECK_ERR( mem_view.CopyFrom( instances ) == size );
		}

		if ( _instanceBuffer->HasHistory() )
		{
			for (uint i = 1; i < ctx.GetFrameId().MaxFrames(); ++i)
			{
				BufferCopy	copy;
				copy.srcOffset	= 0_b;
				copy.dstOffset	= 0_b;
				copy.size		= size;

				ctx.CopyBuffer( _instanceBuffer->GetBufferId(0), _instanceBuffer->GetBufferId(i), {copy} );
			}
		}
		return true;
	}

/*
=================================================
	Build
=================================================
*/
	bool  RTScene::Build (DirectCtx::ASBuild &ctx, EBuildMode mode) __Th___
	{
		CHECK_ERR( _scratchBuffer );

		const uint		fid = ctx.GetFrameId().Index();

		RTSceneBuild	scene_build{ uint(_instances.size()), _options };
		scene_build.SetInstanceData( _instanceBuffer->GetBufferId( fid ));
		scene_build.SetScratchBuffer( _scratchBuffer );

		switch_enum( mode )
		{
			case EBuildMode::Direct :
				ctx.Build( scene_build, _sceneId );
				break;

			case EBuildMode::Indirect :
				CHECK_ERR( _indirectBuffer );
				ctx.BuildIndirect( scene_build, _sceneId, _indirectBuffer->GetBufferId( fid ));
				break;

			case EBuildMode::IndirectEmulated :
				CHECK_ERR( _BuildIndirectEmulated( ctx, scene_build, _sceneId ));
				break;

			default :
				return false;
		}
		switch_end

		for (auto& [geom, ver] : _uniqueGeometries) {
			ver = geom->GetVersion( fid );
		}

		return true;
	}

/*
=================================================
	_BuildIndirectEmulated
=================================================
*/
	bool  RTScene::_BuildIndirectEmulated (DirectCtx::ASBuild &ctx, RTSceneBuild &build, RTSceneID sceneId) const
	{
		CHECK_ERR( _indirectBufferMem != null );

		const uint	fid			= ctx.GetFrameId().Index();
		const uint	inst_count	= _indirectBufferMem[ fid ].primitiveCount;

		build.maxInstanceCount = Min( build.maxInstanceCount, inst_count );

		ctx.Build( build, sceneId );


		// copy indirect command
		DirectCtx::Transfer		tctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

		BufferCopy	copy;
		copy.srcOffset	= 0_b;
		copy.dstOffset	= SizeOf<ASBuildIndirectCommand> * fid;
		copy.size		= SizeOf<ASBuildIndirectCommand>;

		tctx.CopyBuffer( _indirectBuffer->GetBufferId( fid ), _indirectBufferHostVis, {copy} );

		Reconstruct( INOUT ctx, tctx.GetRenderTask(), tctx.ReleaseCommandBuffer() );
		return true;
	}

/*
=================================================
	Validate
=================================================
*/
	void  RTScene::Validate (FrameUID fid) const
	{
	/*	for (auto& [geom, ver] : _uniqueGeometries)
		{
			CHECK_Eq( geom->GetVersion(fid), ver );
		}*/
	}


} // AE::ResEditor
