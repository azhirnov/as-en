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
		RC<CommandBatch>			batch;
		Atomic<uint>				counter		{0};
	};

	
	class FC_TestTask final : public RenderTask
	{
	public:
		FC_TestData&	t;

		FC_TestTask (FC_TestData& t, RC<CommandBatch> batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DirectCtx::Transfer	ctx{ GetBatchPtr() };
			CHECK_TE( ctx.IsValid() );
			
			const uint	id = t.counter.fetch_add( 1 );
			ctx.FillBuffer( t.buf, Bytes{id} * 4_b, 4_b, uint(GetFrameId().Unique()) );
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};

	class FC_FrameTask final : public Threading::IAsyncTask
	{
	public:
		FC_TestData&	t;

		FC_FrameTask (FC_TestData& t) :
			IAsyncTask{ EThread::Worker },
			t{ t }
		{}

		void  Run () override
		{
			if ( t.counter.load() >= t.maxCount )
				return;

			auto&	rts = RenderTaskScheduler();

			AsyncTask	begin = rts.BeginFrame();
			CHECK_TE( begin );

			t.batch	= rts.CreateBatch( EQueueType::Graphics, 0, "FrameCounter" );
			CHECK_TE( t.batch );
			
			AsyncTask	test = t.batch->Add<FC_TestTask>( MakeTuple(ArgRef(t)), MakeTuple(begin), "test task" );
			CHECK_TE( test );

			AsyncTask	end = rts.EndFrame( MakeTuple(test) );
			CHECK_TE( end );

			CHECK_TE( Continue( MakeTuple( end )));
		}
	};


	static bool  FrameCounterTest ()
	{
		auto&		rts			= RenderTaskScheduler();
		auto&		res_mngr	= rts.GetResourceManager();
		FC_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );

		auto	task = Scheduler().Run<FC_FrameTask>( MakeTuple(ArgRef(t)) );

		CHECK_ERR( Scheduler().Wait( {task} ));
		CHECK_ERR( rts.WaitAll() );
		CHECK_ERR( t.counter.load() >= t.maxCount );

		CHECK_ERR( res_mngr.ReleaseResources( t.buf ));
		return true;
	}

}	// namespace


bool RGTest::Test_FrameCounter ()
{
	CHECK_ERR( FrameCounterTest() );
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
