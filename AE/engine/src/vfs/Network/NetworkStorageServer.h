// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorageBase.h"

namespace AE::VFS
{

    //
    // Network Storage Server
    //
    class NetworkStorageServer final : public NetworkStorageBase
    {
    // types
    private:
        struct NetWriteRequest
        {
            RC<SharedMem>           dst;
            DSGen_t                 gen {0};
            Bytes32u                recv;
            uint                    partIdx {0};
            NetDataSourceID         fileId;
        };

        struct NetRDataSource
        {
            DSGen_t                 gen {0};
            RC<AsyncRDataSource>    ds;
        };

        struct NetWDataSource
        {
            DSGen_t                 gen {0};
            RC<AsyncWDataSource>    ds;
        };

        using WRequestPool_t        = Unique< NetWriteRequest []>;  // [_ReqCount]
        using RDataSourcePool_t     = Unique< NetRDataSource []>;   // [_FileCount]
        using WDataSourcePool_t     = Unique< NetWDataSource []>;   // [_FileCount]


        class MsgProducer final :
            public AsyncCSMessageProducer< LfLinearAllocator< usize{NetConfig::ChannelStorageSize * 4}, usize{8_b}, NetConfig::TCP_Reliable_MaxClients/2 >>
        {
        public:
            EnumSet<EChannel>  GetChannels ()   C_NE_OV { return {EChannel::Reliable}; }
        };


        class MsgConsumer final : public ICSMessageConsumer
        {
        private:
            NetworkStorageServer&   _server;

        public:
            MsgConsumer (NetworkStorageServer &)                __NE___;

            // ICSMessageConsumer //
            CSMessageGroupID  GetGroupID ()                     C_NE_OV { return CSMessageGroup::NetVFS; }
            void  Consume (ChunkList<const CSMessagePtr>)       __NE_OV;
        };


        class SendReadResultTask final : public IAsyncTask
        {
        // variables
        private:
            const NDSRequestID      _id;
            const EClientLocalID    _clientId;
            AsyncDSRequest          _req;
            Bytes                   _sent;
            uint                    _partIdx    = 0;

        // methods
        public:
            SendReadResultTask (NDSRequestID id, EClientLocalID cid, AsyncDSRequest req) __NE___ :
                IAsyncTask{ ETaskQueue::Background },
                _id{id}, _clientId{cid}, _req{ RVRef(req) }
            {}

            void  Run ()            __Th_OV;
            void  OnCancel ()       __NE_OV;

            StringView  DbgName ()  C_NE_OV { return "SendReadResultTask"; }
        };


        struct ClientData
        {
            RDataSourcePool_t       readDSPool;     // -.
            WDataSourcePool_t       writeDSPool;    //  |-- used in single thread
                                                    //  |
            WRequestPool_t          writeReqPool;   // -'

            ClientData ()               __NE___;
            ClientData (ClientData &&)  __NE___ = default;
            ~ClientData ()              __NE___;
        };

        using ClientMap_t       = FixedMap< EClientLocalID, ClientData, 8 >;

        using GlobalFileMap_t   = IVirtualFileStorage::GlobalFileMap_t;
        using StorageArray_t    = FixedArray< RC<IVirtualFileStorage>, 8 >;
        using FolderArray_t     = FixedArray< Path, 8 >;


    // variables
    private:
        StaticRC<MsgProducer>   _msgProducer;
        StaticRC<MsgConsumer>   _msgConsumer;

        ClientMap_t             _clients;

        Atomic<bool>            _isImmutable    {false};
        GlobalFileMap_t         _globalMap;
        StorageArray_t          _storageArr;
        FolderArray_t           _folders;


    // methods
    public:
        NetworkStorageServer ()                                                 __NE___;
        ~NetworkStorageServer ()                                                __NE___;


        // Add file storage to the server VFS.
        // All static files will be added to the global map.
        // FileName hash must be unique for all files in the VFS.
        //  Thread-safe:  no
        //
        ND_ bool  AddStorage (RC<IVirtualFileStorage> storage)                  __NE___;

        ND_ bool  AddFolder (Path folder)                                       __NE___;
        ND_ bool  AddFolders (ArrayView<Path> folders)                          __NE___;


        // Must be called from 'IClientListener' implementation
        // when client is connected or disconnected.
        //  Thread-safe:  no
        //
        bool  AddClient (EClientLocalID)                                        __NE___;
        void  RemoveClient (EClientLocalID)                                     __NE___;


        // Must be attached to 'ClientServerBase'.
        //  Thread-safe:  no
        //
        ND_ ICSMessageProducer&  GetMessageProducer ()                          __NE___ { return *_msgProducer; }
        ND_ ICSMessageConsumer&  GetMessageConsumer ()                          __NE___ { return *_msgConsumer; }


    private:
        ND_ NetRDataSource*     _GetReadDS (EClientLocalID, NetDataSourceID)    __NE___;
        ND_ NetWDataSource*     _GetWriteDS (EClientLocalID, NetDataSourceID)   __NE___;
        ND_ NetWriteRequest*    _GetWriteReq (EClientLocalID, NDSRequestID)     __NE___;

        void  _WriteRequestComplete (NDSRequestID, Bytes, EClientLocalID)       __NE___;
        void  _WriteRequestFailed (NDSRequestID id, EClientLocalID cid)         __NE___ { _WriteRequestComplete( id, 0_b, cid ); }

        template <typename T>
        ND_ auto  _CreateMsg (EClientLocalID cid, Bytes extraSize = 0_b)        __NE___ { return _msgProducer->CreateMsg<T>( cid, extraSize ); }

        template <typename T>
        ND_ auto  _CreateMsgOpt (EClientLocalID cid, Bytes extraSize = 0_b)     __NE___ { return _msgProducer->CreateMsgOpt<T>( cid, extraSize ); }

        template <typename T>
        ND_ bool  _AddMessage (T &msg)                                          __NE___ { return _msgProducer->AddMessage( msg ); }

        ND_ bool  _OpenForRead (OUT RC<AsyncRDataSource> &, FileName::Ref)      __NE___;
        ND_ bool  _OpenForWrite (OUT RC<AsyncWDataSource> &, FileName::Ref)     __NE___;


    private:
        void  _Init (CSMsg_VFS_Init const&)                                     __NE___;

        void  _OpenForReadRequest (CSMsg_VFS_OpenForReadRequest const&)         __NE___;
        void  _OpenForWriteRequest (CSMsg_VFS_OpenForWriteRequest const&)       __NE___;

        void  _CloseReadFile (CSMsg_VFS_CloseReadFile const&)                   __NE___;
        void  _CloseWriteFile (CSMsg_VFS_CloseWriteFile const&)                 __NE___;

        void  _CancelAllReadRequests (CSMsg_VFS_CancelAllReadRequests const&)   __NE___;
        void  _CancelAllWriteRequests (CSMsg_VFS_CancelAllWriteRequests const&) __NE___;

        void  _ReadRequest (CSMsg_VFS_ReadRequest const&)                       __NE___;

        void  _WriteBegin (CSMsg_VFS_WriteBegin const&)                         __NE___;
        void  _WritePart (CSMsg_VFS_WritePart const&)                           __NE___;
        void  _WriteEnd (CSMsg_VFS_WriteEnd const&)                             __NE___;
    };


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
