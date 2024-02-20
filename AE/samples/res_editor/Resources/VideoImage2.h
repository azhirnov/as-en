// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/DataTransferQueue.h"

namespace AE::ResEditor
{

    //
    // Video Image (YUV)
    //

    class alignas(AE_CACHE_LINE) VideoImage2 final : public IResource
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

        using Seconds_t     = Video::IVideoDecoder::Second_t;
        using ImageArr_t    = StaticArray< Strong<VideoImageID>, _MaxGpuImages >;
        using FrameTimes_t  = StaticArray< Seconds_t,           _MaxCpuImages >;
        using MemArr_t      = StaticArray< Video::IVideoDecoder::ImageMemViewArr, _MaxCpuImages >;
        using Allocator_t   = LinearAllocator< UntypedAllocator, _MaxCpuImages, false >;    // use as block allocator
        using StreamArr_t   = FixedArray< VideoImageStream, 3 >;


    // variables
    private:
        Atomic<uint>                _imageIdx   {0};    // used for '_ids', '_views'
        StructAtomic<States>        _states;            // used for '_frameTimes', '_imageMemView'
        FAtomic<double>             _curTime;           // Seconds_t

        ImageArr_t                  _ids;

        FrameTimes_t                _frameTimes;
        MemArr_t                    _imageMemView;
        Allocator_t                 _allocator;

        RC<DynamicDim>              _outDynSize;        // triggered when current image has been resized
        RC<Video::IVideoDecoder>    _decoder;
        uint2                       _dimension;
        StreamArr_t                 _streamArr;

        AsyncTask                   _lastDecoding;

        const String                _dbgName;


    // methods
    public:
        VideoImage2 (Renderer &             renderer,
                     const ImageDesc &      desc,
                     const VFS::FileName    &filename,
                     RC<DynamicDim>         outDynSize,
                     const SamplerName      &ycbcrConversion,
                     PipelinePackID         packId,
                     StringView             dbgName)            __Th___;

        ~VideoImage2 ()                                         __NE_OV;

            bool  Resize (TransferCtx_t &)                      __Th_OV { return true; }
            bool  RequireResize ()                              C_Th_OV { return false; }

        ND_ VideoImageID    GetVideoImageId ()                  C_NE___ { return _ids[ _CurrentIdx() ]; }
        ND_ StringView      GetName ()                          C_NE___ { return _dbgName; }


    // IResource //
        EUploadStatus       Upload (TransferCtx_t &)            __Th_OV;
        EUploadStatus       Readback (TransferCtx_t &)          __Th_OV { return EUploadStatus::Canceled; }
        void                Cancel ()                           __NE_OV;

    private:
        ND_ uint            _CurrentIdx ()                      C_NE___ { return _imageIdx.load() % _MaxGpuImages; }

            static CoroTask _DecodeFrameTask (RC<VideoImage2>)  __NE___;
        ND_ uint            _DecodeFrame ()                     __NE___;

            static void     _Validate (States)                  __NE___;
    };


} // AE::ResEditor
