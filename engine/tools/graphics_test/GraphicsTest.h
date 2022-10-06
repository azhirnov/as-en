// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageMemView.h"
//#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::GraphicsTest
{
	using namespace AE::Graphics;


	//
	// Image Comparator
	//

	class ImageComparator
	{
	// methods
	public:
		ImageComparator () {}
		~ImageComparator ();

			bool  LoadReference (Path imgName);
		ND_ bool  Compare (const ImageMemView &view);
	};


} // AE::GraphicsTest
