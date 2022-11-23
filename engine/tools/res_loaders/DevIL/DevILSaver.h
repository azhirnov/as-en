// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_DEVIL

#include "res_loaders/ImageSaver.h"

namespace AE::ResLoader
{

	//
	// DevIL Image Saver
	//

	class DevILSaver final : public IImageSaver
	{
	// types
	public:
		enum class EImageFormat
		{
			BMP,
			JPG,
			PCX,
			PNG,
			PNM,
			RAW,
			SGI,
			TGA,
		};


	// variables
	private:
		EImageFormat	_format		= EImageFormat::PNG;


	// methods
	public:
		bool  SaveImage (WStream &stream, const IntermImage &image) override;
		using IImageSaver::SaveImage;

		void  SetFormat (EImageFormat fmt)	{ _format = fmt; }
	};


} // AE::ResLoader

#endif // AE_ENABLE_DEVIL
