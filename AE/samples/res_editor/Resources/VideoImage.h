// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/DataTransferQueue.h"

namespace AE::ResEditor
{

    //
    // Video Image
    //

    class alignas(AE_CACHE_LINE) VideoImage final : public IResource
    {
    // types
    private:
        static constexpr uint   _MaxCpuImages   = 8;
        static constexpr uint   _PosBits        = CT_IntLog2< _MaxCpuImages >;
        static constexpr uint   _MaxGpuImages   = 4;

        struct States
        {
            uint    emptyBits       : _MaxCpuImages;    // available for decoding
            uint    decodedBits     : _MaxCpuImages;    // available for uploading
            uint    pos             : _PosBits;         // current or next uploading

            States () : emptyBits{0xFF}, decodedBits{0}, pos{0} {}
        };

        StaticAssert( sizeof(States) == sizeof(uint) );
        StaticAssert( ToBit<uint>( _PosBits ) == _MaxCpuImages );

        using ImageArr_t    = StaticArray< Strong<ImageID>,     _MaxGpuImages >;
        using ViewArr_t     = StaticArray< Strong<ImageViewID>, _MaxGpuImages >;
        using Seconds_t     = Video::IVideoDecoder::Second_t;
        using FrameTimes_t  = StaticArray< Seconds_t,           _MaxCpuImages >;
        using MemArr_t      = StaticArray< ImageMemView,        _MaxCpuImages >;
        using Allocator_t   = LinearAllocator< UntypedAllocator, _MaxCpuImages, false >;    // use as block allocator


    // variables
    private:
        Atomic<uint>                _imageIdx   {0};    // used for '_ids', '_views'
        StructAtomic<States>        _states;            // used for '_frameTimes', '_imageMemView'
        FAtomic<double>             _curTime;           // Seconds_t

        ImageArr_t                  _ids;
        ViewArr_t                   _views;

        FrameTimes_t                _frameTimes;
        MemArr_t                    _imageMemView;
        Allocator_t                 _allocator;

        RC<DynamicDim>              _outDynSize;        // triggered when current image has been resized
        RC<Video::IVideoDecoder>    _decoder;
        uint2                       _dimension;
        ImageStream                 _stream;

        AsyncTask                   _lastDecoding;

        const String                _dbgName;


    // methods
    public:
        VideoImage (Renderer &          renderer,
                    const ImageDesc &   desc,
                    const VFS::FileName &filename,
                    RC<DynamicDim>      outDynSize,
                    StringView          dbgName)                __Th___;

        ~VideoImage ()                                          __NE_OV;

            bool  Resize (TransferCtx_t &)                      __Th_OV { return true; }
            bool  RequireResize ()                              C_Th_OV { return false; }

        ND_ ImageID         GetImageId ()                       C_NE___ { return _ids[ _CurrentIdx() ]; }
        ND_ ImageViewID     GetViewId ()                        C_NE___ { return _views[ _CurrentIdx() ]; }
        ND_ StringView      GetName ()                          C_NE___ { return _dbgName; }


    // IResource //
        EUploadStatus       Upload (TransferCtx_t &)            __Th_OV;
        EUploadStatus       Readback (TransferCtx_t &)          __Th_OV { return EUploadStatus::Canceled; }
        void                Cancel ()                           __NE_OV;

    private:
        ND_ uint            _CurrentIdx ()                      C_NE___ { return _imageIdx.load() % _MaxGpuImages; }

            static CoroTask _DecodeFrameTask (RC<VideoImage>)   __NE___;
        ND_ uint            _DecodeFrame ()                     __NE___;

            static void     _Validate (States)                  __NE___;
    };


} // AE::ResEditor
