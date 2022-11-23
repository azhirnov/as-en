// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{

	//
	// Image Loader interface
	//

	class IImageLoader
	{
	// types
	public:
		using Allocator_t	= SharedMem::Allocator_t;


	// methods
	public:
		virtual bool  LoadImage (INOUT IntermImage &image, RStream &stream, bool flipY = false, Allocator_t allocator = null) = 0;

		bool  LoadImage (INOUT IntermImage &image, ArrayView<Path> directories, bool flipY = false, Allocator_t allocator = null);


	protected:
		static bool  _FindImage (const Path &name, ArrayView<Path> directories, OUT Path &result);
	};


} // AE::ResLoader
