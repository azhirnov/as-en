// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ASBuildCtx -->  DirectASBuildCtx   --> BarrierMngr --> Vulkan device 
				\-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct AS Build Context implementation
	//
	
	class _VDirectASBuildCtx : public VBaseDirectContext
	{
	// methods
	public:
		void  Copy (const VkCopyAccelerationStructureInfoKHR &info)							__Th___;
		void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)	__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)__Th___;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()												__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()											__Th___;
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectASBuildCtx (const RenderTask &task);
		_VDirectASBuildCtx (const RenderTask &task, VCommandBuffer cmdbuf);
		
		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
		void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
		void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, Bytes size, const VQueryManager::Query &query);
	};



	//
	// Vulkan Indirect AS Build Context implementation
	//
	
	class _VIndirectASBuildCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  Copy (const VkCopyAccelerationStructureInfoKHR &info)							__Th___;
		void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)	__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)__Th___;
		
		ND_ VBakedCommands		EndCommandBuffer ()											__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()										__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectASBuildCtx (const RenderTask &task);
		_VIndirectASBuildCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf);

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
		void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
		void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, Bytes size, const VQueryManager::Query &query);
	};


	
	//
	// Vulkan AS Build Context implementation
	//

	template <typename CtxImpl>
	class _VASBuildContextImpl : public CtxImpl, public IASBuildContext
	{
	// types
	public:
		static constexpr bool	IsASBuildContext		= true;
		static constexpr bool	IsVulkanASBuildContext	= true;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= VAccumBarriers< _VASBuildContextImpl< CtxImpl >>;
		using DeferredBar	= VAccumDeferredBarriersForCtx< _VASBuildContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VASBuildContextImpl (const RenderTask &task)														__Th___;
		
		template <typename RawCmdBufType>
		_VASBuildContextImpl (const RenderTask &task, RawCmdBufType cmdbuf)											__Th___;

		_VASBuildContextImpl ()																						= delete;
		_VASBuildContextImpl (const _VASBuildContextImpl &)															= delete;
		
		using RawCtx::Copy;
		
		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)													__Th_OV	{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)								__Th_OV	{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)				__Th_OV;

		void  Build  (const RTSceneBuild &cmd, RTSceneID dst)														__Th_OV;
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)										__Th_OV;
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)						__Th_OV;
		
		void  SerializeToMemory (RTGeometryID src, VDeviceAddress dst)												__Th___;
		void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)									__Th___;
		
		void  SerializeToMemory (RTSceneID src, VDeviceAddress dst)													__Th___;
		void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)										__Th___;

		void  DeserializeFromMemory (VDeviceAddress src, RTGeometryID dst)											__Th___;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)								__Th___;
		
		void  DeserializeFromMemory (VDeviceAddress src, RTSceneID dst)												__Th___;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)									__Th___;
		
		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV	{ return _WriteProperty( property, as, dstBuffer, offset, size ); }
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV	{ return _WriteProperty( property, as, dstBuffer, offset, size ); }
		
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)										__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)											__Th_OV	{ return _ReadProperty( property, as ); }

		VBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ASType>
		void  _WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size);
		
		template <typename ASType>
		ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as);

		ND_ VQueryManager::Query  _ASQueryOrThrow (ERTASProperty property) const;
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectASBuildContext		= Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VDirectASBuildCtx >;
	using VIndirectASBuildContext	= Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VIndirectASBuildCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	_Build
=================================================
*/
	inline void  _VDirectASBuildCtx::_Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges)
	{
		DEBUG_ONLY(
		switch ( info.mode )
		{
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR :
				ASSERT( info.dstAccelerationStructure != Default );
				break;
			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR :
				ASSERT( info.srcAccelerationStructure != Default );
				ASSERT( info.dstAccelerationStructure != Default );
				break;

			case VK_BUILD_ACCELERATION_STRUCTURE_MODE_MAX_ENUM_KHR :
			default :
				DBG_WARNING( "unknown build mode" );
				break;
		})

		vkCmdBuildAccelerationStructuresKHR( _cmdbuf.Get(), 1, &info, &ranges );
	}
		
/*
=================================================
	Copy***
=================================================
*/
	inline void  _VDirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info)
	{
		vkCmdCopyAccelerationStructureKHR( _cmdbuf.Get(), &info );
	}
	
	inline void  _VDirectASBuildCtx::SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)
	{
		ASSERT( info.dst.deviceAddress != Default );
		vkCmdCopyAccelerationStructureToMemoryKHR( _cmdbuf.Get(), &info );
	}

	inline void  _VDirectASBuildCtx::DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)
	{
		ASSERT( info.src.deviceAddress != Default );
		vkCmdCopyMemoryToAccelerationStructureKHR( _cmdbuf.Get(), &info );
	}
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VASBuildContextImpl<C>::_VASBuildContextImpl (const RenderTask &task) : RawCtx{ task }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}
		
	template <typename C>
	template <typename RawCmdBufType>
	_VASBuildContextImpl<C>::_VASBuildContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}

/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode)
	{
		auto  [src_geom, dst_geom] = _GetResourcesOrThrow( src, dst );

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
	Build
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Build (const RTSceneBuild &cmd, RTSceneID dst)
	{
		auto  [scene, scratch_buf, inst_buf] = _GetResourcesOrThrow( dst, cmd.scratch.id, cmd.instanceData.id );

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_ERRV( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));
		
		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= scene.Handle();
		build_info.scratchData.deviceAddress	= scratch_buf.GetDeviceAddress() + cmd.scratch.offset;

		geom.geometry.instances.data.deviceAddress = inst_buf.GetDeviceAddress() + cmd.instanceData.offset;
		
		ASSERT( build_info.scratchData.deviceAddress % this->_mngr.GetDevice().GetProperties().accelerationStructureProps.minAccelerationStructureScratchOffsetAlignment == 0 );
		ASSERT( geom.geometry.instances.data.deviceAddress % 16 == 0 );

		return RawCtx::_Build( build_info, p_ranges );
	}
	
/*
=================================================
	Update
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)
	{
		auto  [src_scene, dst_scene, scratch_buf, inst_buf] =
					_GetResourcesOrThrow( (src != Default ? src : dst), dst, cmd.scratch.id, cmd.instanceData.id );

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_ERRV( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));
		
		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure		= src_scene.Handle();
		build_info.dstAccelerationStructure		= dst_scene.Handle();
		build_info.scratchData.deviceAddress	= scratch_buf.GetDeviceAddress() + cmd.scratch.offset;
		
		geom.geometry.instances.data.deviceAddress = inst_buf.GetDeviceAddress() + cmd.instanceData.offset;
		
		ASSERT( build_info.scratchData.deviceAddress % this->_mngr.GetDevice().GetProperties().accelerationStructureProps.minAccelerationStructureScratchOffsetAlignment == 0 );
		ASSERT( geom.geometry.instances.data.deviceAddress % 16 == 0 );

		return RawCtx::_Build( build_info, p_ranges );
	}
	
/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode)
	{
		auto  [src_scene, dst_scene] = _GetResourcesOrThrow( src, dst );

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
	VQueryManager::Query  _VASBuildContextImpl<C>::_ASQueryOrThrow (ERTASProperty property) const
	{
		EQueryType	q_type = Default;

		BEGIN_ENUM_CHECKS();
		switch ( property )
		{
			case ERTASProperty::CompactedSize :		q_type = EQueryType::AccelStructCompactedSize;		break;
			case ERTASProperty::SerializationSize :	q_type = EQueryType::AccelStructSerializationSize;	break;
			case ERTASProperty::Size :				CHECK_THROW( this->_GetExtensions().rayTracingMaintenance1 );  q_type = EQueryType::AccelStructSize;  break;
			default :								CHECK_THROW( false ); // unknown ERTASProperty
		}
		END_ENUM_CHECKS();

		auto&	qm		= this->_mngr.GetQueryManager();
		auto	query	= qm.AllocQuery( this->_mngr.GetQueueType(), q_type );
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
	void  _VASBuildContextImpl<C>::_WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto  [src_as, dst_buf] = _GetResourcesOrThrow( as, dstBuffer );
		auto  query				= _ASQueryOrThrow( property );

		RawCtx::_WriteProperty( src_as.Handle(), dst_buf.Handle(), offset, size, query );
	}
	
/*
=================================================
	_ReadProperty
=================================================
*/
	template <typename C>
	template <typename ASType>
	Promise<Bytes>  _VASBuildContextImpl<C>::_ReadProperty (ERTASProperty property, ASType as)
	{
		auto&	src_as	= _GetResourcesOrThrow( as );
		auto	query	= _ASQueryOrThrow( property );
		
		RawCtx::_WriteProperty( src_as.Handle(), query );

		return Threading::MakePromise( [query] () -> Threading::PromiseResult<Bytes>
										{
											auto&	rts			= RenderTaskScheduler();
											auto&	query_mngr	= rts.GetQueryManager();
											Bytes	size;
											CHECK_PE( query_mngr.GetRTASProperty( rts.GetDevice(), query, OUT &size, Sizeof(size) ));
											return size;
										},
										Tuple{ this->_mngr.GetBatchRC() }
									  );
	}
	
/*
=================================================
	SerializeToMemory
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, VDeviceAddress dst)
	{
		auto&	src_as = _GetResourcesOrThrow( src );

		VkCopyAccelerationStructureToMemoryInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
		info.pNext				= null;
		info.src				= src_as.Handle();
		info.dst.deviceAddress	= VkDeviceAddress(dst);
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

		return RawCtx::SerializeToMemory( info );
	}
	
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)
	{
		auto&	dst_buf	= _GetResourcesOrThrow( dst );
		ASSERT( dst_buf.Size() > dstOffset );
		ASSERT( dst_buf.HasDeviceAddress() );	// TODO: throw?

		return SerializeToMemory( src, dst_buf.GetDeviceAddress() + dstOffset );
	}
		
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, VDeviceAddress dst)
	{
		auto&	src_as = _GetResourcesOrThrow( src );

		VkCopyAccelerationStructureToMemoryInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR;
		info.pNext				= null;
		info.src				= src_as.Handle();
		info.dst.deviceAddress	= VkDeviceAddress(dst);
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_SERIALIZE_KHR;

		return RawCtx::SerializeToMemory( info );
	}
	
	template <typename C>
	void  _VASBuildContextImpl<C>::SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)
	{
		auto&	dst_buf	= _GetResourcesOrThrow( dst );
		ASSERT( dst_buf.Size() > dstOffset );
		ASSERT( dst_buf.HasDeviceAddress() );	// TODO: throw?

		return SerializeToMemory( src, dst_buf.GetDeviceAddress() + dstOffset );
	}
	
/*
=================================================
	DeserializeFromMemory
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (VDeviceAddress src, RTGeometryID dst)
	{
		auto&	dst_as = _GetResourcesOrThrow( dst );

		VkCopyMemoryToAccelerationStructureInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext				= null;
		info.src.deviceAddress	= VkDeviceAddress(src);
		info.dst				= dst_as->Handle();
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

		return RawCtx::DeserializeFromMemory( info );
	}
	
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)
	{
		auto&	src_buf	= _GetResourcesOrThrow( src );
		ASSERT( src_buf.Size() > srcOffset );
		ASSERT( src_buf.HasDeviceAddress() );	// TODO: throw?

		return SerializeToMemory( src_buf.GetDeviceAddress() + srcOffset, dst );
	}
		
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (VDeviceAddress src, RTSceneID dst)
	{
		auto&	dst_as = _GetResourcesOrThrow( dst );

		VkCopyMemoryToAccelerationStructureInfoKHR	info;
		info.sType				= VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext				= null;
		info.src.deviceAddress	= VkDeviceAddress(src);
		info.dst				= dst_as.Handle();
		info.mode				= VK_COPY_ACCELERATION_STRUCTURE_MODE_DESERIALIZE_KHR;

		return RawCtx::DeserializeFromMemory( info );
	}
	
	template <typename C>
	void  _VASBuildContextImpl<C>::DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)
	{
		auto&	src_buf	= _GetResourcesOrThrow( src );
		ASSERT( src_buf.Size() > srcOffset );
		ASSERT( src_buf.HasDeviceAddress() );	// TODO: throw?

		return SerializeToMemory( src_buf.GetDeviceAddress() + srcOffset, dst );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
