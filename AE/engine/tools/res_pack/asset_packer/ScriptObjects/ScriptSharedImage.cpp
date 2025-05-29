// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptSharedImage.h"
#include "ScriptObjects/ScriptImageAtlas.h"
#include "ScriptObjects/ScriptRasterFont.h"
#include "ScriptObjects/ScriptResourceMeta.h"

#include "atlas_tools/RectPackerSTB.h"

#include "res_loaders/AllImages/AllImageLoaders.h"

#include "graphics_rhi/Private/EnumToString.h"

namespace AE::AssetPacker
{
namespace {
#	include "Packer/ImagePacker.cpp.h"
}
	using namespace AE::Graphics;
	using namespace AE::ResLoader;

/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptSharedImage::ScriptSharedImage () :
		_image{ new ScriptTexture{} }
	{}

	ScriptSharedImage::~ScriptSharedImage ()
	{
		CHECK( _state == EState::Stored );
	}

/*
=================================================
	Store
=================================================
*/
	void  ScriptSharedImage::Store (const String &nameInArchive) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _image );
		CHECK_THROW_MSG( not (_atlases.empty() and _fonts.empty()), "nothing to save" );
		CHECK_THROW_MSG( not _metaDataName.empty(), "Add to meta data before storing" );

		_Arrange( nameInArchive ); // throw

		for (auto [atlas, i] : WithIndex(_atlases)) {
			CHECK_THROW( atlas->_CopyPixels( *_image->_imgData, _GetResult( uint(i) | c_AtlasBit )));
		}

		for (auto [font, i] : WithIndex(_fonts)) {
			CHECK_THROW( font->_CopyPixels( *_image->_imgData, _GetResult( uint(i) | c_FontBit )));
		}

		CHECK_THROW( _image->_ConvertImage() );
		_image->_fileName = nameInArchive;

		{
			auto	wmem = MakeRC<ArrayWStream>();
			CHECK_THROW( _image->_StoreData( wmem ));

			MemRefRStream	rmem {wmem->GetData()};
			ObjectStorage::Instance()->AddToArchive( nameInArchive, rmem, EArchivePackerFileType::Raw ); // throw
		}

		_atlases.clear();
		_atlasMap.clear();

		_fonts.clear();
		_fontMap.clear();

		_image = null;		// stil alive in 'ScriptResourceMeta'
		_state = EState::Stored;
	}

/*
=================================================
	PutMeta
=================================================
*/
	void  ScriptSharedImage::PutMeta (const ScriptResourceMetaPtr &meta, const String &name) __Th___
	{
		CHECK_THROW( _state < EState::Stored );
		CHECK_THROW( meta );
		CHECK_THROW( _image );
		CHECK_THROW_MSG( _metaDataName.empty(), "Already added to meta data" );

		_metaDataName = name;
		meta->Add( *_image, name );  // throw
	}

/*
=================================================
	SetFormat
=================================================
*/
	void  ScriptSharedImage::SetFormat (EPixelFormat fmt) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _image );

		if ( fmt != _image->DstFormat() )
			CHECK_THROW_MSG( not _hasFormat, "pixel format already defined" );

		_image->SetFormat( fmt );  // throw
		_hasFormat = true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptSharedImage::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptSharedImage>	binder{ se };
		binder.CreateRef();

		AS_METHOD( binder, ScriptSharedImage::Store,		"Store",		{"nameInArchive"} );
		AS_METHOD( binder, ScriptSharedImage::SetFormat,	"Format",		{"newFormat"} );
		AS_METHOD( binder, ScriptSharedImage::PutMeta,		"PutMeta",		{"metaFile", "nameInMeta"} );
	}

/*
=================================================
	_Arrange
=================================================
*/
	void  ScriptSharedImage::_Arrange (const String &nameInArchive) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _image );
		CHECK_THROW( _hasFormat );
		CHECK_THROW( not _imageRegions.empty() );

		// place rects in atlas
		AtlasTools::RectPackerSTB	rect_packer;
		{
			for (usize i = 0; i < _imageRegions.size(); ++i)
			{
				const auto&	src = _imageRegions[i];
				rect_packer.Add( src.region.Size(), uint(i) );
			}

			CHECK_THROW( rect_packer.Pack(), "internal error" );
			AE_LOGI( "SharedImage '"s << nameInArchive << "' size: "s << ToString(rect_packer.TargetSize())
						<< ", packing rate: " << ToString( rect_packer.PackingRate(), 2 ));
		}

		for (auto& r : rect_packer.GetResult())
		{
			auto&	src	= _imageRegions[ r.id ];

			src.region = RectU{uint2{int2{ r.w, r.h }}} + uint2{int2{ r.x, r.y }};
		}

		// allocate & clear image
		{
			_image->_imgData.reset( new IntermImage{} );
			CHECK_THROW( _image->_imgData->Allocate( EImage_2D, _image->_intermFormat, uint3{rect_packer.TargetSize(), 1u} ));

			auto		dst_view = RWImageMemView{ _image->_imgData->ToView() };
			const auto&	fmt_info = EPixelFormat_GetInfo( dst_view.Format() );

			if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::SNorm | PixelFormatInfo::EType::SFloat )) {
				CHECK_THROW( dst_view.Fill( RGBA32f{-1.0f} ));
			}else
			if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::UNorm )) {
				CHECK_THROW( dst_view.Fill( RGBA32f{0} ));
			}else
			if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::SInt | PixelFormatInfo::EType::UInt )) {
				CHECK_THROW( dst_view.Fill( RGBA32u{0} ));
			}
		}

		_state = EState::Arranged;
	}

/*
=================================================
	AddSubImages
=================================================
*/
	void  ScriptSharedImage::AddSubImages (ScriptImageAtlas &atlas, ArrayView<Result> subImages) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _image );

		auto [it, inserted] = _atlasMap.emplace( &atlas, uint(_atlases.size()) );
		if_unlikely( inserted )
		{
			_atlases.push_back( &atlas );

			if ( _hasFormat ){
				if ( atlas._dstFormat != _image->DstFormat() ){
					CHECK_THROW_MSG( false,
						"Can not add font glyphs to shared image.\n"
						"ImageAtlas pixel format '"s << ToString(atlas._dstFormat) << "' doesn't match with current pixel format '" << ToString(_image->DstFormat()) << "'." );
				}
			}else
				SetFormat( atlas._dstFormat );
		}

		for (auto& src : subImages)
		{
			ImageRegion&	reg = _imageRegions.emplace_back();
			reg.res		= c_AtlasBit | it->second;
			reg.id		= src.id;
			reg.region	= RectU{ src.region.Size()};
		}
	}

/*
=================================================
	AddSubImages
=================================================
*/
	void  ScriptSharedImage::AddSubImages (ScriptRasterFont &font, ArrayView<Result> subImages) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _image );

		auto [it, inserted] = _fontMap.emplace( &font, uint(_fonts.size()) );
		if_unlikely( inserted )
		{
			_fonts.push_back( &font );

			if ( _hasFormat ){
				if ( font._dstFormat != _image->DstFormat() ){
					CHECK_THROW_MSG( false,
						"Can not add font glyphs to shared image.\n"
						"Font pixel format '"s << ToString(font._dstFormat) << "' doesn't match with current pixel format '" << ToString(_image->DstFormat()) << "'." );
				}
			}else
				SetFormat( font._dstFormat );
		}

		for (auto& src : subImages)
		{
			ImageRegion&	reg = _imageRegions.emplace_back();
			reg.res		= c_FontBit | it->second;
			reg.id		= src.id;
			reg.region	= RectU{ src.region.Size()};
		}
	}

/*
=================================================
	_GetResult
=================================================
*/
	Array<ScriptSharedImage::Result>  ScriptSharedImage::_GetResult (const uint res) C_Th___
	{
		Array<Result>	result;
		result.reserve( 64 );

		for (auto& src : _imageRegions)
		{
			if ( src.res == res )
				result.push_back( src );
		}
		return result;
	}


} // AE::AssetPacker
