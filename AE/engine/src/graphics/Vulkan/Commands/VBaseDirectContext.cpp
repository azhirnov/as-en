// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	_EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VBaseDirectContext::_EndCommandBuffer () __Th___
	{
		GCTX_CHECK( _IsValid() );

		VkCommandBuffer	cmd = _cmdbuf.Get();

		DBG_GRAPHICS_ONLY( _PopDebugGroup();)

		// end recording and release ownership
		CHECK_THROW( _cmdbuf.EndAndRelease() );	// throw

		return cmd;
	}

/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VBaseDirectContext::_ReleaseCommandBuffer ()
	{
		GCTX_CHECK( _IsValid() );

		// don't call vkEndCommandBuffer

		VCommandBuffer	res = RVRef(_cmdbuf);
		ASSERT( not _IsValid() );
		return res;
	}

/*
=================================================
	_ReuseOrCreateCommandBuffer
=================================================
*/
	VCommandBuffer  _VBaseDirectContext::_ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg, bool firstInQueue) __NE___
	{
		if_likely( cmdbuf.IsValid() )
		{
			cmdbuf.SetDebugName( dbg.label );
			return RVRef(cmdbuf);
		}

		auto&	rts = GraphicsScheduler();
		cmdbuf = rts.GetCommandPoolManager().GetCommandBuffer(
						batch.GetQueueType(),
						batch.GetCmdBufType(),
						null );

		if_unlikely( batch.GetQueueType() == EQueueType::Graphics and firstInQueue )
			rts.GetQueryManager().ResetQueries( cmdbuf.Get() );

		DBG_GRAPHICS_ONLY( cmdbuf.PushDebugGroup( rts.GetDevice(), dbg.label, dbg.color );)
		cmdbuf.SetDebugName( dbg.label );

		return RVRef(cmdbuf);
	}

	VCommandBuffer  _VBaseDirectContext::_ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, const RenderTask &task, DebugLabel dbg) __NE___
	{
		return _ReuseOrCreateCommandBuffer( batch, RVRef(cmdbuf),
										    dbg ? dbg : DebugLabel{ task.DbgFullName(), task.DbgColor() },
										    (batch.GetSubmitIndex() == 0 and task.IsFirstInBatch()) );
	}

	VCommandBuffer  _VBaseDirectContext::_ReuseOrCreateCommandBuffer (const VDrawCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg) __NE___
	{
		if_likely( cmdbuf.IsValid() )
		{
			cmdbuf.SetDebugName( dbg.label );
			return RVRef(cmdbuf);
		}

		auto&	rts = GraphicsScheduler();
		cmdbuf = rts.GetCommandPoolManager().GetCommandBuffer(
						batch.GetQueueType(),
						batch.GetCmdBufType(),
						&batch.GetPrimaryCtxState() );

		DBG_GRAPHICS_ONLY( cmdbuf.PushDebugGroup( rts.GetDevice(), dbg.label, dbg.color );)
		cmdbuf.SetDebugName( dbg.label );

		return RVRef(cmdbuf);
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
