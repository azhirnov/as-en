// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceManager.h"
#include "graphics/Public/CommandBuffer.h"

namespace AE::Graphics
{

    //
    // Resource Upload Manager
    //

    class ResourceUploadManager
    {
    // types
    public:
        struct BufferUploadResult
        {
            BufferID        id;
            Bytes           offset;
            Bytes           size;

            BufferUploadResult ()                                   __NE___ = default;
            BufferUploadResult (BufferUploadResult &&)              __NE___ = default;

            BufferUploadResult&  operator = (BufferUploadResult &&) __NE___ = default; 
        };

        struct ImageUploadResult
        {
            ImageID     id;
            uint3       offset;
            uint3       dimension;

            ImageUploadResult ()                                    __NE___ = default;
            ImageUploadResult (ImageUploadResult &&)                __NE___ = default;

            ImageUploadResult&  operator = (ImageUploadResult &&)   __NE___ = default; 
        };

        using BufferResult      = Promise< BufferUploadResult >;
        using ImageResult       = Promise< ImageUploadResult >;
        using TransferContext_t = ITransferContext;


    private:
        //using AsyncReader = Threading::AsyncRStreamBufferedReader;

        // Disk -> RAM -> VRAM

        template <typename StreamType, typename ResultType>
        struct UploadTask
        {
        //  AsyncReader     diskToMem;
            StreamType      memToGPU;
            ResultType      result;
        };

        using BufferTask_t          = UploadTask< BufferStream, BufferResult >;
        using BufferUploadTasks_t   = Array< Unique< BufferTask_t >>;   // TODO: block allocator

        using ImageTask_t           = UploadTask< ImageStream, ImageResult >;
        using ImageUploadTasks_t    = Array< Unique< ImageTask_t >>;

        static constexpr uint   MaxFailed = 4;


    // variables
    private:
        ImageUploadTasks_t      _imageUpload;
        BufferUploadTasks_t     _bufferUpload;
        RC<IAllocator>          _allocator;


    // methods
    public:
        ResourceUploadManager ()                                                                                                                    __NE___;
        ~ResourceUploadManager ()                                                                                                                   __NE___;

        ND_ ImageResult  ReadImage (ImageID imageId, const uint2 &imageOffset, const uint2 &dim, RC<RStream> stream, const Range<Bytes> &fileRange) __NE___;
        //ND_ ImageResult  UploadImage ()                                                                                                           __NE___;

        ND_ BufferResult  ReadBuffer (BufferID bufferId, const Range<Bytes> &bufferRange, RC<RStream> stream, const Range<Bytes> &fileRange)        __NE___;
        ND_ BufferResult  ReadBuffer (BufferID bufferId, RC<RStream> stream, const Range<Bytes> &fileRange)                                         __NE___;
        //ND_ BufferResult  UploadBuffer ()                                                                                                         __NE___;


        void  Upload (TransferContext_t &ctx, EStagingHeapType heapType)                                                                            __NE___
        {
            _UploadImages( ctx, heapType );
            _UploadBuffers( ctx, heapType );
        }

    private:
        void  _UploadImages (TransferContext_t &ctx, EStagingHeapType heapType)                                                                     __NE___;
        void  _UploadBuffers (TransferContext_t &ctx, EStagingHeapType heapType)                                                                    __NE___;
    };


} // AE::Graphics
