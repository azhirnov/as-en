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
            Byte32u                 recv;
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
            const NDSRequestID  _id;
            AsyncDSRequest      _req;
            Bytes               _sent;
            uint                _partIdx    = 0;

        // methods
        public:
            SendReadResultTask (NDSRequestID id, AsyncDSRequest req)    __NE___ :
                IAsyncTask{ ETaskQueue::Background },
                _id{id}, _req{ RVRef(req) }
            {}

            void  Run ()            __Th_OV;
            void  OnCancel ()       __NE_OV;

            StringView  DbgName ()  C_NE_OV { return "SendReadResultTask"; }
        };


    // variables
    private:
        StaticRC<MsgConsumer>   _msgConsumer;

        RDataSourcePool_t       _readDSPool;    // -.
        WDataSourcePool_t       _writeDSPool;   //  |-- used in single thread
                                                //  |
        WRequestPool_t          _writeReqPool;  // -'


    // methods
    public:
        NetworkStorageServer ()                                                 __NE___;
        ~NetworkStorageServer ()                                                __NE___;

        ND_ ICSMessageProducer&  GetMessageProducer ()                          __NE___ { return *_msgProducer; }
        ND_ ICSMessageConsumer&  GetMessageConsumer ()                          __NE___ { return *_msgConsumer; }


    private:
        ND_ NetRDataSource*     _GetReadDS (NetDataSourceID id)                 __NE___;
        ND_ NetWDataSource*     _GetWriteDS (NetDataSourceID id)                __NE___;

        void  _WriteRequestComplete (NDSRequestID id, Bytes written)            __NE___;
        void  _WriteRequestFailed (NDSRequestID id)                             __NE___ { _WriteRequestComplete( id, 0_b ); }

    private:
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
