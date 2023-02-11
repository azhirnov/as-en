// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_DEVIL
# include "res_loaders/ImageSaver.h"

namespace AE::ResLoader
{

	//
	// DevIL Image Saver
	//

	class DevILSaver final : public IImageSaver
	{
	// methods
	public:
		bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat) __NE_OV;
		using IImageSaver::SaveImage;
	};


} // AE::ResLoader

#endif // AE_ENABLE_DEVIL
