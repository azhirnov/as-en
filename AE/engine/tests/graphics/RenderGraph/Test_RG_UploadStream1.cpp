// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct US1_TestData
	{
		GAutorelease<BufferID>		buf;
		const Bytes					buf_size	= 32_Mb;
		Array<ubyte>				buffer_data;
		CommandBatchPtr				batch;
		GfxMemAllocatorPtr			gfxAlloc;
		BufferStream				stream;
		Atomic<uint>				counter		{0};
	};

	static constexpr Bytes	upload_limit = 1_Mb;


	class US1_UploadStreamTask final : public RenderTask
	{
	public:
		US1_TestData&	t;

		US1_UploadStreamTask (US1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg }, t{ t }
		{}

		void  Run () __Th_OV
		{
			DirectCtx::Transfer	ctx{ *this };

			const Bytes	pos = t.stream.pos;

			BufferMemView	mem_view;
			ctx.UploadBuffer( INOUT t.stream, OUT mem_view );

			auto	arr = ArrayView<ubyte>{t.buffer_data}.section( usize(pos), UMax );
			CHECK_TE( mem_view.CopyFrom( arr ) == mem_view.DataSize() );

			Execute( ctx );

			const auto	stat = GraphicsScheduler().GetResourceManager().GetStagingBufferFrameStat( GetFrameId() );
			CHECK( stat.dynamicWrite > 0 );
			CHECK( stat.dynamicWrite <= upload_limit );
		}
	};


	class US1_FrameTask final : public Threading::IAsyncTask
	{
	public:
		US1_TestData&	t;

		US1_FrameTask (US1_TestData& t) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			if ( t.stream.IsCompleted() )
				return;

			++t.counter;

			auto&	rts = GraphicsScheduler();

			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = upload_limit;

			CHECK_TE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_TE( rts.BeginFrame( cfg ));

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"UploadStream1"} );
			CHECK_TE( t.batch );

			AsyncTask	test	= t.batch->Run< US1_UploadStreamTask >( Tuple{ArgRef(t)}, Tuple{}, True{"Last"}, {"test task"} );
			AsyncTask	end		= rts.EndFrame( Tuple{test} );

			return Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "US1_FrameTask"; }
	};


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

		auto	task = Scheduler().Run<US1_FrameTask>( Tuple{ArgRef(t)} );

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
