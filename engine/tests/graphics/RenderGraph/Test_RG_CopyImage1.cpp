// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct CI1_TestData
	{
		Strong<ImageID>				img_1;
		Strong<ImageID>				img_2;
		ImageMemView				img_view;
		uint2						src_offset;
		uint2						dst_offset;
		uint2						copy_dim;
		AsyncTask					result;
		RC<CommandBatch>			batch;
		bool						isOK		= false;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};


	template <typename Ctx>
	class CI1_CopyImageTask final : public RenderTask
	{
	public:
		CI1_TestData&	t;

		CI1_CopyImageTask (CI1_TestData& t, RC<CommandBatch> batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			Ctx		ctx{ GetBatchPtr() };

			CHECK_TE( ctx.IsValid() );
			
			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc )
				.ImageBarrier( t.img_1, EResourceState::Invalidate, EResourceState::CopyDst );

			UploadImageDesc	upload;
			upload.aspectMask	= EImageAspect::Color;
			upload.heapType		= EStagingHeapType::Static;

			ImageMemView	upload_mem;
			ctx.UploadImage( t.img_1, upload, OUT upload_mem );

			CHECK_TE( upload_mem.Copy( t.img_view ) == t.img_view.ImageSize() );

			ctx.AccumBarriers()
				.ImageBarrier( t.img_1, EResourceState::CopyDst, EResourceState::CopySrc )
				.ImageBarrier( t.img_2, EResourceState::Invalidate, EResourceState::CopyDst );

			ImageCopy	copy;
			copy.srcOffset				= uint3{ t.src_offset, 0u };
			copy.dstOffset				= uint3{ t.dst_offset, 0u };
			copy.extent					= uint3{ t.copy_dim,   1u };
			copy.srcSubres.aspectMask	= EImageAspect::Color;
			copy.dstSubres.aspectMask	= EImageAspect::Color;
			ctx.CopyImage( t.img_1, t.img_2, {copy} );
			
			ctx.AccumBarriers().ImageBarrier( t.img_2, EResourceState::CopyDst, EResourceState::CopySrc );
			
			ReadbackImageDesc	read;
			read.imageOffset	= copy.dstOffset;
			read.imageSize		= copy.extent;
			read.heapType		= EStagingHeapType::Static;

			t.result = AsyncTask{ ctx.ReadbackImage( t.img_2, read )
						.Then( [p = &t] (const ImageMemView &view)
								{
									p->isOK = (view == p->img_view);
								})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};

	
	template <typename Ctx>
	static bool  CopyImage1Test ()
	{
		auto&			rts				= RenderTaskScheduler();
		auto&			res_mngr		= rts.GetResourceManager();
		Array<ubyte>	img_data;
		CI1_TestData	t;
		const uint2		src_dim			= {64, 64};
		const uint2		dst_dim			= {128, 128};
		const int2		img_offset		= {16, 27};
		const Bytes		bpp				= 4_b;
		const Bytes		src_row_pitch	= src_dim.x * bpp;
		const auto		format			= EPixelFormat::RGBA8_UNorm;
		
		t.gfxAlloc = MakeRC<GfxLinearMemAllocator>();

		t.img_1 = res_mngr.CreateImage( ImageDesc{}.SetDimension( src_dim ).SetFormat( format ).SetUsage( EImageUsage::Transfer ), "SrcImage", t.gfxAlloc );
		t.img_2 = res_mngr.CreateImage( ImageDesc{}.SetDimension( dst_dim ).SetFormat( format ).SetUsage( EImageUsage::Transfer ), "DstImage", t.gfxAlloc );
		CHECK_ERR( t.img_1 and t.img_2 );
	
		img_data.resize( usize(src_row_pitch * src_dim.y) );
		for (uint y = 0; y < src_dim.y; ++y)
		{
			for (uint x = 0; x < src_dim.x; ++x)
			{
				ubyte*	ptr = &img_data[ usize(x * bpp + y * src_row_pitch) ];

				ptr[0] = ubyte(x);
				ptr[1] = ubyte(y);
				ptr[2] = ubyte(Max( x, y ));
				ptr[3] = 0;
			}
		}
		t.src_offset	= uint2{0, 0};
		t.dst_offset	= uint2{16, 27};
		t.copy_dim		= src_dim;
		t.img_view		= ImageMemView{ img_data, uint3{}, uint3{src_dim, 0}, 0_b, 0_b, format, EImageAspect::Color };

		AsyncTask	begin	= rts.BeginFrame();
		CHECK_ERR( begin );

		auto	batch = rts.CreateBatch( EQueueType::Graphics, 0, "CopyImage2" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< CI1_CopyImageTask<Ctx> >( MakeTuple(ArgRef(t)), MakeTuple(begin), "Copy image task" );
		CHECK_ERR( task1 );

		AsyncTask	end		= rts.EndFrame( MakeTuple(task1) );
		CHECK_ERR( end );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.img_1, t.img_2 ));

		CHECK_ERR( t.isOK );
		return true;
	}

}	// namespace


bool RGTest::Test_CopyImage1 ()
{
	CHECK_ERR( CopyImage1Test< DirectCtx::Transfer   >());
	CHECK_ERR( CopyImage1Test< IndirectCtx::Transfer >());
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
