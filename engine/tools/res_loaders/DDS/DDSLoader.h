// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/IImageLoader.h"

namespace AE::ResLoader
{
	
	//
	// DDS Loader
	//

	class DDSLoader final : public IImageLoader
	{
	// methods
	public:
		bool  LoadImage (INOUT IntermImage &image, RStream &stream, bool flipY, Allocator_t allocator) override;
		using IImageLoader::LoadImage;
	};


} // AE::ResLoader
