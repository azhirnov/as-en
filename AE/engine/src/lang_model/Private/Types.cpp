// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "lang_model/Public/Types.h"

namespace AE::LangModel
{

/*
=================================================
	ImageStorage
=================================================
*/
	namespace {
		static const Bytes		c_ImgStOffset = 8_b;
	}

	ImageStorage::~ImageStorage () __NE___
	{
		Deallocate();
	}


	bool  ImageStorage::Allocate () __NE___
	{
		Deallocate();

		CHECK_ERR( dim.x != 0 and dim.y != 0 );

		if ( rowPitch == 0 )
		{
			Bytes	bpp;
			switch ( format )
			{
				case EPixelFormat::RGBA8_UNorm :	bpp = 4_b;	break;
				case EPixelFormat::RGB8_UNorm :		bpp = 3_b;	break;
				case EPixelFormat::RG8_UNorm :		bpp = 2_b;	break;
				case EPixelFormat::R8_UNorm :		bpp = 1_b;	break;
				default :							RETURN_ERR( "unsupported format" );
			}
			rowPitch = dim.x * bpp;
		}

		size	= rowPitch * dim.y;
		_size	= size + c_ImgStOffset;

		_ptr	= UntypedAllocator::Allocate( size );
		CHECK_ERR( _ptr != null );

		pixels	= _ptr + c_ImgStOffset;
		return true;
	}


	void  ImageStorage::Deallocate () __NE___
	{
		if ( _ptr == null )
			return;

		UntypedAllocator::Deallocate( _ptr, _size );
		_size	= 0_b;
		_ptr	= null;
		pixels	= null;
	}


	bool  ImageStorage::IsValid () C_NE___
	{
		return	_ptr + c_ImgStOffset	== pixels	and
				_size - c_ImgStOffset	== size		and
				dim.y * rowPitch		== size;
	}

} // AE::LangModel
