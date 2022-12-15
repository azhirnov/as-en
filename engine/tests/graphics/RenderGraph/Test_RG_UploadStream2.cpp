// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct US2_TestData
	{
		Strong<ImageID>				image;
		ImageMemView				imageData;
		const uint2					dimension	{1u << 12};
		CommandBatchPtr				batch;
		RC<GfxLinearMemAllocator>	gfxAlloc;
		ImageStream					stream;
		Atomic<uint>				counter		{0};
	};

	static constexpr Bytes	upload_limit = 1_Mb;


	class US2_UploadStreamTask final : public RenderTask
	{
	public:
		US2_TestData&	t;

		US2_UploadStreamTask (US2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg }, t{ t }
		{}

		void  Run () override
		{
			DirectCtx::Transfer	ctx{ *this };

			const uint3	pos = uint3{ 0u, t.stream.posYZ };
			
			if ( t.counter.fetch_add(1) == 0 )
			{
				ctx.AccumBarriers()
					.ImageBarrier( t.stream.Image(), EResourceState::Invalidate, EResourceState::CopyDst );
			}

			ImageMemView	mem_view;
			ctx.UploadImage( INOUT t.stream, OUT mem_view, EStagingHeapType::Dynamic );

			Bytes	copied;
			CHECK_TE( mem_view.Copy( uint3{0}, pos, t.imageData, mem_view.Dimension(), OUT copied ) and
					  copied == mem_view.ImageSize() );

			Execute( ctx );
			
			const auto	stat = RenderTaskScheduler().GetResourceManager().GetStagingBufferFrameStat( GetFrameId() );
			ASSERT( stat.dynamicWrite <= upload_limit );
		}
	};
	

	class US2_FrameTask final : public Threading::IAsyncTask
	{
	public:
		US2_TestData&	t;

		US2_FrameTask (US2_TestData& t) :
			IAsyncTask{ ETaskQueue::Worker },
			t{ t }
		{}

		void  Run () override
		{
			if ( t.stream.IsCompleted() )
				return;

			auto&	rts = RenderTaskScheduler();
			
			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = upload_limit;

			AsyncTask	begin = rts.BeginFrame( cfg );

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Immediately, {"UploadStream2"} );
			CHECK_TE( t.batch );
			
		#ifdef AE_HAS_COROUTINE
			AsyncTask	test = t.batch->Add(
				[] (US2_TestData &t) -> CoroutineRenderTask
				{
					// same as 'US2_UploadStreamTask'
					auto	hnd = co_await RenderTask_Get;

					DirectCtx::Transfer	ctx{ *hnd };

					const uint3	pos = uint3{ 0u, t.stream.posYZ };
			
					if ( t.counter.fetch_add(1) == 0 )
					{
						ctx.AccumBarriers()
							.ImageBarrier( t.stream.Image(), EResourceState::Invalidate, EResourceState::CopyDst );
					}

					ImageMemView	mem_view;
					ctx.UploadImage( INOUT t.stream, OUT mem_view, EStagingHeapType::Dynamic );

					Bytes	copied;
					CHECK_CE( mem_view.Copy( uint3{0}, pos, t.imageData, mem_view.Dimension(), OUT copied ) and
							  copied == mem_view.ImageSize() );

					co_await RenderTask_Execute( ctx );
			
					const auto	stat = RenderTaskScheduler().GetResourceManager().GetStagingBufferFrameStat( hnd->GetFrameId() );
					ASSERT( stat.dynamicWrite <= upload_limit );
					
					co_return;
				}( t ),
				Tuple{begin}, True{"Last"}, {"test task"} );
		#else
			AsyncTask	test = t.batch->Add< US2_UploadStreamTask >( Tuple{ArgRef(t)}, Tuple{begin}, True{"Last"}, {"test task"} );
		#endif

			AsyncTask	end = rts.EndFrame( Tuple{test} );

			Continue( Tuple{end} );
		}

		StringView  DbgName ()	C_NE_OV	{ return "US2_FrameTask"; }
	};

	
	static bool  UploadStream2Test ()
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Array<ubyte>	img_data;
		US2_TestData	t;
		const Bytes		bpp				= 4_b;
		const Bytes		src_row_pitch	= t.dimension.x * bpp;
		const auto		format			= EPixelFormat::RGBA8_UNorm;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
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

		UploadImageDesc	upload_desc;
		upload_desc.imageSize = uint3{t.dimension, 1u};
		t.stream = ImageStream{ t.image, upload_desc };
		
		auto	task = Scheduler().Run<US2_FrameTask>( Tuple{ArgRef(t)} );

		CHECK_ERR( Scheduler().Wait( {task} ));
		CHECK_ERR( rts.WaitAll() );
		CHECK_ERR( t.stream.IsCompleted() );
		CHECK_ERR( t.counter.load() >= uint(t.imageData.ImageSize() / upload_limit) );
	
		CHECK_ERR( res_mngr.ReleaseResources( t.image ));
		return true;
	}

} // namespace


bool RGTest::Test_UploadStream2 ()
{
	CHECK_ERR( UploadStream2Test() );
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
