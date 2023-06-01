// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/IImageLoader.h"

namespace AE::ResLoader
{
	
	//
	// All Image Loaders
	//

	class AllImageLoaders final : public IImageLoader
	{
	// methods
	public:
		bool  LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE_OV;
		using IImageLoader::LoadImage;
	};


} // AE::ResLoader