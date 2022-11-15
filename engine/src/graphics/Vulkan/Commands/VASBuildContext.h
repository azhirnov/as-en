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
		void  Copy  (const VkCopyAccelerationStructureInfoKHR &info);
		void  WriteCompactedSize (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, Bytes size);
		
		ND_ VkCommandBuffer	EndCommandBuffer ();
		ND_ VCommandBuffer  ReleaseCommandBuffer ();
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectASBuildCtx (const RenderTask &task);
		_VDirectASBuildCtx (const RenderTask &task, VCommandBuffer cmdbuf);
		
		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
		void  _WriteAccelerationStructuresProperties (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
	};



	//
	// Vulkan Indirect AS Build Context implementation
	//
	
	class _VIndirectASBuildCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  Copy  (const VkCopyAccelerationStructureInfoKHR &info);
		void  WriteCompactedSize (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset, Bytes size);
		
		ND_ VBakedCommands		EndCommandBuffer ();
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectASBuildCtx (const RenderTask &task);
		_VIndirectASBuildCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf);

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst);
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst);

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info, VkAccelerationStructureBuildRangeInfoKHR const* const& ranges);
		void  _WriteAccelerationStructuresProperties (VkAccelerationStructureKHR as, const VQueryManager::Query &query);
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
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VASBuildContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VASBuildContextImpl (const RenderTask &task) : RawCtx{ task } {}
		
		template <typename RawCmdBufType>
		_VASBuildContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_VASBuildContextImpl () = delete;
		_VASBuildContextImpl (const _VASBuildContextImpl &) = delete;

		using RawCtx::Copy;
		
		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)										{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)					{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone);
		
		void  Build  (const RTSceneBuild &cmd, RTSceneID dst);
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst);
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone);
		
		using RawCtx::WriteCompactedSize;

		void  WriteCompactedSize (RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size);
		void  WriteCompactedSize (RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size);
		
		Promise<Bytes>  ReadCompactedSize (RTGeometryID as);
		Promise<Bytes>  ReadCompactedSize (RTSceneID as);

		ND_ Promise<Bytes>  ReadCompactedSize (VkAccelerationStructureKHR as);
		
		void  DebugMarker (NtStringView text, RGBA8u color)												{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)											{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()																			{ RawCtx::_PopDebugGroup(); }

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectASBuildContext		= _hidden_::_VASBuildContextImpl< _hidden_::_VDirectASBuildCtx >;
	using VIndirectASBuildContext	= _hidden_::_VASBuildContextImpl< _hidden_::_VIndirectASBuildCtx >;

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
	Copy
=================================================
*/
	inline void  _VDirectASBuildCtx::Copy (const VkCopyAccelerationStructureInfoKHR &info)
	{
		vkCmdCopyAccelerationStructureKHR( _cmdbuf.Get(), &info );
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	Copy
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode)
	{
		auto*	src_geom = this->_mngr.Get( src );
		auto*	dst_geom = this->_mngr.Get( dst );

		CHECK_ERRV( src_geom != null and
					dst_geom != null );

		VkCopyAccelerationStructureInfoKHR	info;
		info.sType	= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext	= null;
		info.src	= src_geom->Handle();
		info.dst	= dst_geom->Handle();
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
		auto*	scene		= this->_mngr.Get( dst );
		auto*	scratch_buf	= this->_mngr.Get( cmd.scratch.id );
		auto*	inst_buf	= this->_mngr.Get( cmd.instanceData.id );

		CHECK_ERRV( scene		!= null and
					scratch_buf != null and
					inst_buf	!= null );

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_ERRV( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));
		
		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		build_info.dstAccelerationStructure		= scene->Handle();
		build_info.scratchData.deviceAddress	= scratch_buf->GetDeviceAddress() + cmd.scratch.offset;

		geom.geometry.instances.data.deviceAddress = inst_buf->GetDeviceAddress() + cmd.instanceData.offset;
		
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
		auto*	src_scene	= this->_mngr.Get( src != Default ? src : dst );
		auto*	dst_scene	= this->_mngr.Get( dst );
		auto*	scratch_buf	= this->_mngr.Get( cmd.scratch.id );
		auto*	inst_buf	= this->_mngr.Get( cmd.instanceData.id );

		CHECK_ERRV( src_scene	!= null and
					dst_scene	!= null and
					scratch_buf != null and
					inst_buf	!= null );

		VkAccelerationStructureBuildGeometryInfoKHR	build_info;
		VkAccelerationStructureGeometryKHR			geom;
		VkAccelerationStructureBuildRangeInfoKHR	range;
		VkAccelerationStructureBuildRangeInfoKHR*	p_ranges	= &range;

		CHECK_ERRV( VRTScene::ConvertBuildInfo( this->_mngr.GetResourceManager(), cmd, OUT geom, OUT range, OUT build_info ));
		
		build_info.mode							= VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
		build_info.srcAccelerationStructure		= src_scene->Handle();
		build_info.dstAccelerationStructure		= dst_scene->Handle();
		build_info.scratchData.deviceAddress	= scratch_buf->GetDeviceAddress() + cmd.scratch.offset;
		
		geom.geometry.instances.data.deviceAddress = inst_buf->GetDeviceAddress() + cmd.instanceData.offset;
		
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
		auto*	src_scene = this->_mngr.Get( src );
		auto*	dst_scene = this->_mngr.Get( dst );

		CHECK_ERRV( src_scene != null and
					dst_scene != null );

		VkCopyAccelerationStructureInfoKHR	info;
		info.sType	= VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
		info.pNext	= null;
		info.src	= src_scene->Handle();
		info.dst	= dst_scene->Handle();
		info.mode	= VEnumCast( mode );

		RawCtx::Copy( info );
	}
	
/*
=================================================
	WriteCompactedSize
=================================================
*/
	template <typename C>
	void  _VASBuildContextImpl<C>::WriteCompactedSize (RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto*	src_as	= this->_mngr.Get( as );
		auto*	dst_buf	= this->_mngr.Get( dstBuffer );

		CHECK_ERRV( src_as != null and dst_buf != null );

		RawCtx::WriteCompactedSize( src_as->Handle(), dst_buf->Handle(), offset, size );
	}
	
	template <typename C>
	void  _VASBuildContextImpl<C>::WriteCompactedSize (RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)
	{
		auto*	src_as	= this->_mngr.Get( as );
		auto*	dst_buf	= this->_mngr.Get( dstBuffer );

		CHECK_ERRV( src_as != null and dst_buf != null );

		RawCtx::WriteCompactedSize( src_as->Handle(), dst_buf->Handle(), offset, size );
	}
	
/*
=================================================
	ReadCompactedSize
=================================================
*/
	template <typename C>
	Promise<Bytes>  _VASBuildContextImpl<C>::ReadCompactedSize (RTGeometryID as)
	{
		auto*	src_as	= this->_mngr.Get( as );
		CHECK_ERR( src_as != null );

		return ReadCompactedSize( src_as->Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _VASBuildContextImpl<C>::ReadCompactedSize (RTSceneID as)
	{
		auto*	src_as	= this->_mngr.Get( as );
		CHECK_ERR( src_as != null );

		return ReadCompactedSize( src_as->Handle() );
	}
	
	template <typename C>
	Promise<Bytes>  _VASBuildContextImpl<C>::ReadCompactedSize (VkAccelerationStructureKHR as)
	{
		auto&	qm		= this->_mngr.GetQueryManager();
		auto	qsize	= qm.AllocQuery( this->_mngr.GetQueueType(), EQueryType::AccelStructCompactedSize );
		CHECK_ERR( qsize );
		
		RawCtx::_WriteAccelerationStructuresProperties( as, qsize );

		return Threading::MakePromise( [qsize] () -> Threading::PromiseResult<Bytes>
										{
											auto&	rts			= RenderTaskScheduler();
											auto&	query_mngr	= rts.GetResourceManager().GetQueryManager();
											Bytes	size;
											CHECK_ERR( query_mngr.GetRTASCompactedSize( rts.GetDevice(), qsize, OUT &size, Sizeof(size) ),
													   Threading::CancelPromise );
											return size;
										},
										Tuple{ this->_mngr.GetBatchRC() }
									  );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
