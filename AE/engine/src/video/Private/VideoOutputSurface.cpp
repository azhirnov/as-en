// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Private/VideoOutputSurface.h"

namespace AE::Video
{
	using namespace AE::Graphics;

	//
	// Readback Image Task
	//
	class VideoOutputSurface::ReadbackImageTask final : public RenderTask
	{
		VideoOutputSurface &	p;

		ReadbackImageTask (VideoOutputSurface* parent, CommandBatchPtr batch, StringView dbgName, RGBA8u) :
			RenderTask{ batch, dbgName },	// throw
			p{ *parent }
		{}

		void  Run () __Th_OV;
		/*{
			DirectCtx::Transfer		ctx{ GetBatchPtr() };

			//ctx.AccumBarriers()
			//	.ImageBarrier( t.img_1, EResourceState::ColorAttachment_Write, EResourceState::CopySrc );


			ReadbackImageDesc	read;
			//read.imageOffset	= copy.dstOffset;
			//read.imageDim		= copy.extent;
			read.heapType		= EStagingHeapType::Dynamic;

			auto	task = AsyncTask{ctx.ReadbackImage( p.img_2, read )
								.Then( [t = &p] (const ImageMemView &view)
										{
											t->_encoder->AddFrame( view );
									  })};
			GraphicsScheduler().AddNextFrameDeps( task );

			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			Execute( ctx );
		}*/
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Begin
=================================================
*/
	bool  VideoOutputSurface::Begin (const IVideoEncoder::Config &cfg, const Path &filename) __NE___
	{
		CHECK_ERR( _encoder->Begin( cfg, filename ));

		// TODO: create render target

		return false;
	}

/*
=================================================
	End
=================================================
*/
	bool  VideoOutputSurface::End () __NE___
	{
		CHECK_ERR( _encoder->End() );

		return false;
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  VideoOutputSurface::IsInitialized () C_NE___
	{
		return false;
	}

/*
=================================================
	GetRenderPassInfo
=================================================
*/
	VideoOutputSurface::RenderPassInfo  VideoOutputSurface::GetRenderPassInfo () C_NE___
	{
		return Default;
	}

/*
=================================================
	Begin
=================================================
*
	bool  VideoOutputSurface::Begin (Graphics::CommandBatch &cmdBatch) __NE___
	{
		return false;
	}

/*
=================================================
	GetTargets
=================================================
*/
	bool  VideoOutputSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
	{
		Unused( targets );
		return false;
	}

/*
=================================================
	End
=================================================
*
	bool  VideoOutputSurface::End (Graphics::CommandBatch &cmdBatch, ArrayView<AsyncTask> deps) __NE___
	{
		auto&	rts = GraphicsScheduler();

		//auto	task = cmdBatch.Add<ReadbackImageTask>( Tuple{}, Tuple{deps} );

		// - readback
		// - encode
		// - write to file
		// - allow next frame		rts.AddNextFrameDeps( );

		return false;
	}
*/

} // AE::Video
