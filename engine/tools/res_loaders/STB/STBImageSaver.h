// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_STB
# include "res_loaders/ImageSaver.h"

namespace AE::ResLoader
{

	//
	// STB Image Saver
	//

	class STBImageSaver final : public IImageSaver
	{
	// methods
	public:
		bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY = False{}) __NE_OV;
		using IImageSaver::SaveImage;
	};


} // AE::ResLoader

#endif // AE_ENABLE_STB
