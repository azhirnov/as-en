// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageMemView.h"

namespace AE::GraphicsTest
{
	using namespace AE::Graphics;


	//
	// Image Comparator
	//

	class ImageComparator
	{
	// variables
	private:
		Path					_fname;
		bool					_loaded		= false;
		bool					_newRef		= false;
	//	ResLoader::IntermImage	_image;


	// methods
	public:
		ImageComparator () {}
		~ImageComparator ();

			bool  LoadReference (Path imgName);
		ND_ bool  Compare (const ImageMemView &view);
	};


} // AE::GraphicsTest
