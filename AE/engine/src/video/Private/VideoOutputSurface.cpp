// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "video/Private/VideoOutputSurface.h"

namespace AE::Video
{
	using namespace AE::Graphics;

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
