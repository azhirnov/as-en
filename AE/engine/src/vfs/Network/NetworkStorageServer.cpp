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
            auto        msg     = s_NetVFS->_CreateMsgOpt< CSMsg_VFS_ReadResult >( _clientId, size-1 );

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
        auto    msg = s_NetVFS->_CreateMsgOpt< CSMsg_VFS_ReadComplete >( _clientId );
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

        auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_ReadComplete >( _clientId );
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
    NetworkStorageServer::ClientData::ClientData () __NE___ :
        readDSPool{ new NetRDataSource[_FileCount] },
        writeDSPool{ new NetWDataSource[_FileCount] },
        writeReqPool{ new NetWriteRequest[_ReqCount] }
    {}

/*
=================================================
    destructor
=================================================
*/
    NetworkStorageServer::ClientData::~ClientData () __NE___
    {
    #ifdef AE_DEBUG
        if ( readDSPool )
        {
            for (uint i = 0; i < _FileCount; ++i)
            {
                CHECK( readDSPool[i].gen == 0 );
                CHECK( readDSPool[i].ds == null );
            }
        }
        if ( writeDSPool )
        {
            for (uint i = 0; i < _FileCount; ++i)
            {
                CHECK( writeDSPool[i].gen == 0 );
                CHECK( writeDSPool[i].ds == null );
            }
        }
        if ( writeReqPool )
        {
            for (uint i = 0; i < _ReqCount; ++i)
            {
                CHECK( writeReqPool[i].gen == 0 );
            }
        }
    #endif
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
    NetworkStorageServer::~NetworkStorageServer () __NE___
    {
        s_NetVFS = null;
    }

/*
=================================================
    AddStorage
=================================================
*/
    bool  NetworkStorageServer::AddStorage (RC<IVirtualFileStorage> storage) __NE___
    {
        CHECK_ERR( not _isImmutable.load() );
        CHECK_ERR( storage );

        _storageArr.push_back( storage );

        NOTHROW_ERR( storage->_Append( INOUT _globalMap ));
        return true;
    }

/*
=================================================
    AddFolder
=================================================
*/
    bool  NetworkStorageServer::AddFolder (Path folder) __NE___
    {
        CHECK_ERR( not _isImmutable.load() );

        if ( not folder.is_absolute() )
            folder = FileSystem::ToAbsolute( folder );

        CHECK_ERR( FileSystem::IsDirectory( folder ));

        _folders.push_back( RVRef(folder) );
        return true;
    }

    bool  NetworkStorageServer::AddFolders (ArrayView<Path> folders) __NE___
    {
        CHECK_ERR( not _isImmutable.load() );

        for (Path folder : folders)
        {
            if ( not folder.is_absolute() )
                folder = FileSystem::ToAbsolute( folder );

            CHECK_ERR( FileSystem::IsDirectory( folder ));

            _folders.push_back( RVRef(folder) );
        }
        return true;
    }

/*
=================================================
    _OpenForRead
=================================================
*/
    bool  NetworkStorageServer::_OpenForRead (OUT RC<AsyncRDataSource> &result, FileName::Ref name) __NE___
    {
        CHECK_ERR( _isImmutable.load() );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
            {
                ASSERT( iter->second.storage != null );
                ASSERT( iter->second.ref != null );

                DEBUG_ONLY(
                    bool    found = false;
                    for (auto& st : _storageArr) {
                        found |= (st.get() == iter->second.storage);
                    }
                    ASSERT_MSG( found, "memory corruption - iterator contains pointer to storage which is not exists" );
                )

                return iter->second.storage->_OpenByIter( OUT result, name, iter->second.ref );
            }
        }

        // search in all storages
        for (auto& st : _storageArr)
        {
            if_unlikely( st->Open( OUT result, name ))
                return true;
        }

        #if not AE_OPTIMIZE_IDS
        DBG_WARNING( "Failed to open VFS file '"s << name.GetName() << "'" );
        #endif
        return false;
    }

/*
=================================================
    _OpenForWrite
=================================================
*/
    bool  NetworkStorageServer::_OpenForWrite (OUT RC<AsyncWDataSource> &result, FileName::Ref name) __NE___
    {
        CHECK_ERR( _isImmutable.load() );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
            {
                ASSERT( iter->second.storage != null );
                ASSERT( iter->second.ref != null );

                DEBUG_ONLY(
                    bool    found = false;
                    for (auto& st : _storageArr) {
                        found |= (st.get() == iter->second.storage);
                    }
                    ASSERT_MSG( found, "memory corruption - iterator contains pointer to storage which is not exists" );
                )

                return iter->second.storage->_OpenByIter( OUT result, name, iter->second.ref );
            }
        }

        // search in all storages
        for (auto& st : _storageArr)
        {
            if_unlikely( st->Open( OUT result, name ))
                return true;
        }

        #if not AE_OPTIMIZE_IDS
        DBG_WARNING( "Failed to open VFS file '"s << name.GetName() << "'" );
        #endif
        return false;
    }

/*
=================================================
    AddClient
=================================================
*/
    bool  NetworkStorageServer::AddClient (EClientLocalID clientId) __NE___
    {
        CHECK_ERR( clientId != Default );
        CHECK_ERR( not _clients.contains( clientId ));

        _clients.insert_or_assign( clientId, ClientData{} );
        return true;
    }

/*
=================================================
    RemoveClient
=================================================
*/
    void  NetworkStorageServer::RemoveClient (EClientLocalID clientId) __NE___
    {
        _clients.EraseByKey( clientId );
    }

/*
=================================================
    _Init
=================================================
*/
    void  NetworkStorageServer::_Init (CSMsg_VFS_Init const& inMsg) __NE___
    {
        CHECK_ERRV( not _isImmutable.load() );

        StringView  prefix { inMsg.prefix, usize{inMsg.size} };

        for (auto& path : _folders)
        {
            auto    storage = VirtualFileStorageFactory::CreateStaticFolder( path, prefix );
            CHECK_ERRV( AddStorage( RVRef(storage) ));
        }

        // TODO: per client
        CHECK( not _isImmutable.exchange( true ));
    }

/*
=================================================
    _OpenForReadRequest
=================================================
*/
    inline void  NetworkStorageServer::_OpenForReadRequest (CSMsg_VFS_OpenForReadRequest const& inMsg) __NE___
    {
        CHECK_ERRV( inMsg.fileId.Index() < _FileCount );

        auto    it = _clients.find( inMsg.ClientId() );
        CHECK_ERRV( it != _clients.end() );

        auto&   dst     = it->second.readDSPool[ inMsg.fileId.Index() ];
        auto    out_msg = _CreateMsg< CSMsg_VFS_OpenForReadResult >( inMsg.ClientId() );
        CHECK_ERRV( out_msg );

        out_msg->ok     = false;
        out_msg->fileId = inMsg.fileId;

        [this, &inMsg, &out_msg, &dst] ()
        {{
            RC<AsyncRDataSource>    ds;
            CHECK_ERRV( _OpenForRead( OUT ds, FileName{inMsg.name} ));

            out_msg->ok     = true;
            out_msg->size   = ds->Size();

            ASSERT( dst.gen == 0 );
            dst.gen = inMsg.fileId.Generation();
            dst.ds  = RVRef(ds);
        }}();

        if_unlikely( not _AddMessage( out_msg ))
        {
            // cleanup
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

        auto    it = _clients.find( inMsg.ClientId() );
        CHECK_ERRV( it != _clients.end() );

        auto&   dst     = it->second.writeDSPool[ inMsg.fileId.Index() ];
        auto    out_msg = _CreateMsg< CSMsg_VFS_OpenForWriteResult >( inMsg.ClientId() );
        CHECK_ERRV( out_msg );

        out_msg->ok     = false;
        out_msg->fileId = inMsg.fileId;

        [this, &inMsg, &out_msg, &dst] ()
        {{
            RC<AsyncWDataSource>    ds;
            CHECK_ERRV( _OpenForWrite( OUT ds, FileName{inMsg.name} ));

            out_msg->ok = true;

            ASSERT( dst.gen == 0 );
            dst.gen = inMsg.fileId.Generation();
            dst.ds  = RVRef(ds);
        }}();

        if_unlikely( not _AddMessage( out_msg ))
        {
            // cleanup
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
        auto*   dst = _GetReadDS( msg.ClientId(), msg.fileId );
        if_likely( dst != null )
        {
            ASSERT( dst->ds );
            dst->ds  = null;
            dst->gen = 0;
        }
    }

    inline void  NetworkStorageServer::_CloseWriteFile (CSMsg_VFS_CloseWriteFile const& msg) __NE___
    {
        auto*   dst = _GetWriteDS( msg.ClientId(), msg.fileId );
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
        auto*   dst = _GetReadDS( msg.ClientId(), msg.fileId );
        if_likely( dst != null )
        {
            dst->ds->CancelAllRequests();
        }
    }

    inline void  NetworkStorageServer::_CancelAllWriteRequests (CSMsg_VFS_CancelAllWriteRequests const& msg) __NE___
    {
        auto*   dst = _GetWriteDS( msg.ClientId(), msg.fileId );
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
        auto*   dst = _GetReadDS( inMsg.ClientId(), inMsg.fileId );

        if_unlikely( dst == null )
        {
            AE_LOGI( "failed to find file" );

            auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_ReadComplete >( inMsg.ClientId() );
            CHECK_ERRV( msg );

            msg->reqId  = inMsg.reqId;
            msg->size   = 0_b;  // error
            msg->hash   = HashVal64{0};

            CHECK( s_NetVFS->_AddMessage( msg ));
            return;
        }

        auto    req = dst->ds->ReadBlock( inMsg.pos, inMsg.size );

        Scheduler().Run<SendReadResultTask>( Tuple{ inMsg.reqId, inMsg.ClientId(), req }, Tuple{req} );
    }

/*
=================================================
    _WriteRequestComplete
=================================================
*/
    inline void  NetworkStorageServer::_WriteRequestComplete (NDSRequestID reqId, Bytes written, EClientLocalID clientId) __NE___
    {
        auto    msg = s_NetVFS->_CreateMsg< CSMsg_VFS_WriteComplete >( clientId );
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
        bool    ok = [&]() -> bool
        {
            auto*   dst = _GetWriteDS( inMsg.ClientId(), inMsg.fileId );
            CHECK_ERR( dst != null );

            auto    mem = dst->ds->Alloc( inMsg.size );
            CHECK_ERR( mem )

            auto*   req = _GetWriteReq( inMsg.ClientId(), NDSRequestID{ inMsg.reqId.Index(), 0 });
            CHECK_ERR( req != null );

            req->gen        = inMsg.reqId.Generation();
            req->dst        = RVRef(mem);
            req->recv       = 0_b;
            req->partIdx    = 0;
            req->fileId     = inMsg.fileId;
            return true;
        }();

        if ( not ok )
            _WriteRequestFailed( inMsg.reqId, inMsg.ClientId() );
    }

/*
=================================================
    _WritePart
=================================================
*/
    inline void  NetworkStorageServer::_WritePart (CSMsg_VFS_WritePart const& inMsg) __NE___
    {
        auto*   req = _GetWriteReq( inMsg.ClientId(), inMsg.reqId );

        if_unlikely( req == null                            or
                     ushort(req->partIdx) != inMsg.index    or  // invalid order
                     not req->dst                           or
                     req->recv + inMsg.size > req->dst->Size() )
        {
            _WriteRequestFailed( inMsg.reqId, inMsg.ClientId() );
            return;
        }

        MemCopy( OUT req->dst->Data() + req->recv, inMsg.data, inMsg.size );
        req->partIdx ++;
        req->recv += inMsg.size;
    }

/*
=================================================
    _WriteEnd
=================================================
*/
    inline void  NetworkStorageServer::_WriteEnd (CSMsg_VFS_WriteEnd const& inMsg) __NE___
    {
        auto*   req = _GetWriteReq( inMsg.ClientId(), inMsg.reqId );
        if_unlikely( req == null )
        {
            _WriteRequestFailed( inMsg.reqId, inMsg.ClientId() );
            return;
        }

        const HashVal64 hash = XXHash64( req->dst->Data(), usize{req->dst->Size()} );
        auto*           file = _GetWriteDS( inMsg.ClientId(), req->fileId );

        if_unlikely( hash != inMsg.hash or
                     file == null       or
                     file->ds == null )
        {
            _WriteRequestFailed( inMsg.reqId, inMsg.ClientId() );
            return;
        }

        auto    p = file->ds->WriteBlock( inMsg.pos, req->recv, RVRef(req->dst) )->AsPromise( ETaskQueue::Background );

        p.Then( [req_id = inMsg.reqId, cid = inMsg.ClientId()] (const AsyncWDataSource::Result_t &res)
                {
                    if ( s_NetVFS )
                        s_NetVFS->_WriteRequestComplete( req_id, res.dataSize, cid );
                });
        p.Except( [req_id = inMsg.reqId, cid = inMsg.ClientId()] ()
                {
                    if ( s_NetVFS )
                        s_NetVFS->_WriteRequestFailed( req_id, cid );
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
                CASE( Init )
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
    NetworkStorageServer::NetRDataSource*  NetworkStorageServer::_GetReadDS (EClientLocalID cid, NetDataSourceID id) __NE___
    {
        auto    it = _clients.find( cid );

        if_likely( id.Index() < _FileCount and it != _clients.end() )
        {
            auto&   dst = it->second.readDSPool[ id.Index() ];
            ASSERT( dst.gen == id.Generation() );
            return dst.gen == id.Generation() ? &dst : null;
        }
        return null;
    }

    NetworkStorageServer::NetWDataSource*  NetworkStorageServer::_GetWriteDS (EClientLocalID cid, NetDataSourceID id) __NE___
    {
        auto    it = _clients.find( cid );

        if_likely( id.Index() < _FileCount and it != _clients.end() )
        {
            auto&   dst = it->second.writeDSPool[ id.Index() ];
            ASSERT( dst.gen == id.Generation() );
            return dst.gen == id.Generation() ? &dst : null;
        }
        return null;
    }

/*
=================================================
    _GetWriteReq
=================================================
*/
    NetworkStorageServer::NetWriteRequest*  NetworkStorageServer::_GetWriteReq (EClientLocalID cid, NDSRequestID id) __NE___
    {
        auto    it = _clients.find( cid );

        if_likely( id.Index() < _ReqCount and it != _clients.end() )
        {
            auto&   dst = it->second.writeReqPool[ id.Index() ];
            ASSERT( dst.gen == id.Generation() );
            return dst.gen == id.Generation() ? &dst : null;
        }
        return null;
    }


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
