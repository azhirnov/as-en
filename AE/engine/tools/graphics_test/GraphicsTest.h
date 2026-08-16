// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/ImageMemView.h"
#include "res_loaders/Intermediate/IntermImage.h"

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
		Mutex						_guard;
		RC<WStream>					_dstFile;
		Function< RC<WStream>() >	_openDiffFile;
		Path						_fname;
		bool						_loaded		= false;
		bool						_newRef		= false;
		ResLoader::IntermImage		_image;


	// methods
	public:
		ImageComparator () {}
		~ImageComparator ();

			bool  LoadReference (RC<RStream> imgFile, Path imgName);
			bool  LoadReference (RC<RStream> imgFile, Path imgName, Function< RC<WStream>() > openDiffFile);

			void  Reset (RC<WStream> imgFile, Path imgName);
			void  Reset (RC<WStream> imgFile, Path imgName, Function< RC<WStream>() > openDiffFile);

		ND_ bool  Compare (const ImageMemView &view);

		ND_ static bool  Diff (const ImageMemView &originImage, const ImageMemView &newImage, OUT ResLoader::IntermImage &diff);
		ND_ static bool  Diff (const ImageMemView &originImage, const ImageMemView &newImage,
								const Function< RC<WStream>() > &openDiffFile, OUT bool &hasDiff);
	};


} // AE::GraphicsTest
