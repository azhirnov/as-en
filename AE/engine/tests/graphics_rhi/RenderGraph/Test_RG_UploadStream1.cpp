// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct US1_TestData
	{
		GAutorelease<BufferID>		buf;
		const Bytes					buf_size	= 32_MiB;
		Array<ubyte>				buffer_data;
		CommandBatchPtr				batch;
		GfxMemAllocatorPtr			gfxAlloc;
		BufferStream				stream;
		Atomic<uint>				counter		{0};
	};

	static constexpr Bytes	upload_limit = 1_MiB;


	static RenderCoro  US1_UploadStreamTask (US1_TestData& t)
	{
		DirectCtx::Transfer	ctx{ RenderCoro_Get() };

		const Bytes	pos = t.stream.pos;

		BufferMemView	mem_view;
		ctx.UploadBuffer( INOUT t.stream, OUT mem_view );

		auto	arr = ArrayView<ubyte>{t.buffer_data}.section( usize(pos), UMax );
		CHECK_CE( mem_view.CopyFrom( arr ) == mem_view.DataSize() );

		RenderCoro_Execute( ctx );

		const auto	stat = GraphicsScheduler().GetResourceManager().GetStagingBufferFrameStat( ctx.GetFrameId() );
		CHECK( stat.dynamicWrite > 0 );
		CHECK( stat.dynamicWrite <= upload_limit );
	}


	static AsyncCoro  US1_FrameTask (US1_TestData& t)
	{
		for (; not t.stream.IsCompleted();)
		{
			++t.counter;

			auto&	rts = GraphicsScheduler();

			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = upload_limit;

			CHECK_CE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rts.BeginFrame( cfg ));

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"UploadStream1"} );
			CHECK_CE( t.batch );

			AsyncTask	test	= t.batch->Run( US1_UploadStreamTask( t ), Tuple{}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			Coro_Continue( end );
		}
	}


	static bool  UploadStream1Test ()
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		US1_TestData	t;

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.buf		= res_mngr.CreateBuffer( BufferDesc{ t.buf_size, EBufferUsage::Transfer }.SetMemory( EMemoryType::DeviceLocal ), "dst_buf", t.gfxAlloc );
		CHECK_ERR( t.buf );

		t.buffer_data.resize( uint(t.buf_size) );
		for (usize i = 0; i < t.buffer_data.size(); ++i) {
			t.buffer_data[i] = ubyte(i);
		}

		t.stream = BufferStream{ t.buf, UploadBufferDesc{ 0_b, t.buf_size }.DynamicHeap() };

		AsyncTask	task = Scheduler().Run( US1_FrameTask( t ));

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( t.stream.IsCompleted() );
		CHECK_ERR( t.counter.load() >= uint(t.buf_size / upload_limit) );

		return true;
	}

} // namespace


bool RGTest::Test_UploadStream1 ()
{
	bool	result = true;

	RG_CHECK( UploadStream1Test() );

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
