// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/MemStream.h"
#include "graphics/Private/EnumUtils.h"

#include "ScriptObjects/ScriptTexture.h"
#include "Packer/ImagePacker.h"

#include "scripting/Impl/ClassBinder.h"

#include "res_loaders/AllImages/AllImageLoaders.h"

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptTexture,	"Texture" );


namespace AE::AssetPacker
{
namespace {
#	include "Packer/ImagePacker.cpp.h"
}

	using namespace AE::Graphics;
	using namespace AE::ResLoader;

/*
=================================================
	constructor
=================================================
*/
	ScriptTexture::ScriptTexture ()
	{}

/*
=================================================
	destructor
=================================================
*/
	ScriptTexture::~ScriptTexture ()
	{}

/*
=================================================
	Alloc*
=================================================
*/
	void  ScriptTexture::Alloc1 (const packed_uint2 &dim, EPixelFormat fmt) __Th___
	{
		return Alloc4( dim, fmt, 1_layer, 1_mipmap );
	}

	void  ScriptTexture::Alloc2 (const packed_uint2 &dim, EPixelFormat fmt, const ImageLayer &layers) __Th___
	{
		return Alloc4( dim, fmt, layers, 1_mipmap );
	}

	void  ScriptTexture::Alloc3 (const packed_uint2 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps) __Th___
	{
		return Alloc4( dim, fmt, 1_layer, mipmaps );
	}

	void  ScriptTexture::Alloc4 (const packed_uint2 &dim, EPixelFormat fmt, const ImageLayer &layers, const MipmapLevel &mipmaps) __Th___
	{
		CHECK_THROW_MSG( not _imgData );

		_imgData.reset( new IntermImage{} );
		CHECK_THROW_MSG( _imgData->Allocate( (layers > 1_layer ? EImage_2DArray : EImage_2D), fmt, uint3{dim, 1}, layers, mipmaps ));

		_dstFormat = _intermFormat = fmt;
	}

	void  ScriptTexture::Alloc5 (const packed_uint3 &dim, EPixelFormat fmt) __Th___
	{
		return Alloc6( dim, fmt, 1_mipmap );
	}

	void  ScriptTexture::Alloc6 (const packed_uint3 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps) __Th___
	{
		CHECK_THROW_MSG( not _imgData );

		_imgData.reset( new IntermImage{} );
		CHECK_THROW_MSG( _imgData->Allocate( EImage_3D, fmt, dim, 1_layer, mipmaps ));

		_dstFormat = _intermFormat = fmt;
	}

	void  ScriptTexture::AllocCube1 (const packed_uint2 &dim, EPixelFormat fmt) __Th___
	{
		return AllocCube2( dim, fmt, 1_mipmap );
	}

	void  ScriptTexture::AllocCube2 (const packed_uint2 &dim, EPixelFormat fmt, const MipmapLevel &mipmaps) __Th___
	{
		CHECK_THROW_MSG( not _imgData );

		_imgData.reset( new IntermImage{} );
		CHECK_THROW_MSG( _imgData->Allocate( EImage::Cube, fmt, uint3{dim, 1}, 6_layer, mipmaps ));

		_dstFormat = _intermFormat = fmt;
	}

/*
=================================================
	_Load
=================================================
*/
	Unique<IntermImage>  ScriptTexture::_Load (const String &imageFile, bool flipY) __Th___
	{
		Path	path = ObjectStorage::Instance()->GetScriptFolder();
		path.append( imageFile );

		Unique<IntermImage>	img{ new IntermImage{ path }};
		AllImageLoaders		loader;

		if ( loader.LoadImage( *img, Default, Bool{flipY} ))
			return img;

		CHECK_THROW_MSG( false, "failed to load image '"s << imageFile << "'" );
	}

	Unique<IntermImage>  ScriptTexture::_Load (const String &imageFile, const RectU &region) __Th___
	{
		Unique<IntermImage>	tmp = _Load( imageFile, false ); // throw

		CHECK_THROW_MSG( tmp->MipLevels() == 1 );
		CHECK_THROW_MSG( tmp->ArrayLayers() == 1 );
		CHECK_THROW_MSG( tmp->Dimension().z == 1 );

		if ( All( region == RectU{uint2(tmp->Dimension())} ))
			return tmp;

		CHECK_THROW_MSG( All( region.Max() <= uint2(tmp->Dimension()) ));

		Unique<IntermImage>	img{ new IntermImage{} };
		CHECK_THROW_MSG( img->Allocate( tmp->GetType(), tmp->PixelFormat(), uint3{region.Size(), 1} ));

		ImageMemView	src{ tmp->ToView() };
		RWImageMemView	dst{ img->ToView() };

		CHECK_THROW_MSG( dst.CopyFrom( uint3{}, uint3{region.Min(), 0}, src, dst.Dimension() ));

		return img;
	}

/*
=================================================
	Load*
=================================================
*/
	void  ScriptTexture::Load1 (const String &imageFile) __Th___
	{
		return Load2( imageFile, false );
	}

	void  ScriptTexture::Load2 (const String &imageFile, bool flipY) __Th___
	{
		CHECK_THROW_MSG( not _imgData );

		_imgData = _Load( imageFile, flipY ); // throw

		_dstFormat = _intermFormat = _imgData->PixelFormat();
	}

	void  ScriptTexture::Load3 (const String &imageFile, const RectU &region) __Th___
	{
		CHECK_THROW_MSG( not _imgData );

		_imgData = _Load( imageFile, region ); // throw

		_dstFormat = _intermFormat = _imgData->PixelFormat();
	}

/*
=================================================
	LoadChannel*
=================================================
*/
	void  ScriptTexture::LoadChannel1 (const String &imageFile, const String &srcSwizzle, const String &dstSwizzle) __Th___
	{
		Unique<IntermImage>	tmp = _Load( imageFile, false );

		if ( not _imgData )
		{
			_imgData   = RVRef(tmp);
			_dstFormat = _intermFormat = _imgData->PixelFormat();
		}
		else
		{
			RWImageMemView	src{ tmp->ToView() };
			RWImageMemView	dst{ _imgData->ToView() };
			const auto		sw  = RWImageMemView::Swizzle{ ImageSwizzle::FromString( srcSwizzle ), ImageSwizzle::FromString( dstSwizzle )};

			CHECK_THROW_MSG( dst.Blit( src, sw ));
		}
	}

/*
=================================================
	AddLayer*
=================================================
*/
	void  ScriptTexture::AddLayer1 (const String &imageFile, uint layer) __Th___
	{
		CHECK_THROW_MSG( _imgData, "image must be preallocated by 'Alloc()'" );

		auto	img = _Load( imageFile, false ); // throw

		_AddLayer( *img, layer ); // throw
	}

	void  ScriptTexture::AddLayer2 (const String &imageFile, uint layer, bool flipY) __Th___
	{
		CHECK_THROW_MSG( _imgData, "image must be preallocated by 'Alloc()'" );

		auto	img = _Load( imageFile, flipY ); // throw

		_AddLayer( *img, layer ); // throw
	}

	void  ScriptTexture::AddLayer3 (const String &imageFile, const RectU &region, uint layer) __Th___
	{
		CHECK_THROW_MSG( _imgData, "image must be preallocated by 'Alloc()'" );

		auto	img = _Load( imageFile, region ); // throw

		_AddLayer( *img, layer ); // throw
	}

	void  ScriptTexture::_AddLayer (IntermImage &img, uint layer) __Th___
	{
		CHECK_THROW_MSG( layer < _imgData->ArrayLayers() );
		CHECK_THROW_MSG( img.MipLevels() == 1 );
		CHECK_THROW_MSG( img.ArrayLayers() == 1 );
		CHECK_THROW_MSG( img.Dimension().z == 1 );
		CHECK_THROW_MSG( All( uint2(img.Dimension()) <= uint2(_imgData->Dimension()) ));

		ImageMemView	src{ img.ToView() };
		RWImageMemView	dst{ _imgData->ToView( ImageLayer{layer} )};

		CHECK_THROW_MSG( dst.Fill( RGBA32f{0.f} ) or dst.Fill( RGBA32u{0} ));

		CHECK_THROW_MSG( dst.CopyFrom( uint3{}, uint3{}, src, Min( src.Dimension(), dst.Dimension() )));
	}

/*
=================================================
	Store
=================================================
*/
	void  ScriptTexture::Store (const String &nameInArchive) __Th___
	{
		CHECK_THROW_MSG( _imgData );

		auto	wmem = MakeRC<ArrayWStream>();
		CHECK_THROW_MSG( _Pack( nameInArchive, wmem ));

		MemRefRStream	rmem {wmem->GetData()};
		ObjectStorage::Instance()->AddToArchive( nameInArchive, rmem, EArchivePackerFileType::Raw ); // throw

		ASSERT( not _imgData );
	}

/*
=================================================
	SetFormat
=================================================
*/
	void  ScriptTexture::SetFormat (EPixelFormat fmt) __Th___
	{
		_dstFormat		= fmt;
		_intermFormat	= EPixelFormat_ToNoncompressed( _dstFormat, false );
	}

/*
=================================================
	_Pack
=================================================
*/
	bool  ScriptTexture::_Pack (const String &, RC<WStream> stream)
	{
		// convert images
		IntermImage		dst_image;
		{
			if ( _dstFormat != _intermFormat )
			{
				RETURN_ERR( "compression is not supported yet" );	// TODO
			}

			CHECK_ERR( dst_image.Reserve( _imgData->GetType(), _intermFormat, _imgData->Dimension(), ImageLayer{_imgData->ArrayLayers()}, MipmapLevel{_imgData->MipLevels()} ));

			auto&	src_img_data	= *_imgData->GetMutableData();
			auto&	dst_img_data	= *dst_image.GetMutableData();
			CHECK_ERR( src_img_data.size() == dst_img_data.size() );

			for (usize mip = 0; mip < src_img_data.size(); ++mip)
			{
				auto&	src_layers = src_img_data[mip];
				auto&	dst_layers = dst_img_data[mip];
				CHECK_ERR( src_layers.size() == dst_layers.size() );

				for (usize layer = 0; layer < src_layers.size(); ++layer)
				{
					auto&	src	= src_layers[layer];
					auto&	dst	= dst_layers[layer];

					if ( src == dst )
					{
						dst = src;
						continue;
					}

					RWImageMemView	dst_view{ dst_image.ToView( MipmapLevel{mip}, ImageLayer{layer} )};
					RWImageMemView	src_view{ _imgData->ToView( MipmapLevel{mip}, ImageLayer{layer} )};

					CHECK_ERR( dst_view.Blit( src_view ));
				}
			}

		}
		_imgData.reset();

		// serialize
		{
			ImagePacker::Header2	img_hdr;
			img_hdr.hdr.dimension	= ushort3{dst_image.Dimension()};
			img_hdr.hdr.arrayLayers	= CheckCast<ushort>(dst_image.ArrayLayers());
			img_hdr.hdr.mipmaps		= CheckCast<ushort>(dst_image.MipLevels());
			img_hdr.hdr.format		= _dstFormat;
			img_hdr.hdr.viewType	= dst_image.GetType();

			CHECK_ERR( ImagePacker_SaveHeader( *stream, img_hdr ));
			CHECK_ERR( ImagePacker_SaveImage( *stream, img_hdr.hdr, dst_image ));
		}
		return true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptTexture::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptTexture>	binder{ se };
		binder.CreateRef();

		binder.AddMethod( &ScriptTexture::Alloc1,		"Alloc",		{"dimension", "format"} );
		binder.AddMethod( &ScriptTexture::Alloc2,		"Alloc",		{"dimension", "format", "layers"} );
		binder.AddMethod( &ScriptTexture::Alloc3,		"Alloc",		{"dimension", "format", "mipmaps"} );
		binder.AddMethod( &ScriptTexture::Alloc4,		"Alloc",		{"dimension", "format", "layers", "mipmaps"} );
		binder.AddMethod( &ScriptTexture::Alloc5,		"Alloc",		{"dimension", "format"} );
		binder.AddMethod( &ScriptTexture::Alloc6,		"Alloc",		{"dimension", "format", "mipmaps"} );
		binder.AddMethod( &ScriptTexture::AllocCube1,	"AllocCube",	{"dimension", "format"} );
		binder.AddMethod( &ScriptTexture::AllocCube2,	"AllocCube",	{"dimension", "format", "mipmaps"} );

		binder.AddMethod( &ScriptTexture::Load1,		"Load",			{"imageFile"} );
		binder.AddMethod( &ScriptTexture::Load2,		"Load",			{"imageFile", "flipY"} );
		binder.AddMethod( &ScriptTexture::Load3,		"Load",			{"imageFile", "region"} );

		binder.AddMethod( &ScriptTexture::AddLayer1,	"AddLayer",		{"imageFile", "layer"} );
		binder.AddMethod( &ScriptTexture::AddLayer2,	"AddLayer",		{"imageFile", "layer", "flipY"} );
		binder.AddMethod( &ScriptTexture::AddLayer3,	"AddLayer",		{"imageFile", "region", "layer"} );

		binder.AddMethod( &ScriptTexture::LoadChannel1,	"LoadChannel",	{"imageFile", "srcSwizzle", "dstSwizzle"} );

		binder.AddMethod( &ScriptTexture::Store,		"Store",		{"nameInArchive"} );
		binder.AddMethod( &ScriptTexture::SetFormat,	"Format",		{"newFormat"} );
	}


} // AE::AssetPacker
