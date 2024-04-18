// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_KTX
# include "res_loaders/Public/ImageLoader.h"

namespace AE::ResLoader
{

	//
	// KTX image Loader
	//

	class KTXImageLoader final : public IImageLoader
	{
	// methods
	public:
		bool  LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE_OV;
		using IImageLoader::LoadImage;
	};


} // AE::ResLoader

#endif // AE_ENABLE_KTX
