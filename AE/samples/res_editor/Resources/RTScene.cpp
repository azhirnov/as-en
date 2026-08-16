// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Resources/Buffer.h"
#include "Resources/Image.h"
#include "Resources/VideoImage.h"
#include "Resources/RTScene.h"
#include "Core/RenderGraph.h"
#include "Passes/Renderer.h"

namespace AE::ResEditor
{
/*
=================================================
	constructor
=================================================
*/
	RTGeometry::RTGeometry (TriangleMeshes_t	triangleMeshes,
							Micromaps_t			micromaps,
							RC<Buffer>			indirectBuffer,
							Renderer &			renderer,
							StringView			dbgName,
							Bool				isMutable) __NE___ :
		IResource{ renderer },
		_indirectBuffer{ RVRef(indirectBuffer) },
		_triangleMeshes{ RVRef(triangleMeshes) },
		_micromaps{ RVRef(micromaps) },
		_isMutable{ isMutable },
		_dbgName{ dbgName }
	{}

	RTGeometry::RTGeometry (Renderer &	renderer,
							StringView	dbgName) __NE___ :
		IResource{ renderer },
		_dbgName{ dbgName }
	{}

/*
=================================================
	Create
=================================================
*/
	RC<RTGeometry>  RTGeometry::Create (TriangleMeshes_t	triangleMeshes,
										Micromaps_t			micromaps,
										RC<Buffer>			indirectBuffer,
										Renderer &			renderer,
										StringView			dbgName,
										Bool				allowUpdate) __Th___
	{
		RC<RTGeometry>	res { new RTGeometry{ RVRef(triangleMeshes), RVRef(micromaps), RVRef(indirectBuffer), renderer, dbgName, allowUpdate }};
		res->_Init1();  // throw
		return res;
	}

	RC<RTGeometry>  RTGeometry::Create (Renderer &	renderer,
										StringView	dbgName) __Th___
	{
		RC<RTGeometry>	res { new RTGeometry{ renderer, dbgName }};
		res->_Init2();  // throw
		return res;
	}

/*
=================================================
	_Init1
=================================================
*/
	void  RTGeometry::_Init1 () __Th___
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

		Array<RTGeometryBuild::MicromapInfo>	micromaps;
		if ( not _micromaps.empty() )
		{
			for (auto& src : _micromaps)
			{
				auto&	dst = micromaps.emplace_back( src );
				dst.micromapId = src.micromap->GetMicromapID(0);
			}
			build.SetMicromaps( micromaps );
		}

		build.options	= _options;
		build.triangles = RTGeometryBuild::Triangles{ tri_infos, Default };

		const auto	sizes			= res_mngr.GetResourceManager().GetRTGeometrySizes( build );
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
	_Init2
=================================================
*/
	void  RTGeometry::_Init2 () __Th___
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		auto	id = _Renderer().GetDummyRTGeometry();
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

		for (auto& mm : _micromaps)
		{
			EUploadStatus	mm_status	= mm.micromap->GetStatus();

			complete &= (mm_status == EUploadStatus::Completed);
			failed   |= (mm_status == EUploadStatus::Canceled);
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
				CHECK_ERR( _GetTriangles( INOUT tris_geom, ctx.GetFrameId(), alloc ));
				tris_geom.SetScratchBuffer( _scratchBuffer );
				tris_geom.options = _options;

				ctx.Build( tris_geom, _geomId );
				break;
			}

			case EBuildMode::Indirect :
			{
			  #if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
				CHECK_ERR( _indirectBuffer );

				RTGeometryBuild		tris_geom;
				CHECK_ERR( _GetTriangles( INOUT tris_geom, ctx.GetFrameId(), alloc ));
				tris_geom.SetScratchBuffer( _scratchBuffer );
				tris_geom.options = _options;

				ctx.BuildIndirect( tris_geom, _geomId, _indirectBuffer->GetBufferId( ctx.GetFrameId() ));
			  #else
				CHECK_MSG( false, "BuildIndirect is not supported" );
			  #endif
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

		// TODO: micromap
		ASSERT( _micromaps.empty() );

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
	bool  RTGeometry::_GetTriangles (INOUT RTGeometryBuild &buildInfo, FrameUID fid, Allocator_t &alloc) C_NE___
	{
		auto*	triangle_info_arr	= alloc.Allocate<RTGeometryBuild::TrianglesInfo>( _triangleMeshes.size() );
		auto*	triangle_data_arr	= alloc.Allocate<RTGeometryBuild::TrianglesData>( _triangleMeshes.size() );
		CHECK_ERR( triangle_info_arr != null and triangle_data_arr != null );

		for (usize i : IndicesOnly( _triangleMeshes ))
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
			ASSERT( info.micromapIndex == UMax or info.micromapIndex < _micromaps.size() );
		}

		ASSERT( buildInfo.triangles.empty() );
		buildInfo.SetTriangles({ triangle_info_arr, _triangleMeshes.size() }, { triangle_data_arr, _triangleMeshes.size() });

		if ( not _micromaps.empty() )
		{
			auto*	micromap_arr = alloc.Allocate<RTGeometryBuild::MicromapInfo>( _micromaps.size() );
			CHECK_ERR( micromap_arr != null );

			for (usize i : IndicesOnly( _micromaps ))
			{
				auto&	src = _micromaps[i];
				auto&	dst = micromap_arr[i];

				dst = src;
				dst.micromapId = src.micromap->GetMicromapID( fid );
			}
			buildInfo.SetMicromaps({ micromap_arr, _micromaps.size() });
		}
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
					  Bool			allowUpdate) __NE___ :
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
	{}

/*
=================================================
	_Init
=================================================
*/
	void  RTScene::_Init () __Th___
	{
		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		_uniqueGeometries.reserve( _instances.size() );
		for (auto& inst : _instances) {
			_uniqueGeometries.emplace( inst.geometry, 0 );
		}

		auto&		res_mngr	= RenderGraph().GetStateTracker();
		const auto	sizes		= res_mngr.GetResourceManager().GetRTSceneSizes( RTSceneBuild{ uint(_instances.size()), _options });

		_sceneId		= res_mngr.CreateRTScene( RTSceneDesc{ sizes.rtasSize, _options }, _dbgName,
												  _Renderer().ChooseAllocator( False{"static"}, sizes.rtasSize ));
		_scratchBuffer	= res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::ASBuild_Scratch },
												 _dbgName + "-Scratch",
												 _Renderer().ChooseAllocator( Bool{_isMutable}, sizes.buildScratchSize ));
		CHECK_THROW( _scratchBuffer and _sceneId );

		if ( _indirectBuffer )
		{
			if ( res_mngr.GetFeatureSet().accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
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
			else
			{
				CHECK_THROW( _indirectBuffer->GetDeviceAddress(0) != 0 );
			}
		}
	}

/*
=================================================
	Create
=================================================
*/
	RC<RTScene>  RTScene::Create (Instances_t	instances,
								  RC<Buffer>	instanceBuffer,
								  RC<Buffer>	indirectBuffer,
								  Renderer &	renderer,
								  StringView	dbgName,
								  Bool			allowUpdate) __Th___
	{
		RC<RTScene>		res { new RTScene{ RVRef(instances), RVRef(instanceBuffer), RVRef(indirectBuffer), renderer, dbgName, allowUpdate }};
		res->_Init();  // throw
		return res;
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
			  #if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
				CHECK_ERR( _indirectBuffer );
				ctx.BuildIndirect( scene_build, _sceneId, _indirectBuffer->GetBufferId( fid ));
			  #else
				CHECK_MSG( false, "BuildIndirect is not supported" );
			  #endif
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
//-----------------------------------------------------------------------------




/*
=================================================
	constructor
=================================================
*/
	RTMicromap::RTMicromap (BuildData	info,
							Renderer &	renderer,
							StringView	dbgName,
							Bool		allowUpdate) __NE___ :
		IResource{ renderer },
		_info{ RVRef(info) },
		_isMutable{ allowUpdate },
		_dbgName{ dbgName }
	{}

/*
=================================================
	_Init
=================================================
*/
	void  RTMicromap::_Init () __Th___
	{
		CHECK_THROW( _info.data );
		CHECK_THROW( _info.triangleArray );

		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		RTMicromapInfo		mm_info;
		mm_info.type		= _info.type;
		mm_info.buildFlags	= _info.buildFlags;
		mm_info.usage		= _info.usage;

		auto&		res_mngr	= RenderGraph().GetStateTracker();
		const auto	sizes		= res_mngr.GetResourceManager().GetRTMicromapSizes( mm_info );

		_micromapId		= res_mngr.CreateRTMicromap( RTMicromapDesc{ sizes.micromapSize, _info.type }, _dbgName,
													 _Renderer().ChooseAllocator( False{"static"}, sizes.micromapSize ));
		_scratchBuffer	= res_mngr.CreateBuffer( BufferDesc{ sizes.buildScratchSize, EBufferUsage::MMBuild_Scratch },
												 _dbgName + "-Scratch",
												 _Renderer().ChooseAllocator( Bool{_isMutable}, sizes.buildScratchSize ));
		CHECK_THROW( _scratchBuffer and _micromapId );
	}

/*
=================================================
	Create
=================================================
*/
	RC<RTMicromap>  RTMicromap::Create (BuildData	info,
										Renderer &	renderer,
										StringView	dbgName,
										Bool		allowUpdate) __Th___
	{
		RC<RTMicromap>	res { new RTMicromap{ RVRef(info), renderer, dbgName, allowUpdate }};
		res->_Init();  // throw
		return res;
	}

/*
=================================================
	destructor
=================================================
*/
	RTMicromap::~RTMicromap () __NE___
	{
		auto&	res_mngr = RenderGraph().GetStateTracker();
		res_mngr.ReleaseResources( _scratchBuffer, _micromapId );
	}

/*
=================================================
	Build
=================================================
*/
	bool  RTMicromap::Build (DirectCtx::ASBuild &ctx) __Th___
	{
		CHECK_ERR( _scratchBuffer );

		const uint		fid = ctx.GetFrameId().Index();
		RTMicromapBuild	cmd;

		cmd.type					= _info.type;
		cmd.buildFlags				= _info.buildFlags;
		cmd.usage					= _info.usage;
		cmd.data.id					= _info.data->GetBufferId( fid );
		cmd.data.offset				= _info.dataOffset;
		cmd.triangleArray.id		= _info.triangleArray->GetBufferId( fid );
		cmd.triangleArray.offset	= _info.triangleArrayOffset;
		cmd.triangleArray.stride	= SizeOf< RTMicromapBuild::Triangle >;
		cmd.scratch.id				= _scratchBuffer;

		ctx.Build( cmd, _micromapId );
		return true;
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  RTMicromap::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		bool	complete	= true;
		bool	failed		= false;

		{
			EUploadStatus	status	= _info.data->GetStatus();

			complete &= (status == EUploadStatus::Completed);
			failed   |= (status == EUploadStatus::Canceled);
		}{
			EUploadStatus	status	= _info.triangleArray->GetStatus();

			complete &= (status == EUploadStatus::Completed);
			failed   |= (status == EUploadStatus::Canceled);
		}

		if ( failed )
			_SetUploadStatus( EUploadStatus::Canceled );
		else
		if ( complete and _isMutable )
		{
			// will build from BuildRTMicromap pass
			_SetUploadStatus( EUploadStatus::Completed );
		}
		else
		if ( complete )
		{
			DirectCtx::ASBuild	as_ctx{ ctx.GetRenderTask(), ctx.ReleaseCommandBuffer() };

			if ( not Build( as_ctx ))
				_SetUploadStatus( EUploadStatus::Canceled );

			if ( not _isMutable )
				RenderGraph().GetStateTracker().ReleaseResource( INOUT _scratchBuffer );

			Reconstruct( INOUT ctx, as_ctx.GetRenderTask(), as_ctx.ReleaseCommandBuffer() );
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}


} // AE::ResEditor
