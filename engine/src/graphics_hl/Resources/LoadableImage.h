// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/CommandBuffer.h"
#include "threading/Stream/AsyncStream.h"

namespace AE::Graphics
{
	using AE::Threading::AsyncRStream;


	//
	// Loadable Image
	//

	class LoadableImage final : public EnableRC<LoadableImage>
	{
	// types
	public:
		struct AsyncLoader {
			ND_ Promise<RC<LoadableImage>>  Load (RC<AsyncRStream> stream);
		};
		
		struct Loader {
			ND_ RC<LoadableImage>  Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc = Default);
		};


	// variables
	private:
		Strong<ImageID>		_imageId;
		EImage				_viewType	= Default;


	// methods
	public:
		LoadableImage () {}
		~LoadableImage ();

		ND_ ImageID		GetImageID ()	const	{ return _imageId; }
		ND_ EImage		ViewType ()		const	{ return _viewType; }

		ND_ auto		ReleaseImage ()			{ return Strong<ImageID>{ _imageId.Release() }; }
		ND_ auto		ReleaseImageAndView () -> StrongImageAndViewID;
	};


} // AE::Graphics
