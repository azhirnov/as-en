// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/File.h"
#include "GraphicsTest.h"
#include "res_loaders/STB/STBImageLoader.h"
#include "res_loaders/STB/STBImageSaver.h"

namespace AE::GraphicsTest
{
	using namespace AE::ResLoader;


/*
=================================================
	destructor
=================================================
*/
	ImageComparator::~ImageComparator ()
	{
		EXLOCK( _guard );
		if ( _newRef )
		{
			ASSERT( _dstFile );
			if ( _dstFile )
			{
				STBImageSaver	saver;
				saver.SaveImage( *_dstFile, _image, EImageFormat::PNG );
			}
		}
	}

/*
=================================================
	LoadReference
=================================================
*/
	bool  ImageComparator::LoadReference (RC<RStream> imgFile, Path imgName)
	{
		_fname		= RVRef(imgName);
		_loaded		= false;
		_newRef		= false;
		_dstFile	= null;

		if ( imgFile and imgFile->IsOpen() )
		{
			STBImageLoader	loader;
			_loaded = loader.LoadImage( OUT _image, *imgFile, False{"don't flipY"}, null, Default );

			if ( _loaded )
			{
				CHECK_ERR( _image.MipLevels() == 1 );
				CHECK_ERR( _image.ArrayLayers() == 1 );
			}
		}
		return _loaded;
	}

/*
=================================================
	Reset
=================================================
*/
	void  ImageComparator::Reset (RC<WStream> imgFile, Path imgName)
	{
		_dstFile	= RVRef(imgFile);
		_fname		= RVRef(imgName);
		_loaded		= false;
		_newRef		= false;
	}

/*
=================================================
	Compare
=================================================
*/
	bool  ImageComparator::Compare (const ImageMemView &view)
	{
		EXLOCK( _guard );

		if ( _loaded )
		{
			CHECK_ERR_MSG( view == _image.ToView( 0_mipmap, 0_layer ),
				"Image is not equal to reference '"s << ToString(_fname.filename()) << "'" );
		}
		else
		{
			_newRef	= true;
			_loaded	= true;
			CHECK_ERR( _image.Copy( view ));
			CHECK_ERR( _dstFile );
		}
		return true;
	}

} // AE::GraphicsTest
