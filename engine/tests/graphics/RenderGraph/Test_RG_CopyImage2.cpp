// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"
#include "graphics/RenderGraph/RenderGraphImpl.h"

namespace
{
	struct CI2_TestData
	{
		GAutorelease<ImageID>		img_1;
		GAutorelease<ImageID>		img_2;
		ImageMemView				img_view;
		uint2						src_offset;
		uint2						dst_offset;
		uint2						copy_dim;
		AsyncTask					result;
		CommandBatchPtr				batch;
		bool						isOK		= false;
		RC<GfxLinearMemAllocator>	gfxAlloc;
		RG::RenderGraph				rg;
	};


	template <typename Ctx>
	class CI2_CopyImageTask final : public RenderTask
	{
	public:
		CI2_TestData&	t;

		CI2_CopyImageTask (CI2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			Ctx		ctx{ *this };

			UploadImageDesc	upload;
			upload.aspectMask	= EImageAspect::Color;
			upload.heapType		= EStagingHeapType::Static;

			ImageMemView	upload_mem;
			ctx.UploadImage( t.img_1, upload, OUT upload_mem );

			Bytes			copied;
			CHECK_TE( upload_mem.Copy( t.img_view, OUT copied ) and
					  copied == t.img_view.Image2DSize() );

			ImageCopy		copy;
			copy.srcOffset				= uint3{ t.src_offset, 0u };
			copy.dstOffset				= uint3{ t.dst_offset, 0u };
			copy.extent					= uint3{ t.copy_dim,   1u };
			copy.srcSubres.aspectMask	= EImageAspect::Color;
			copy.dstSubres.aspectMask	= EImageAspect::Color;
			ctx.CopyImage( t.img_1, t.img_2, {copy} );
			
			ReadbackImageDesc	read;
			read.imageOffset	= copy.dstOffset;
			read.imageSize		= copy.extent;
			read.heapType		= EStagingHeapType::Static;

			t.result = AsyncTask{ ctx.ReadbackImage( t.img_2, read )
						.Then(	[p = &t] (const ImageMemView &view)
								{
									p->isOK = (view == p->img_view);
								})};
			
			Execute( ctx );
		}
	};

	
	template <typename Ctx>
	static bool  CopyImage2Test ()
	{
		Array<ubyte>	img_data;
		CI2_TestData	t;
		const uint2		src_dim			= {64, 64};
		const uint2		dst_dim			= {128, 128};
		const int2		img_offset		= {16, 27};
		const Bytes		bpp				= 4_b;
		const Bytes		src_row_pitch	= src_dim.x * bpp;
		const auto		format			= EPixelFormat::RGBA8_UNorm;
		
		t.gfxAlloc = MakeRC<GfxLinearMemAllocator>();

		t.img_1 = t.rg.CreateImage( ImageDesc{}.SetDimension( src_dim ).SetFormat( format ).SetUsage( EImageUsage::Transfer ), "SrcImage", t.gfxAlloc );
		t.img_2 = t.rg.CreateImage( ImageDesc{}.SetDimension( dst_dim ).SetFormat( format ).SetUsage( EImageUsage::Transfer ), "DstImage", t.gfxAlloc );
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

		AsyncTask	begin	= t.rg.BeginFrame();

		auto		batch	= t.rg.CmdBatch( EQueueType::Graphics, {"CopyImage2"} )
								.UseResource( t.img_1 )
								.UploadMemory()
								.ReadbackMemory()
								.Begin();
		CHECK_ERR( batch );

		AsyncTask	task1	= batch.Task< CI2_CopyImageTask<Ctx> >( Tuple{ArgRef(t)}, {"Copy image task"} )
								.UseResource( t.img_2 )
								.Last().Run( Tuple{begin} );
		AsyncTask	end		= t.rg.EndFrame( Tuple{task1} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( t.rg.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_CopyImage2 ()
{
	bool	result = true;

	RG_CHECK( CopyImage2Test< RG::DirectCtx::Transfer   >());
	RG_CHECK( CopyImage2Test< RG::IndirectCtx::Transfer >());
	
	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
