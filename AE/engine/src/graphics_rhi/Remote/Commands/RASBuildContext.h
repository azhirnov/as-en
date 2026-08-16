// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Commands/RBaseContext.h"
# include "graphics_rhi/Remote/Commands/RAccumBarriers.h"

namespace AE::Graphics
{

	//
	// Remote Graphics AS Build Context implementation
	//

	class RASBuildContext final : public Graphics::_hidden_::RBaseContext, public IASBuildContextVk
	{
	// types
	private:
		using AccumBar		= Graphics::_hidden_::AccumBarriers< RASBuildContext >;
		using DeferredBar	= Graphics::_hidden_::AccumDeferredBarriersForCtx< RASBuildContext >;
		using Validator_t	= Graphics::_hidden_::ASBuildContextValidation;


	// methods
	public:
		explicit RASBuildContext (RenderCoroRef task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)			__Th___;

		RASBuildContext ()																							= delete;
		RASBuildContext (const RASBuildContext &)																	= delete;

		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)													__Th_OV;
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)								__Th_OV;
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)				__Th_OV;

		void  Build  (const RTSceneBuild &cmd, RTSceneID dst)														__Th_OV;
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)										__Th_OV;
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)						__Th_OV;

		void  Build (const RTMicromapBuild &cmd, RTMicromapID dst)													__Th_OV	{}	// TODO
		void  Copy (RTMicromapID src, RTMicromapID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)					__Th_OV	{}

		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV;
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV;

		void  WriteProperty (ERTASProperty, RmRTGeometryID as, RmBufferID dstBuffer, Bytes offset, Bytes size)		__Th___;
		void  WriteProperty (ERTASProperty, RmRTSceneID as, RmBufferID dstBuffer, Bytes offset, Bytes size)			__Th___;

		void  WriteProperty (ERTASProperty property, RTMicromapID micromap, BufferID dstBuffer, Bytes offset, Bytes size = UMax) __Th_OV {}

		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)										__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)											__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTMicromapID micromap)								__Th_OV	{ return {}; }


		// Vulkan only //
		void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer,
							 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)									__Th_OV;
		void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
							 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
							 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)									__Th_OV;

		void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer)					__Th_OV;
		void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
							 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)								__Th_OV;

		void  SerializeToMemory (RTGeometryID src, DeviceAddress dst)												__Th_OV;
		void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)									__Th_OV;

		void  SerializeToMemory (RTSceneID src, DeviceAddress dst)													__Th_OV;
		void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)										__Th_OV;

		void  SerializeToMemory (RTMicromapID src, DeviceAddress dst)												__Th_OV	{}	// TODO
		void  SerializeToMemory (RTMicromapID src, BufferID dst, Bytes dstOffset)									__Th_OV	{}

		void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)											__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)								__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)												__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)									__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTMicromapID dst)											__Th_OV	{}	// TODO
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTMicromapID dst)								__Th_OV	{}

		void  BuildClusterIndirect (const RTClusterBuild &)															__Th_OV	{ UNTESTED; }
		void  BuildPartitionedIndirect (const RTPartitionedSceneBuild &)											__Th_OV	{ UNTESTED; }


		ND_ RmCommandBufferID	EndCommandBuffer ()																	__Th___	{ return _EndCommandBuffer( ECtxType::ASBuild ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																__Th___	{ return _ReleaseCommandBuffer( ECtxType::ASBuild ); }

		RBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ASType>
		ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as)										__Th___;
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
