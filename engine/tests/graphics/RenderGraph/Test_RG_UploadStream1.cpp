// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct US1_TestData
	{
		Strong<BufferID>			buf;
		const Bytes					buf_size	= 32_Mb;
		Array<ubyte>				buffer_data;
		CommandBatchPtr				batch;
		RC<GfxLinearMemAllocator>	gfxAlloc;
		BufferStream				stream;
		Atomic<uint>				counter		{0};
	};

	static constexpr Bytes	upload_limit = 1_Mb;


	class US1_UploadStreamTask final : public RenderTask
	{
	public:
		US1_TestData&	t;

		US1_UploadStreamTask (US1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg }, t{ t }
		{}

		void  Run () override
		{
			DirectCtx::Transfer	ctx{ *this };

			const Bytes	pos = t.stream.pos;

			BufferMemView	mem_view;
			ctx.UploadBuffer( INOUT t.stream, OUT mem_view, EStagingHeapType::Dynamic );

			auto	arr = ArrayView<ubyte>{t.buffer_data}.section( usize(pos), UMax );
			CHECK_TE( mem_view.Copy( arr ) == mem_view.DataSize() );
			
			Execute( ctx );
			
			const auto	stat = RenderTaskScheduler().GetResourceManager().GetStagingBufferFrameStat( GetFrameId() );
			ASSERT( stat.dynamicWrite <= upload_limit );
		}
	};
	

	class US1_FrameTask final : public Threading::IAsyncTask
	{
	public:
		US1_TestData&	t;

		US1_FrameTask (US1_TestData& t) :
			IAsyncTask{ ETaskQueue::Worker },
			t{ t }
		{}

		void  Run () override
		{
			if ( t.stream.IsCompleted() )
				return;

			++t.counter;

			auto&	rts = RenderTaskScheduler();

			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = upload_limit;

			AsyncTask	begin = rts.BeginFrame( cfg );

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Immediately, {"UploadStream1"} );
			CHECK_TE( t.batch );
			
			AsyncTask	test	= t.batch->Add< US1_UploadStreamTask >( Tuple{ArgRef(t)}, Tuple{begin}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "US1_FrameTask"; }
	};

	
	static bool  UploadStream1Test ()
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		US1_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );
	
		t.buffer_data.resize( uint(t.buf_size) );
		for (usize i = 0; i < t.buffer_data.size(); ++i) {
			t.buffer_data[i] = ubyte(i);
		}

		t.stream = BufferStream{ t.buf, 0_b, t.buf_size };
		
		auto	task = Scheduler().Run<US1_FrameTask>( Tuple{ArgRef(t)} );

		CHECK_ERR( Scheduler().Wait( {task} ));
		CHECK_ERR( rts.WaitAll() );
		CHECK_ERR( t.stream.IsCompleted() );
		CHECK_ERR( t.counter.load() >= uint(t.buf_size / upload_limit) );
	
		CHECK_ERR( res_mngr.ReleaseResources( t.buf ));
		return true;
	}

} // namespace


bool RGTest::Test_UploadStream1 ()
{
	CHECK_ERR( UploadStream1Test() );
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
