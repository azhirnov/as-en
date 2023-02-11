// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/ImageSaver.h"

namespace AE::ResLoader
{

	//
	// DDS Image Saver
	//

	class DDSSaver final : public IImageSaver
	{
	// methods
	public:
		bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat) __NE_OV;
		using IImageSaver::SaveImage;
	};


} // AE::ResLoader
