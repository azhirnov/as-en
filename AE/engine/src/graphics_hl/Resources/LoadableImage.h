// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/GraphicsHL.pch.h"

namespace AE::Graphics
{

    //
    // Loadable Image
    //

    class LoadableImage final : public EnableRC<LoadableImage>
    {
    // types
    public:
        struct AsyncLoader {
        //  ND_ Promise<RC<LoadableImage>>  Load (RC<AsyncRStream> stream) __NE___;
        };

        struct Loader {
            ND_ static RC<LoadableImage>  Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc) __NE___;

            ND_ static bool  _Load (RStream &stream, ImageID imageId, const void* hdr, ITransferContext &ctx) __NE___;
        };


    // variables
    private:
        Strong<ImageID>     _imageId;
        EImage              _viewType   = Default;


    // methods
    public:
        LoadableImage ()                            __NE___ {}
        ~LoadableImage ()                           __NE___;

        ND_ ImageID     GetImageID ()               C_NE___ { return _imageId; }
        ND_ EImage      ViewType ()                 C_NE___ { return _viewType; }

        ND_ auto        ReleaseImage ()             __NE___ { return Strong<ImageID>{ _imageId.Release() }; }
        ND_ auto        ReleaseImageAndView ()      __NE___ -> StrongImageAndViewID;
    };


} // AE::Graphics
