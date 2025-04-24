// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_rhi/Private/EnumUtils.h"

#include "serializing/Public/ObjectFactory.h"

#include "scripting/Impl/ClassBinder.h"

#include "ScriptObjects/ScriptImageAtlas.h"
#include "ScriptObjects/ScriptResourceMeta.h"
#include "ScriptObjects/ScriptSharedImage.h"

#include "atlas_tools/RectPackerSTB.h"

#include "Packer/ImageAtlasPacker.h"

#include "res_loaders/AllImages/AllImageLoaders.h"

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptImageAtlas,	"ImageAtlas" );


namespace AE::AssetPacker
{
namespace {
#	include "Packer/ImagePacker.cpp.h"
#	include "Packer/ImageAtlasPacker.cpp.h"
}
	using namespace AE::Graphics;
	using namespace AE::ResLoader;

/*
=================================================
	constructor
=================================================
*/
	ScriptImageAtlas::ScriptImageAtlas () :
		_info{MakeRC<ImageAtlasInfo>()}
	{
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptImageAtlas::~ScriptImageAtlas ()
	{
		CHECK( _state == EState::Stored );
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptImageAtlas::Add (const String &imageName, const String &filename) __Th___
	{
		return Add2( imageName, filename, RectU::MaxSize() );
	}

	void  ScriptImageAtlas::Add2 (const String &imageName, const String &filename, const RectU &region) __Th___
	{
		CHECK_THROW( _state == EState::Recording );

		Path	path = ObjectStorage::Instance()->GetScriptFolder();
		path.append( filename );

		CHECK_THROW_MSG( FileSystem::IsFile( path ),
			"file '"s << filename << "' is not exists" );

		ObjectStorage::Instance()->AddName<ImageInAtlasName>( imageName );

		path = FileSystem::ToAbsolute( path );

		auto [img_it, img_inserted] = _uniqueImages.emplace( path, uint(_imageFiles.size()) );

		if ( img_inserted )
			_imageFiles.push_back( ImageInfo{ path });

		ImageRegion	img_region;
		img_region.region	= region;
		img_region.imageIdx	= img_it->second;

		auto [rect_it, rect_inserted] = _imageRegMap.emplace( img_region, uint(_imageRegions.size()) );
		if ( rect_inserted )
			_imageRegions.push_back( img_region );

		_map.emplace( imageName, rect_it->second );
		_info->Add( imageName );
	}

/*
=================================================
	_LoadImages
=================================================
*/
	void  ScriptImageAtlas::_LoadImages () __Th___
	{
		CHECK_THROW( _state == EState::Recording );

		for (auto& img : _imageFiles)
		{
			img.data.reset( new IntermImage{ img.path });

			AllImageLoaders	loader;
			CHECK_THROW_MSG( loader.LoadImage( *img.data, Default, False{"don't flipY"} ),
				"failed to load image '"s << ToString(img.path) << "'" );
		}
	}

/*
=================================================
	PutMeta
=================================================
*/
	void  ScriptImageAtlas::PutMeta (const ScriptResourceMetaPtr &meta, const String &nameInMeta) __Th___
	{
		CHECK_THROW( _state >= EState::Recording );
		CHECK_THROW( meta );

		meta->Add( *this, nameInMeta );  // throw

		_info->SetMetaResource( nameInMeta );
	}

/*
=================================================
	PutData
=================================================
*/
	void  ScriptImageAtlas::PutData (const ScriptSharedImagePtr &image) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( image );
		CHECK_THROW_MSG( not image->MetaName().empty(), "call 'SharedImage::PutMeta()' before this" );

		_LoadImages();  // throw

		Array<ScriptSharedImage::Result>	regions;
		regions.resize( _imageRegions.size() );

		// update regions
		for (usize i = 0; i < _imageRegions.size(); ++i)
		{
			auto&			src	= _imageRegions[i];
			const auto&		img = _imageFiles[ src.imageIdx ];
			const uint3		dim = img.data->Dimension();

			CHECK_THROW( dim.x > 0 and dim.y > 0 and dim.z == 1 );

			CHECK_THROW( src.region.left < dim.x );
			CHECK_THROW( src.region.top < dim.y );

			src.region.right	= Min( src.region.right,	dim.x );
			src.region.bottom	= Min( src.region.bottom,	dim.y );

			auto&	dst = regions[i];
			dst.region	= RectU{ src.region.Size() + uint(_paddingPix*2) };
			dst.id		= uint(i);
		}

		image->AddSubImages( *this, regions );  // throw

		_sharedImageMeta = image->MetaName();
		_state = EState::Immutable;
	}

/*
=================================================
	Store
----
	store meta data and image data into a single file
=================================================
*/
	void  ScriptImageAtlas::Store (const String &nameInArchive) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _info->MetaResName().empty() );

		auto&	storage = *ObjectStorage::Instance();

		_LoadImages();  // throw

		ScriptTexture	tex;
		CHECK_THROW( _ToTexture( OUT tex, nameInArchive ));

		_state = EState::Arranged;

		auto	wmem = MakeRC<ArrayWStream>();
		CHECK_THROW( _StoreMeta( wmem ));
		CHECK_THROW( tex._StoreData( wmem ));

		MemRefRStream	rmem {wmem->GetData()};
		storage.AddToArchive( nameInArchive, rmem, EArchivePackerFileType::Raw ); // throw

		_info->SetFileName( nameInArchive );
		storage.AddAtlas( _info );	// throw

		_info  = null;
		_state = EState::Stored;
	}

/*
=================================================
	StoreData
----
	store only image data, meta data stored to meta data file, see 'PutMeta()'
=================================================
*/
	void  ScriptImageAtlas::StoreData (const String &nameInArchive) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		CHECK_THROW( _sharedImageMeta.empty() );	// use 'PutData()' instead

		_LoadImages();  // throw

		ScriptTexture	tex;
		CHECK_THROW( _ToTexture( OUT tex, nameInArchive ));

		auto	wmem = MakeRC<ArrayWStream>();
		CHECK_THROW( tex._StoreData( wmem ));

		MemRefRStream	rmem {wmem->GetData()};
		ObjectStorage::Instance()->AddToArchive( nameInArchive, rmem, EArchivePackerFileType::Raw ); // throw

		_imageFileName	= nameInArchive;
		_state			= EState::StoreData;
	}

/*
=================================================
	_ToTexture
=================================================
*/
	bool  ScriptImageAtlas::_ToTexture (OUT ScriptTexture &tex, const String &name) __NE___
	{
		// update regions
		for (usize i = 0; i < _imageRegions.size(); ++i)
		{
			auto&			src	= _imageRegions[i];
			const auto&		img = _imageFiles[ src.imageIdx ];
			const uint3		dim = img.data->Dimension();

			CHECK_ERR( dim.x > 0 and dim.y > 0 and dim.z == 1 );

			CHECK_ERR( src.region.left < dim.x );
			CHECK_ERR( src.region.top < dim.y );

			src.region.right	= Min( src.region.right,	dim.x );
			src.region.bottom	= Min( src.region.bottom,	dim.y );
		}

		// place rects in atlas
		AtlasTools::RectPackerSTB	rect_packer;
		{
			for (usize i = 0; i < _imageRegions.size(); ++i)
			{
				const auto&	src = _imageRegions[i];
				rect_packer.Add( src.region.Size() + uint(_paddingPix)*2, uint(i) );
			}

			CHECK_ERR( rect_packer.Pack() );
			AE_LOGI( "Atlas '"s << name << "' size: "s << ToString(rect_packer.TargetSize())
						<< ", packing rate: " << ToString( rect_packer.PackingRate(), 2 ));
		}

		// convert images
		{
			tex._imgData.reset( new IntermImage{} );
			tex._dstFormat		= _dstFormat;
			tex._intermFormat	= _intermFormat;

			IntermImage&	dst_image = *tex._imgData;

			CHECK_ERR( dst_image.Allocate( EImage::_2D, _intermFormat, uint3{rect_packer.TargetSize(),1} ));

			auto	dst_view = RWImageMemView{ dst_image.ToView() };

			for (auto& r : rect_packer.GetResult())
			{
				auto&		src		= _imageRegions[ r.id ];
				const RectI	texc	= RectI{ int2{r.w, r.h} - _paddingPix*2 } + (int2{r.x, r.y} + _paddingPix);
				auto		src_img	= RWImageMemView{ _imageFiles[ src.imageIdx ].data->ToView() };

				ASSERT( All( texc.Size() == int2(src.region.Size()) ));

				CHECK_ERR( dst_view.Blit( uint3{int3{ texc.left, texc.top, 0 }}, uint3{src.region.left, src.region.top, 0u}, src_img, uint3{int3{ texc.Size(), 1 }} ));

				src.region = RectU{texc};
			}

			// compress if needed
			CHECK_ERR( tex._ConvertImage() );
		}

		// to image header
		{
			IntermImage&	dst_image = *tex._imgData;

			CHECK_ERR( dst_image.ArrayLayers() == 1 );
			CHECK_ERR( dst_image.MipLevels() == 1 );
			CHECK_ERR( dst_image.GetType() == EImage_2D );

			_imageHeader.dimension		= ImageDim_t(dst_image.Dimension());
			_imageHeader.arrayLayers	= 1;
			_imageHeader.mipmaps		= 1;
			_imageHeader.viewType		= EImage_2D;
			_imageHeader.format			= dst_image.PixelFormat();
			_imageHeader.flags			= 0;
			_imageHeader.rowAlignPOT	= POTBytes{};

			StaticAssert( sizeof(_imageHeader) == 16 );
		}
		return true;
	}

/*
=================================================
	SetPadding
=================================================
*/
	void  ScriptImageAtlas::SetPadding (uint pix) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		_paddingPix = pix;
	}

/*
=================================================
	SetFormat
=================================================
*/
	void  ScriptImageAtlas::SetFormat (EPixelFormat fmt) __Th___
	{
		CHECK_THROW( _state == EState::Recording );
		_dstFormat		= fmt;
		_intermFormat	= EPixelFormat_ToNoncompressed( _dstFormat, false );
	}

/*
=================================================
	_CopyPixels
=================================================
*/
	bool  ScriptImageAtlas::_CopyPixels (INOUT ResLoader::IntermImage &dstImage, ArrayView<ScriptSharedImage::Result> regions) __NE___
	{
		CHECK_ERR( _state == EState::Immutable );
		CHECK_ERR( _intermFormat == dstImage.PixelFormat() );
		CHECK_ERR( regions.size() == _imageRegions.size() );

		auto	dst_view = RWImageMemView{ dstImage.ToView() };

		for (auto& r : regions)
		{
			CHECK_ERR( r.id < _imageRegions.size() );

			auto&		src		= _imageRegions[ r.id ];
			const RectI	texc	= RectI{ int2{r.region.LeftTop()} + _paddingPix, int2{r.region.RightBottom()} - _paddingPix };
			auto		src_img	= RWImageMemView{ _imageFiles[ src.imageIdx ].data->ToView() };

			CHECK_ERR( All( texc.Size() == int2(src.region.Size()) ));
			CHECK_ERR( texc.IsValid() );
			CHECK_ERR( texc.left >= 0 and texc.top >= 0 );
			CHECK_ERR( texc.right <= int(dst_view.Dimension().x) );
			CHECK_ERR( texc.bottom <= int(dst_view.Dimension().y) );

			// 'src.region' - region inside 'src_img'
			CHECK_ERR( dst_view.Blit( uint3{int3{ texc.left, texc.top, 0 }}, uint3{src.region.left, src.region.top, 0u}, src_img, uint3{int3{ texc.Size(), 1 }} ));

			src.region = RectU{texc};
		}

		_state = EState::Arranged;
		return true;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptImageAtlas::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptImageAtlas>	binder{ se };
		binder.CreateRef();
		AS_METHOD( binder, ScriptImageAtlas::Add,			"Add",			{"imageNameInAtlas", "filename"} );
		AS_METHOD( binder, ScriptImageAtlas::Add2,			"Add",			{"imageNameInAtlas", "filename", "regionInSrcImage"} );
		AS_METHOD( binder, ScriptImageAtlas::Store,			"Store",		{"nameInArchive"} );
		AS_METHOD( binder, ScriptImageAtlas::StoreData,		"StoreData",	{"nameInArchive"} );
		AS_METHOD( binder, ScriptImageAtlas::PutMeta,		"PutMeta",		{"metaFile", "nameInMeta"} );
		AS_METHOD( binder, ScriptImageAtlas::PutData,		"PutData",		{"image"} );
		AS_METHOD( binder, ScriptImageAtlas::SetPadding,	"Padding",		{"paddingInPixels"} );
		AS_METHOD( binder, ScriptImageAtlas::SetFormat,		"Format",		{"newFormat"} );
	}

/*
=================================================
	_StoreMeta
=================================================
*/
	bool  ScriptImageAtlas::_StoreMeta (RC<WStream> stream, const String &metaArchive) C_NE___
	{
		ImageAtlasPacker	atlas_packer;
		atlas_packer.map.reserve( _map.size() );
		atlas_packer.rects.resize( _imageRegions.size() );

		if ( not _sharedImageMeta.empty() )
		{
			CHECK_ERR( _state == EState::Arranged );
			atlas_packer._header.flags	= ImageAtlasPacker::EFileFlags::HasResName;
			atlas_packer._imageResName	= CachedResourceName{_sharedImageMeta};
		}else
		if ( not _imageFileName.empty() )
		{
			CHECK_ERR( _state == EState::StoreData );
			atlas_packer._header.flags	= ImageAtlasPacker::EFileFlags::SeparateData;
			atlas_packer._imageHeader	= _imageHeader;
			atlas_packer._imageFileName	= VFS::FileName{_imageFileName};
		}
		else{
			CHECK_ERR( _state == EState::Arranged );
			atlas_packer._header.flags	= ImageAtlasPacker::EFileFlags::HasImage;
			atlas_packer._imageHeader	= _imageHeader;
		}

		for (const auto& [name, idx] : _map)
		{
			CHECK_ERR( idx < atlas_packer.rects.size() );
			CHECK_ERR( atlas_packer.map.emplace( ImageInAtlasName::Optimized_t{name}, idx ).second );
		}

		for (usize i = 0; i < _imageRegions.size(); ++i)
		{
			atlas_packer.rects[i] = Rectangle<ushort>{ _imageRegions[i].region };
		}

		{
			Serializing::Serializer	ser {stream};
			CHECK_ERR( ImageAtlasPacker_Serialize( atlas_packer, ser ));
		}

		if ( not _info->MetaResName().empty() )
		{
			CHECK_ERR( not metaArchive.empty() );
			NOTHROW_ERR( ObjectStorage::Instance()->AddAtlas( metaArchive, _info ));
		}

		_state = EState::Stored;
		return true;
	}


} // AE::AssetPacker
