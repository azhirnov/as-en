// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/ResourceQueue.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

/*
=================================================
    EnqueueForUpload
=================================================
*/
    void  ResourceQueue::EnqueueForUpload (RC<IResource> res)
    {
        if ( res and res->GetStatus() == EUploadStatus::InProgress )
        {
            EXLOCK( _upload.guard );
            _upload.queue.push_back( res );
            _upload.counter.fetch_add( 1 );
        }
    }

/*
=================================================
    EnqueueForReadback
=================================================
*/
    void  ResourceQueue::EnqueueForReadback (RC<IResource> res)
    {
        if ( res and res->GetStatus() == EUploadStatus::InProgress )
        {
            EXLOCK( _readback.guard );
            _readback.queue.push_back( res );
            _readback.counter.fetch_add( 1 );
        }
    }

/*
=================================================
    EnqueueImageTransition
=================================================
*/
    void  ResourceQueue::EnqueueImageTransition (ImageID id)
    {
        if ( not id )
            return;

        EXLOCK( _transitions.guard );
        _transitions.arr.push_back( id );
    }

/*
=================================================
    Upload
=================================================
*/
    AsyncTask  ResourceQueue::Upload (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps)
    {
        CHECK_ERR( batch );

        Array<ImageID>  img_arr;
        {
            EXLOCK( _transitions.guard );
            std::swap( img_arr, _transitions.arr );
        }

        if ( img_arr.empty() and _upload.counter.load() == 0 )
        {
            DEBUG_ONLY(
                EXLOCK( _upload.guard );
                ASSERT( _upload.queue.empty() );
            )
            return null;
        }

        {
            EXLOCK( _upload.guard );
            _upload.queue.AppendBack( _upload.nextFrameQueue );
            _upload.nextFrameQueue.clear();
        }

        return batch.Task(  [] (ResourceQueue *rq) -> RenderTaskCoro
                            {
                                auto&                   rtask   = co_await RenderTask_GetRef;
                                DirectCtx::Transfer     ctx     {rtask};

                                rq->_Upload( ctx );
                                co_await RenderTask_Execute( ctx );
                            }(this),
                            DebugLabel{"Upload"} )
                        .UseResources( ArrayView{img_arr} )     // from initial to default state
                        .Run( Tuple{deps} );
    }

/*
=================================================
    _Upload
=================================================
*/
    void  ResourceQueue::_Upload (TransferCtx_t &ctx)
    {
        const uint  max_uploads     = 100;
        const uint  max_low_mem     = 4;
        uint        low_mem         = 0;

        for (uint i = 0; i < max_uploads and low_mem < max_low_mem; ++i)
        {
            RC<IResource>   res;

            // extract
            {
                EXLOCK( _upload.guard );

                if ( _upload.queue.empty() )
                    return;

                res = _upload.queue.ExtractFront();
            }

            // upload
            {
                auto    status = res->Upload( ctx );

                BEGIN_ENUM_CHECKS();
                switch ( status )
                {
                    case EUploadStatus::Complete :
                    case EUploadStatus::Canceled :
                        _upload.counter.fetch_sub( 1 );
                        break;

                    case EUploadStatus::NoMemory :
                    {
                        ++low_mem;
                        EXLOCK( _upload.guard );
                        _upload.nextFrameQueue.push_back( RVRef(res) );
                        break;
                    }
                    case EUploadStatus::InProgress :
                    {
                        EXLOCK( _upload.guard );
                        _upload.queue.push_back( RVRef(res) );
                        break;
                    }
                }
                END_ENUM_CHECKS();
            }
        }
    }

/*
=================================================
    Readback
=================================================
*/
    AsyncTask  ResourceQueue::Readback (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps)
    {
        CHECK_ERR( batch );

        if ( _readback.counter.load() == 0 )
        {
            DEBUG_ONLY(
                EXLOCK( _readback.guard );
                ASSERT( _readback.queue.empty() );
            )
            return null;
        }

        {
            EXLOCK( _readback.guard );
            _readback.queue.AppendBack( _readback.nextFrameQueue );
            _readback.nextFrameQueue.clear();
        }

        return batch.Task(  [] (ResourceQueue *rq) -> RenderTaskCoro
                            {
                                auto&                   rtask   = co_await RenderTask_GetRef;
                                DirectCtx::Transfer     ctx     {rtask};

                                rq->_Readback( ctx );
                                co_await RenderTask_Execute( ctx );
                            }(this),
                            DebugLabel{"Readback"} )
                        //.UseResources( ArrayView{img_arr}, EResourceState::Invalidate, Default )
                        .Run( Tuple{deps} );
    }

/*
=================================================
    _Readback
=================================================
*/
    void  ResourceQueue::_Readback (TransferCtx_t &ctx)
    {
        const uint  max_readbacks   = 10;
        const uint  max_low_mem     = 4;
        uint        low_mem         = 0;

        for (uint i = 0; i < max_readbacks and low_mem < max_low_mem; ++i)
        {
            RC<IResource>   res;

            // extract
            {
                EXLOCK( _readback.guard );

                if ( _readback.queue.empty() )
                    return;

                res = _readback.queue.ExtractFront();
            }

            // upload
            {
                auto    status = res->Upload( ctx );

                BEGIN_ENUM_CHECKS();
                switch ( status )
                {
                    case EUploadStatus::Complete :
                    case EUploadStatus::Canceled :
                        _readback.counter.fetch_sub( 1 );
                        break;

                    case EUploadStatus::NoMemory :
                    {
                        ++low_mem;
                        EXLOCK( _readback.guard );
                        _readback.nextFrameQueue.push_back( RVRef(res) );
                        break;
                    }
                    case EUploadStatus::InProgress :
                    {
                        EXLOCK( _readback.guard );
                        _readback.queue.push_back( RVRef(res) );
                        break;
                    }
                }
                END_ENUM_CHECKS();
            }
        }
    }

/*
=================================================
    CancelAll
=================================================
*/
    void  ResourceQueue::CancelAll ()
    {
        const auto  CancelQueue = [] (auto& q)
        {{
            int count = 0;
            for (;; ++count)
            {
                RC<IResource>   res;

                // extract
                {
                    EXLOCK( q.guard );

                    if ( q.queue.empty() )
                        break;

                    res = q.queue.ExtractFront();
                }

                res->Cancel();
            }
            q.counter.fetch_sub( count );
        }};

        CancelQueue( _upload );
        CancelQueue( _readback );
    }


} // AE::ResEditor
