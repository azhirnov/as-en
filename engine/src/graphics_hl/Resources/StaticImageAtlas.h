// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/GraphicsCreateInfo.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{
	class ITransferContext;


	//
	// Static Image Atlas
	//

	class StaticImageAtlas final : public EnableRC<StaticImageAtlas>
	{
	// types
	public:
		using Rect_t		= AssetPacker::ImageAtlasPacker::Rect_t;
		using ImageMap_t	= AssetPacker::ImageAtlasPacker::ImageMap_t;
		using ImageRects_t	= AssetPacker::ImageAtlasPacker::ImageRects_t;

		struct AsyncLoader {
		///	ND_ Promise<RC<StaticImageAtlas>>  Load (RC<AsyncRStream> stream);
		};
		
		struct Loader {
			ND_ RC<StaticImageAtlas>  Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc = Default);
		};


	// variables
	private:
		StrongImageAndViewID	_imageAndView;
		float2					_invImgSize		{1};
		ImageMap_t				_nameToIdx;
		ImageRects_t			_imageRects;


	// methods
	public:
		StaticImageAtlas () {}
		~StaticImageAtlas ();

		bool  Get (const ImageInAtlasName &name, OUT RectI &region) const;
		bool  Get (const ImageInAtlasName &name, OUT RectF &region) const;

		ND_ ImageID			GetImageID ()		const	{ return _imageAndView.image; }
		ND_ ImageViewID		GetViewID ()		const	{ return _imageAndView.view; }
		ND_ ImageAndViewID	GetImageAndView ()	const	{ return _imageAndView; }
	};


} // AE::Graphics
