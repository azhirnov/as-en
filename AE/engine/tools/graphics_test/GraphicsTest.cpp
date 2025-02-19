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
				CHECK( saver.SaveImage( *_dstFile, _image, EImageFormat::PNG ));
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
		return LoadReference( RVRef(imgFile), RVRef(imgName), {} );
	}

	bool  ImageComparator::LoadReference (RC<RStream> imgFile, Path imgName, Function< RC<WStream>() > openDiffFile)
	{
		ASSERT( not imgName.empty() );

		_fname		= RVRef(imgName);
		_loaded		= false;
		_newRef		= false;
		_dstFile	= null;
		_openDiffFile = RVRef(openDiffFile);

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
		return Reset( RVRef(imgFile), RVRef(imgName), Default );
	}

	void  ImageComparator::Reset (RC<WStream> imgFile, Path imgName, Function< RC<WStream>() > openDiffFile)
	{
		ASSERT( not imgName.empty() );

		_dstFile	= RVRef(imgFile);
		_fname		= RVRef(imgName);
		_loaded		= false;
		_newRef		= false;
		_openDiffFile = RVRef(openDiffFile);
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
			bool	has_diff;
			CHECK_ERR( Diff( _image.ToView(), view, _openDiffFile, OUT has_diff ));

			CHECK_ERR_MSG( not has_diff,
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

/*
=================================================
	Diff
=================================================
*/
	bool  ImageComparator::Diff (const ImageMemView &originImage, const ImageMemView &newImage, OUT ResLoader::IntermImage &diff)
	{
		CHECK_ERR( diff.IsEmpty() );
		CHECK_ERR( All( originImage.Dimension() == newImage.Dimension() ));
		CHECK_ERR( originImage.Format() == newImage.Format() );
		CHECK_ERR( originImage.Format() == EPixelFormat::RGBA8_UNorm );	// only RGBA8 format is supported

		if_likely( originImage == newImage )
			return true;

		const uint2		dim		= uint2{originImage.Dimension()};
		const Bytes		bpp		= 4_b;	// RGBA8
		const uint		border	= 4;	// px

		CHECK_ERR( diff.Allocate( EImage_2D, originImage.Format(), uint3{ dim.x*2 + border, dim.y, 1u } ));
		diff.ZeroMem();

		ImageMemView	dst_view = diff.ToView();

		for (uint y = 0; y < dim.y; ++y)
		{
			auto	lhs	= originImage.GetRow( y );
			auto	rhs	= newImage.GetRow( y );
			auto	dst = dst_view.GetRow( y );
			CHECK_ERR( lhs.size == rhs.size );

			// fill border with white color
			std::memset( OUT dst.ptr + dim.x * bpp, 0xFF, usize{bpp*border} );

			if_likely( MemEqual( lhs.ptr, rhs.ptr, lhs.size ))
				continue;

			// fill diff pixels
			for (uint x = 0; x < dim.x; ++x)
			{
				RGBA8u	c0	= BitCastPtr< RGBA8u >( lhs.ptr + x * bpp );
				RGBA8u	c1	= BitCastPtr< RGBA8u >( rhs.ptr + x * bpp );

				if_likely( c0 == c1 )
					continue;

				RGBA8u	c2	= RGBA8u{ Abs( RGBA32f{c0} - RGBA32f{c1} )};			c2.a = 255;
				RGBA8u	c3	= RGBA8u{ Abs( RGBA32f{c0} - RGBA32f{c1} ) * 10.f };	c3.a = 255;

				MemCopy( OUT dst.ptr + x * bpp, &c2, Sizeof(c2) );
				MemCopy( OUT dst.ptr + (dim.x + border + x) * bpp, &c3, Sizeof(c3) );
			}
		}

		return true;
	}

/*
=================================================
	Diff
=================================================
*/
	bool  ImageComparator::Diff (const ImageMemView &originImage, const ImageMemView &newImage, const Function< RC<WStream>() > &openDiffFile, OUT bool &hasDiff)
	{
		hasDiff = false;

		IntermImage		interm;
		CHECK_ERR( Diff( originImage, newImage, OUT interm ));

		if ( interm.IsEmpty() )
			return true;

		hasDiff = true;
		if ( not openDiffFile )
			return true;

		auto	diff_file = openDiffFile();
		CHECK_ERR( diff_file );

		STBImageSaver	saver;
		CHECK_ERR( saver.SaveImage( *diff_file, interm, EImageFormat::PNG ));

		return true;
	}

} // AE::GraphicsTest
