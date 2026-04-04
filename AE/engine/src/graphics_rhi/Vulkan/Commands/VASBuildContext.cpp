// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VASBuildContext.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
	_Build
=================================================
*/
	void  _VDirectASBuildCtx::_Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges) __Th___
	{
		GFX_DBG_ONLY(
		switch ( info.mode )
		{
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
				CHECK( info.dstAccelerationStructure != Default );
				break;
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
				CHECK( info.srcAccelerationStructure != Default );
				CHECK( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
			default_unlikely :
				DBG_WARNING( "unknown build mode" );
				break;
		})

		vkCmdBuildAccelerationStructuresKHR( _cmdbuf.Get(), 1, &info, &ranges );
	}

/*
=================================================
	_Build (RTGeometryID)
=================================================
*/
	void  _VDirectASBuildCtx::_Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
	{
		auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

		VALIDATE_GCTX( Build( geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureBuildRangeInfoKHR*		ranges		= null;
		IAllocatorAdaptor< VTempLinearAllocator >		allocator;

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

		_Build( build_info, ranges );
	}

/*
=================================================
	_Update (RTGeometryID)
=================================================
*/
	void  _VDirectASBuildCtx::_Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
	{
		auto  [src_geom, dst_geom, scratch_buf] = _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id );

		VALIDATE_GCTX( Update( src_geom.Description(), dst_geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureBuildRangeInfoKHR*		ranges		= null;
		IAllocatorAdaptor< VTempLinearAllocator >		allocator;

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure		= src_geom.Handle();
		build_info.dstAccelerationStructure		= dst_geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

		_Build( build_info, ranges );
	}

/*
=================================================
	_BuildIndirect (RTGeometryID)
=================================================
*/
	void  _VDirectASBuildCtx::_BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, VkDeviceAddress indirectMem, Bytes indirectStride) __Th___
	{
		auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

		VALIDATE_GCTX( BuildIndirect( geom.Description(),
									  scratch_buf.Description(), cmd.scratch.offset,
									  indirectMem, indirectStride ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		IAllocatorAdaptor< VTempLinearAllocator >		allocator;
		uint*											max_prim_count	= null;			// [info_count][build_info.geometryCount]
		const uint										stride			= uint(indirectStride);
		const uint										info_count		= 1;

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT max_prim_count, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
		GCTX_CHECK( max_prim_count != null );

		vkCmdBuildAccelerationStructuresIndirectKHR( _cmdbuf.Get(), info_count, &build_info, &indirectMem, &stride, &max_prim_count );
	}

/*
=================================================
	_Build (RTSceneID)
=================================================
*/
	void  _VDirectASBuildCtx::_Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
	{
		auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( Build( scene.Description(),
							  scratch_buf.Description(), cmd.scratch.offset,
							  inst_buf.Description(), cmd.instanceData.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure			= scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom.geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>(  inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom.geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));

		_Build( build_info, p_ranges );
	}

/*
=================================================
	_Update (RTSceneID)
=================================================
*/
	void  _VDirectASBuildCtx::_Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
	{
		auto  [src_scene, dst_scene, scratch_buf, inst_buf] =
					_GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( Update( src_scene.Description(), dst_scene.Description(),
							   scratch_buf.Description(), cmd.scratch.offset,
							   inst_buf.Description(), cmd.instanceData.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure			= src_scene.Handle();
		build_info.dstAccelerationStructure			= dst_scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom.geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom.geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));

		_Build( build_info, p_ranges );
	}

/*
=================================================
	_BuildIndirect
=================================================
*/
	void  _VDirectASBuildCtx::_BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem) __Th___
	{
		auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( BuildIndirect( scene.Description(),
									  scratch_buf.Description(), cmd.scratch.offset,
									  inst_buf.Description(), cmd.instanceData.offset,
									  indirectMem ));

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		uint const*									max_inst_count	= &cmd.maxInstanceCount;	// [info_count][build_info.geometryCount]
		const uint									stride			= sizeof(VkAccelerationStructureBuildRangeInfoKHR);
		const uint									info_count		= 1;

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure			= scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom.geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom.geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));
		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( build_info.geometryCount == 1 );

		vkCmdBuildAccelerationStructuresIndirectKHR( _cmdbuf.Get(), info_count, &build_info, &indirectMem, &stride, &max_inst_count );
	}

/*
=================================================
	_WriteProperty (VkAccelerationStructureKHR)
=================================================
*/
	void  _VDirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
	{
		constexpr VkDeviceSize	stride = 8;

		_WriteProperty( as, query );

		// If VK_QUERY_RESULT_WAIT_BIT is set, this command defines an execution dependency with any earlier commands that writes one of the identified queries.
		vkCmdCopyQueryPoolResults( _cmdbuf.Get(), query.pool, query.first, 1, dstBuffer, VkDeviceSize(offset), stride, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT );
	}

	void  _VDirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query) __Th___
	{
		GCTX_CHECK( query );
		vkCmdWriteAccelerationStructuresPropertiesKHR( _cmdbuf.Get(), 1, &as, VEnumCast( query.type ), query.pool, query.first );
	}

/*
=================================================
	_WriteProperty (VkMicromapEXT)
=================================================
*/
	void  _VDirectASBuildCtx::_WriteProperty (VkMicromapEXT micromap, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
	{
		constexpr VkDeviceSize	stride = 8;

		_WriteProperty( micromap, query );

		// If VK_QUERY_RESULT_WAIT_BIT is set, this command defines an execution dependency with any earlier commands that writes one of the identified queries.
		vkCmdCopyQueryPoolResults( _cmdbuf.Get(), query.pool, query.first, 1, dstBuffer, VkDeviceSize(offset), stride, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT );
	}

	void  _VDirectASBuildCtx::_WriteProperty (VkMicromapEXT micromap, const VQueryManager::Query &query) __Th___
	{
		GCTX_CHECK( query );
		vkCmdWriteMicromapsPropertiesEXT( _cmdbuf.Get(), 1, &micromap, VEnumCast( query.type ), query.pool, query.first );
	}

/*
=================================================
	_BuildClusterIndirect
=================================================
*/
	void  _VDirectASBuildCtx::_BuildClusterIndirect (const RTClusterBuild &cmd) __Th___
	{
		VkClusterAccelerationStructureCommandsInfoNV	cmd_info;
		VRTCluster::InputStorage_t						storage;

		CHECK_THROW( VRTCluster::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT cmd_info, OUT storage ));

		vkCmdBuildClusterAccelerationStructureIndirectNV( _cmdbuf.Get(), &cmd_info );
	}

/*
=================================================
	_BuildPartitionedIndirect
=================================================
*/
	void  _VDirectASBuildCtx::_BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd) __Th___
	{
		VkBuildPartitionedAccelerationStructureInfoNV	cmd_info;

		CHECK_THROW( VRTPartitionedScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT cmd_info ));

		vkCmdBuildPartitionedAccelerationStructuresNV( _cmdbuf.Get(), &cmd_info );
	}

/*
=================================================
	_Build (RTMicromapID)
=================================================
*/
	void  _VDirectASBuildCtx::_Build (const RTMicromapBuild &cmd, RTMicromapID dst) __Th___
	{
		VkMicromapBuildInfoEXT	build_info;

		auto&  dst_micromap = _GetResourcesOrThrow( dst );
		CHECK_THROW( VRTMicromap::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT build_info ));

		build_info.dstMicromap = dst_micromap.Handle();

		vkCmdBuildMicromapsEXT( _cmdbuf.Get(), 1, &build_info );
	}

/*
=================================================
	_Copy (RTMicromapID)
=================================================
*/
	void  _VDirectASBuildCtx::_Copy (RTMicromapID srcId, RTMicromapID dstId, ERTASCopyMode mode) __Th___
	{
		auto  [src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VkCopyMicromapInfoEXT	info = {};
		info.sType	= VK_STRUCTURE_TYPE_COPY_MICROMAP_INFO_EXT;
		info.src	= src.Handle();
		info.dst	= dst.Handle();
		info.mode	= VEnumCast( mode, VkCopyMicromapModeEXT(0) );

		vkCmdCopyMicromapEXT( _cmdbuf.Get(), &info );
	}

/*
=================================================
	Copy***
=================================================
*/
	void  _VDirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info) __Th___
	{
		vkCmdCopyAccelerationStructureKHR( _cmdbuf.Get(), &info );
	}

	void  _VDirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info) __Th___
	{
		vkCmdCopyAccelerationStructureToMemoryKHR( _cmdbuf.Get(), &info );
	}

	void  _VDirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info) __Th___
	{
		vkCmdCopyMemoryToAccelerationStructureKHR( _cmdbuf.Get(), &info );
	}

	void  _VDirectASBuildCtx::SerializeToMemory (const VkCopyMicromapToMemoryInfoEXT &info) __Th___
	{
		vkCmdCopyMicromapToMemoryEXT( _cmdbuf.Get(), &info );
	}

	void  _VDirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToMicromapInfoEXT &info) __Th___
	{
		vkCmdCopyMemoryToMicromapEXT( _cmdbuf.Get(), &info );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectASBuildCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::ASBuild ));

		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectASBuildCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::ASBuild ));

		return VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Build
=================================================
*/
	void  _VIndirectASBuildCtx::_Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
										VkAccelerationStructureBuildRangeInfoKHR const* const& srcRanges) __Th___
	{
		ASSERT( info.sType == VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR );
		ASSERT( info.pNext == null );
		NonNull( srcRanges );

		GFX_DBG_ONLY(
		switch ( info.mode )
		{
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
				CHECK( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
				CHECK( info.srcAccelerationStructure != Default );
				CHECK( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
			default_unlikely :
				DBG_WARNING( "unknown build mode" );
				break;
		})

		auto&	cmd		= _cmdbuf->CreateCmd< BuildASCmd >();	// throw
		cmd.info		= info;
		cmd.pRangeInfos	= srcRanges;
	}

/*
=================================================
	_Build (RTGeometryID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
	{
		auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

		VALIDATE_GCTX( Build( geom.Description(), scratch_buf.Description(), cmd.scratch.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureBuildRangeInfoKHR*		ranges		= null;
		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator	{_GetFrameId()};

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

		_Build( build_info, ranges );
	}

/*
=================================================
	_Update (RTGeometryID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
	{
		auto  [src_geom, dst_geom, scratch_buf] = _GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id );

		VALIDATE_GCTX( Update( src_geom.Description(), dst_geom.Description(),
							   scratch_buf.Description(), cmd.scratch.offset ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureBuildRangeInfoKHR*		ranges		= null;
		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator	{_GetFrameId()};

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT ranges, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure		= src_geom.Handle();
		build_info.dstAccelerationStructure		= dst_geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));

		_Build( build_info, ranges );
	}

/*
=================================================
	_Build (RTSceneID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
	{
		auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( Build( scene.Description(),
							  scratch_buf.Description(), cmd.scratch.offset,
							  inst_buf.Description(), cmd.instanceData.offset ));

		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator	{_GetFrameId()};
		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureGeometryKHR*				geom		= allocator.Allocate< VkAccelerationStructureGeometryKHR >();
		VkAccelerationStructureBuildRangeInfoKHR*		range		= allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >();

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT *range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure			= scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom->geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom->geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));

		_Build( build_info, range );
	}

/*
=================================================
	_Update (RTSceneID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
	{
		auto  [src_scene, dst_scene, scratch_buf, inst_buf] =
					_GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( Update( src_scene.Description(), dst_scene.Description(),
							   scratch_buf.Description(), cmd.scratch.offset,
							   inst_buf.Description(), cmd.instanceData.offset ));

		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator	{_GetFrameId()};
		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureGeometryKHR*				geom		= allocator.Allocate< VkAccelerationStructureGeometryKHR >();
		VkAccelerationStructureBuildRangeInfoKHR*		range		= allocator.Allocate< VkAccelerationStructureBuildRangeInfoKHR >();

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT *range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure			= src_scene.Handle();
		build_info.dstAccelerationStructure			= dst_scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom->geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom->geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));

		_Build( build_info, range );
	}

/*
=================================================
	_BuildIndirect
=================================================
*/
	void  _VIndirectASBuildCtx::_BuildIndirect (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkDeviceAddress indirectMem,
												const uint* maxPrimCount, uint indirectStride) __Th___
	{
		ASSERT( info.sType == VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR );
		ASSERT( info.pNext == null );
		ASSERT( indirectMem != Default );
		NonNull( maxPrimCount );

		GFX_DBG_ONLY(
		switch ( info.mode )
		{
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
				CHECK( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
				CHECK( info.srcAccelerationStructure != Default );
				CHECK( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
			default_unlikely :
				DBG_WARNING( "unknown build mode" );
				break;
		})

		auto&	cmd			= _cmdbuf->CreateCmd< BuildASIndirectCmd >();	// throw
		cmd.info			= info;
		cmd.indirectMem		= indirectMem;
		cmd.maxPrimCount	= maxPrimCount;
		cmd.indirectStride	= indirectStride;
	}

/*
=================================================
	_BuildIndirect (RTGeometryID)
=================================================
*/
	void  _VIndirectASBuildCtx::_BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, VkDeviceAddress indirectMem, Bytes indirectStride) __Th___
	{
		auto  [geom, scratch_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id );

		VALIDATE_GCTX( BuildIndirect( geom.Description(),
									  scratch_buf.Description(), cmd.scratch.offset,
									  indirectMem, indirectStride ));

		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator		{_GetFrameId()};
		uint*											max_prim_count	= null;			// [1][build_info.geometryCount]

		CHECK_THROW( VRTGeometry::ConvertBuildInfo( this->_mngr.GetResourceManager(), allocator, cmd, OUT max_prim_count, OUT build_info ));

		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= geom.Handle();
		build_info.scratchData.deviceAddress	= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress, _RTProps().scratchBufferAlign ));
		GCTX_CHECK( max_prim_count != null );

		_BuildIndirect( build_info, indirectMem, max_prim_count, uint(indirectStride) );
	}

/*
=================================================
	_BuildIndirect (RTSceneID)
=================================================
*/
	void  _VIndirectASBuildCtx::_BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem) __Th___
	{
		auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

		VALIDATE_GCTX( BuildIndirect( scene.Description(),
									  scratch_buf.Description(), cmd.scratch.offset,
									  inst_buf.Description(), cmd.instanceData.offset,
									  indirectMem ));

		IAllocatorAdaptor< GraphicsFrameAllocatorRef >	allocator		{_GetFrameId()};
		VkAccelerationStructureBuildGeometryInfoKHR		build_info;
		VkAccelerationStructureGeometryKHR*				geom			= allocator.Allocate< VkAccelerationStructureGeometryKHR >();
		VkAccelerationStructureBuildRangeInfoKHR		range;
		uint const*										max_inst_count	= &cmd.maxInstanceCount;	// [1][build_info.geometryCount]

		CHECK_THROW( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT *geom, OUT range, OUT build_info ));

		build_info.mode								= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure			= scene.Handle();
		build_info.scratchData.deviceAddress		= BitCast<VkDeviceAddress>( scratch_buf.GetDeviceAddress() + cmd.scratch.offset );

		geom->geometry.instances.data.deviceAddress	= BitCast<VkDeviceAddress>( inst_buf.GetDeviceAddress() + cmd.instanceData.offset );

		GCTX_CHECK( IsMultipleOf( build_info.scratchData.deviceAddress,			_RTProps().scratchBufferAlign ));
		GCTX_CHECK( IsMultipleOf( geom->geometry.instances.data.deviceAddress,	_RTProps().instanceDataAlign ));

		_BuildIndirect( build_info, indirectMem, max_inst_count, sizeof(VkAccelerationStructureBuildRangeInfoKHR) );
	}

/*
=================================================
	Copy
=================================================
*/
	void  _VIndirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info) __Th___
	{
		ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR );
		ASSERT( info.pNext == null );

		auto&	cmd = _cmdbuf->CreateCmd< CopyASCmd >();	// throw
		cmd.src		= info.src;
		cmd.dst		= info.dst;
		cmd.mode	= info.mode;
	}

/*
=================================================
	_WriteProperty (VkAccelerationStructureKHR)
=================================================
*/
	void  _VIndirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
	{
		constexpr VkDeviceSize	stride = 8;

		_WriteProperty( as, query );

		auto&	cmd		= _cmdbuf->CreateCmd< CopyQueryPoolResultsCmd >();	// throw
		cmd.srcPool		= query.pool;
		cmd.srcIndex	= query.first;
		cmd.srcCount	= 1;
		cmd.dstBuffer	= dstBuffer;
		cmd.dstOffset	= offset;
		cmd.stride		= Bytes{stride};
		cmd.flags		= VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT;
	}

/*
=================================================
	_WriteProperty (VkAccelerationStructureKHR)
=================================================
*/
	void  _VIndirectASBuildCtx::_WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query) __Th___
	{
		GCTX_CHECK( query );

		auto&	cmd = _cmdbuf->CreateCmd< WriteASPropertiesCmd >();	// throw
		cmd.as		= as;
		cmd.pool	= query.pool;
		cmd.index	= query.first;
		cmd.type	= VEnumCast( query.type );
	}

/*
=================================================
	_WriteProperty (VkMicromapEXT)
=================================================
*/
	void  _VIndirectASBuildCtx::_WriteProperty (VkMicromapEXT micromap, VkBuffer dstBuffer, Bytes offset, const VQueryManager::Query &query) __Th___
	{
		constexpr VkDeviceSize	stride = 8;

		_WriteProperty( micromap, query );

		auto&	cmd		= _cmdbuf->CreateCmd< CopyQueryPoolResultsCmd >();	// throw
		cmd.srcPool		= query.pool;
		cmd.srcIndex	= query.first;
		cmd.srcCount	= 1;
		cmd.dstBuffer	= dstBuffer;
		cmd.dstOffset	= offset;
		cmd.stride		= Bytes{stride};
		cmd.flags		= VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT;
	}

/*
=================================================
	_WriteProperty (VkMicromapEXT)
=================================================
*/
	void  _VIndirectASBuildCtx::_WriteProperty (VkMicromapEXT micromap, const VQueryManager::Query &query) __Th___
	{
		GCTX_CHECK( query );

		auto&	cmd		= _cmdbuf->CreateCmd< WriteMMPropertiesCmd >();	// throw
		cmd.micromap	= micromap;
		cmd.pool		= query.pool;
		cmd.index		= query.first;
		cmd.type		= VEnumCast( query.type );
	}

/*
=================================================
	_BuildClusterIndirect
=================================================
*/
	void  _VIndirectASBuildCtx::_BuildClusterIndirect (const RTClusterBuild &cmd) __Th___
	{
		// TODO
		Unused( cmd );
	}

/*
=================================================
	_BuildPartitionedIndirect
=================================================
*/
	void  _VIndirectASBuildCtx::_BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd) __Th___
	{
		// TODO
		Unused( cmd );
	}

/*
=================================================
	SerializeToMemory / DeserializeFromMemory
=================================================
*/
	void  _VIndirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info) __Th___
	{
		ASSERT( info.pNext == null );
		ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR );
		GCTX_CHECK( info.dst.deviceAddress != Default );

		auto&	cmd = _cmdbuf->CreateCmd< CopyASToMemoryCmd >();	// throw

		cmd.src		= info.src;
		cmd.dst		= info.dst.deviceAddress;
		cmd.mode	= info.mode;
	}

	void  _VIndirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info) __Th___
	{
		ASSERT( info.pNext == null );
		ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR );
		GCTX_CHECK( info.src.deviceAddress != Default );

		auto&	cmd = _cmdbuf->CreateCmd< CopyMemoryToASCmd >();	// throw

		cmd.src		= info.src.deviceAddress;
		cmd.dst		= info.dst;
		cmd.mode	= info.mode;
	}

/*
=================================================
	SerializeToMemory / DeserializeFromMemory
=================================================
*/
	void  _VIndirectASBuildCtx::SerializeToMemory (const VkCopyMicromapToMemoryInfoEXT &info) __Th___
	{
		ASSERT( info.pNext == null );
		ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_MICROMAP_TO_MEMORY_INFO_EXT );
		GCTX_CHECK( info.dst.deviceAddress != Default );

		auto&	cmd = _cmdbuf->CreateCmd< CopyMicromapToMemoryCmd >();	// throw

		cmd.src		= info.src;
		cmd.dst		= info.dst.deviceAddress;
		cmd.mode	= info.mode;
	}

	void  _VIndirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToMicromapInfoEXT &info) __Th___
	{
		ASSERT( info.pNext == null );
		ASSERT( info.sType == VK_STRUCTURE_TYPE_COPY_MEMORY_TO_MICROMAP_INFO_EXT );
		GCTX_CHECK( info.src.deviceAddress != Default );

		auto&	cmd = _cmdbuf->CreateCmd< CopyMemoryToMicromapCmd >();	// throw

		cmd.src		= info.src.deviceAddress;
		cmd.dst		= info.dst;
		cmd.mode	= info.mode;
	}

/*
=================================================
	_Build (RTMicromapID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Build (const RTMicromapBuild &buildCmd, RTMicromapID dst) __Th___
	{
		VkMicromapBuildInfoEXT	build_info;

		auto&  dst_micromap = _GetResourcesOrThrow( dst );
		CHECK_THROW( VRTMicromap::ConvertBuildInfo( this->_mngr.GetResourceManager(), buildCmd, OUT build_info ));

		ASSERT( build_info.pNext == null );
		ASSERT( build_info.pUsageCounts != null );

		build_info.dstMicromap = dst_micromap.Handle();

		auto&	cmd		= _cmdbuf->CreateCmd< BuildMicromapCmd, VkMicromapUsageEXT >( build_info.usageCountsCount );	// throw
		auto*	usage	= Cast<VkMicromapUsageEXT>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkMicromapUsageEXT> ));

		cmd.info = build_info;
		MemCopy( OUT usage, build_info.pUsageCounts, SizeOf<VkMicromapUsageEXT> * build_info.usageCountsCount );
	}

/*
=================================================
	_Copy (RTMicromapID)
=================================================
*/
	void  _VIndirectASBuildCtx::_Copy (RTMicromapID srcId, RTMicromapID dstId, ERTASCopyMode mode) __Th___
	{
		auto  [src, dst] = _GetResourcesOrThrow( srcId, dstId );

		VkCopyMicromapInfoEXT	info = {};
		info.sType	= VK_STRUCTURE_TYPE_COPY_MICROMAP_INFO_EXT;
		info.src	= src.Handle();
		info.dst	= dst.Handle();
		info.mode	= VEnumCast( mode, VkCopyMicromapModeEXT(0) );

		auto&	cmd	= _cmdbuf->CreateCmd< CopyMicromapCmd >();	// throw
		cmd.info	= info;
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectASBuildCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::ASBuild ));

		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectASBuildCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::ASBuild ));

		return VBaseIndirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VASBuildContextImpl<C>::_VASBuildContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RawCtx{ task, RVRef(cmdbuf), dbg }
	{
		Validator_t::CtxInit( task.QueueMask() );
	}

/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode) __Th___
	{
		auto  [src_geom, dst_geom] = _GetResourcesOrThrow( src, dst );

		VALIDATE_GCTX( Copy( src_geom.Description(), dst_geom.Description(), mode ));

		VkCopyAccelerationStructureInfoKHR	info;
		info.sType	= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext	= null;
		info.src	= src_geom.Handle();
		info.dst	= dst_geom.Handle();
		info.mode	= VEnumCast( mode );

		RawCtx::Copy( info );
	}

/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode) __Th___
	{
		auto  [src_scene, dst_scene] = _GetResourcesOrThrow( src, dst );

		VALIDATE_GCTX( Copy( src_scene.Description(), dst_scene.Description(), mode ));

		VkCopyAccelerationStructureInfoKHR	info;
		info.sType	= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext	= null;
		info.src	= src_scene.Handle();
		info.dst	= dst_scene.Handle();
		info.mode	= VEnumCast( mode );

		RawCtx::Copy( info );
	}

/*
=================================================
	_ASQueryOrThrow
=================================================
*/
	template <typename C>
	VQueryManager::Query  _VASBuildContextImpl<C>::_ASQueryOrThrow (ERTASProperty property) C_Th___
	{
		EQueryType	q_type = Default;

		switch_enum( property )
		{
			case ERTASProperty::CompactedSize :		q_type = EQueryType::AccelStructCompactedSize;		break;
			case ERTASProperty::SerializationSize :	q_type = EQueryType::AccelStructSerializationSize;	break;
			case ERTASProperty::Size :				q_type = EQueryType::AccelStructSize;				break;
			default_unlikely :						CHECK_THROW( false ); // unknown ERTASProperty
		}
		switch_end

		auto&	qm		= this->_mngr.GetQueryManager();
		auto	query	= qm.AllocQuery( GetFrameId(), this->_mngr.GetQueueType(), q_type );
		CHECK_THROW( query );

		return query;
	}

/*
=================================================
	_MMQueryOrThrow
=================================================
*/
	template <typename C>
	VQueryManager::Query  _VASBuildContextImpl<C>::_MMQueryOrThrow (ERTASProperty property) C_Th___
	{
		EQueryType	q_type = Default;

		switch_enum( property )
		{
			case ERTASProperty::CompactedSize :		q_type = EQueryType::MicromapCompactedSize;		break;
			case ERTASProperty::SerializationSize :	q_type = EQueryType::MicromapSerializationSize;	break;
			case ERTASProperty::Size :
			default_unlikely :						CHECK_THROW( false ); // unknown ERTASProperty
		}
		switch_end

		auto&	qm		= this->_mngr.GetQueryManager();
		auto	query	= qm.AllocQuery( GetFrameId(), this->_mngr.GetQueueType(), q_type );
		CHECK_THROW( query );

		return query;
	}

/*
=================================================
	_WriteProperty
----
	TODO: optimize barriers:
		- write property to query
		- query wait result
		- copy from query to buffer
=================================================
*/
	template <typename C>
	template <typename ASType>
	void  _VASBuildContextImpl<C>::_WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );
		auto  query				= _ASQueryOrThrow( property );

		VALIDATE_GCTX( WriteProperty( property, dst_buf.Description(), offset, size ));
		Unused( size );

		RawCtx::_WriteProperty( src_as.Handle(), dst_buf.Handle(), offset, query );
	}

/*
=================================================
	_ReadProperty
=================================================
*/
	template <typename C>
	template <typename ASType>
	Promise<Bytes>  _VASBuildContextImpl<C>::_ReadProperty (ERTASProperty property, ASType as) __Th___
	{
		VALIDATE_GCTX( ReadProperty( property ));

		auto&	src_as	= _GetResourcesOrThrow( as );
		auto	query	= _ASQueryOrThrow( property );

		RawCtx::_WriteProperty( src_as.Handle(), query );

		return Scheduler().Run(
					ETaskQueue::PerFrame,
					[](auto query) -> Promise<Bytes>
					{
						auto&	query_mngr	= GraphicsScheduler().GetQueryManager();
						Bytes	size;
						CHECK_CE( query_mngr.GetRTASProperty( query, OUT &size, Sizeof(size) ));
						co_return size;
					}( query ),
					Tuple{ this->_mngr.GetBatchRC() },
					"VASBuildContext::ReadProperty"
				);
	}

/*
=================================================
	WriteProperty (RTMicromapID)
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTMicromapID micromap, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		auto  [src_mm, dst_buf] = _GetResourcesOrThrow( micromap, dstBuffer );
		auto  query				= _MMQueryOrThrow( property );

		VALIDATE_GCTX( WriteProperty( property, dst_buf.Description(), offset, size ));
		Unused( size );

		RawCtx::_WriteProperty( src_mm.Handle(), dst_buf.Handle(), offset, query );
	}

/*
=================================================
	ReadProperty (RTMicromapID)
=================================================
*/
	template <typename C>
	Promise<Bytes>  _VASBuildContextImpl<C>::ReadProperty (ERTASProperty property, RTMicromapID micromap) __Th___
	{
		VALIDATE_GCTX( ReadProperty( property ));

		auto&	src_mm	= _GetResourcesOrThrow( micromap );
		auto	query	= _MMQueryOrThrow( property );

		RawCtx::_WriteProperty( src_mm.Handle(), query );

		return Scheduler().Run(
					ETaskQueue::PerFrame,
					[](auto query) -> Promise<Bytes>
					{
						auto&	query_mngr	= GraphicsScheduler().GetQueryManager();
						Bytes	size;
						CHECK_CE( query_mngr.GetRTASProperty( query, OUT &size, Sizeof(size) ));
						co_return size;
					}( query ),
					Tuple{ this->_mngr.GetBatchRC() },
					"VASBuildContext::ReadProperty"
				);
	}

/*
=================================================
	SerializeToMemory
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, DeviceAddress dst) __Th___
	{
		auto&	src_as = _GetResourcesOrThrow( src );
		VALIDATE_GCTX( SerializeToMemory( dst ));

		VkCopyAccelerationStructureToMemoryInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
		info.pNext				= null;
		info.src				= src_as.Handle();
		info.dst.deviceAddress	= VkDeviceAddress(dst);
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;
		return RawCtx::SerializeToMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset) __Th___
	{
		auto&	dst_buf	= _GetResourcesOrThrow( dst );
		VALIDATE_GCTX( SerializeToMemory( dst_buf.Description(), dstOffset ));
		return SerializeToMemory( src, dst_buf.GetDeviceAddress() + dstOffset );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, DeviceAddress dst) __Th___
	{
		auto&	src_as = _GetResourcesOrThrow( src );
		VALIDATE_GCTX( SerializeToMemory( dst ));

		VkCopyAccelerationStructureToMemoryInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
		info.pNext				= null;
		info.src				= src_as.Handle();
		info.dst.deviceAddress	= VkDeviceAddress(dst);
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;
		return RawCtx::SerializeToMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset) __Th___
	{
		auto&	dst_buf	= _GetResourcesOrThrow( dst );
		VALIDATE_GCTX( SerializeToMemory( dst_buf.Description(), dstOffset ));
		return SerializeToMemory( src, dst_buf.GetDeviceAddress() + dstOffset );
	}

/*
=================================================
	DeserializeFromMemory
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTGeometryID dst) __Th___
	{
		auto&	dst_as = _GetResourcesOrThrow( dst );
		VALIDATE_GCTX( DeserializeFromMemory( src ));

		VkCopyMemoryToAccelerationStructureInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext				= null;
		info.src.deviceAddress	= VkDeviceAddress(src);
		info.dst				= dst_as.Handle();
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;
		return RawCtx::DeserializeFromMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst) __Th___
	{
		auto&	src_buf	= _GetResourcesOrThrow( src );
		VALIDATE_GCTX( DeserializeFromMemory( src_buf.Description(), srcOffset ));
		return DeserializeFromMemory( src_buf.GetDeviceAddress() + srcOffset, dst );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTSceneID dst) __Th___
	{
		auto&	dst_as = _GetResourcesOrThrow( dst );
		VALIDATE_GCTX( DeserializeFromMemory( src ));

		VkCopyMemoryToAccelerationStructureInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext				= null;
		info.src.deviceAddress	= VkDeviceAddress(src);
		info.dst				= dst_as.Handle();
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;
		return RawCtx::DeserializeFromMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst) __Th___
	{
		auto&	src_buf	= _GetResourcesOrThrow( src );
		VALIDATE_GCTX( DeserializeFromMemory( src_buf.Description(), srcOffset ));
		return DeserializeFromMemory( src_buf.GetDeviceAddress() + srcOffset, dst );
	}

/*
=================================================
	SerializeToMemory (RTMicromapID)
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTMicromapID src, DeviceAddress dst) __Th___
	{
		auto&	src_mm = _GetResourcesOrThrow( src );

		VkCopyMicromapToMemoryInfoEXT	info = {};
		info.sType				= VK_STRUCTURE_TYPE_COPY_MICROMAP_TO_MEMORY_INFO_EXT;
		info.src				= src_mm.Handle();
		info.dst.deviceAddress	= BitCast<VkDeviceAddress>( dst );
		info.mode				= VK_COPY_MICROMAP_MODE_SERIALIZE_EXT;
		return RawCtx::SerializeToMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTMicromapID src, BufferID dst, Bytes dstOffset) __Th___
	{
		auto&	dst_buf	= _GetResourcesOrThrow( dst );
		VALIDATE_GCTX( SerializeToMemory( dst_buf.Description(), dstOffset ));
		return SerializeToMemory( src, dst_buf.GetDeviceAddress() + dstOffset );
	}

/*
=================================================
	DeserializeFromMemory (RTMicromapID)
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (DeviceAddress src, RTMicromapID dst) __Th___
	{
		auto&	dst_mm = _GetResourcesOrThrow( dst );

		VkCopyMemoryToMicromapInfoEXT	info = {};
		info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_MICROMAP_INFO_EXT;
		info.src.deviceAddress	= BitCast<VkDeviceAddress>( src );
		info.dst				= dst_mm.Handle();
		info.mode				= VK_COPY_MICROMAP_MODE_DESERIALIZE_EXT;
		return RawCtx::DeserializeFromMemory( info );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTMicromapID dst) __Th___
	{
		auto&	src_buf	= _GetResourcesOrThrow( src );
		VALIDATE_GCTX( DeserializeFromMemory( src_buf.Description(), srcOffset ));
		return DeserializeFromMemory( src_buf.GetDeviceAddress() + srcOffset, dst );
	}

/*
=================================================
	BuildIndirect
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer, Bytes indirectStride) __Th___
	{
		return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>(indirectBuffer), indirectStride );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, BufferID indirectBuffer,
												  Bytes indirectBufferOffset, Bytes indirectStride) __Th___
	{
		auto&	ibuf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( BuildIndirect( cmd, ibuf.Description(), indirectBufferOffset, indirectStride ));
		return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>( ibuf.GetDeviceAddress() + indirectBufferOffset ), indirectStride );
	}

/*
=================================================
	BuildIndirect
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer) __Th___
	{
		return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>(indirectBuffer) );
	}

	template <typename C>
	void  _VASBuildContextImpl<C>::BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto&	ibuf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( BuildIndirect( cmd, ibuf.Description(), indirectBufferOffset ));
		return RawCtx::_BuildIndirect( cmd, dst, BitCast<VkDeviceAddress>( ibuf.GetDeviceAddress() + indirectBufferOffset ));
	}

/*
=================================================
	WriteProperty
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___ { return _WriteProperty( property, as, dstBuffer, offset, size ); }

	template <typename C>
	void  _VASBuildContextImpl<C>::WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___ { return _WriteProperty( property, as, dstBuffer, offset, size ); }
//-----------------------------------------------------------------------------

    template class _VASBuildContextImpl< _VDirectASBuildCtx >;
    template class _VASBuildContextImpl< _VIndirectASBuildCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
