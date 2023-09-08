// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/RasterFont.h"

namespace AE::Graphics
{
    using namespace AE::Serializing;
    using namespace AE::AssetPacker;

/*
=================================================
    destructor
=================================================
*/
    RasterFont::~RasterFont () __NE___
    {
        if ( _imageAndView )
            RenderTaskScheduler().GetResourceManager().DelayedReleaseResources( _imageAndView.image, _imageAndView.view );
    }

/*
=================================================
    ValidateHeight
=================================================
*/
    uint  RasterFont::ValidateHeight (float heightInPx) C_NE___
    {
        if_unlikely( _fontHeight.empty() )
            return 0;

        // SDF must have x2 lower resolution
        heightInPx *= IsSDF() ? 0.5f : 1.0f;

        if_unlikely( heightInPx <= float(_fontHeight.front()) )
            return _fontHeight.front();

        for (usize i = 1; i < _fontHeight.size(); ++i)
        {
            float   h1 = float(_fontHeight[i-1]);
            float   h2 = float(_fontHeight[i]);

            if_unlikely( heightInPx <= h2 )
            {
                return heightInPx < Lerp( h1, h2, 0.5f ) ?
                        _fontHeight[i-1] : _fontHeight[i];
            }
        }

        return _fontHeight.back();
    }

/*
=================================================
    ScreenPixRange
=================================================
*/
    float  RasterFont::ScreenPixRange (float heightInPx) C_NE___
    {
        ASSERT( IsSDF() );
        // = font_height / bitmap_size * pixel_range
        return heightInPx / float(ValidateHeight( heightInPx )) * _sdfConfig.pixRange2D;
    }

/*
=================================================
    GetGlyph
=================================================
*/
    RasterFont::Glyph const*  RasterFont::GetGlyph (CharUtf32 symbol, uint height) C_NE___
    {
        GlyphKey    key { symbol, height };
        auto        it  = _glyphMap.find( key );

        return it != _glyphMap.end() ? &it->second : null;
    }

/*
=================================================
    CalculateDimensions
=================================================
*/
    void  RasterFont::CalculateDimensions (const float2 &areaSizeInPix, INOUT PrecalculatedFormattedText &result) C_NE___
    {
        using Chunk = FormattedText::Chunk;

        float2      line_px;
        uint        max_height  = 0;

        result._size = float2{0.0f};
        result._lineHeight.clear();

        const auto  ToNextLine = [&] (Chunk const* chunk)
        {
            float   height  = result.Spacing() * max_height;
            result._lineHeight.push_back( height );

            result._size.x  = Max( result._size.x, line_px.x );
            line_px.y       += height;
            line_px.x       = 0.0f;
            max_height      = chunk ? chunk->height : 0;
        };

        for (auto* chunk = result.Text().GetFirst(); chunk; chunk = chunk->next)
        {
            const uint      font_h_px       = ValidateHeight( chunk->Height() );
            const float     font_scale_px   = chunk->Height() / font_h_px;

            max_height = Max( max_height, chunk->height );

            for (size_t pos = 0; pos < chunk->length;)
            {
                const CharUtf32     c = Utf8Decode( chunk->string, chunk->length, INOUT pos );

                if_unlikely( c == '\n' )
                {
                    ToNextLine( chunk );
                    continue;
                }

                auto*   glyph = GetGlyph( c, font_h_px );
                if_unlikely( glyph == null )
                    continue;

                const float width_px = glyph->advance * font_scale_px;

                if_unlikely( result.IsWordWrap() & (line_px.x + width_px > areaSizeInPix.x) )
                    ToNextLine( chunk );
                else
                    line_px.x += width_px;
            }
        }

        //if ( line_px.x > 0.0f )
        ToNextLine( null );

        result._size.y = Max( result._size.y, line_px.y );
    }

/*
=================================================
    operator =
=================================================
*/
    RasterFont&  RasterFont::operator = (RasterFont &&rhs) __NE___
    {
        _imageAndView   = RVRef(rhs._imageAndView);
        _glyphMap       = RVRef(rhs._glyphMap);
        _fontHeight     = rhs._fontHeight;
        _sdfConfig      = rhs._sdfConfig;

        return *this;
    }

/*
=================================================
    Loader::Load
=================================================
*/
    RC<RasterFont>  RasterFont::Loader::Load (RC<RStream> stream, ITransferContext &ctx, const GfxMemAllocatorPtr &alloc) __NE___
    {
        CHECK_ERR( stream and stream->IsOpen() );

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        RasterFontPacker    unpacker;
        {
            Serializing::Deserializer   des{ MakeRC<BufferedRStream>( stream )};
            CHECK_ERR( unpacker.Deserialize( des ));
        }
        ASSERT( unpacker.header.viewType == EImage_2D );

        auto    img_id = res_mngr.CreateImage( ImageDesc{}
                            .SetDimension( uint3{unpacker.header.dimension} ).SetArrayLayers( unpacker.header.arrayLayers ).SetMaxMipmaps( unpacker.header.mipmaps )
                            .SetType( unpacker.header.viewType ).SetFormat( unpacker.header.format ).SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ),
                            Default, alloc );
        CHECK_ERR( img_id );

        auto    view_id = res_mngr.CreateImageView( ImageViewDesc{ unpacker.header.viewType }, img_id, Default );
        CHECK_ERR( view_id );

        ImagePacker::ImageData  img_data;
        CHECK_ERR( unpacker.ReadImage( *stream, INOUT img_data ));

        // copy to staging buffer
        {
            ctx.ImageBarrier( img_id, EResourceState::Unknown, EResourceState::CopyDst );
            ctx.CommitBarriers();

            UploadImageDesc desc;
            desc.heapType   = EStagingHeapType::Dynamic;

            ImageMemView    dst_mem;
            ctx.UploadImage( img_id, desc, OUT dst_mem );

            CHECK_ERR( dst_mem.Copy( img_data.memView ));
        }

        auto    font = MakeRC<RasterFont>();

        font->_imageAndView = StrongImageAndViewID{ RVRef(img_id), RVRef(view_id) };
        font->_glyphMap     = RVRef(unpacker.glyphMap);
        font->_fontHeight   = unpacker.fontHeight;
        font->_sdfConfig    = unpacker.sdfConfig;

        return font;
    }

} // AE::Graphics
