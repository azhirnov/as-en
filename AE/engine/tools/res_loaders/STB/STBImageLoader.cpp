// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_STB
# include "base/Defines/StdInclude.h"

# ifdef AE_COMPILER_MSVC
#  pragma warning (push, 0)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wcast-qual"
#	pragma clang diagnostic ignored "-Wcast-align"
#	pragma clang diagnostic ignored "-Wdouble-promotion"
#	pragma clang diagnostic ignored "-Wmissing-field-initializers"
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wcast-qual"
#	pragma GCC diagnostic ignored "-Wdouble-promotion"
# endif

# define STB_IMAGE_IMPLEMENTATION
# include "stb_image.h"

# ifdef AE_COMPILER_MSVC
#  pragma warning (pop)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
# endif

# include "res_loaders/STB/STBImageLoader.h"
# include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
	using namespace AE::Graphics;

/*
=================================================
	LoadImage
=================================================
*/
	bool  STBImageLoader::LoadImage (INOUT IntermImage &image, RStream &stream, Bool flipY, RC<IAllocator> allocator, EImageFormat fileFormat) __NE___
	{
		Unused( fileFormat );

		stbi__context		ctx;
		stbi__result_info	ri;
		int					x, y, comp;		// 'comp' - origin number of channels
		const int			req_comp = 4;	// TODO: 0 - auto, but 3 channel images are not supported, 1 channel will be converted to 4 channel which is bad.
		Array<ubyte>		temp_data;

		if ( auto* mstream = DynCast<MemRefRStream>( &stream ))
		{
			stbi__start_mem( OUT &ctx, mstream->GetData().data(), int(mstream->GetData().size()) );
		}
		else
		{
			CHECK_ERR( stream.Read( stream.RemainingSize(), OUT temp_data ));
			stbi__start_mem( OUT &ctx, temp_data.data(), int(temp_data.size()) );
		}

		void*	data_ptr = stbi__load_main( &ctx, OUT &x, OUT &y, OUT &comp, req_comp, OUT &ri, 8 );
		if ( data_ptr == null )
			return false;

		//ASSERT( comp != 1 );	// TODO: optimize for 1 channel
		comp = req_comp;

		if ( (stbi__vertically_flip_on_load) or flipY )
			stbi__vertical_flip( data_ptr, x, y, comp * ri.bits_per_channel );

		if ( not allocator )
			allocator = AE::GetDefaultAllocator();

		IntermImage::Mipmaps_t	image_data;
		image_data.resize( 1 );
		image_data[0].resize( 1 );

		IntermImage::Level&		image_level = image_data[0][0];
		image_level.dimension	= uint3{ x, y, 1 };
		image_level.mipmap		= 0_mipmap;
		image_level.layer		= 0_layer;
		image_level.rowPitch	= Bytes{uint( comp * x )};
		image_level.slicePitch	= image_level.rowPitch * y;

		// ri.bits_per_channel:	8, 16
		// comp:				1, 2, 3, 4
		// ri.channel_order:	STBI_ORDER_RGB
		CHECK_ERR( comp >= 1 and comp <= 4 );
		CHECK_ERR( ri.bits_per_channel == 8 or ri.bits_per_channel == 16 );

		switch ( comp ) {
			case 1 :	image_level.format = (ri.bits_per_channel == 8 ? EPixelFormat::R8_UNorm		: EPixelFormat::R16_UNorm);		break;
			case 2 :	image_level.format = (ri.bits_per_channel == 8 ? EPixelFormat::RG8_UNorm	: EPixelFormat::RG16_UNorm);	break;
			case 3 :	image_level.format = (ri.bits_per_channel == 8 ? EPixelFormat::RGB8_UNorm	: EPixelFormat::RGB16_UNorm);	break;
			case 4 :	image_level.format = (ri.bits_per_channel == 8 ? EPixelFormat::RGBA8_UNorm	: EPixelFormat::RGBA16_UNorm);	break;
		}

		bool	result = false;
		if ( image_level.SetPixelData( SharedMem::Create( RVRef(allocator), image_level.slicePitch * image_level.dimension.z )))
		{
			MemCopy( OUT image_level.PixelData(), data_ptr, image_level.DataSize() );
			result = true;
		}
		stbi_image_free( data_ptr );

		CHECK_ERR( image.SetData( RVRef(image_data), EImage_2D ));
		return result;
	}


} // AE::ResLoader

#endif // AE_ENABLE_STB
