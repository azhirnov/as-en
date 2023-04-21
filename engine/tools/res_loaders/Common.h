// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::ResLoader
{
	using namespace AE::Base;
	using namespace AE::Graphics;


	enum class EImageFormat : uint
	{
		Unknown			= 0,
		DDS,			// .dds
		BMP,			// .bmp
		JPG,			// .jpg .jpe .jpeg
		PCX,			// .pcx
		PNG,			// .png
		TGA,			// .tga
		TIF,			// .tif .tiff
		PSD,			// .psd
		RadianceHDR,	// .hdr
		OpenEXR,		// .exr
		KTX,			// .ktx
		_Count
	};
	
	ND_ EImageFormat	PathToImageFileFormat (const Path &path)	__NE___;
	ND_ StringView		ImageFileFormatToExt (EImageFormat)			__NE___;


} // AE::ResLoader
