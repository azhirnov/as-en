// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AllImages/AllImageLoaders.h"
#include "res_loaders/DevIL/DevILLoader.h"
#include "res_loaders/STB/STBImageLoader.h"
#include "res_loaders/DDS/DDSImageLoader.h"
#include "res_loaders/KTX/KTXImageLoader.h"
#include "res_loaders/AE/AEImageLoader.h"

namespace AE::ResLoader
{

/*
=================================================
	LoadImage
=================================================
*/
	bool  AllImageLoaders::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE___
	{
		// multithreaded
		if ( fileFormat == Default or fileFormat == EImageFormat::DDS )
		{
			DDSImageLoader	loader;
			if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
				return true;
		}

		// multithreaded
		if ( fileFormat == Default or fileFormat == EImageFormat::AEImg )
		{
			AEImageLoader	loader;
			if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
				return true;
		}

		// multithreaded
		#ifdef AE_ENABLE_STB
		{
			STBImageLoader	loader;
			if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
				return true;
		}
		#endif

		// multithreaded
		#ifdef AE_ENABLE_KTX
		if ( fileFormat == Default or fileFormat == EImageFormat::KTX )
		{
			KTXImageLoader	loader;
			if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
				return true;
		}
		#endif

		// DevIL is single threaded set low priority
		#ifdef AE_ENABLE_DEVIL
		{
			DevILLoader		loader;
			if ( loader.LoadImage( image, stream, flipY, allocator, fileFormat ))
				return true;
		}
		#endif

		return false;
	}


} // AE::ResLoader
