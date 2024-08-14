// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/REnumCast.h"
# include "graphics/Remote/Commands/RBaseContext.h"
# include "graphics/Remote/Commands/RAccumBarriers.h"
# include "graphics/Remote/Resources/RRTGeometry.h"
# include "graphics/Remote/Resources/RRTScene.h"

namespace AE::Graphics
{

	//
	// Remote Graphics AS Build Context implementation
	//

	class RASBuildContext final : public Graphics::_hidden_::RBaseContext, public IASBuildContext
	{
	// types
	private:
		using AccumBar		= Graphics::_hidden_::RAccumBarriers< RASBuildContext >;
		using DeferredBar	= Graphics::_hidden_::RAccumDeferredBarriersForCtx< RASBuildContext >;
		using Validator_t	= Graphics::_hidden_::ASBuildContextValidation;


	// methods
	public:
		explicit RASBuildContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)		__Th___;

		RASBuildContext ()																							= delete;
		RASBuildContext (const RASBuildContext &)																	= delete;

		void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)													__Th_OV;
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)								__Th_OV;
		void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)				__Th_OV;

		void  Build  (const RTSceneBuild &cmd, RTSceneID dst)														__Th_OV;
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)										__Th_OV;
		void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)						__Th_OV;

		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV;
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)	__Th_OV;

		void  WriteProperty (ERTASProperty, RmRTGeometryID as, RmBufferID dstBuffer, Bytes offset, Bytes size)		__Th___;
		void  WriteProperty (ERTASProperty, RmRTSceneID as, RmBufferID dstBuffer, Bytes offset, Bytes size)			__Th___;

		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)										__Th_OV	{ return _ReadProperty( property, as ); }
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)											__Th_OV	{ return _ReadProperty( property, as ); }


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

		void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)											__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)								__Th_OV;

		void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)												__Th_OV;
		void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)									__Th_OV;

		ND_ RmCommandBufferID	EndCommandBuffer ()																	__Th___	{ return _EndCommandBuffer( ECtxType::ASBuild ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																__Th___	{ return _ReleaseCommandBuffer( ECtxType::ASBuild ); }

		RBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ASType>
		ND_ Promise<Bytes>  _ReadProperty (ERTASProperty property, ASType as)										__Th___;
	};


/*
=================================================
	_ReadProperty
=================================================
*/
	template <typename ASType>
	Promise<Bytes>  RASBuildContext::_ReadProperty (ERTASProperty property, ASType asId) __Th___
	{
		//VALIDATE_GCTX( ReadProperty( property ));	// TODO

		auto&		as	 = _GetResourcesOrThrow( asId );
		const auto	size = SizeOf<Bytes>;

		RStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		RStagingBufferManager::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, size, size, GraphicsConfig::StagingBufferOffsetAlign,
							 GetFrameId(), EStagingHeapType::Static, False{"readback"} );

		if_unlikely( buffers.empty() )
			RETURN_ERR( "failed to allocate staging buffer" );	// TODO: throw?

		ASSERT( buffers.size() == 1 );
		WriteProperty( property, as.Handle(), buffers[0].bufferHandle, buffers[0].bufferOffset, size );

		const void*	ptr = _ReadbackAlloc( buffers[0].devicePtr, size );

		return Threading::MakePromise(	[ptr] () { return *Cast<Bytes>(ptr); },
										Tuple{ this->_mngr.GetBatchRC() },
										"RASBuildContext::ReadProperty",
										ETaskQueue::PerFrame
									 );
	}

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
