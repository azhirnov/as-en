// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct FC_TestData
	{
		RC<GfxLinearMemAllocator>	gfxAlloc;
		Strong<BufferID>			buf;
		const uint					maxCount	= 1000;
		const Bytes					buf_size	= 4_b * maxCount;
		CommandBatchPtr				batch;
		Atomic<uint>				counter		{0};
	};

	
	class FC_TestTask final : public RenderTask
	{
	public:
		FC_TestData&	t;

		FC_TestTask (FC_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () override
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

		FC_FrameTask (FC_TestData& t) :
			IAsyncTask{ ETaskQueue::Worker },
			t{ t }
		{}

		void  Run () override
		{
			if ( t.counter.load() >= t.maxCount )
				return;

			auto&	rts = RenderTaskScheduler();

			AsyncTask	begin = rts.BeginFrame();

			t.batch	 = rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Deferred, {"FrameCounter"} );
			CHECK_TE( t.batch );
			
			AsyncTask	test	= t.batch->Add< FC_TestTask >( Tuple{ArgRef(t)}, Tuple{begin}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "FC_FrameTask"; }
	};


	static bool  FrameCounterTest ()
	{
		auto&		rts			= RenderTaskScheduler();
		auto&		res_mngr	= rts.GetResourceManager();
		FC_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );

		auto	task = Scheduler().Run<FC_FrameTask>( Tuple{ArgRef(t)} );

		CHECK_ERR( Scheduler().Wait( {task} ));
		CHECK_ERR( rts.WaitAll() );
		CHECK_ERR( t.counter.load() >= t.maxCount );

		CHECK_ERR( res_mngr.ReleaseResources( t.buf ));
		return true;
	}

} // namespace


bool RGTest::Test_FrameCounter ()
{
	CHECK_ERR( FrameCounterTest() );
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
