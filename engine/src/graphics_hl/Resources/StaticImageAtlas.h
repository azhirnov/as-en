// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/GraphicsHL.pch.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{
    class ITransferContext;


    //
    // Static Image Atlas
    //

    class StaticImageAtlas final : public EnableRC<StaticImageAtlas>
    {
    // types
    public:
        using Rect_t        = AssetPacker::ImageAtlasPacker::Rect_t;
        using ImageMap_t    = AssetPacker::ImageAtlasPacker::ImageMap_t;
        using ImageRects_t  = AssetPacker::ImageAtlasPacker::ImageRects_t;

        struct AsyncLoader {
        /// ND_ Promise<RC<StaticImageAtlas>>  Load (RC<AsyncRStream> stream) __NE___;
        };

        struct Loader {
            ND_ RC<StaticImageAtlas>  Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc) __NE___;
        };


    // variables
    private:
        StrongImageAndViewID    _imageAndView;
        float2                  _invImgSize     {1};
        ImageMap_t              _nameToIdx;
        ImageRects_t            _imageRects;


    // methods
    public:
        StaticImageAtlas ()                                         __NE___ {}
        ~StaticImageAtlas ()                                        __NE___;

        bool  Get (const ImageInAtlasName &name, OUT RectI &region) C_NE___;
        bool  Get (const ImageInAtlasName &name, OUT RectF &region) C_NE___;

        ND_ ImageID         GetImageID ()                           C_NE___ { return _imageAndView.image; }
        ND_ ImageViewID     GetViewID ()                            C_NE___ { return _imageAndView.view; }
        ND_ ImageAndViewID  GetImageAndView ()                      C_NE___ { return _imageAndView; }
    };


} // AE::Graphics
