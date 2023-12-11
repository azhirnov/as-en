// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorageServer.h"

namespace AE::VFS
{
    INTERNAL_LINKAGE( Ptr<NetworkStorageServer>  s_NetVFS );

//-----------------------------------------------------------------------------



/*
=================================================
    SendReadResultTask::Run
=================================================
*/
    void  NetworkStorageServer::SendReadResultTask::Run () __Th___
    {
        auto    res = _req->GetResult();

        for (uint i = 0; (i < _maxParts) and (res.dataSize > _sent); ++i)
        {
            const Bytes size    = Min( res.dataSize - _sent, _partSize );
            auto        msg     = s_NetVFS->_CreateMsgOpt< CSMsg_VFS_ReadResult >( size-1 );

            if_likely( msg )
            {
                msg->reqId  = _id;
                msg->size   = size;
                msg->index  = ushort(_partIdx);
                MemCopy( OUT msg->data, res.data + _sent, size );

                if_likely( s_NetVFS->_AddMessage( msg ))
                {
                    _sent += size;
                    _partIdx ++;
                    continue;
                }
            }
            break;
        }

        if ( res.dataSize > _sent )
            return Continue();  // try again

        ASSERT( res.dataSize == _sent );

        // complete
        auto    msg = s_NetVFS->_CreateMsgOpt< CSMsg_VFS_ReadComplete >();
        if_likely( msg )
        {
            msg->reqId  = _id;
            msg->size   = res.dataSize;
            msg->hash   = XXHash64( res.data, usize(res.dataSize) );

            if_likely( s_NetVFS->_AddMessage( msg ))
            {
                _req = null;
                return;  // complete
            }
        }

        return Continue();  // try again
    }

/*
=================================================
    SendReadResultTask::OnCancel
=================================================
*/
    void  NetworkStorageServer::SendReadResultTask::OnCancel () __NE___
    {
        DEBUG_ONLY( IAsyncTask::OnCancel();)
        AE_LOGI( "failed to read data" );

        _req = null;

        auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_ReadComplete >();
        CHECK_ERRV( msg );

        msg->reqId  = _id;
        msg->size   = 0_b;  // error
        msg->hash   = HashVal64{0};

        CHECK( s_NetVFS->_AddMessage( msg ));
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    NetworkStorageServer::MsgConsumer::MsgConsumer (NetworkStorageServer &s) __NE___ :
        _server{s}
    {}

    NetworkStorageServer::NetworkStorageServer () __NE___ :
        _msgConsumer{ *this },
        _readDSPool{ new NetRDataSource[_FileCount] },
        _writeDSPool{ new NetWDataSource[_FileCount] },
        _writeReqPool{ new NetWriteRequest[_ReqCount] }
    {
        CHECK_FATAL( s_NetVFS == null );
        s_NetVFS = this;
    }

/*
=================================================
    destructor
=================================================
*/
    NetworkStorageServer::~NetworkStorageServer () __NE___
    {
        if ( _readDSPool )
        {
            for (uint i = 0; i < _FileCount; ++i)
            {
                CHECK( _readDSPool[i].gen == 0 );
                CHECK( _readDSPool[i].ds == null );
            }
        }
        if ( _writeDSPool )
        {
            for (uint i = 0; i < _FileCount; ++i)
            {
                CHECK( _writeDSPool[i].gen == 0 );
                CHECK( _writeDSPool[i].ds == null );
            }
        }

        s_NetVFS = null;
    }

/*
=================================================
    _OpenForReadRequest
=================================================
*/
    inline void  NetworkStorageServer::_OpenForReadRequest (CSMsg_VFS_OpenForReadRequest const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.fileId.Index() < _FileCount );

        auto    out_msg = _CreateMsg< CSMsg_VFS_OpenForReadResult >();
        CHECK_ERRV( out_msg );

        out_msg->ok     = false;
        out_msg->fileId = inMsg.fileId;

        [this, &inMsg, &out_msg] ()
        {{
            auto    ds  = GetVFS().Open<AsyncRDataSource>( FileName{inMsg.name} );
            CHECK_ERRV( ds );

            out_msg->ok     = true;
            out_msg->size   = ds->Size();

            auto&   dst = _readDSPool[ inMsg.fileId.Index() ];
            ASSERT( dst.gen == 0 );
            dst.gen = inMsg.fileId.Generation();
            dst.ds  = RVRef(ds);
        }}();

        if_unlikely( not _AddMessage( out_msg ))
        {
            // cleanup
            auto&   dst = _readDSPool[ inMsg.fileId.Index() ];
            dst.ds = null;

            DBG_WARNING( "failed to add message" );
        }
    }

/*
=================================================
    _OpenForWriteRequest
=================================================
*/
    inline void  NetworkStorageServer::_OpenForWriteRequest (CSMsg_VFS_OpenForWriteRequest const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.fileId.Index() < _FileCount );

        auto    out_msg = _CreateMsg< CSMsg_VFS_OpenForWriteResult >();
        CHECK_ERRV( out_msg );

        out_msg->ok     = false;
        out_msg->fileId = inMsg.fileId;

        [this, &inMsg, &out_msg] ()
        {{
            auto    ds  = GetVFS().Open<AsyncWDataSource>( FileName{inMsg.name} );
            CHECK_ERRV( ds );

            out_msg->ok = true;

            auto&   dst = _writeDSPool[ inMsg.fileId.Index() ];
            ASSERT( dst.gen == 0 );
            dst.gen = inMsg.fileId.Generation();
            dst.ds  = RVRef(ds);
        }}();

        if_unlikely( not _AddMessage( out_msg ))
        {
            // cleanup
            auto&   dst = _writeDSPool[ inMsg.fileId.Index() ];
            dst.ds = null;

            DBG_WARNING( "failed to add message" );
        }
    }

/*
=================================================
    _CloseReadFile / _CloseWriteFile
=================================================
*/
    inline void  NetworkStorageServer::_CloseReadFile (CSMsg_VFS_CloseReadFile const& msg) __NE___
    {
        auto*   dst = _GetReadDS( msg.fileId );
        if_likely( dst != null )
        {
            ASSERT( dst->ds );
            dst->ds  = null;
            dst->gen = 0;
        }
    }

    inline void  NetworkStorageServer::_CloseWriteFile (CSMsg_VFS_CloseWriteFile const& msg) __NE___
    {
        auto*   dst = _GetWriteDS( msg.fileId );
        if_likely( dst != null )
        {
            // write request will keep ref to 'ds' until complete
            ASSERT( dst->ds );
            dst->ds  = null;
            dst->gen = 0;
        }
    }

/*
=================================================
    _CancelAllReadRequests / _CancelAllWriteRequests
=================================================
*/
    inline void  NetworkStorageServer::_CancelAllReadRequests (CSMsg_VFS_CancelAllReadRequests const& msg) __NE___
    {
        auto*   dst = _GetReadDS( msg.fileId );
        if_likely( dst != null )
        {
            dst->ds->CancelAllRequests();
        }
    }

    inline void  NetworkStorageServer::_CancelAllWriteRequests (CSMsg_VFS_CancelAllWriteRequests const& msg) __NE___
    {
        auto*   dst = _GetWriteDS( msg.fileId );
        if_likely( dst != null )
        {
            dst->ds->CancelAllRequests();
        }
    }

/*
=================================================
    _ReadRequest
=================================================
*/
    inline void  NetworkStorageServer::_ReadRequest (CSMsg_VFS_ReadRequest const& inMsg) __NE___
    {
        auto*   dst = _GetReadDS( inMsg.fileId );

        if_unlikely( dst == null )
        {
            AE_LOGI( "failed to find file" );

            auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_ReadComplete >();
            CHECK_ERRV( msg );

            msg->reqId  = inMsg.reqId;
            msg->size   = 0_b;  // error
            msg->hash   = HashVal64{0};

            CHECK( s_NetVFS->_AddMessage( msg ));
            return;
        }

        auto    req = dst->ds->ReadBlock( inMsg.pos, inMsg.size );

        Scheduler().Run<SendReadResultTask>( Tuple{ inMsg.reqId, req }, Tuple{req} );
    }

/*
=================================================
    _WriteRequestComplete
=================================================
*/
    inline void  NetworkStorageServer::_WriteRequestComplete (NDSRequestID reqId, Bytes written) __NE___
    {
        auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_WriteComplete >();
        CHECK_ERRV( msg );

        msg->reqId  = reqId;
        msg->size   = written;

        CHECK( s_NetVFS->_AddMessage( msg ));
    }

/*
=================================================
    _WriteBegin
=================================================
*/
    inline void  NetworkStorageServer::_WriteBegin (CSMsg_VFS_WriteBegin const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.fileId.Index() < _FileCount );
        CHECK_ERRV( inMsg.reqId.Index()  < _ReqCount );

        auto*   dst = _GetWriteDS( inMsg.fileId );
        if_unlikely( dst == null )
        {
            AE_LOGI( "failed to find file" );
            _WriteRequestFailed( inMsg.reqId );
            return;
        }

        auto    mem = dst->ds->Alloc( inMsg.size );
        if_unlikely( not mem )
        {
            AE_LOGI( "failed to allocate dst memory" );
            _WriteRequestFailed( inMsg.reqId );
            return;
        }

        auto&   req = _writeReqPool[ inMsg.reqId.Index() ];
        ASSERT( req.gen == 0 );

        req.gen     = inMsg.reqId.Generation();
        req.dst     = RVRef(mem);
        req.recv    = 0_b;
        req.partIdx = 0;
        req.fileId  = inMsg.fileId;
    }

/*
=================================================
    _WritePart
=================================================
*/
    inline void  NetworkStorageServer::_WritePart (CSMsg_VFS_WritePart const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.reqId.Index() < _ReqCount );

        auto&   req = _writeReqPool[ inMsg.reqId.Index() ];

        if_unlikely( req.gen != inMsg.reqId.Generation()    or
                     ushort(req.partIdx) != inMsg.index     or  // invalid order
                     not req.dst                            or
                     req.recv + inMsg.size > req.dst->Size() )
        {
            _WriteRequestFailed( inMsg.reqId );
            return;
        }

        MemCopy( OUT req.dst->Data() + req.recv, inMsg.data, inMsg.size );
        req.partIdx ++;
        req.recv += inMsg.size;
    }

/*
=================================================
    _WriteEnd
=================================================
*/
    inline void  NetworkStorageServer::_WriteEnd (CSMsg_VFS_WriteEnd const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.reqId.Index() < _ReqCount );

        auto&   req = _writeReqPool[ inMsg.reqId.Index() ];
        CHECK_ERRV( req.gen == inMsg.reqId.Generation() );

        const HashVal64 hash = XXHash64( req.dst->Data(), usize{req.dst->Size()} );
        auto*           file = _GetWriteDS( req.fileId );

        if_unlikely( hash != inMsg.hash or
                     file == null       or
                     file->ds == null )
        {
            _WriteRequestFailed( inMsg.reqId );
            return;
        }

        file->ds->WriteBlock( inMsg.pos, req.recv, RVRef(req.dst) )->AsPromise( ETaskQueue::Background )
            .Then(  [req_id = inMsg.reqId] (const AsyncWDataSource::Result_t &res)
                    {
                        if ( s_NetVFS )
                            s_NetVFS->_WriteRequestComplete( req_id, res.dataSize );
                    });
    }

/*
=================================================
    MsgConsumer::Consume
=================================================
*/
    void  NetworkStorageServer::MsgConsumer::Consume (ChunkList<const CSMessagePtr> msgList) __NE___
    {
        for (auto& msg : msgList)
        {
            ASSERT( msg->GroupId() == CSMessageGroup::NetVFS );
            switch ( msg->UniqueId() )
            {
                #define CASE( _name_ )      case CSMsg_VFS_ ## _name_::UID :    _server._ ## _name_( *msg->As< CSMsg_VFS_ ## _name_ >() );  break;
                CASE( OpenForReadRequest )
                CASE( OpenForWriteRequest )
                CASE( CloseReadFile )
                CASE( CloseWriteFile )
                CASE( CancelAllReadRequests )
                CASE( CancelAllWriteRequests )
                CASE( ReadRequest )
                CASE( WriteBegin )
                CASE( WritePart )
                CASE( WriteEnd )
                default :   DBG_WARNING( "unknown message id" ); break;
                #undef CASE
            }
        }
    }

/*
=================================================
    _GetReadDS / _GetWriteDS
=================================================
*/
    NetworkStorageServer::NetRDataSource*  NetworkStorageServer::_GetReadDS (NetDataSourceID id) __NE___
    {
        if_likely( id.Index() < _FileCount )
        {
            auto&   dst = _readDSPool[ id.Index() ];
            ASSERT( dst.gen == id.Generation() );
            return dst.gen == id.Generation() ? &dst : null;
        }
        return null;
    }

    NetworkStorageServer::NetWDataSource*  NetworkStorageServer::_GetWriteDS (NetDataSourceID id) __NE___
    {
        if_likely( id.Index() < _FileCount )
        {
            auto&   dst = _writeDSPool[ id.Index() ];
            ASSERT( dst.gen == id.Generation() );
            return dst.gen == id.Generation() ? &dst : null;
        }
        return null;
    }


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
