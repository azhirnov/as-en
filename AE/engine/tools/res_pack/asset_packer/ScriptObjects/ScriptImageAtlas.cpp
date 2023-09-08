// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/DataSource/MemStream.h"

#include "graphics/Private/EnumUtils.h"

#include "serializing/ObjectFactory.h"

#include "scripting/Impl/ClassBinder.h"

#include "ScriptObjects/ScriptImageAtlas.h"
#include "Packer/ImageAtlasPacker.h"

#include "atlas_tools/RectPackerSTB.h"

#include "res_loaders/AllImages/AllImageLoaders.h"

AE_DECL_SCRIPT_OBJ_RC(  AE::AssetPacker::ScriptImageAtlas,  "ImageAtlas" );


namespace AE::AssetPacker
{
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
        CHECK( _map.empty() );
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
        Path    path = ObjectStorage::Instance()->GetScriptFolder();
        path.append( filename );

        CHECK_THROW_MSG( FileSystem::IsFile( path ),
            "file '"s << filename << "' is not exists" );

        ObjectStorage::Instance()->AddName<ImageInAtlasName>( imageName );

        path = FileSystem::ToAbsolute( path );

        auto [img_it, img_inserted] = _uniqueImages.emplace( path, uint(_imageFiles.size()) );

        if ( img_inserted )
            _imageFiles.push_back( ImageInfo{ path });

        ImageRegion img_region;
        img_region.region   = region;
        img_region.imageIdx = img_it->second;

        auto [rect_it, rect_inserted] = _imageRegMap.emplace( img_region, uint(_imageRegions.size()) );

        if ( rect_inserted )
            _imageRegions.push_back( img_region );

        _map.emplace( imageName, rect_it->second );
        _info->Add( imageName );
    }

/*
=================================================
    Store
=================================================
*/
    void  ScriptImageAtlas::Store (const String &nameInArchive) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();

        _LoadImages(); // throw

        {
            auto    wmem = MakeRC<MemWStream>();
            CHECK_THROW_MSG( _Pack( nameInArchive, wmem ));

            auto    rmem = wmem->ToRStream();
            storage.AddToArchive( nameInArchive, *rmem, EArchivePackerFileType::Raw );  // throw
        }

        _info->SetName( nameInArchive );            // throw
        storage.AddAtlas( nameInArchive, _info );   // throw

        _map.clear();
        _uniqueImages.clear();
        _imageFiles.clear();
        _imageRegMap.clear();
        _imageRegions.clear();
        _paddingPix = 1;
        _dstFormat  = _intermFormat = EPixelFormat::RGBA8_UNorm;
        _info       = null;
    }

/*
=================================================
    SetPadding
=================================================
*/
    void  ScriptImageAtlas::SetPadding (uint pix) __Th___
    {
        _paddingPix = pix;
    }

/*
=================================================
    SetFormat
=================================================
*/
    void  ScriptImageAtlas::SetFormat (EPixelFormat fmt) __Th___
    {
        _dstFormat      = fmt;
        _intermFormat   = EPixelFormat_ToNoncompressed( _dstFormat, false );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptImageAtlas::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptImageAtlas>    binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptImageAtlas::Add,           "Add",      {"imageNameInAtlas", "filename"} );
        binder.AddMethod( &ScriptImageAtlas::Add2,          "Add",      {"imageNameInAtlas", "filename", "region"} );
        binder.AddMethod( &ScriptImageAtlas::Store,         "Store",    {"nameInArchive"} );
        binder.AddMethod( &ScriptImageAtlas::SetPadding,    "Padding",  {"paddingInPixels"} );
        binder.AddMethod( &ScriptImageAtlas::SetFormat,     "Format",   {"newFormat"} );
    }

/*
=================================================
    _Pack
=================================================
*/
    bool  ScriptImageAtlas::_Pack (const String &nameInArchive, RC<WStream> stream)
    {
        // update regions
        for (usize i = 0; i < _imageRegions.size(); ++i)
        {
            auto&           src = _imageRegions[i];
            const auto&     img = _imageFiles[ src.imageIdx ];
            const uint3     dim = img.data->Dimension();

            CHECK_ERR( dim.x > 0 and dim.y > 0 and dim.z == 1 );

            CHECK_ERR( src.region.left < dim.x );
            CHECK_ERR( src.region.top < dim.y );

            src.region.right    = Min( src.region.right,    dim.x );
            src.region.bottom   = Min( src.region.bottom,   dim.y );
        }

        // place rects in atlas
        AtlasTools::RectPackerSTB   rect_packer;
        {
            for (usize i = 0; i < _imageRegions.size(); ++i)
            {
                const auto& src = _imageRegions[i];
                rect_packer.Add( src.region.Size() + uint(_paddingPix)*2, uint(i) );
            }

            CHECK_ERR( rect_packer.Pack() );
            AE_LOGI( "Atlas '"s << nameInArchive << "' size: "s << ToString(rect_packer.TargetSize())
                        << ", packing rate: " << ToString( rect_packer.PackingRate(), 2 ));
        }

        // convert images
        IntermImage     dst_image;
        {
            CHECK_ERR( dst_image.Allocate( EImage::_2D, _intermFormat, uint3{rect_packer.TargetSize(),1} ));

            auto    dst_view = RWImageMemView{ dst_image.ToView() };

            for (auto& r : rect_packer.GetResult())
            {
                const auto& src     = _imageRegions[ r.id ];
                const RectI texc    = RectI{ int2{r.w, r.h} - _paddingPix*2 } + (int2{r.x, r.y} + _paddingPix);
                auto        src_img = RWImageMemView{ _imageFiles[ src.imageIdx ].data->ToView() };

                ASSERT( All( texc.Size() == int2(src.region.Size()) ));

                CHECK_ERR( dst_view.Blit( uint3{texc.left, texc.top, 0}, uint3{src.region.left, src.region.top, 0}, src_img, uint3(texc.Size(),1) ));

                // TODO: fill border?
            }

            if ( _dstFormat != _intermFormat )
            {
                RETURN_ERR( "compression is not supported yet" );
            }
        }

        // serialize
        {
            ImageMemView    dst_view = dst_image.ToView();

            ImageAtlasPacker    atlas_packer;
            atlas_packer.header.version     = ImagePacker::Version;
            atlas_packer.header.dimension   = ushort3{uint3{ rect_packer.TargetSize(), 1 }};
            atlas_packer.header.arrayLayers = 1;
            atlas_packer.header.mipmaps     = 1;
            atlas_packer.header.format      = _dstFormat;
            atlas_packer.header.viewType    = EImage_2D;
            atlas_packer.header.rowSize     = uint(dst_view.RowPitch());

            atlas_packer.map.reserve( _map.size() );
            atlas_packer.rects.resize( _imageRegions.size() );

            for (const auto& [name, idx] : _map)
            {
                CHECK_ERR( idx < atlas_packer.rects.size() );
                CHECK_ERR( atlas_packer.map.emplace( ImageInAtlasName::Optimized_t{name}, idx ).second );
            }

            for (auto& r : rect_packer.GetResult())
            {
                CHECK_ERR( r.id < int(atlas_packer.rects.size()) );

                const RectI texc = RectI{ int2{r.w, r.h} - _paddingPix*2 } + (int2{r.x, r.y} + _paddingPix);
                CHECK_ERR( texc.IsValid() );
                CHECK_ERR( texc.left >= 0 and texc.top >= 0 );
                CHECK_ERR( texc.right < int(rect_packer.TargetSize().x) );
                CHECK_ERR( texc.bottom < int(rect_packer.TargetSize().y) );

                auto&   dst = atlas_packer.rects[ r.id ];
                dst = Rectangle<ushort>{ texc };
            }

            {
                Serializing::Serializer ser {stream};
                CHECK_ERR( atlas_packer.Serialize( ser ));
            }
            CHECK_ERR( atlas_packer.SaveImage( *stream, dst_view ));
        }

        return true;
    }

/*
=================================================
    _LoadImages
=================================================
*/
    void  ScriptImageAtlas::_LoadImages () __Th___
    {
        for (auto& img : _imageFiles)
        {
            img.data.reset( new IntermImage{ img.path });

            AllImageLoaders loader;
            CHECK_THROW_MSG( loader.LoadImage( *img.data, Default, False{"don't flipY"} ),
                "failed to load image '"s << ToString(img.path) << "'" );
        }
    }


} // AE::AssetPacker
