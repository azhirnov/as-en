// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_KTX
# include "res_loaders/Public/ImageSaver.h"

namespace AE::ResLoader
{

	//
	// KTX Image Saver
	//

	class KTXImageSaver final : public IImageSaver
	{
	// methods
	public:
		bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool flipY = False{}) __NE_OV;
		using IImageSaver::SaveImage;
	};


} // AE::ResLoader

#endif // AE_ENABLE_KTX
