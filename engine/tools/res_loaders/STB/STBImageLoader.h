// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_STB
#include "res_loaders/IImageLoader.h"

namespace AE::ResLoader
{
	
	//
	// STB image Loader
	//

	class STBImageLoader final : public IImageLoader
	{
	// methods
	public:
		bool  LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, Allocator_t allocator, EImageFormat fileFormat) __NE_OV;
		using IImageLoader::LoadImage;
	};


} // AE::ResLoader

#endif // AE_ENABLE_STB
