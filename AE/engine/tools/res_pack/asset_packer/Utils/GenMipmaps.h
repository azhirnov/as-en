// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::AssetPacker
{

	struct MipMapGenerator
	{
		ND_ static bool  Gen (const ResLoader::IntermImage &srcImage, OUT ResLoader::IntermImage &dstImage) __NE___;
	};

} // AE::AssetPacker
