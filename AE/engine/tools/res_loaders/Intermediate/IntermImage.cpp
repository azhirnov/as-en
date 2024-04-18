// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::ResLoader
{
	using namespace AE::Graphics;

/*
=================================================
	Level::operator ==
=================================================
*/
	bool  IntermImage::Level::operator == (const Level &rhs) C_NE___
	{
		// ignore 'pixels', '_storage'
		return	All( dimension	== rhs.dimension )	and
				format			== rhs.format		and
				layer			== rhs.layer		and
				mipmap			== rhs.mipmap		and
				rowPitch		== rhs.rowPitch		and
				slicePitch		== rhs.slicePitch;
	}

/*
=================================================
	Level::SetPixelData
=================================================
*/
	bool  IntermImage::Level::SetPixelData (RC<SharedMem> storage) __NE___
	{
		void*	ptr		= storage ? storage->Data() : null;
		Bytes	size	= storage ? storage->Size() : 0_b;

		return SetPixelData( RVRef(storage), ptr, size );
	}

	bool  IntermImage::Level::SetPixelData (RC<SharedMem> storage, void* pixels) __NE___
	{
		return SetPixelData( RVRef(storage), pixels, DataSize() );
	}

	bool  IntermImage::Level::SetPixelData (RC<SharedMem> storage, void* pixels, Bytes size) __NE___
	{
		CHECK_ERR( storage and pixels != null );
		CHECK_ERR( storage->Contains( pixels, size ));
		CHECK_ERR( size == DataSize() );

		_storage	= RVRef(storage);
		_pixels		= pixels;

		return true;
	}

/*
=================================================
	Level::SetPixelDataRef
=================================================
*/
	bool  IntermImage::Level::SetPixelDataRef (void* pixels) __NE___
	{
		return SetPixelDataRef( pixels, DataSize() );
	}

	bool  IntermImage::Level::SetPixelDataRef (void* pixels, Bytes size) __NE___
	{
		CHECK_ERR( size == DataSize() );

		_storage	= null;
		_pixels		= pixels;

		return true;
	}

/*
=================================================
	Level::Pixels
=================================================
*/
	ArrayView<ubyte>  IntermImage::Level::Pixels () C_NE___
	{
		return not Empty() ?
				ArrayView<ubyte>{ Cast<ubyte>(PixelData()), usize(DataSize()) } :
				Default;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	IntermImage::IntermImage (IntermImage &&other) __NE___ :
		_srcPath{ RVRef(other._srcPath) },
		_data{ RVRef(other._data) },
		_imageType{ other._imageType },
		_immutable{ other._immutable }
	{}

/*
=================================================
	operator =
=================================================
*/
	IntermImage&  IntermImage::operator = (IntermImage &&rhs) __NE___
	{
		_srcPath	= RVRef(rhs._srcPath);
		_data		= RVRef(rhs._data);
		_imageType	= rhs._imageType;
		_immutable	= rhs._immutable;
		return *this;
	}

/*
=================================================
	SetData
=================================================
*/
	bool  IntermImage::SetData (Mipmaps_t data, EImage type) __NE___
	{
		CHECK_ERR( IsMutable() );

		_data		= RVRef(data);
		_imageType	= type;

		return true;
	}

/*
=================================================
	SetData
=================================================
*/
	bool  IntermImage::SetData (const ImageMemView &view, RC<SharedMem> storage) __NE___
	{
		CHECK_ERR( IsMutable() );
		CHECK_ERR( view.Parts().size() == 1 );

		if ( storage )
		{
			auto&	part = view.Parts().front();
			CHECK_ERR( storage->Contains( part.ptr, part.size ));
		}

		_imageType = view.Dimension().z > 1 ? EImage_3D :
					 view.Dimension().y > 1 ? EImage_2D :
											  EImage_1D;

		_data.resize( 1 );
		_data[0].resize( 1 );

		auto&	level	= _data[0][0];
		level.dimension		= Max( 1u, view.Dimension() );
		level.format		= view.Format();
		level.layer			= 0_layer;
		level.mipmap		= 0_mipmap;
		level.rowPitch		= view.RowPitch();
		level.slicePitch	= view.SlicePitch();

		if ( storage )
			CHECK_ERR( level.SetPixelData( RVRef(storage), const_cast<void*>(view.Parts().front().ptr) ))
		else
			CHECK_ERR( level.SetPixelDataRef( const_cast<void*>(view.Parts().front().ptr) ))

		return true;
	}

/*
=================================================
	Copy
=================================================
*/
	bool  IntermImage::Copy (const ImageMemView &memView) __NE___
	{
		return Copy( memView, null );
	}

	bool  IntermImage::Copy (const ImageMemView &memView, RC<IAllocator> allocator) __NE___
	{
		CHECK_ERR( IsMutable() );

		if ( not allocator )
			allocator = AE::GetDefaultAllocator();

		auto	storage = SharedMem::Create( RVRef(allocator), memView.SlicePitch() * memView.Dimension().z );
		CHECK_ERR( storage );

		Bytes	offset = 0_b;
		for (auto& part : memView.Parts())
		{
			Bytes	size = part.size;
			MemCopy( OUT storage->Data() + offset, part.ptr, size );
			offset += size;
		}

		ASSERT( offset == storage->Size() );

		ImageMemView	view{ storage->Data(), storage->Size(), uint3{}, memView.Dimension(), memView.RowPitch(), memView.SlicePitch(), memView.Format(), memView.Aspect() };
		return SetData( view, RVRef(storage) );
	}

/*
=================================================
	GetImageDim
=================================================
*/
	EImageDim  IntermImage::GetImageDim () C_NE___
	{
		switch ( _imageType )
		{
			case EImage_1D :
			case EImage_1DArray :	return EImageDim_1D;
			case EImage_2D :
			case EImage_2DArray :
			case EImage_Cube :
			case EImage_CubeArray :	return EImageDim_2D;
			case EImage_3D :		return EImageDim_3D;
		}
		return Default;
	}

/*
=================================================
	ToView
=================================================
*/
	ImageMemView  IntermImage::ToView (MipmapLevel mipmap, ImageLayer layer) __NE___
	{
		CHECK_ERR( IsMutable() );
		CHECK_ERR( mipmap.Get() < _data.size() );
		CHECK_ERR( layer.Get() < _data[mipmap.Get()].size() );

		auto&	img = _data [mipmap.Get()] [layer.Get()];

		return ImageMemView{ img.PixelData(), img.DataSize(), uint3{}, img.dimension, img.rowPitch, img.slicePitch, img.format, EImageAspect::Color };
	}

/*
=================================================
	IsValid
=================================================
*/
	bool  IntermImage::IsValid () C_NE___
	{
		return	GetData().size() > 0	and
				GetData()[0].size() > 0;
	}

/*
=================================================
	Reserve
=================================================
*/
	bool  IntermImage::Reserve (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps) __NE___
	{
		CHECK_ERR( IsMutable() );
		CHECK_ERR( layers.Get() > 0 );
		CHECK_ERR( mipmaps.Get() > 0 );
		CHECK_ERR( All( dim > uint3{0} ));

		mipmaps = MipmapLevel{ Min( mipmaps.Get(), ImageUtils::NumberOfMipmaps( dim ))};

		switch ( type )
		{
			case EImage_1D :
				CHECK_ERR( layers.Get() == 1 );
				CHECK_ERR( dim.y == 1 and dim.z == 1 );
				break;
			case EImage_1DArray :
				break;
			case EImage_2D :
				CHECK_ERR( layers.Get() == 1 );
				CHECK_ERR( dim.z == 1 );
				break;
			case EImage_2DArray :
				CHECK_ERR( dim.z == 1 );
				break;
			case EImage_Cube :
				CHECK_ERR( layers.Get() == 6 );
				CHECK_ERR( dim.z == 1 );
				break;
			case EImage_CubeArray :
				CHECK_ERR( IsMultipleOf( layers.Get(), 6 ));
				CHECK_ERR( dim.z == 1 );
				break;
			case EImage_3D :
				CHECK_ERR( layers.Get() == 1 );
				break;
			default :
				return false;
		}

		auto&	fmt_info = EPixelFormat_GetInfo( fmt );
		CHECK_ERR( fmt_info.IsValid() );

		NOTHROW_ERR( _data.resize( mipmaps.Get() ));

		for (usize mip = 0; mip < _data.size(); ++mip)
		{
			NOTHROW_ERR( _data[mip].resize( layers.Get() ));

			for (uint layer = 0; layer < layers.Get(); ++layer)
			{
				auto&	img = _data[mip][layer];
				img.dimension	= ImageUtils::MipmapDimension( dim, mip, fmt_info.TexBlockDim() );
				img.format		= fmt;
				img.layer		= ImageLayer{layer};
				img.mipmap		= MipmapLevel{mip};
				img.rowPitch	= ImageUtils::RowSize( img.dimension.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
				img.slicePitch	= ImageUtils::SliceSize( img.dimension.y, img.rowPitch, fmt_info.TexBlockDim() );
			}
		}

		_srcPath.clear();
		_imageType	= type;
		_immutable	= false;

		return true;
	}

/*
=================================================
	Allocate
=================================================
*/
	bool  IntermImage::Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps, RC<IAllocator> allocator) __NE___
	{
		if ( not allocator )
			allocator = AE::GetDefaultAllocator();

		CHECK_ERR( Reserve( type, fmt, dim, layers, mipmaps ));

		auto&	fmt_info = EPixelFormat_GetInfo( fmt );
		CHECK_ERR( fmt_info.IsValid() );

		for (ArrayLayers_t& arr_layers : _data)
		{
			for (Level& img : arr_layers)
			{
				auto	storage = SharedMem::Create( allocator, img.slicePitch * img.dimension.z );
				CHECK_ERR( storage );
				CHECK_ERR( img.SetPixelData( RVRef(storage) ));
			}
		}
		return true;
	}

	bool  IntermImage::Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, RC<IAllocator> allocator) __NE___
	{
		return Allocate( type, fmt, dim, ImageLayer{1u}, MipmapLevel{1u}, RVRef(allocator) );
	}

	bool  IntermImage::Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps) __NE___
	{
		return Allocate( type, fmt, dim, layers, mipmaps, null );
	}

	bool  IntermImage::Allocate (EImage type, EPixelFormat fmt, const uint3 &dim) __NE___
	{
		return Allocate( type, fmt, dim, ImageLayer{1u}, MipmapLevel{1u} );
	}

/*
=================================================
	GetLevel
=================================================
*/
	IntermImage::Level const*  IntermImage::GetLevel (MipmapLevel mipmap, ImageLayer layer) C_NE___
	{
		CHECK_ERR( mipmap.Get() < _data.size() );
		CHECK_ERR( layer.Get() < _data[mipmap.Get()].size() );

		return &_data [mipmap.Get()] [layer.Get()];
	}


} // AE::ResLoader
