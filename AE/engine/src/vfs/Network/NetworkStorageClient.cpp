// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorageClient.h"

namespace AE::VFS
{
	using RWReqPromise_t = AsyncDSRequest::Value_t::Promise_t;

	INTERNAL_LINKAGE( Ptr<NetworkStorageClient>  s_NetVFS );
//-----------------------------------------------------------------------------



/*
=================================================
	_Cleanup
=================================================
*/
	void  NetworkStorageClient::_RequestBase::_Cleanup () __NE___
	{
		_actualSize.store( 0_b );

		ASSERT( AnyEqual( _status.load(), EStatus::Cancelled, EStatus::Completed ));
		_status.store( EStatus::Destroyed );

		DEBUG_ONLY({
			EXLOCK( _depsGuard );
			CHECK( _deps.empty() );
		})
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  NetworkStorageClient::NetReadRequest::Init (Bytes pos, Bytes size, void* data, RC<> mem) __NE___
	{
		ASSERT( _guard.is_locked() );

		_pos		= pos;
		_dataSize	= size;
		_memRC		= RVRef(mem);
		_data		= data;
		_partCount	= 0;
		_actualSize.store( 0_b );

		CHECK( _status.exchange( EStatus::InProgress ) == EStatus::Destroyed );
		return true;
	}

/*
=================================================
	Complete
=================================================
*/
	void  NetworkStorageClient::NetReadRequest::Complete (const Bytes inSize, const HashVal64 inHash) __NE___
	{
		ASSERT( _guard.is_locked() );

		// content of '_data' changed only in message processing thread
		//MemoryBarrier( EMemoryOrder::Acquire );

		const auto		hash		= XXHash64( _data, usize(_actualSize.load()) );
		const bool		complete	= (Bytes{_actualSize.load()} == inSize) and (inHash == hash) and (inSize > 0) and (inHash != HashVal64{0});
		const EStatus	stat		= _status.exchange( complete ? EStatus::Completed : EStatus::Cancelled );

		ASSERT( complete );
		ASSERT( stat == EStatus::InProgress );	Unused( stat );

		_SetDependencyCompleteStatus( complete );
	}

/*
=================================================
	Update
=================================================
*/
	void  NetworkStorageClient::NetReadRequest::Update (const ushort partIdx, const void* data, const Bytes size) __NE___
	{
		ASSERT( _guard.is_locked() );
		CHECK( partIdx == ushort(_partCount++) );	// TODO: skip?

		Bytes	pos = _actualSize.fetch_add( size );
		MemCopy( OUT _data + pos, data, size );

		// content of '_data' changed only in message processing thread
		//MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	GetResult
=================================================
*/
	NetworkStorageClient::NetReadRequest::Result  NetworkStorageClient::NetReadRequest::GetResult () C_NE___
	{
		SHAREDLOCK( _guard );
		ASSERT( IsFinished() );

		Result	res;
		res.pos			= _pos;
		res.dataSize	= _actualSize.load();
		res.data		= IsCompleted() ? _data : null;
		return res;
	}

/*
=================================================
	Cancel
=================================================
*/
	bool  NetworkStorageClient::NetReadRequest::Cancel () __NE___
	{
		// TODO ?
		return false;
	}

/*
=================================================
	_GetResult
=================================================
*/
	NetworkStorageClient::NetReadRequest::ResultWithRC  NetworkStorageClient::NetReadRequest::_GetResult () __NE___
	{
		SHAREDLOCK( _guard );
		ASSERT( IsFinished() );

		ResultWithRC	res;
		res.pos			= _pos;
		res.dataSize	= _actualSize.load();
		res.data		= IsCompleted() ? _data : null;
		res.rc			= _memRC;
		return res;
	}

/*
=================================================
	AsPromise
=================================================
*/
	RWReqPromise_t  NetworkStorageClient::NetReadRequest::AsPromise (Threading::ETaskQueue queueType) __NE___
	{
		auto	result = MakeDelayedPromise( [self = GetRC<NetReadRequest>()] () { return self->_GetResult(); }, "AsyncReadRequest", queueType );
		if_likely( Scheduler().Run( AsyncTask{result}, Tuple{GetRC()} ))
			return result;
		else
			return Default;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  NetworkStorageClient::NetReadRequest::_ReleaseObject () __NE___
	{
		EXLOCK( _guard );

		// TODO: cancel read request if not complete

		++_generation;
		_memRC = null;
		_Cleanup();

		s_NetVFS->_readResultPool.Unassign( this );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	AsyncWriteBlockTask::Run
=================================================
*/
	void  NetworkStorageClient::AsyncWriteBlockTask::Run () __Th___
	{
		// is alive
		{
			auto	req = s_NetVFS->_GetWriteReq( _id );
			if_unlikely( not req )
				return OnFailure();
		}

		for (uint i = 0; (i < _maxParts) and (_dataSize > _sent); ++i)
		{
			const Bytes	size	= Min( _dataSize - _sent, _partSize );
			auto		msg		= s_NetVFS->_CreateMsgOpt< CSMsg_VFS_WritePart >( size-1 );

			if_likely( msg )
			{
				msg->reqId	= _id;
				msg->size	= size;
				msg->index	= ushort(_partIdx);
				msg.Put( &CSMsg_VFS_WritePart::data, _data + _sent, size );

				if_likely( s_NetVFS->_AddMessage( msg ))
				{
					_sent += size;
					_partIdx ++;
					continue;
				}
			}
			break;
		}

		if ( _dataSize > _sent )
			return Continue();  // try again

		ASSERT( _dataSize == _sent );
		_memRC = null;

		// send completion message
		auto	msg = s_NetVFS->_CreateMsgOpt< CSMsg_VFS_WriteEnd >();
		if_likely( msg )
		{
			msg->reqId	= _id;
			msg->hash	= XXHash64( _data, usize(_dataSize) );
			msg->pos	= _pos;

			if_likely( s_NetVFS->_AddMessage( msg ))
			{
				return;  // complete
			}
		}

		return Continue();  // try again
	}

/*
=================================================
	AsyncWriteBlockTask::OnCancel
=================================================
*/
	void  NetworkStorageClient::AsyncWriteBlockTask::OnCancel () __NE___
	{
		DEBUG_ONLY( IAsyncTask::OnCancel();)

		_memRC = null;

		{
			Exclusive<NetWriteRequest>	req {s_NetVFS->_GetWriteReq( _id )};
			if_likely( req )
				req->Failed();
		}

		auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_WriteEnd >();
		CHECK_ERRV( msg );

		msg->reqId	= _id;
		msg->hash	= HashVal64{0};  // error
		msg->pos	= _pos;

		CHECK( s_NetVFS->_AddMessage( msg ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  NetworkStorageClient::NetWriteRequest::Init (AsyncTask task, Bytes pos) __NE___
	{
		ASSERT( _guard.is_locked() );
		ASSERT( task );

		_task	= RVRef(task);
		_pos	= pos;
		_actualSize.store( 0_b );

		CHECK( _status.exchange( EStatus::InProgress ) == EStatus::Destroyed );
		return true;
	}

/*
=================================================
	Complete
=================================================
*/
	void  NetworkStorageClient::NetWriteRequest::Complete (const Bytes written) __NE___
	{
		ASSERT( _guard.is_locked() );

		const bool		complete = written > 0_b;
		EStatus			exp		 = EStatus::InProgress;
		const EStatus	des		 = complete ? EStatus::Completed : EStatus::Cancelled;

		if ( complete )
			_actualSize.store( written );

		for (;;)
		{
			exp = EStatus::InProgress;

			if_likely( _status.CAS( INOUT exp, des ))
				break;

			if_likely( AnyEqual( exp, EStatus::Completed, EStatus::Cancelled ))
				break;  // already complete
		}

		// only one thread will execute it
		if ( exp == EStatus::InProgress )
		{
			// if used in message consumer
			if ( _task and not _task->IsFinished() )
			{
				ASSERT( not complete );
				Scheduler().Cancel( _task );
			}

			_task = null;
			_SetDependencyCompleteStatus( complete );
		}
	}

/*
=================================================
	GetResult
=================================================
*/
	NetworkStorageClient::NetWriteRequest::Result  NetworkStorageClient::NetWriteRequest::GetResult () C_NE___
	{
		SHAREDLOCK( _guard );
		ASSERT( IsFinished() );

		Result	res;
		res.pos			= _pos;
		res.dataSize	= _actualSize.load();
		res.data		= null;
		return res;
	}

/*
=================================================
	Cancel
=================================================
*/
	bool  NetworkStorageClient::NetWriteRequest::Cancel () __NE___
	{
		// TODO ?
		return false;
	}

/*
=================================================
	_GetResult
=================================================
*/
	NetworkStorageClient::NetWriteRequest::ResultWithRC  NetworkStorageClient::NetWriteRequest::_GetResult () __NE___
	{
		SHAREDLOCK( _guard );
		ASSERT( IsFinished() );

		ResultWithRC	res;
		res.pos			= _pos;
		res.dataSize	= _actualSize.load();
		res.data		= null;
		return res;
	}

/*
=================================================
	AsPromise
=================================================
*/
	RWReqPromise_t  NetworkStorageClient::NetWriteRequest::AsPromise (Threading::ETaskQueue queueType) __NE___
	{
		auto	result = MakeDelayedPromise( [self = GetRC<NetWriteRequest>()] () { return self->_GetResult(); }, "AsyncWriteRequest", queueType );
		if_likely( Scheduler().Run( AsyncTask{result}, Tuple{GetRC()} ))
			return result;
		else
			return Default;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  NetworkStorageClient::NetWriteRequest::_ReleaseObject () __NE___
	{
		EXLOCK( _guard );

		++_generation;
		_Cleanup();

		s_NetVFS->_writeResultPool.Unassign( this );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  NetworkStorageClient::NetRDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::RandomAccess | ESourceType::Async | ESourceType::FixedSize |
				ESourceType::ThreadSafe	| ESourceType::ReadAccess | ESourceType::DeferredOpen;
	}

/*
=================================================
	ReadBlock
=================================================
*/
	bool  NetworkStorageClient::NetRDataSource::_ReadBlockImpl (OUT ReadRequestPtr &outReq, const Bytes pos, void* data, const Bytes dataSize, RC<> mem) __NE___
	{
		CHECK_ERR( IsOpen() );

		auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_ReadRequest >();
		CHECK_ERR( msg );

		Index_t		idx;
		CHECK_ERR( s_NetVFS->_readResultPool.Assign( OUT idx ));

		RC<NetReadRequest>  req{ &s_NetVFS->_readResultPool[ idx ]};
		EXLOCK( req->Guard() );

		if_unlikely( not req->Init( pos, dataSize, data, RVRef(mem) ))
		{
			UNTESTED;
			s_NetVFS->_readResultPool.Unassign( idx );
			RETURN_ERR( "failed to init read request" );
		}

		msg->fileId	= ID();
		msg->reqId	= NDSRequestID{ idx, req->Generation() };
		msg->pos	= pos;
		msg->size	= dataSize;

		if_unlikely( not s_NetVFS->_AddMessage( msg ))
		{
			UNTESTED;
			s_NetVFS->_readResultPool.Unassign( idx );
			RETURN_ERR( "failed to add message" );
		}

		outReq = RVRef(req);
		return true;
	}

	AsyncDSRequest  NetworkStorageClient::NetRDataSource::ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
	{
		AsyncDSRequest	req;
		if_likely( _ReadBlockImpl( OUT req, pos, data, dataSize, RVRef(mem) ));
		else
			req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
		return req;
	}

	AsyncDSRequest  NetworkStorageClient::NetRDataSource::ReadBlock (Bytes pos, Bytes size) __NE___
	{
		RC<SharedMem>	mem = SharedMem::Create( AE::GetDefaultAllocator(), size );	// TODO: optimize, delayed allocation
		NonNull( mem );

		void*	data	= mem ? mem->Data() : null;
				size	= mem ? size : 0_b;

		return ReadBlock( pos, data, size, RVRef(mem) );
	}

/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  NetworkStorageClient::NetRDataSource::CancelAllRequests () __NE___
	{
		auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_CancelAllReadRequests >();
		CHECK_ERR( msg );

		msg->fileId = NetDataSourceID{ _index.load(), Generation() };

		CHECK_ERR( s_NetVFS->_AddMessage( msg ));
		return true;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  NetworkStorageClient::NetRDataSource::_ReleaseObject () __NE___
	{
		// close
		{
			auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_CloseReadFile >();
			if ( msg ) {
				msg->fileId = ID();
				Unused( s_NetVFS->_AddMessage( msg ));
			}
		}

		_generation.fetch_add( 1 );
		_open.store( EStatus::Initial );
		_fileSize.store( 0_b );

		s_NetVFS->_readDSPool.Unassign( _index.load() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  NetworkStorageClient::NetWDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::RandomAccess | ESourceType::WriteAccess |
				ESourceType::ThreadSafe | ESourceType::Async | ESourceType::DeferredOpen;
	}

/*
=================================================
	Alloc
=================================================
*/
	RC<SharedMem>  NetworkStorageClient::NetWDataSource::Alloc (const SizeAndAlign value) __NE___
	{
		return SharedMem::Create( AE::GetDefaultAllocator(), value );	// TODO: optimize
	}

/*
=================================================
	WriteBlock
=================================================
*/
	bool  NetworkStorageClient::NetWDataSource::_WriteBlockImpl (OUT WriteRequestPtr &outReq, Bytes pos, const void* data, const Bytes dataSize, RC<> mem) __NE___
	{
		CHECK_ERR( IsOpen() );

		auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_WriteBegin >();
		CHECK_ERR( msg );

		Index_t	idx;
		CHECK_ERR( s_NetVFS->_writeResultPool.Assign( OUT idx ));

		RC<NetWriteRequest>  req{ &s_NetVFS->_writeResultPool[ idx ]};
		EXLOCK( req->Guard() );

		auto	task = Scheduler().Run<AsyncWriteBlockTask>(
							Tuple{	NDSRequestID{ idx, req->Generation() },
									data,
									dataSize,
									pos,
									RVRef(mem) },
							Tuple{} );

		if_unlikely( not req->Init( RVRef(task), pos ))
		{
			UNTESTED;
			s_NetVFS->_writeResultPool.Unassign( idx );
			RETURN_ERR( "failed to init write request" );
		}

		msg->fileId	= ID();
		msg->reqId	= NDSRequestID{ idx, req->Generation() };
		msg->size	= dataSize;

		if_unlikely( not s_NetVFS->_AddMessage( msg ))
		{
			UNTESTED;
			s_NetVFS->_writeResultPool.Unassign( idx );
			RETURN_ERR( "failed to add message" );
		}

		outReq = RVRef(req);
		return true;
	}

	AsyncDSRequest  NetworkStorageClient::NetWDataSource::WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
	{
		AsyncDSRequest	req;
		if_likely( _WriteBlockImpl( OUT req, pos, data, dataSize, RVRef(mem) ));
		else
			req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
		return req;
	}

/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  NetworkStorageClient::NetWDataSource::CancelAllRequests () __NE___
	{
		auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_CancelAllWriteRequests >();
		CHECK_ERR( msg );

		msg->fileId = NetDataSourceID{ _index.load(), Generation() };

		CHECK( s_NetVFS->_AddMessage( msg ));
		return true;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  NetworkStorageClient::NetWDataSource::_ReleaseObject () __NE___
	{
		// close
		{
			auto	msg = s_NetVFS->_CreateMsg< CSMsg_VFS_CloseWriteFile >();
			if ( msg ) {
				msg->fileId = ID();
				Unused( s_NetVFS->_AddMessage( msg ));
			}
		}

		_generation.fetch_add( 1 );
		_open.store( EStatus::Initial );

		s_NetVFS->_writeDSPool.Unassign( _index.load() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	NetworkStorageClient::MsgConsumer::MsgConsumer (NetworkStorageClient &c) __NE___ :
		_client{c}
	{}

	NetworkStorageClient::NetworkStorageClient () __NE___ :
		_msgConsumer{ *this }
	{
		CHECK_FATAL( s_NetVFS == null );
		s_NetVFS = this;
	}

/*
=================================================
	destructor
=================================================
*/
	NetworkStorageClient::~NetworkStorageClient () __NE___
	{
		s_NetVFS = null;
	}

/*
=================================================
	Init
=================================================
*/
	bool  NetworkStorageClient::Init (StringView prefix) __NE___
	{
		auto	msg = _CreateMsg< CSMsg_VFS_Init >( StringSizeOf( prefix ));
		CHECK_ERR( msg );

		msg.Put( &CSMsg_VFS_Init::prefix, prefix );
		msg->size = ubyte(prefix.size());

		return _AddMessage( msg );
	}

/*
=================================================
	_OpenForReadResult
=================================================
*/
	inline void  NetworkStorageClient::_OpenForReadResult (CSMsg_VFS_OpenForReadResult const& msg) __NE___
	{
		auto*	ds = _GetReadDS( msg.fileId );
		CHECK_ERRV( ds != null );

		if ( msg.ok ){
			ds->_fileSize.store( msg.size );
			CHECK( ds->_open.exchange( EStatus::Open ) == EStatus::Pending );
		}else{
			CHECK( ds->_open.exchange( EStatus::Failed ) == EStatus::Pending );
			AE_LOGI( "OpenForReadResult: fail" );
		}
	}

/*
=================================================
	_OpenForWriteResult
=================================================
*/
	inline void  NetworkStorageClient::_OpenForWriteResult (CSMsg_VFS_OpenForWriteResult const& msg) __NE___
	{
		auto*	ds = _GetWriteDS( msg.fileId );
		CHECK_ERRV( ds != null );

		if ( msg.ok ){
			CHECK( ds->_open.exchange( EStatus::Open ) == EStatus::Pending );
		}else{
			CHECK( ds->_open.exchange( EStatus::Failed ) == EStatus::Pending );
			AE_LOGI( "OpenForWriteResult: fail" );
		}
	}

/*
=================================================
	_CloseReadFile / _CloseWriteFile
=================================================
*/
	inline void  NetworkStorageClient::_CloseReadFile (CSMsg_VFS_CloseReadFile const& msg) __NE___
	{
		auto*	ds = _GetReadDS( msg.fileId );
		if ( ds != null ) {
			CHECK( AnyEqual( ds->_open.exchange( EStatus::Closed ), EStatus::Pending, EStatus::Open ));	// TODO: CAS
		}
	}

	inline void  NetworkStorageClient::_CloseWriteFile (CSMsg_VFS_CloseWriteFile const& msg) __NE___
	{
		auto*	ds = _GetWriteDS( msg.fileId );
		if ( ds != null ) {
			CHECK( AnyEqual( ds->_open.exchange( EStatus::Closed ), EStatus::Pending, EStatus::Open ));	// TODO: CAS
		}
	}

/*
=================================================
	_ReadResult
=================================================
*/
	inline void  NetworkStorageClient::_ReadResult (CSMsg_VFS_ReadResult const& msg) __NE___
	{
		Exclusive<NetReadRequest>	req {_GetReadReq( msg.reqId )};
		CHECK_ERRV( req );

		req->Update( msg.index, msg.data, msg.size );
	}

/*
=================================================
	_ReadComplete
=================================================
*/
	inline void  NetworkStorageClient::_ReadComplete (CSMsg_VFS_ReadComplete const& msg) __NE___
	{
		Exclusive<NetReadRequest>	req {_GetReadReq( msg.reqId )};
		CHECK_ERRV( req );

		req->Complete( msg.size, msg.hash );
	}

/*
=================================================
	_WriteComplete
=================================================
*/
	inline void  NetworkStorageClient::_WriteComplete (CSMsg_VFS_WriteComplete const& msg) __NE___
	{
		Exclusive<NetWriteRequest>	req {_GetWriteReq( msg.reqId )};
		CHECK_ERRV( req );

		req->Complete( msg.size );
	}

/*
=================================================
	OpenForRead
=================================================
*/
	auto  NetworkStorageClient::OpenForRead (FileName::Ref name) __NE___ -> RC<AsyncRDataSource>
	{
		auto	msg = _CreateMsg< CSMsg_VFS_OpenForReadRequest >();
		CHECK_ERR( msg );

		Index_t	idx;
		CHECK_ERR( _readDSPool.Assign( OUT idx ));

		auto&	file = _readDSPool[idx];

		file._open.store( EStatus::Pending );
		file._index.store( idx );

		msg->fileId	= NetDataSourceID{ idx, file.Generation() };
		msg->name	= FileName::Optimized_t{name};

		CHECK_ERR( _AddMessage( msg ));

		return file.GetRC<AsyncRDataSource>();
	}

/*
=================================================
	OpenForWrite
=================================================
*/
	auto  NetworkStorageClient::OpenForWrite (FileName::Ref name) __NE___ -> RC<AsyncWDataSource>
	{
		auto	msg = _CreateMsg< CSMsg_VFS_OpenForWriteRequest >();
		CHECK_ERR( msg );

		Index_t	idx;
		CHECK_ERR( _writeDSPool.Assign( OUT idx ));

		auto&	file = _writeDSPool[idx];

		file._open.store( EStatus::Pending );
		file._index.store( idx );

		msg->fileId	= NetDataSourceID{ idx, file.Generation() };
		msg->name	= FileName::Optimized_t{name};

		CHECK_ERR( _AddMessage( msg ));

		return file.GetRC<AsyncWDataSource>();
	}

/*
=================================================
	_GetReadReq / _GetWriteReq
=================================================
*/
	auto  NetworkStorageClient::_GetReadReq (NDSRequestID id) __NE___ -> Shared<NetReadRequest>
	{
		auto*	req = _readResultPool.At( id.Index() );
		if_likely( req != null )
		{
			req->Guard().lock_shared();

			if_likely( req->Generation() == id.Generation() )
				return Shared{ req };

			req->Guard().unlock_shared();
		}
		return Default;
	}

	auto  NetworkStorageClient::_GetWriteReq (NDSRequestID id) __NE___ -> Shared<NetWriteRequest>
	{
		auto*	req = _writeResultPool.At( id.Index() );
		if_likely( req != null )
		{
			req->Guard().lock_shared();

			if_likely( req->Generation() == id.Generation() )
				return Shared{ req };

			req->Guard().unlock_shared();
		}
		return Default;
	}

/*
=================================================
	_GetReadDS / _GetWriteDS
=================================================
*/
	auto  NetworkStorageClient::_GetReadDS (NetDataSourceID id) __NE___ -> NetRDataSource*
	{
		auto*	ds = _readDSPool.At( id.Index() );

		if_likely( ds != null and ds->Generation() == id.Generation() )
			return ds;

		return null;
	}

	auto  NetworkStorageClient::_GetWriteDS (NetDataSourceID id) __NE___ -> NetWDataSource*
	{
		auto*	ds = _writeDSPool.At( id.Index() );

		if_likely( ds != null and ds->Generation() == id.Generation() )
			return ds;

		return null;
	}

/*
=================================================
	MsgConsumer::Consume
=================================================
*/
	void  NetworkStorageClient::MsgConsumer::Consume (ChunkList<const CSMessagePtr> msgList) __NE___
	{
		for (auto& msg : msgList)
		{
			ASSERT( msg->GroupId() == CSMessageGroup::NetVFS );

			switch ( msg->UniqueId() )
			{
				#define CASE( _name_ )		case CSMsg_VFS_ ## _name_::UID :	_client._ ## _name_( *msg->As< CSMsg_VFS_ ## _name_ >() );	break;
				CASE( OpenForReadResult )
				CASE( OpenForWriteResult )
				CASE( CloseReadFile )
				CASE( CloseWriteFile )
				CASE( ReadResult )
				CASE( ReadComplete )
				CASE( WriteComplete )
				default :	DBG_WARNING( "unknown message id" ); break;
				#undef CASE
			}
		}
	}


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
