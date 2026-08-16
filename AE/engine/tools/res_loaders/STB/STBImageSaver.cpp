// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_STB
# include "base/Defines/StdInclude.h"

# ifdef AE_COMPILER_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4996)
#  pragma warning (disable: 5039)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wcast-qual"
#	pragma clang diagnostic ignored "-Wcast-align"
#	pragma clang diagnostic ignored "-Wdouble-promotion"
#	pragma clang diagnostic ignored "-Wmissing-field-initializers"
#	pragma clang diagnostic ignored "-Wdeprecated-declarations"
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wcast-qual"
#	pragma GCC diagnostic ignored "-Wdouble-promotion"
#	pragma GCC diagnostic ignored "-Wmissing-field-initializers"
# endif

# define STB_IMAGE_WRITE_IMPLEMENTATION
# include "stb_image_write.h"

# ifdef AE_COMPILER_MSVC
#  pragma warning (pop)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
# endif

# include "res_loaders/STB/STBImageSaver.h"
# include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
	using namespace AE::Graphics;

namespace
{
	static void  StbiWriteFn (void* context, void* data, int size) __NE___
	{
		WStream&	stream = *Cast<WStream>(context);
		Unused( stream.WriteSeq( data, Bytes{uint(size)} ));
	}
}

/*
=================================================
	SaveImage
=================================================
*/
	bool  STBImageSaver::SaveImage (WStream &stream, const IntermImage &image, EImageFormat fileFormat, Bool) __NE___
	{
		CHECK_ERR( image.GetData().size() == 1 );
		CHECK_ERR( image.GetData()[0].size() == 1 );

		const auto&		level		= image.GetData()[0][0];
		const auto&		fmt_info	= EPixelFormat_GetInfo( level.format );
		const int		comp		= fmt_info.channels;
		const void*		data_ptr	= level.PixelData();
		const int		stride		= int(level.rowPitch);
		const int		jpg_quality	= 90;
		const int		w			= int(level.dimension.x);
		const int		h			= int(level.dimension.y);

		CHECK_ERR( level.dimension.z == 1 );
		CHECK_ERR( fmt_info.BitsPerChannel() == 8 );

		//if ( flipY )
		//	stbi__vertical_flip( data_ptr, w, y, comp * 8 );

		Array<ubyte>	storage;

		switch ( image.PixelFormat() )
		{
			case EPixelFormat::RGBA8_UNorm :
			case EPixelFormat::sRGB8_A8 :
				break;	// ok

			case EPixelFormat::BGRA8_UNorm :
			case EPixelFormat::sBGR8_A8 :
			{
				// swap R and B
				NOTHROW_ERR( storage.resize( usize{level.DataSize()} ));

				for (int y = 0; y < h; ++y)
				{
					const ubyte*	src_row		= Cast<ubyte>(level.PixelData()) + y * level.rowPitch;
					ubyte*			dst_row		= storage.data() + y * level.rowPitch;

					for (int x = 0; x < w; ++x)
					{
						int		i	= x * 4;
						ubyte	b	= src_row[i+0];
						ubyte	g	= src_row[i+1];
						ubyte	r	= src_row[i+2];
						ubyte	a	= src_row[i+3];

						dst_row[i+0] = r;
						dst_row[i+1] = g;
						dst_row[i+2] = b;
						dst_row[i+3] = a;
					}
				}
				break;
			}

			default :
				RETURN_ERR( "unsupported format" );
		}

		switch_enum( fileFormat )
		{
			case EImageFormat::PNG :
				CHECK_ERR( stbi_write_png_to_func( &StbiWriteFn, &stream, w, h, comp, data_ptr, stride ) != 0 );
				break;

			case EImageFormat::BMP :
				CHECK_ERR( stbi_write_bmp_to_func( &StbiWriteFn, &stream, w, h, comp, data_ptr ) != 0 );
				break;

			case EImageFormat::TGA :
				CHECK_ERR( stbi_write_tga_to_func( &StbiWriteFn, &stream, w, h, comp, data_ptr ) != 0 );
				break;

			case EImageFormat::RadianceHDR :
				CHECK_ERR( stbi_write_hdr_to_func( &StbiWriteFn, &stream, w, h, comp, Cast<float>(data_ptr) ) != 0 );
				break;

			case EImageFormat::JPG :
				CHECK_ERR( stbi_write_jpg_to_func( &StbiWriteFn, &stream, w, h, comp, data_ptr, jpg_quality ) != 0 );
				break;

			case EImageFormat::DDS :
			case EImageFormat::PCX :
			case EImageFormat::TIF :
			case EImageFormat::PSD :
			case EImageFormat::OpenEXR :
			case EImageFormat::KTX :
			case EImageFormat::AEImg :
			case EImageFormat::Unknown :
			case EImageFormat::_Count :
				break;
		}
		switch_end
		return true;
	}


} // AE::ResLoader

#endif // AE_ENABLE_STB
