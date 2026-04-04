// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct FC_TestData
	{
		GfxMemAllocatorPtr			gfxAlloc;
		GAutorelease<BufferID>		buf;
		const uint					maxCount	= 1000;
		const Bytes					buf_size	= 4_b * maxCount;
		CommandBatchPtr				batch;
		Atomic<uint>				counter		{0};
	};


	static RenderCoro  FC_TestTask (FC_TestData& t)
	{
		DirectCtx::Transfer	ctx{ RenderCoro_Get() };

		const uint	id = t.counter.fetch_add( 1 );
		ctx.FillBuffer( t.buf, Bytes{id} * 4_b, 4_b, uint(RenderCoro_Get().FrameId().Unique()) );

		RenderCoro_Execute( ctx );
	}


	static AsyncCoro  FC_FrameTask (FC_TestData& t)
	{
		for (; t.counter.load() < t.maxCount; )
		{
			auto&	rts = GraphicsScheduler();

			CHECK_CE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rts.BeginFrame() );

			t.batch	 = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"FrameCounter"} );
			CHECK_CE( t.batch );

			AsyncTask	test	= t.batch->Run( FC_TestTask(t), Tuple{}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			Coro_Continue( end );
		}
	}


	static bool  FrameCounterTest ()
	{
		auto&		rts			= GraphicsScheduler();
		auto&		res_mngr	= rts.GetResourceManager();
		FC_TestData	t;

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );

		AsyncTask	task = Scheduler().Run( FC_FrameTask( t ));

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( task->IsCompleted() );
		CHECK_ERR( t.counter.load() >= t.maxCount );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_FrameCounter ()
{
	bool	result = true;

	RG_CHECK( FrameCounterTest() );

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
