// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RGraphicsContext.h"
# include "graphics/Remote/RRenderTaskScheduler.h"
# include "graphics/RenderGraphImpl.h"

namespace AE::Graphics
{
#	include "graphics/Private/RenderTaskScheduler.cpp.h"
	using namespace AE::RemoteGraphics;

/*
=================================================
	GraphicsContextApi
=================================================
*/
	RC<RDrawCommandBatch>  RenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch (RenderTaskScheduler &rts,
																						   const RPrimaryCmdBufState &primaryState, const RenderPassDesc &desc,
																						   DebugLabel dbg) __NE___
	{
		return null;
	}

	RC<RDrawCommandBatch>  RenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch (RenderTaskScheduler &rts,
																						  const RDrawCommandBatch &prevBatch, DebugLabel dbg) __NE___
	{
		return null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Initialize
=================================================
*/
	bool  RenderTaskScheduler::_Initialize (const GraphicsCreateInfo &info) __NE___
	{
		Msg::RTS_Initialize			msg;
		RC<Msg::DefaultResponse>	res;

		msg.info = info;

		CHECK_ERR( _device.SendAndWait( msg, OUT res ));
		CHECK_ERR( res->ok );

		return true;
	}

/*
=================================================
	_Deinitialize
=================================================
*/
	void  RenderTaskScheduler::_Deinitialize () __NE___
	{
		Msg::RTS_Deinitialize	msg;
		CHECK( _device.Send( msg ));
	}

/*
=================================================
	_BeginFrame
=================================================
*/
	bool  RenderTaskScheduler::_BeginFrame (const BeginFrameConfig &cfg) __NE___
	{
		_exeIndex.store( 0 );

		Msg::RTS_BeginFrame			msg;
		RC<Msg::DefaultResponse>	res;

		msg.frameId	= GetFrameId();
		msg.cfg		= cfg;

		CHECK_ERR( _device.SendAndWait( msg, OUT res ));
		return res->ok;
	}

/*
=================================================
	_WaitNextFrame
=================================================
*/
	bool  RenderTaskScheduler::_WaitNextFrame (const FrameUID frameId, nanoseconds timeout) __NE___
	{
		Msg::RTS_WaitNextFrame				msg;
		RC<Msg::RTS_WaitNextFrame_Response>	res;

		msg.frameId	= frameId;
		msg.timeout	= timeout;

		CHECK_ERR( _device.SendAndWait( msg, OUT res ));

		if ( not res->ok )
			return false;

		auto&	frame = _perFrame[ frameId.Index() ];
		EXLOCK( frame.guard );

		for (auto& batch : frame.submitted) {
			batch->_OnComplete();
		}
		frame.submitted.clear();

		_device._SetMemoryUsage( res->memUsage );
		return true;
	}

/*
=================================================
	_SkipCmdBatches
=================================================
*/
	void  RenderTaskScheduler::_SkipCmdBatches (EQueueType queue, uint bits) __NE___
	{
		Msg::RTS_SkipCmdBatches		msg;
		RC<Msg::DefaultResponse>	res;

		msg.queue	= queue;
		msg.bits	= bits;

		CHECK_ERRV( _device.SendAndWait( msg, OUT res ));
		CHECK( res->ok );
	}

/*
=================================================
	_EndFrame
=================================================
*/
	bool  RenderTaskScheduler::_EndFrame (FrameUID frameId) __NE___
	{
		Msg::RTS_EndFrame			msg;
		RC<Msg::DefaultResponse>	res;

		msg.frameId		= frameId;
		msg.submitIdx	= GetExecutionIndex();

		CHECK_ERR( _device.SendAndWait( msg, OUT res ));
		return res->ok;
	}

/*
=================================================
	_FlushQueue2
=================================================
*/
	bool  RenderTaskScheduler::_FlushQueue2 (EQueueType, TempBatches_t &pending) __NE___
	{
		StaticArray< RmCommandBufferID, GraphicsConfig::MaxCmdBufPerBatch >			cmdbufs;
		StaticArray< Pair<RmSemaphoreID, ulong>, GraphicsConfig::MaxCmdBatchDeps >	in_deps;
		StaticArray< Pair<RmSemaphoreID, ulong>, GraphicsConfig::MaxCmdBatchDeps >	out_deps;

		for (auto& batch : pending)
		{
			uint	count = 0;
			batch->_cmdPool.GetCommands( OUT cmdbufs.data(), OUT count, uint(cmdbufs.size()) );
			ASSERT( count > 0 );

			uint	in_deps_count = 0;
			CHECK( batch->_GetInputDependencies( OUT in_deps.data(), OUT in_deps_count, in_deps.size() ));

			uint	out_deps_count = 0;
			CHECK( batch->_GetOutputDependencies( OUT out_deps.data(), OUT out_deps_count, out_deps.size() ));

			Msg::RTS_SubmitBatch		msg;
			RC<Msg::DefaultResponse>	res;

			msg.id			= batch->Handle();
			msg.submitIdx	= GetExecutionIndex();
			msg.cmdbufs		= ArrayView{cmdbufs}.section( 0, count );
			msg.inputDeps	= ArrayView{in_deps}.section( 0, in_deps_count );
			msg.outputDeps	= ArrayView{out_deps}.section( 0, out_deps_count );

			CHECK_ERR( _device.SendAndWait( msg, OUT res ));
			CHECK_ERR( res->ok );

			batch->_OnSubmit2();
		}
		return true;
	}

} // AE::Graphics


# include "GraphicsLib.h"

namespace AE::Graphics
{
/*
=================================================
	_WaitAllBatches
=================================================
*/
	bool  RenderTaskScheduler::_WaitAllBatches (const TimePoint_t endTime) __NE___
	{
		const auto	timeout = endTime - TimePoint_t::clock::now();

		Msg::RTS_WaitAll			msg;
		RC<Msg::DefaultResponse>	res;

		msg.frameId	= GetFrameId();
		msg.timeout = timeout;

		CHECK_ERR( _device.SendAndWait( msg, OUT res ));

		FrameUID	fid = msg.frameId;
		for (uint i = 0; i < fid.MaxFrames(); ++i, fid.Inc())
		{
			auto&	frame = _perFrame[ fid.Index() ];
			EXLOCK( frame.guard );

			for (auto& batch : frame.submitted) {
				batch->_OnComplete();
			}
			frame.submitted.clear();
		}

		const auto	end_time = TimePoint_t::clock::now() + timeout;

		if ( auto glib = _device.GetGraphicsLib() )
			Unused( glib->WaitAll( timeout ));

		return res->ok;
	}

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
