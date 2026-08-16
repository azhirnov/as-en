// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	ASBuildCtx -->  DirectASBuildCtx   --> BarrierMngr --> Vulkan device
				\-> IndirectASBuildCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct AS Build Context implementation
	//

	class _VDirectASBuildCtx : public VBaseDirectContext
	{
	// types
	private:
		using Validator_t	= ASBuildContextValidation;


	// methods
	public:
		void  Copy (const VkCopyAccelerationStructureInfoKHR &info)									__Th___;
		void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)			__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)		__Th___;

		void  SerializeToMemory (const VkCopyMicromapToMemoryInfoEXT &info)							__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToMicromapInfoEXT &info)						__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()														__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()													__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VDirectASBuildCtx (RenderCoroRef task, VCommandBuffer cmdbuf, DebugLabel dbg)				__Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::ASBuild } {}

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst)								__Th___;
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)				__Th___;

		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst)										__Th___;
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)						__Th___;

		void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
							  VkDeviceAddress indirectMem, Bytes indirectStride)					__Th___;
		void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem)	__Th___;

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
					  VkAccelerationStructureBuildRangeInfoKHR const* const& ranges)				__Th___;

		void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query)		__Th___;
		void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
							  const VQueryManager::Query &query)									__Th___;

		void  _WriteProperty (VkMicromapEXT micromap, const VQueryManager::Query &query)			__Th___;
		void  _WriteProperty (VkMicromapEXT micromap, VkBuffer dstBuffer, Bytes offset,
							  const VQueryManager::Query &query)									__Th___;

		void  _BuildClusterIndirect (const RTClusterBuild &cmd)										__Th___;
		void  _BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd)						__Th___;

		void  _Build (const RTMicromapBuild &cmd, RTMicromapID dst)									__Th___;
		void  _Copy (RTMicromapID src, RTMicromapID dst, ERTASCopyMode mode)						__Th___;

		ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()								C_NE___	{ return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }
	};



	//
	// Vulkan Indirect AS Build Context implementation
	//

	class _VIndirectASBuildCtx : public VBaseIndirectContext
	{
	// types
	private:
		using Validator_t	= ASBuildContextValidation;


	// methods
	public:
		void  Copy (const VkCopyAccelerationStructureInfoKHR &info)									__Th___;
		void  SerializeToMemory (const VkCopyAccelerationStructureToMemoryInfoKHR &info)			__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToAccelerationStructureInfoKHR &info)		__Th___;

		void  SerializeToMemory (const VkCopyMicromapToMemoryInfoEXT &info)							__Th___;
		void  DeserializeFromMemory (const VkCopyMemoryToMicromapInfoEXT &info)						__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()													__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()												__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VIndirectASBuildCtx (RenderCoroRef task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)		__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::ASBuild } {}

		void  _Build  (const RTGeometryBuild &cmd, RTGeometryID dst)								__Th___;
		void  _Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)				__Th___;

		void  _Build  (const RTSceneBuild &cmd, RTSceneID dst)										__Th___;
		void  _Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)						__Th___;

		void  _Build (const VkAccelerationStructureBuildGeometryInfoKHR &info,
					  VkAccelerationStructureBuildRangeInfoKHR const* const& ranges)				__Th___;

		void  _BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
							  VkDeviceAddress indirectMem, Bytes indirectStride)					__Th___;
		void  _BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, VkDeviceAddress indirectMem)	__Th___;

		void  _BuildIndirect (const VkAccelerationStructureBuildGeometryInfoKHR &info,
							  VkDeviceAddress indirectMem, uint const* maxPrimCount, uint indirectStride)__Th___;

		void  _WriteProperty (VkAccelerationStructureKHR as, const VQueryManager::Query &query)		__Th___;
		void  _WriteProperty (VkAccelerationStructureKHR as, VkBuffer dstBuffer, Bytes offset,
							  const VQueryManager::Query &query)									__Th___;

		void  _WriteProperty (VkMicromapEXT micromap, const VQueryManager::Query &query)			__Th___;
		void  _WriteProperty (VkMicromapEXT micromap, VkBuffer dstBuffer, Bytes offset,
							  const VQueryManager::Query &query)									__Th___;

		void  _BuildClusterIndirect (const RTClusterBuild &cmd)										__Th___;
		void  _BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd)						__Th___;

		void  _Build (const RTMicromapBuild &cmd, RTMicromapID dst)									__Th___;
		void  _Copy (RTMicromapID src, RTMicromapID dst, ERTASCopyMode mode)						__Th___;

		ND_ DeviceProperties::RayTracingProperties const&  _RTProps ()								C_NE___	{ return _GetBarrierMngr().GetDevice().GetDeviceProperties().rayTracing; }
	};



	//
	// Vulkan AS Build Context implementation
	//

	template <typename CtxImpl>
	class _VASBuildContextImpl final : public CtxImpl, public IASBuildContextVk
	{
	// types
	public:
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using RenderCoroRef	= typename CtxImpl::RenderCoroRef;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= AccumBarriers< _VASBuildContextImpl< CtxImpl >>;
		using DeferredBar	= AccumDeferredBarriersForCtx< _VASBuildContextImpl< CtxImpl >>;
		using Validator_t	= ASBuildContextValidation;


	// methods
	public:
		explicit _VASBuildContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)		__Th___;

		_VASBuildContextImpl ()																						= delete;
		_VASBuildContextImpl (const _VASBuildContextImpl &)															= delete;

		using RawCtx::Copy;
		using RawCtx::SerializeToMemory;
		using RawCtx::DeserializeFromMemory;

		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)													__Th_OV	{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)								__Th_OV	{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)				__Th_OV;

		void  Build  (const RTSceneBuild &cmd, RTSceneID dst)														__Th_OV	{ RawCtx::_Build( cmd, dst ); }
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)										__Th_OV	{ RawCtx::_Update( cmd, src, dst ); }
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)						__Th_OV;

		void  Build (const RTMicromapBuild &cmd, RTMicromapID dst)													__Th_OV	{ RawCtx::_Build( cmd, dst ); }
		void  Copy (RTMicromapID src, RTMicromapID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)					__Th_OV	{ RawCtx::_Copy( src, dst, mode ); }

		void  SerializeToMemory (RTGeometryID src, DeviceAddress dst)												__Th_OV;
		void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)									__Th_OV;

		void  SerializeToMemory (RTSceneID src, DeviceAddress dst)													__Th_OV;
		void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)										__Th_OV;

		void  SerializeToMemory (RTMicromapID src, DeviceAddress dst)												__Th_OV;
		void  SerializeToMemory (RTMicromapID src, BufferID dst, Bytes dstOffset)									__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)											__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)								__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)												__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)									__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTMicromapID dst)											__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTMicromapID dst)								__Th_OV;

		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)		 __Th_OV;
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)			 __Th_OV;
		void  WriteProperty (ERTASProperty property, RTMicromapID micromap, BufferID dstBuffer, Bytes offset, Bytes size = UMax) __Th_OV;

		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)										__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)											__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTMicromapID micromap)								__Th_OV;

		void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer,
							 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)									__Th_OV;
		void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
							 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
							 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)									__Th_OV;

		void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer)					__Th_OV;
		void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
							 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)								__Th_OV;

		void  BuildClusterIndirect (const RTClusterBuild &cmd)														__Th_OV	{ RawCtx::_BuildClusterIndirect( cmd ); }
		void  BuildPartitionedIndirect (const RTPartitionedSceneBuild &cmd)											__Th_OV	{ RawCtx::_BuildPartitionedIndirect( cmd ); }

		VBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ASType>
		void  _WriteProperty (ERTASProperty property, ASType as, BufferID dstBuffer, Bytes offset, Bytes size)		__Th___;

		template <typename ASType>
		ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as)										__Th___;

		ND_ VQueryManager::Query  _ASQueryOrThrow (ERTASProperty property)											C_Th___;
		ND_ VQueryManager::Query  _MMQueryOrThrow (ERTASProperty property)											C_Th___;
	};

    extern template class _VASBuildContextImpl< _VDirectASBuildCtx >;
    extern template class _VASBuildContextImpl< _VIndirectASBuildCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectASBuildContext		= Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VDirectASBuildCtx >;
	using VIndirectASBuildContext	= Graphics::_hidden_::_VASBuildContextImpl< Graphics::_hidden_::_VIndirectASBuildCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
