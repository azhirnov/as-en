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


	class FC_TestTask final : public RenderTask
	{
	public:
		FC_TestData&	t;

		FC_TestTask (FC_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DirectCtx::Transfer	ctx{ *this };

			const uint	id = t.counter.fetch_add( 1 );
			ctx.FillBuffer( t.buf, Bytes{id} * 4_b, 4_b, uint(GetFrameId().Unique()) );

			Execute( ctx );
		}
	};

	class FC_FrameTask final : public Threading::IAsyncTask
	{
	public:
		FC_TestData&	t;

		FC_FrameTask (FC_TestData& t) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			if ( t.counter.load() >= t.maxCount )
				return;

			auto&	rts = GraphicsScheduler();

			CHECK_TE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_TE( rts.BeginFrame() );

			t.batch	 = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"FrameCounter"} );
			CHECK_TE( t.batch );

			AsyncTask	test	= t.batch->Run< FC_TestTask >( Tuple{ArgRef(t)}, Tuple{}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			return Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "FC_FrameTask"; }
	};


	static bool  FrameCounterTest ()
	{
		auto&		rts			= GraphicsScheduler();
		auto&		res_mngr	= rts.GetResourceManager();
		FC_TestData	t;

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );

		auto	task = Scheduler().Run<FC_FrameTask>( Tuple{ArgRef(t)} );

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( task->IsCompleted() );
		CHECK_ERR( t.counter.load() >= t.maxCount );

		return true;
	}

} // namespace


bool RGTest::Test_FrameCounter ()
{
	bool	result = true;

	RG_CHECK( FrameCounterTest() );

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
