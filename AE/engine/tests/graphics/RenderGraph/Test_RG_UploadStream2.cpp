// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct US2_TestData
	{
		GAutorelease<ImageID>		image;
		ImageMemView				imageData;
		const uint2					dimension	{1u << 12};
		CommandBatchPtr				batch;
		GfxMemAllocatorPtr			gfxAlloc;
		ImageStream					stream;
		Atomic<uint>				counter		{0};
	};

	static constexpr Bytes	upload_limit = 1_Mb;


	class US2_UploadStreamTask final : public RenderTask
	{
	public:
		US2_TestData&	t;

		US2_UploadStreamTask (US2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg }, t{ t }
		{}

		void  Run () __Th_OV
		{
			DirectCtx::Transfer	ctx{ *this };

			const uint3	pos = uint3{ 0u, t.stream.posYZ };

			if ( t.counter.fetch_add(1) == 0 )
			{
				ctx.AccumBarriers()
					.ImageBarrier( t.stream.ImageId(), EResourceState::Invalidate, EResourceState::CopyDst );
			}

			ImageMemView	mem_view;
			ctx.UploadImage( INOUT t.stream, OUT mem_view );

			Bytes	copied;
			CHECK_TE( mem_view.CopyFrom( uint3{0}, pos, t.imageData, mem_view.Dimension(), OUT copied ) and
					  copied == mem_view.Image2DSize() );

			Execute( ctx );

			const auto	stat = GraphicsScheduler().GetResourceManager().GetStagingBufferFrameStat( GetFrameId() );
			CHECK( stat.dynamicWrite > 0 );
			CHECK( stat.dynamicWrite <= upload_limit );
		}
	};


	class US2_FrameTask final : public Threading::IAsyncTask
	{
	public:
		US2_TestData&	t;

		US2_FrameTask (US2_TestData& t) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			if ( t.stream.IsCompleted() )
				return;

			auto&	rts = GraphicsScheduler();

			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = upload_limit;

			CHECK_TE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_TE( rts.BeginFrame( cfg ));

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"UploadStream2"} );
			CHECK_TE( t.batch );

		  #ifdef AE_HAS_COROUTINE
			AsyncTask	test = t.batch->Run(
				[] (US2_TestData &t) -> RenderTaskCoro
				{
					CHECK( not co_await Coro_IsCanceled );
					CHECK( (co_await Coro_Status) == EStatus::InProgress );
					CHECK( (co_await Coro_TaskQueue) == ETaskQueue::Renderer );

					// same as 'US2_UploadStreamTask'
					RenderTask&		self = co_await RenderTask_GetRef;

					DirectCtx::Transfer	ctx{ self };

					const uint3	pos = uint3{ 0u, t.stream.posYZ };

					if ( t.counter.fetch_add(1) == 0 )
					{
						ctx.AccumBarriers()
							.ImageBarrier( t.stream.ImageId(), EResourceState::Invalidate, EResourceState::CopyDst );
					}

					ImageMemView	mem_view;
					ctx.UploadImage( INOUT t.stream, OUT mem_view );

					Bytes	copied;
					CHECK_CE( mem_view.CopyFrom( uint3{0}, pos, t.imageData, mem_view.Dimension(), OUT copied ) and
							  copied == mem_view.Image2DSize() );

					co_await RenderTask_Execute( ctx );

					const auto	stat = GraphicsScheduler().GetResourceManager().GetStagingBufferFrameStat( self.GetFrameId() );
					CHECK( stat.dynamicWrite <= upload_limit );

					co_return;
				}( t ),
				Tuple{}, True{"Last"}, {"test task"} );
		  #else
			AsyncTask	test = t.batch->Run< US2_UploadStreamTask >( Tuple{ArgRef(t)}, Tuple{}, True{"Last"}, {"test task"} );
		  #endif

			AsyncTask	end = rts.EndFrame( Tuple{test} );

			return Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "US2_FrameTask"; }
	};


	static bool  UploadStream2Test ()
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Array<ubyte>	img_data;
		US2_TestData	t;
		const Bytes		bpp				= 4_b;
		const Bytes		src_row_pitch	= t.dimension.x * bpp;
		const auto		format			= EPixelFormat::RGBA8_UNorm;

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.image		= res_mngr.CreateImage( ImageDesc{}.SetDimension( t.dimension ).SetFormat( format ).SetUsage( EImageUsage::Transfer ), "image", t.gfxAlloc );
		CHECK_ERR( t.image );

		img_data.resize( usize(src_row_pitch * t.dimension.y) );
		for (uint y = 0; y < t.dimension.y; ++y)
		{
			for (uint x = 0; x < t.dimension.x; ++x)
			{
				ubyte*	ptr = &img_data[ usize(x * bpp + y * src_row_pitch) ];

				ptr[0] = ubyte(x);
				ptr[1] = ubyte(y);
				ptr[2] = ubyte(Max( x, y ));
				ptr[3] = 0;
			}
		}

		t.imageData = ImageMemView{ img_data, uint3{}, uint3{t.dimension, 0}, 0_b, 0_b, format, EImageAspect::Color };

		UploadImageDesc			upload_desc;
		upload_desc.imageDim	= uint3{t.dimension, 1u};
		upload_desc.heapType	= EStagingHeapType::Dynamic;
		t.stream = ImageStream{ t.image, upload_desc };

		auto	task = Scheduler().Run<US2_FrameTask>( Tuple{ArgRef(t)} );

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( t.stream.IsCompleted() );
		CHECK_ERR( t.counter.load() >= uint(t.imageData.Image2DSize() / upload_limit) );

		return true;
	}

} // namespace


bool RGTest::Test_UploadStream2 ()
{
	bool	result = true;

	RG_CHECK( UploadStream2Test() );

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
