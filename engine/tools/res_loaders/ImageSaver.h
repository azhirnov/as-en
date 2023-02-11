// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: yes
*/

#pragma once

#include "base/Utils/FileSystem.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{

	//
	// Image Saver interface
	//

	class IImageSaver
	{
	// methods
	public:
		bool  SaveImage (const Path &filename, const IntermImage &image, EImageFormat fileFormat = Default)		__NE___;

		virtual bool  SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat = Default)	__NE___ = 0;
	};


} // AE::ResLoader
