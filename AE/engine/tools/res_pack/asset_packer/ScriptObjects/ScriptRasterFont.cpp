// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/MemStream.h"
#include "base/DataSource/FileStream.h"
#include "base/Utils/Helpers.h"
#include "base/Math/Packing.h"
#include "base/Algorithms/StringUtils.h"

#include "graphics/Private/EnumUtils.h"

#include "ScriptObjects/ScriptRasterFont.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"

#include "atlas_tools/RectPackerSTB.h"

AE_DECL_SCRIPT_OBJ_RC(  AE::AssetPacker::ScriptRasterFont,                  "RasterFont"        );
AE_DECL_SCRIPT_TYPE(    AE::AssetPacker::ScriptRasterFont::ERasterFontMode, "ERasterFontMode"   );


namespace AE::AssetPacker
{
namespace {
#   include "Packer/ImagePacker.cpp.h"
#   include "Packer/RasterFontPacker.cpp.h"
}

    using namespace AE::Graphics;
    using namespace AE::ResLoader;

/*
=================================================
    constructor
=================================================
*/
    ScriptRasterFont::ScriptRasterFont ()
    {}

/*
=================================================
    destructor
=================================================
*/
    ScriptRasterFont::~ScriptRasterFont ()
    {}

/*
=================================================
    AddCharset
=================================================
*/
    void  ScriptRasterFont::AddCharset (uint unicodeFirst, uint unicodeLast) __Th___
    {
        for (auto [first, last] : _charset)
        {
            CHECK_THROW_MSG( not IsIntersects( uint{first}, uint{last}+1, unicodeFirst, unicodeLast+1 ),
                "Charset range ["s << ToString(unicodeFirst) << ", " << ToString(unicodeLast) << "] intersects with " <<
                "[" << ToString(first) << ", " << ToString(last) << "]" );
        }

        _charset.emplace_back( unicodeFirst, unicodeLast );

        std::sort( _charset.begin(), _charset.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
    }

/*
=================================================
    AddCharset
=================================================
*/
    void  ScriptRasterFont::AddCharset_Rus () __Th___
    {
        Array<CharUtf32>    rus_chars = {
            /* А */0x0410, /* Б */0x0411, /* В */0x0412, /* Г */0x0413, /* Д */0x0414, /* Е */0x0415, /* Ё */0x0401, /* Ж */0x0416, /* З */0x0417,
            /* И */0x0418, /* Й */0x0419, /* К */0x041A, /* Л */0x041B, /* М */0x041C, /* Н */0x041D, /* О */0x041E, /* П */0x041F, /* Р */0x0420,
            /* С */0x0421, /* Т */0x0422, /* У */0x0423, /* Ф */0x0424, /* Х */0x0425, /* Ц */0x0426, /* Ч */0x0427, /* Ш */0x0428, /* Щ */0x0429,
            /* Ъ */0x042A, /* Ы */0x042B, /* Ь */0x042C, /* Э */0x042D, /* Ю */0x042E, /* Я */0x042F,
            /* а */0x0430, /* б */0x0431, /* в */0x0432, /* г */0x0433, /* д */0x0434, /* е */0x0435, /* ё */0x0451, /* ж */0x0436, /* з */0x0437,
            /* и */0x0438, /* й */0x0439, /* к */0x043A, /* л */0x043B, /* м */0x043C, /* н */0x043D, /* о */0x043E, /* п */0x043F, /* р */0x0440,
            /* с */0x0441, /* т */0x0442, /* у */0x0443, /* ф */0x0444, /* х */0x0445, /* ц */0x0446, /* ч */0x0447, /* ш */0x0448, /* щ */0x0449,
            /* ъ */0x044A, /* ы */0x044B, /* ь */0x044C, /* э */0x044D, /* ю */0x044E, /* я */0x044F,
            /* ruble sign */0x20BD
        };

        std::sort( rus_chars.begin(), rus_chars.end(), [](auto lhs, auto rhs) { return lhs < rhs; });

        CharUtf32   prev        = rus_chars[0];
        usize       begin_range = 0;

        for (usize i = 1; i < rus_chars.size(); ++i)
        {
            if ( prev+1 != rus_chars[i] )
            {
                AddCharset( rus_chars[begin_range], rus_chars[i-1] );
                begin_range = i;
            }
            prev = rus_chars[i];
        }

        AddCharset( rus_chars[begin_range], rus_chars.back() );
    }

/*
=================================================
    AddCharset_Ascii
=================================================
*/
    void  ScriptRasterFont::AddCharset_Ascii () __Th___
    {
        AddCharset( 9, 9 );     // tab
        AddCharset( 32, 126 );
    }

/*
=================================================
    SetGlyphSize
=================================================
*/
    void  ScriptRasterFont::SetGlyphSize (uint value) __Th___
    {
        CHECK_THROW_MSG( value <= _MaxFontHeight );

        _bitmapHeight = value;
    }

/*
=================================================
    SetGlyphPadding
=================================================
*/
    void  ScriptRasterFont::SetGlyphPadding (uint pix) __Th___
    {
        _paddingPix = pix;
    }

/*
=================================================
    Load
=================================================
*/
    void  ScriptRasterFont::Load (const String &fontFile) __Th___
    {
        CHECK_THROW_MSG( _fontFile.empty(), "already loaded" );

        Path    path = ObjectStorage::Instance()->GetScriptFolder();
        path.append( fontFile );

        _fontFile = FileSystem::ToAbsolute( path );
        CHECK_THROW_MSG( FileSystem::IsFile( _fontFile ));
    }

/*
=================================================
    Store
=================================================
*/
    void  ScriptRasterFont::Store (const String &nameInArchive) __Th___
    {
        CHECK_THROW_MSG( not _fontFile.empty() );
        CHECK_THROW_MSG( not _charset.empty() );

        auto&   storage = *ObjectStorage::Instance();

        auto    wmem = MakeRC<MemWStream>();
        CHECK_THROW_MSG( _Pack( nameInArchive, wmem ));

        auto    rmem = wmem->ToRStream();
        storage.AddToArchive( nameInArchive, *rmem, EArchivePackerFileType::Raw );  // throw

        storage.AddFont( nameInArchive );  // throw

        _fontFile.clear();
        // keep '_charset'
        _paddingPix = 1;
        _dstFormat  = _intermFormat = EPixelFormat::R8_UNorm;
    }

/*
=================================================
    SetFormat
=================================================
*/
    void  ScriptRasterFont::SetFormat (EPixelFormat fmt) __Th___
    {
        _dstFormat      = fmt;
        _intermFormat   = EPixelFormat_ToNoncompressed( _dstFormat, false );
    }

/*
=================================================
    SetRasterMode
=================================================
*/
    void  ScriptRasterFont::SetRasterMode (ERasterFontMode value) __Th___
    {
        CHECK_THROW_MSG( value < ERasterFontMode::_Count );

        _fontMode = value;
    }

/*
=================================================
    SetSDFGlyphBorder
=================================================
*/
    void  ScriptRasterFont::SetSDFGlyphBorder (uint pix) __Th___
    {
        CHECK_THROW_MSG( _sdfBorderSize <= _bitmapHeight/4 );

        _sdfBorderSize = pix;
    }

/*
=================================================
    SetSDFPixelRange
=================================================
*/
    void  ScriptRasterFont::SetSDFPixelRange (float range) __Th___
    {
        CHECK_THROW_MSG( range > 0.f );

        _sdfPixRange = double(range);
    }

/*
=================================================
    _Pack
=================================================
*/
    bool  ScriptRasterFont::_Pack (const String &nameInArchive, RC<WStream> stream)
    {
        // load glyphs
        Allocator_t         allocator;      allocator.SetBlockSize( 4_Mb );
        Array<GlyphData>    glyphs;
        float               sdf_scale       = 0.f;
        float               sdf_bias        = 0.f;
        uint                fnt_height      = 0;
        int                 padding_in_atlas = _paddingPix;

        switch_enum( _fontMode )
        {
            case ERasterFontMode::Raster :
                #ifdef AE_ENABLE_FREETYPE
                    CHECK_ERR( _FTLoadGlyphs( allocator, OUT fnt_height, OUT glyphs ));
                #else
                    RETURN_ERR( "FreeType is not supported" );
                #endif
                break;

            case ERasterFontMode::SDF :
            case ERasterFontMode::MC_SDF :
                #ifdef AE_ENABLE_MSDFGEN
                    padding_in_atlas = 0;   // added to border
                    CHECK_ERR( _SDFLoadGlyphs( allocator, OUT sdf_scale, OUT sdf_bias, OUT fnt_height, OUT glyphs ));
                #else
                    RETURN_ERR( "MSDFGen is not supported" );
                #endif
                break;

            case ERasterFontMode::_Count :
            default :
                RETURN_ERR( "unsupported raster font mode" );
        }
        switch_end

        // place rects in atlas
        AtlasTools::RectPackerSTB   rect_packer;
        {
            for (usize i = 0; i < glyphs.size(); ++i)
            {
                const auto& src = glyphs[i];
                rect_packer.Add( src.dimension + uint(padding_in_atlas)*2, uint(i) );
            }

            CHECK_ERR( rect_packer.Pack() );
            AE_LOGI( "RasterFont '"s << nameInArchive << "' atlas size: "s << ToString(rect_packer.TargetSize()) <<
                     ", packing rate: " << ToString( rect_packer.PackingRate(), 2 ));
        }

        // store glyphs to image
        IntermImage     dst_image;
        {
            CHECK_ERR( dst_image.Allocate( EImage_2D, _intermFormat, uint3{rect_packer.TargetSize(),1} ));

            auto        dst_view = RWImageMemView{ dst_image.ToView() };
            const auto& fmt_info = EPixelFormat_GetInfo( dst_view.Format() );

            if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::SNorm | PixelFormatInfo::EType::SFloat )) {
                CHECK_ERR( dst_view.Fill( RGBA32f{-1.0f} ));
            }else
            if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::UNorm )) {
                CHECK_ERR( dst_view.Fill( RGBA32f{0} ));
            }else
            if ( AnyBits( fmt_info.valueType, PixelFormatInfo::EType::Int | PixelFormatInfo::EType::UInt )) {
                CHECK_ERR( dst_view.Fill( RGBA32u{0} ));
            }

            for (auto& r : rect_packer.GetResult())
            {
                auto&   src = glyphs[ r.id ];
                if_unlikely( src.symbol == ' ' or src.symbol == '\t' )
                    continue;

                const RectI     img_rect = RectI{ int2{r.w, r.h} - padding_in_atlas*2 } + (int2{r.x, r.y} + padding_in_atlas);
                RWImageMemView  src_view { BufferMemView{ src.data, Bytes{src.dataSize} }, uint3{}, uint3{src.dimension,1}, 0_b, 0_b, _intermFormat, EImageAspect::Color };

                ASSERT( All( img_rect.Size() == int2(src.dimension) ));

                CHECK_ERR( dst_view.Blit( uint3{img_rect.left, img_rect.top, 0}, uint3{0}, src_view, uint3(img_rect.Size(),1) ));

                const RectI     texc = RectI{ int2{r.w, r.h} - _paddingPix*2 } + (int2{r.x, r.y} + _paddingPix);
                src.texcoord = FloatToUNormShort( RectF{texc} / float2(dst_view.Dimension()) );
            }

            if ( _dstFormat != _intermFormat )
            {
                RETURN_ERR( "compression is not supported yet" );
            }
        }

        // serialize
        {
            ImagePacker::Header img_hdr;
            img_hdr.dimension   = ushort3{uint3{ rect_packer.TargetSize(), 1 }};
            img_hdr.arrayLayers = 1;
            img_hdr.mipmaps     = 1;
            img_hdr.format      = _dstFormat;
            img_hdr.viewType    = EImage_2D;

            RasterFontPacker    fnt_packer {img_hdr};
            fnt_packer.sdfConfig.scale      = sdf_scale;
            fnt_packer.sdfConfig.bias       = sdf_bias;
            fnt_packer.sdfConfig.pixRange2D = float(_sdfPixRange);
            fnt_packer.sdfConfig.pixRange3D = float(_sdfPixRange) / float2(img_hdr.dimension);

            fnt_packer.fontHeight.push_back( CheckCast<ubyte>( fnt_height ));
            fnt_packer.glyphMap.reserve( glyphs.size() );

            for (auto& glyph : glyphs)
            {
                CHECK_ERR( fnt_packer.glyphMap.emplace( GlyphKey{ glyph.symbol, fnt_height }, Glyph{glyph} ).second );
            }

            {
                Serializing::Serializer     ser {stream};
                CHECK_ERR( RasterFontPacker_Serialize( fnt_packer, ser ));
            }
            CHECK_ERR( RasterFontPacker_SaveImage( fnt_packer, *stream, dst_image ));
        }

        return true;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptRasterFont::Bind (const ScriptEnginePtr &se) __Th___
    {
        {
            Scripting::EnumBinder<ERasterFontMode>  binder{ se };
            binder.Create();

            binder.Comment( "Raster font, glyph dimension on screen should be multiple of glyph bitmap size." );
            binder.AddValue( "Raster",  ERasterFontMode::Raster );

            binder.Comment( "SDF font, can be scaled, but have smooth angles." );
            binder.AddValue( "SDF",     ERasterFontMode::SDF );

            binder.Comment( "Multichannel SDF font, can be scaled, have sharp angles, but a bit less performance." );
            binder.AddValue( "MC_SDF",  ERasterFontMode::MC_SDF );
            StaticAssert( uint(ERasterFontMode::_Count) == 3 );
        }
        {
            Scripting::ClassBinder<ScriptRasterFont>    binder{ se };
            binder.CreateRef();

            binder.AddMethod( &ScriptRasterFont::Load,              "Load",             {"fontFile"} );
            binder.AddMethod( &ScriptRasterFont::AddCharset,        "AddCharset",       {"firstCharIndexInUnicode", "lastCharIndexInUnicode"} );
            binder.AddMethod( &ScriptRasterFont::AddCharset_Ascii,  "AddCharset_Ascii", {} );
            binder.AddMethod( &ScriptRasterFont::AddCharset_Rus,    "AddCharset_Rus",   {} );
            binder.AddMethod( &ScriptRasterFont::ClearCharset,      "ClearCharset",     {} );
            binder.AddMethod( &ScriptRasterFont::SetGlyphSize,      "GlyphSize",        {"heightInPixels"} );
            binder.AddMethod( &ScriptRasterFont::SetGlyphPadding,   "GlyphPadding",     {"paddingInPixels"} );
            binder.AddMethod( &ScriptRasterFont::Store,             "Store",            {"nameInArchive"} );
            binder.AddMethod( &ScriptRasterFont::SetFormat,         "Format",           {"newFormat"} );
            binder.AddMethod( &ScriptRasterFont::SetRasterMode,     "RasterMode",       {} );
            binder.AddMethod( &ScriptRasterFont::SetSDFGlyphBorder, "SDFGlyphBorder",   {"borderSizeInPixels"} );

            binder.Comment( "Increase value for better anti-aliasing." );
            binder.AddMethod( &ScriptRasterFont::SetSDFPixelRange,  "SDFPixelRange",    {"range"} );
        }
    }

} // AE::AssetPacker
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_FREETYPE

# define _INC_STDDEF
# define _INC_STRING
# define _INC_STDLIB
# define _INC_ERRNO
# define _INC_STDIO

# include <ft2build.h>
# include <freetype/freetype.h>

# include FT_FREETYPE_H
# include FT_GLYPH_H
# include FT_BITMAP_H
# include FT_TRUETYPE_TABLES_H

namespace AE::AssetPacker
{
    const FT_Int32  ft_load_flags   = FT_LOAD_RENDER;   // FT_LOAD_FORCE_AUTOHINT

/*
=================================================
    _FTLoadGlyph
=================================================
*/
    bool  ScriptRasterFont::_FTLoadGlyph (void* face, CharUtf32 unicodeChar, Allocator_t &allocator, OUT GlyphData &result) const
    {
        FT_Face     ft_face = static_cast<FT_Face>(face);

        FT_UInt     char_index = FT_Get_Char_Index( ft_face, unicodeChar );
        CHECK_ERR( char_index != 0 );
        CHECK_ERR( FT_Load_Glyph( ft_face, char_index, ft_load_flags ) == 0 );

        if ( ft_face->glyph->bitmap.buffer == null )
            RETURN_ERR( "Symbol '"s << ToString(unicodeChar) << "' hasn't bitmap" );

        /*
        // calculate font metrics
        int ascent  =  ft_face->size->metrics.ascender >> 6;    // TODO: float ?
        int descent = -ft_face->size->metrics.descender >> 6;

        if ( TT_OS2* os2 = Cast<TT_OS2>(FT_Get_Sfnt_Table( ft_face, ft_sfnt_os2 )) )
        {
            ascent  = Max( ascent, os2->usWinAscent * ft_face->size->metrics.y_ppem / ft_face->units_per_EM );
            descent = Max( descent, os2->usWinDescent * ft_face->size->metrics.y_ppem / ft_face->units_per_EM );

            ascent  = Max( ascent, os2->sTypoAscender * ft_face->size->metrics.y_ppem / ft_face->units_per_EM );
            descent = Max( descent, -os2->sTypoDescender * ft_face->size->metrics.y_ppem / ft_face->units_per_EM );
        }*/

        const uint  w           = ft_face->glyph->bitmap.width;
        const uint  h           = ft_face->glyph->bitmap.rows;
        const uint  img_size    = w * h;
        void *      data_ptr    = allocator.Allocate( Bytes{img_size} );
        CHECK_ERR( data_ptr != null );

        MemCopy( OUT data_ptr, ft_face->glyph->bitmap.buffer, Bytes{img_size} );

        // Glyph
        result.advance      = float(ft_face->glyph->advance.x) / 64.f;
        result.offset.left  = float(ft_face->glyph->bitmap_left);
        result.offset.right = float(ft_face->glyph->bitmap_left) + float(w);
        result.offset.top   = -float(ft_face->glyph->bitmap_top);
        result.offset.bottom= -(float(ft_face->glyph->bitmap_top) - float(h));

        ASSERT( result.offset.left < result.advance );
        ASSERT( result.offset.right <= result.advance * 1.1f );

        // GlyphData
        result.dimension    = uint2{ w, h };
        result.symbol       = unicodeChar;
        result.data         = data_ptr;
        result.dataSize     = img_size;

        return true;
    }

/*
=================================================
    _FTLoadGlyphs
=================================================
*/
    bool  ScriptRasterFont::_FTLoadGlyphs (Allocator_t &allocator, OUT uint &outFontHeight, OUT Array<GlyphData> &result) const
    {
        CHECK_ERR( _intermFormat == EPixelFormat::R8_UNorm );

        outFontHeight = 0;

        MemRStream  mem_stream;
        {
            FileRStream     file {_fontFile};
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( mem_stream.LoadRemaining( file ));
        }

        FT_Library  ft_library;
        CHECK_ERR( FT_Init_FreeType( &ft_library ) == 0 );

        ON_DESTROY( [&ft_library](){ FT_Done_FreeType( ft_library ); });

        FT_Face     ft_face = null;
        CHECK_ERR( FT_New_Memory_Face( ft_library, mem_stream.GetData().data(), FT_Long(mem_stream.GetData().size()), 0, OUT &ft_face ) == 0 );

        ON_DESTROY( [&ft_face](){ FT_Done_Face( ft_face ); });

        CHECK_ERR( FT_Select_Charmap( ft_face, FT_ENCODING_UNICODE ) == 0 );

        CHECK_ERR( FT_Set_Pixel_Sizes( ft_face, _bitmapHeight, _bitmapHeight ) == 0 );

        float   max_height = 0.f;

        for (auto& cset : _charset)
        {
            for (uint c = cset.first; c <= cset.second; ++c)
            {
                if_unlikely( c == ' ' or c == '\t' )
                {
                    FT_UInt     char_index = FT_Get_Char_Index( ft_face, ' ' );
                    CHECK_ERR( char_index != 0 );
                    CHECK_ERR( FT_Load_Glyph( ft_face, char_index, ft_load_flags ) == 0 );

                    auto&   glyph   = result.emplace_back();
                    glyph.advance   = float(ft_face->glyph->advance.x) / 64.f * (c == '\t' ? _TabSize : 1);
                    glyph.symbol    = c;
                    continue;
                }

                GlyphData   glyph;
                if ( _FTLoadGlyph( ft_face, c, allocator, OUT glyph ))
                {
                    max_height = Max( max_height, float(glyph.dimension.x) );
                    result.push_back( RVRef(glyph) );
                }
            }
        }

        outFontHeight = uint(max_height + 0.5f);
        return true;
    }

} // AE::AssetPacker
#endif // AE_ENABLE_FREETYPE
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_MSDFGEN
# include "msdfgen.h"
# include "msdfgen-ext.h"

namespace AE::AssetPacker
{
/*
=================================================
    _SDFLoadGlyph
=================================================
*/
    bool  ScriptRasterFont::_SDFLoadGlyph (void* face, CharUtf32 unicodeChar, const double projScale, Allocator_t &allocator, OUT GlyphData &result) const
    {
        using namespace msdfgen;

        FontHandle* ft_font = static_cast<FontHandle*>(face);
        Shape       shape;
        double      advance     = 0.0;
        const uint  border_px   = _sdfBorderSize + _paddingPix;

        if ( not loadGlyph( OUT shape, ft_font, unicode_t(unicodeChar), OUT &advance ))
            RETURN_ERR( "Symbol '"s << ToString(unicodeChar) << "' hasn't any data" );

        shape.inverseYAxis = true;
        shape.normalize();
        CHECK_ERR( shape.validate() );

        Vector2             proj_scale  {projScale};                    // glyph coords to pixels
        Vector2             proj_translate;                             // glyph coords
        MSDFGeneratorConfig config;
        auto                bounds      = shape.getBounds();            // in glyph coords
        const uint          bm_size     = _bitmapHeight + border_px*2;  // pixels

        // Glyph
        result.advance      = float(advance * projScale);
        result.offset.left  = float(bounds.l * projScale);
        result.offset.right = float(bounds.r * projScale);
        result.offset.top   = -float(bounds.t * projScale);
        result.offset.bottom= -float(bounds.b * projScale);

        ASSERT( result.offset.left < result.advance );
        ASSERT( result.offset.right <= result.advance * 1.1f );

        result.offset.Stretch2( float(_sdfBorderSize) );    // without padding

        // GlyphData
        result.symbol       = unicodeChar;
        result.dimension    = uint2{ Ceil((bounds.r - bounds.l) * projScale), Ceil((bounds.t - bounds.b) * projScale) } + border_px*2;

        proj_translate.x = -bounds.l + border_px / projScale;
        proj_translate.y = -bounds.b + border_px / projScale;

        const RectU region = RectU{result.dimension};

        CHECK_ERR( result.dimension.x <= bm_size );
        CHECK_ERR( result.dimension.y <= bm_size );

        switch_enum( _fontMode )
        {
            case ERasterFontMode::SDF :
            {
                Bitmap<float, 1>    sdf{ int(bm_size), int(bm_size) };
                generateSDF( OUT sdf, shape, Projection{proj_scale, proj_translate}, _sdfPixRange, config );

                const Bytes     bpp             {sizeof(float)};
                const Bytes     src_row_size    = bm_size * bpp;
                const Bytes     dst_row_size    = region.Width() * bpp;

                result.format   = EPixelFormat::R32F;
                result.dataSize = uint(dst_row_size * region.Height());
                result.data     = allocator.Allocate( Bytes{result.dataSize} );
                CHECK_ERR( result.data != null );

                const uint  h   = region.Height();
                const uint  top = bm_size - region.bottom;
                CHECK_ERR( top < bm_size and top + h <= bm_size );
                CHECK_ERR( region.right <= bm_size );

                for (uint y = 0; y < h; ++y)
                {
                    MemCopy( OUT result.data + (dst_row_size * y),
                             static_cast<float*>(sdf) + (src_row_size * (y + top)) + (bpp * region.left),
                             dst_row_size );
                }
                break;
            }

            case ERasterFontMode::MC_SDF :
            {
                edgeColoringSimple( shape, 3.0 );

                Bitmap<float, 3>    msdf{ int(bm_size), int(bm_size) };
                generateMSDF( OUT msdf, shape, Projection{proj_scale, proj_translate}, _sdfPixRange, config );

                const Bytes     bpp             {sizeof(float) * 3};
                const Bytes     src_row_size    = bm_size * bpp;
                const Bytes     dst_row_size    = region.Width() * bpp;

                result.format   = EPixelFormat::RGB32F;
                result.dataSize = uint(dst_row_size * region.Height());
                result.data     = allocator.Allocate( Bytes{result.dataSize} );
                CHECK_ERR( result.data != null );

                const uint  h   = region.Height();
                const uint  top = bm_size - region.bottom;
                CHECK_ERR( top < bm_size and top + h <= bm_size );
                CHECK_ERR( region.right <= bm_size );

                for (uint y = 0; y < h; ++y)
                {
                    MemCopy( OUT result.data + (dst_row_size * y),
                             static_cast<float*>(msdf) + (src_row_size * (y + top)) + (bpp * region.left),
                             dst_row_size );
                }
                break;
            }

            case ERasterFontMode::Raster :
            case ERasterFontMode::_Count :
            default :
                RETURN_ERR( "unsupported raster font mode for SDF generator" );
        }
        switch_end

        return true;
    }

/*
=================================================
    _SDFLoadGlyphs
=================================================
*/
    bool  ScriptRasterFont::_SDFLoadGlyphs (Allocator_t &allocator, OUT float &outSdfScale, OUT float &outSdfBias, OUT uint &outFontHeight, OUT Array<GlyphData> &result) const
    {
        using namespace msdfgen;

        ASSERT( result.empty() );
        outSdfScale     = 0.f;
        outFontHeight   = 0;

        const auto& fmt_info = EPixelFormat_GetInfo( _intermFormat );
        CHECK_ERR( AllBits( fmt_info.valueType, PixelFormatInfo::EType::SFloat )    or
                   AllBits( fmt_info.valueType, PixelFormatInfo::EType::SNorm )     or
                   AllBits( fmt_info.valueType, PixelFormatInfo::EType::UNorm ));

        switch_enum( _fontMode ) {
            case ERasterFontMode::SDF :     CHECK_ERR( fmt_info.channels == 1 );                            break;
            case ERasterFontMode::MC_SDF :  CHECK_ERR( fmt_info.channels == 3 or fmt_info.channels == 4 );  break;
            case ERasterFontMode::Raster :
            case ERasterFontMode::_Count :
            default :                       return false;
        }
        switch_end

        Allocator_t     tmp_alloc;      tmp_alloc.SetBlockSize( 16_Mb );
        MemRStream      mem_stream;
        {
            FileRStream     file {_fontFile};
            CHECK_ERR( file.IsOpen() );
            CHECK_ERR( mem_stream.LoadRemaining( file ));
        }

        FreetypeHandle* ft = initializeFreetype();
        CHECK_ERR( ft != null );

        ON_DESTROY( [&ft](){ deinitializeFreetype( ft ); });

        FontHandle* ft_font = loadFontData( ft, mem_stream.GetData().data(), int(mem_stream.GetData().size()) );
        CHECK_ERR( ft_font != null );

        ON_DESTROY( [&ft_font](){ destroyFont( ft_font ); });

        // calculate glyph scale
        double  proj_scale      = 1.0e+10;
        double  max_height      = 0.0;
        double2 min_max_advance {1.0e+10, 0.0};

        for (auto& cset : _charset)
        {
            for (uint c = cset.first; c <= cset.second; ++c)
            {
                if_unlikely( c == ' ' or c == '\t' )
                    continue;

                Shape   shape;
                double  advance = 0.0;

                if ( loadGlyph( OUT shape, ft_font, unicode_t(c), OUT &advance ))
                {
                    const auto  bounds  = shape.getBounds();
                    proj_scale          = Min( proj_scale, double(_bitmapHeight) / (bounds.r - bounds.l) );
                    proj_scale          = Min( proj_scale, double(_bitmapHeight) / (bounds.t - bounds.b) );
                    max_height          = Max( max_height, bounds.t );
                    min_max_advance.x   = Min( min_max_advance.x, advance );
                    min_max_advance.y   = Max( min_max_advance.y, advance );
                }
            }
        }
        //proj_scale = Floor( proj_scale * 8.0 ) / 8.0;
        CHECK_ERR( proj_scale > 0.0 );

        outFontHeight = uint(max_height * proj_scale + 0.5);

        // load glyphs
        for (auto& cset : _charset)
        {
            for (uint c = cset.first; c <= cset.second; ++c)
            {
                if_unlikely( c == ' ' or c == '\t' )
                {
                    Shape   shape;
                    double  advance = 0.0;
                    if ( not loadGlyph( OUT shape, ft_font, unicode_t(c), OUT &advance ))
                        advance = Lerp( min_max_advance.x, min_max_advance.y, 0.5 );

                    auto&   glyph = result.emplace_back();
                    glyph.advance   = float(advance * proj_scale * (c == '\t' ? _TabSize : 1));
                    glyph.symbol    = c;
                    continue;
                }

                GlyphData   glyph;
                if ( _SDFLoadGlyph( ft_font, c, proj_scale, tmp_alloc, OUT glyph ))
                    result.push_back( RVRef(glyph) );
            }
        }

        {
            // find min/max of the SDF
            float   min     = 1.0e+10f;
            float   max     = -1.0e+10f;
            float   bias    = 0.f;

            for (auto& glyph : result)
            {
                float*          ptr     = Cast<float>(glyph.data);
                const usize     count   = glyph.dataSize / sizeof(float);

                for (uint i = 0; i < count; ++i)
                {
                //  ptr[i] -= 0.5f; // 0.5 added inside msdfgen

                    min = Min( min, ptr[i] );
                    max = Max( max, ptr[i] );
                }
            }

            // convert float to unorm
            if ( AllBits( fmt_info.valueType, PixelFormatInfo::EType::UNorm ))
            {
                bias        = -min;
                outSdfBias  = min;
                outSdfScale = max - min;
            }
            else
            // convert float to snorm
            {
                outSdfBias  = Average( min, max );
                bias        = -outSdfBias;
                outSdfScale = Average( Abs(min), max );
            }

            const float scale   = 1.0f / outSdfScale;
            float       tx_min  = 1.0e+10f;
            float       tx_max  = -1.0e+10f;

            for (auto& glyph : result)
            {
                float*          ptr     = Cast<float>(glyph.data);
                const usize     count   = glyph.dataSize / sizeof(float);

                for (uint i = 0; i < count; ++i)
                {
                    float   src = ptr[i];
                    ptr[i] = (src + bias) * scale;

                  #ifdef AE_DEBUG
                    tx_min = Min( tx_min, ptr[i] );
                    tx_max = Max( tx_max, ptr[i] );

                    float   res = ptr[i] * outSdfScale + outSdfBias;
                    CHECK( Equal( res, src, 1.0e-5f ));
                  #endif
                }
            }

          #ifdef AE_DEBUG
            if ( AllBits( fmt_info.valueType, PixelFormatInfo::EType::UNorm )) {
                CHECK( tx_min >= 0.f );
                CHECK( tx_max <= 1.f );
            }else{
                CHECK( tx_min >= -1.f );
                CHECK( tx_max <=  1.f );
            }
          #endif
            Unused( tx_min, tx_max );
        }

        // convert float format to '_intermFormat'
        for (auto& glyph : result)
        {
            if ( glyph.data == null )
                continue;

            const uint3     dim     { glyph.dimension, 1 };
            RWImageMemView  src_view{ BufferMemView{ glyph.data, Bytes{glyph.dataSize} }, uint3{}, dim, 0_b, 0_b, glyph.format, EImageAspect::Color };

            glyph.dataSize  = glyph.dimension.x * glyph.dimension.y * (fmt_info.BitsPerPixel() / 8);
            glyph.data      = allocator.Allocate( Bytes{glyph.dataSize} );
            glyph.format    = _intermFormat;

            CHECK_ERR( glyph.data != null );

            RWImageMemView  dst_view{ BufferMemView{ glyph.data, Bytes{glyph.dataSize} }, uint3{}, dim, 0_b, 0_b, glyph.format, EImageAspect::Color };
            CHECK_ERR( dst_view.Blit( src_view ));
        }

        return true;
    }

} // AE::AssetPacker
#endif // AE_ENABLE_MSDFGEN
