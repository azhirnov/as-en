// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/DataTransferQueue.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	DataTransferQueue::DataTransferQueue () __NE___ :
		_destroy{0}
	{
		{
			EXLOCK( _upload.guard );
			_upload.queue.reserve( 64 );
		}{
			EXLOCK( _readback.guard );
			_readback.queue.reserve( 64 );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	DataTransferQueue::~DataTransferQueue ()
	{
		_destroy.store( 1 );
		CancelAll();
	}

/*
=================================================
	_Enqueue
=================================================
*/
	void  DataTransferQueue::EnqueueForUpload (RC<IResource> res)	{ _Enqueue( _upload, RVRef(res) ); }
	void  DataTransferQueue::EnqueueForReadback (RC<IResource> res)	{ _Enqueue( _readback, RVRef(res) ); }

	void  DataTransferQueue::_Enqueue (Queue &q, RC<IResource> res) const
	{
		if ( _destroy.load() == 0							and
			 res											and
			 res->GetStatus() == EUploadStatus::InProgress )
		{
			EXLOCK( q.guard );
			q.queue.push_back( res );
			q.counter.fetch_add( 1 );
			q.framesWithoutWork.store( 0 );
		}
	}

/*
=================================================
	EnqueueImageTransition
=================================================
*/
	void  DataTransferQueue::EnqueueImageTransition (ImageID id)
	{
		if ( not id )
			return;

		EXLOCK( _transitions.guard );
		_transitions.arr.push_back( id );
	}

	void  DataTransferQueue::EnqueueImageTransition (VideoImageID id)
	{
		if ( not id )
			return;

		auto*	res = GraphicsScheduler().GetResourceManager().GetResource( id );
		if ( res != null )
			EnqueueImageTransition( res->GetImageID() );
	}

/*
=================================================
	Upload
=================================================
*/
	AsyncTask  DataTransferQueue::Upload (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps)
	{
		CHECK_ERR( batch );

		if ( _destroy.load() != 0 )
			return null;

		auto&			q		= _upload;
		Array<ImageID>	img_arr;
		{
			EXLOCK( _transitions.guard );
			std::swap( img_arr, _transitions.arr );
		}

		if ( img_arr.empty() and q.counter.load() == 0 )
		{
			DEBUG_ONLY(
				EXLOCK( q.guard );
				ASSERT( q.queue.empty() );
				ASSERT( q.nextFrameQueue.empty() );
			)
			q.framesWithoutWork.fetch_add( 1 );
			return null;
		}

		{
			EXLOCK( q.guard );
			q.queue.AppendBack( q.nextFrameQueue );
			q.nextFrameQueue.clear();
		}

		// TODO: multiple tasks
		return batch.Task(	[] (RC<DataTransferQueue> dtq) -> RenderTaskCoro
							{
								auto&					rtask	= co_await RenderTask_GetRef;
								DirectCtx::Transfer		ctx		{rtask};

								_Upload( ctx, dtq->_upload );
								co_await RenderTask_Execute( ctx );
							}( GetRC() ),
							DebugLabel{"Upload"} )
						.UseResources( ArrayView{img_arr} )		// from initial to default state
						.Run( Tuple{deps} );
	}

/*
=================================================
	_Upload
=================================================
*/
	void  DataTransferQueue::_Upload (TransferCtx_t &ctx, Queue &q)
	{
		const uint	max_uploads		= 100;
		const uint	max_low_mem		= 4;
		uint		low_mem			= 0;
		uint		i				= 0;

		for (; i < max_uploads and low_mem < max_low_mem; ++i)
		{
			RC<IResource>	res;

			// extract
			{
				EXLOCK( q.guard );

				if ( q.queue.empty() )
					break;

				res = q.queue.ExtractFront();
			}

			// upload
			{
				auto	status = EUploadStatus::Failed;
				NOTHROW( status = res->Upload( ctx ));

				switch_enum( status )
				{
					case EUploadStatus::Failed :
						res->Cancel();

					case EUploadStatus::Canceled :
					case EUploadStatus::Completed :
					{
						int		cnt = q.counter.Sub( 1 );
						ASSERT( cnt >= 0 );
						Unused( cnt );
						break;
					}
					case EUploadStatus::NoMemory :
					{
						++low_mem;
						EXLOCK( q.guard );
						q.nextFrameQueue.push_back( RVRef(res) );
						break;
					}
					case EUploadStatus::InProgress :
					{
						EXLOCK( q.guard );
						q.nextFrameQueue.push_back( RVRef(res) );
						break;
					}
				}
				switch_end
			}
		}

		if ( i > 0 )
			q.framesWithoutWork.store( 0 );
	}

/*
=================================================
	Readback
=================================================
*/
	AsyncTask  DataTransferQueue::Readback (RG::CommandBatchPtr batch, ArrayView<AsyncTask> deps)
	{
		CHECK_ERR( batch );

		if ( _destroy.load() != 0 )
			return null;

		auto&	q = _readback;

		if ( q.counter.load() == 0 )
		{
			DEBUG_ONLY(
				EXLOCK( q.guard );
				ASSERT( q.queue.empty() );
				ASSERT( q.nextFrameQueue.empty() );
			)
			q.framesWithoutWork.fetch_add( 1 );
			return null;
		}

		{
			EXLOCK( q.guard );
			q.queue.AppendBack( q.nextFrameQueue );
			q.nextFrameQueue.clear();
		}

		// TODO: multiple tasks
		return batch.Task(	[] (RC<DataTransferQueue> dtq) -> RenderTaskCoro
							{
								auto&					rtask	= co_await RenderTask_GetRef;
								DirectCtx::Transfer		ctx		{rtask};

								_Readback( ctx, dtq->_readback );
								co_await RenderTask_Execute( ctx );
							}( GetRC() ),
							DebugLabel{"Readback"} )
						//.UseResources( ArrayView{img_arr}, EResourceState::Invalidate, Default )
						.Run( Tuple{deps} );
	}

/*
=================================================
	_Readback
=================================================
*/
	void  DataTransferQueue::_Readback (TransferCtx_t &ctx, Queue &q)
	{
		const uint	max_readbacks	= 10;
		const uint	max_low_mem		= 4;
		uint		low_mem			= 0;
		uint		i				= 0;

		for (; i < max_readbacks and low_mem < max_low_mem; ++i)
		{
			RC<IResource>	res;

			// extract
			{
				EXLOCK( q.guard );

				if ( q.queue.empty() )
					break;

				res = q.queue.ExtractFront();
			}

			// upload
			{
				auto	status = EUploadStatus::Failed;
				NOTHROW( status = res->Readback( ctx ));

				switch_enum( status )
				{
					case EUploadStatus::Failed :
						res->Cancel();

					case EUploadStatus::Completed :
					case EUploadStatus::Canceled :
					{
						int		cnt = q.counter.Sub( 1 );
						ASSERT( cnt >= 0 );
						Unused( cnt );
						break;
					}
					case EUploadStatus::NoMemory :
					{
						++low_mem;
						EXLOCK( q.guard );
						q.nextFrameQueue.push_back( RVRef(res) );
						break;
					}
					case EUploadStatus::InProgress :
					{
						EXLOCK( q.guard );
						q.nextFrameQueue.push_back( RVRef(res) );
						break;
					}
				}
				switch_end
			}
		}

		if ( i > 0 )
			q.framesWithoutWork.store( 0 );
	}

/*
=================================================
	CancelAll
=================================================
*/
	void  DataTransferQueue::CancelAll () __NE___
	{
		const auto	CancelQueue = [] (auto& q)
		{{
			int	count = 0;

			for (;; ++count)
			{
				RC<IResource>	res;

				// extract
				{
					EXLOCK( q.guard );

					if ( q.queue.empty() )
						break;

					res = q.queue.ExtractFront();
				}

				res->Cancel();
			}

			for (;; ++count)
			{
				RC<IResource>	res;

				// extract
				{
					EXLOCK( q.guard );

					if ( q.nextFrameQueue.empty() )
						break;

					res = q.nextFrameQueue.back();
					q.nextFrameQueue.pop_back();
				}

				res->Cancel();
			}

			int		cnt = q.counter.Sub( count );
			ASSERT( cnt >= 0 );
			Unused( cnt );


			EXLOCK( q.guard );
			ASSERT( q.queue.empty() );
			ASSERT( q.nextFrameQueue.empty() );

			q.queue.clear();
			q.nextFrameQueue.clear();
		}};

		CancelQueue( _upload );
		CancelQueue( _readback );
	}


} // AE::ResEditor
