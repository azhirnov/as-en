// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/GraphicsCreateInfo.h"
#include "graphics_hl/Resources/FormattedText.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{
    class ITransferContext;


    //
    // Raster Font
    //

    class RasterFont final : public EnableRC<RasterFont>
    {
    // types
    public:
        using Glyph         = AssetPacker::RasterFontPacker::Glyph;
        using SDFConfig     = AssetPacker::RasterFontPacker::SDFConfig;
        using GlyphKey      = AssetPacker::RasterFontPacker::GlyphKey;
        using GlyphMap_t    = AssetPacker::RasterFontPacker::GlyphMap_t;
        using SizeArr_t     = AssetPacker::RasterFontPacker::SizeArr_t;

        struct AsyncLoader {
        //  ND_ Promise<RC<RasterFont>>  Load (RC<AsyncRStream> stream) __NE___;
        };

        struct Loader {
            ND_ RC<RasterFont>  Load (RC<RStream> stream, ITransferContext &ctx, const GfxMemAllocatorPtr &alloc) __NE___;
        };


    // variables
    private:
        StrongImageAndViewID    _imageAndView;
        SDFConfig               _sdfConfig;
        GlyphMap_t              _glyphMap;
        SizeArr_t               _fontHeight;


    // methods
    public:
        RasterFont ()                                                                               __NE___ {}
        ~RasterFont ()                                                                              __NE___;

        RasterFont&  operator = (RasterFont &&)                                                     __NE___;

        ND_ uint  ValidateHeight (float heightInPx)                                                 C_NE___;

        ND_ Glyph const*  GetGlyph (CharUtf32 symbol, uint height)                                  C_NE___;

        void  CalculateDimensions (const float2 &areaSizeInPix, INOUT PrecalculatedFormattedText &) C_NE___;

        ND_ float  ScreenPixRange (float heightInPx)                                                C_NE___;    // 2D SDF only

        ND_ bool                IsSDF ()                                                            C_NE___ { return _sdfConfig.scale != 0.f; }
        ND_ ImageID             GetImageID ()                                                       C_NE___ { return _imageAndView.image; }
        ND_ ImageViewID         GetViewID ()                                                        C_NE___ { return _imageAndView.view; }
        ND_ ImageAndViewID      GetImageAndView ()                                                  C_NE___ { return _imageAndView; }
        ND_ SDFConfig const&    GetSDFConfig ()                                                     C_NE___ { return _sdfConfig; }
    };


} // AE::Graphics
