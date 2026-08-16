// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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

	static constexpr Bytes	c_UploadLimit = 1_MiB;


	static RenderCoro  US2_UploadStreamTask (US2_TestData& t)
	{
		DirectCtx::Transfer	ctx{ RenderCoro_Get() };

		const uint3	pos = uint3{ 0u, t.stream.posYZ };

		if ( t.counter.fetch_add(1) == 0 )
		{
			ctx.AccumBarriers()
				.ResourceBarrier( t.stream.ImageId(), EResourceState::Invalidate, EResourceState::CopyDst );
		}

		ImageMemView	mem_view;
		ctx.UploadImage( INOUT t.stream, OUT mem_view );

		Bytes	copied;
		CHECK_CE( mem_view.CopyFrom( uint3{0}, pos, t.imageData, mem_view.Dimension(), OUT copied ) and
				  copied == mem_view.Image2DSize() );

		RenderCoro_Execute( ctx );

		const auto	stat = GraphicsScheduler().GetResourceManager().GetStagingBufferFrameStat( RenderCoro_Get().FrameId() );
		CHECK( stat.dynamicWrite > 0 );
		CHECK( stat.dynamicWrite <= c_UploadLimit );

		co_return;
	}


	static AsyncCoro  US2_FrameTask (US2_TestData& t)
	{
		for (; not t.stream.IsCompleted(); )
		{
			auto&	rts = GraphicsScheduler();

			BeginFrameConfig	cfg;
			cfg.stagingBufferPerFrameLimits.write = c_UploadLimit;

			CHECK_CE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rts.BeginFrame( cfg ));

			t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"UploadStream2"} );
			CHECK_CE( t.batch );

			AsyncTask	test = t.batch->Run( US2_UploadStreamTask( t ), Tuple{}, True{"Last"}, {"test task"} );

			AsyncTask	end = rts.EndFrame( Tuple{test} );

			Coro_Continue( end );
		}
	}


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
		upload_desc.imageDim	= ImageDim_t{uint3{ t.dimension, 1u }};
		upload_desc.heapType	= EStagingHeapType::Dynamic;
		t.stream = ImageStream{ t.image, upload_desc };

		AsyncTask	task = Scheduler().Run( US2_FrameTask( t ));

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( t.stream.IsCompleted() );
		CHECK_ERR( t.counter.load() >= uint(t.imageData.Image2DSize() / c_UploadLimit) );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_UploadStream2 ()
{
	bool	result = true;

	RG_CHECK( UploadStream2Test() );

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
