// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Resource Queue
    //

    class ResourceQueue final : public EnableRC<ResourceQueue>
    {
    // types
    private:
        using EUploadStatus = IResource::EUploadStatus;
        using TransferCtx_t = IResource::TransferCtx_t;

        struct alignas(AE_CACHE_LINE) Queue
        {
            Atomic<int>                     counter             {0};
            Atomic<ulong>                   framesWithoutWork   {0};
            SpinLock                        guard;
            RingBuffer< RC<IResource> >     queue;
            Array< RC<IResource> >          nextFrameQueue;
        };

        struct ImageTransitions
        {
            Mutex               guard;
            Array< ImageID >    arr;
        };


    // variables
    private:
        Atomic<uint>        _destroy;

        Queue               _upload;
        Queue               _readback;

        ImageTransitions    _transitions;


    // methods
    public:
        ResourceQueue ();
        ~ResourceQueue ();

        void  EnqueueForUpload (RC<IResource> res);
        void  EnqueueForReadback (RC<IResource> res);
        void  EnqueueImageTransition (ImageID id);

        void  CancelAll ()                                                              __NE___;

        ND_ AsyncTask   Upload (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps);
        ND_ AsyncTask   Readback (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps);

        ND_ ulong       UploadFramesWithoutWork ()                                      __NE___ { return _upload.framesWithoutWork.load(); }
        ND_ ulong       ReadbackFramesWithoutWork ()                                    __NE___ { return _readback.framesWithoutWork.load(); }

    private:
        static  void  _Upload (TransferCtx_t &, Queue &);
        static  void  _Readback (TransferCtx_t &, Queue &);
                void  _Enqueue (Queue &, RC<IResource> res) const;
    };


} // AE::ResEditor
