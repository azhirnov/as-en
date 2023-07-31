// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Resource Queue
    //

    class ResourceQueue final
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
        Queue               _upload;
        Queue               _readback;

        ImageTransitions    _transitions;


    // methods
    public:
        void  EnqueueForUpload (RC<IResource> res);
        void  EnqueueForReadback (RC<IResource> res);
        void  EnqueueImageTransition (ImageID id);

        void  CancelAll ();

        ND_ AsyncTask   Upload (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps);
        ND_ AsyncTask   Readback (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps);

        ND_ ulong       UploadFramesWithoutWork ()              { return _upload.framesWithoutWork.load(); }
        ND_ ulong       ReadbackFramesWithoutWork ()            { return _readback.framesWithoutWork.load(); }

    private:
        void  _Upload (TransferCtx_t &);
        void  _Readback (TransferCtx_t &);
    };


} // AE::ResEditor
