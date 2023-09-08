// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/ResourceQueue.h"

namespace AE::ResEditor
{

    //
    // Video Image
    //

    class VideoImage final : public IResource
    {
    // types
    public:
        static constexpr uint   _MaxImages = 4;

        using ImageArr_t    = StaticArray< Strong<ImageID>,     _MaxImages >;
        using ViewArr_t     = StaticArray< Strong<ImageViewID>, _MaxImages >;
        using Second_t      = Video::IVideoDecoder::Second_t;
        using FrameTimes_t  = StaticArray< Second_t,            _MaxImages >;


    // variables
    private:
        ImageArr_t                  _ids;
        ViewArr_t                   _views;
        FrameTimes_t                _frameTimes;
        Atomic<uint>                _imageIdx   {0};

        RC<DynamicDim>              _outDynSize;    // triggered when current image has been resized
        RC<Video::IVideoDecoder>    _decoder;
        uint2                       _dimension;
        ImageStream                 _stream;

        const String                _dbgName;


    // methods
    private:

    public:
        VideoImage (Renderer &          renderer,
                    const ImageDesc &   desc,
                    const Path &        path,
                    RC<DynamicDim>      outDynSize,
                    StringView          dbgName)            __Th___;

        ~VideoImage () override;

            bool  Resize (TransferCtx_t &)                  __Th_OV { return true; }
            bool  RequireResize ()                          C_Th_OV { return false; }

        ND_ ImageID         GetImageId ()                   C_NE___ { return _ids[ _CurrentIdx() ]; }
        ND_ ImageViewID     GetViewId ()                    C_NE___ { return _views[ _CurrentIdx() ]; }
        ND_ StringView      GetName ()                      C_NE___ { return _dbgName; }


    // IResource //
        EUploadStatus       Upload (TransferCtx_t &)        __Th_OV;
        EUploadStatus       Readback (TransferCtx_t &)      __Th_OV { return EUploadStatus::Canceled; }

    private:
        ND_ uint            _CurrentIdx ()                  C_NE___ { return _imageIdx.load() % _MaxImages; }
    };


} // AE::ResEditor