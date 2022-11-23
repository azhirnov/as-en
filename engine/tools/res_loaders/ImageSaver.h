// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		bool  SaveImage (StringView filename, const IntermImage &image);
		bool  SaveImage (const Path &filename, const IntermImage &image);

		virtual bool  SaveImage (WStream &stream, const IntermImage &image) = 0;
	};


} // AE::ResLoader
