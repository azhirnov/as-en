// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_NETWORKING
# include "networking/HighLevel/Messages.h"
# include "vfs/Common.h"

namespace AE::CSMessageGroup
{
    static constexpr auto                                       NetVFS          = CSMessageGroupID(2);
    static constexpr Base::CT_Counter< _MsgCounterCtx<NetVFS> > NetVFS_MsgCounter;
}

namespace AE::Networking
{
    // tag: HandleTmpl UID
    using NetDataSourceID   = HandleTmpl< 16, 16, (9 << 24) + 1 >;
    using NDSRequestID      = HandleTmpl< 16, 16, (9 << 24) + 2 >;


    // Open for read
    DECL_CSMSG( VFS_OpenForReadRequest,  NetVFS,
        NetDataSourceID             fileId;
        VFS::FileName::Optimized_t  name;
    );

    DECL_CSMSG( VFS_OpenForReadResult,  NetVFS,
        NetDataSourceID             fileId;
        bool                        ok      = false;
        IDataSource::ESourceType    type;
        Bytes                       size;
    );


    // Open for write
    DECL_CSMSG( VFS_OpenForWriteRequest,  NetVFS,
        NetDataSourceID             fileId;
        VFS::FileName::Optimized_t  name;
    );

    DECL_CSMSG( VFS_OpenForWriteResult,  NetVFS,
        NetDataSourceID             fileId;
        IDataSource::ESourceType    type;
        bool                        ok      = false;
    );


    // Close
    DECL_CSMSG( VFS_CloseReadFile,  NetVFS,
        NetDataSourceID     fileId;
    );

    DECL_CSMSG( VFS_CloseWriteFile,  NetVFS,
        NetDataSourceID     fileId;
    );


    // CancelAll
    DECL_CSMSG( VFS_CancelAllReadRequests,  NetVFS,
        NetDataSourceID     fileId;
    );

    DECL_CSMSG( VFS_CancelAllWriteRequests,  NetVFS,
        NetDataSourceID     fileId;
    );


    // Read
    DECL_CSMSG( VFS_ReadRequest,  NetVFS,
        NetDataSourceID     fileId;
        NDSRequestID        reqId;
        Bytes               pos;
        Byte32u             size;
    );

    DECL_CSMSG( VFS_ReadResult,  NetVFS,
        NDSRequestID        reqId;
        ushort              index;
        Byte16u             size;
        char                data [1];
    );

    DECL_CSMSG( VFS_ReadComplete,  NetVFS,
        NDSRequestID        reqId;
        Byte32u             size;
        HashVal64           hash;
    );


    // Write
    DECL_CSMSG( VFS_WriteBegin,  NetVFS,
        NetDataSourceID     fileId;
        NDSRequestID        reqId;
        Byte32u             size;
    );

    DECL_CSMSG( VFS_WritePart,  NetVFS,
        NDSRequestID        reqId;
        ushort              index;
        Byte16u             size;
        char                data [1];
    );

    DECL_CSMSG( VFS_WriteEnd,  NetVFS,
        NDSRequestID        reqId;
        HashVal64           hash;
        Bytes               pos;
    );

    DECL_CSMSG( VFS_WriteComplete,  NetVFS,
        NDSRequestID        reqId;
        Byte32u             size;
    );

//-----------------------------------------------------------------------------


    CSMSG_ENC_DEC( VFS_OpenForReadRequest,      fileId, name );
    CSMSG_ENC_DEC( VFS_OpenForReadResult,       fileId, ok, size );

    CSMSG_ENC_DEC( VFS_OpenForWriteRequest,     fileId, name );
    CSMSG_ENC_DEC( VFS_OpenForWriteResult,      fileId, ok );

    CSMSG_ENC_DEC( VFS_CloseReadFile,           fileId );
    CSMSG_ENC_DEC( VFS_CloseWriteFile,          fileId );

    CSMSG_ENC_DEC( VFS_CancelAllReadRequests,   fileId );
    CSMSG_ENC_DEC( VFS_CancelAllWriteRequests,  fileId );

    CSMSG_ENC_DEC( VFS_ReadRequest,             fileId, reqId, pos, size );
    CSMSG_ENC_DEC( VFS_ReadComplete,            reqId, size, hash );

    CSMSG_ENC_DEC( VFS_WriteBegin,              fileId, reqId, size );
    CSMSG_ENC_DEC( VFS_WriteEnd,                reqId, hash, pos );
    CSMSG_ENC_DEC( VFS_WriteComplete,           reqId, size );

    CSMSG_ENC_DEC_INPLACEARR( VFS_ReadResult,   size, data,  AE_ARGS( reqId, index, size ));
    CSMSG_ENC_DEC_INPLACEARR( VFS_WritePart,    size, data,  AE_ARGS( reqId, index, size ));
//-----------------------------------------------------------------------------


/*
=================================================
    Register_NetVFS
=================================================
*/
    ND_ inline bool  Register_NetVFS (MessageFactory &mf) __NE___
    {
        return  mf.Register<
                    CSMsg_VFS_OpenForReadRequest,
                    CSMsg_VFS_OpenForReadResult,
                    CSMsg_VFS_OpenForWriteRequest,
                    CSMsg_VFS_OpenForWriteResult,
                    CSMsg_VFS_CloseReadFile,
                    CSMsg_VFS_CloseWriteFile,
                    CSMsg_VFS_CancelAllReadRequests,
                    CSMsg_VFS_CancelAllWriteRequests,
                    CSMsg_VFS_ReadRequest,
                    CSMsg_VFS_ReadResult,
                    CSMsg_VFS_ReadComplete,
                    CSMsg_VFS_WriteBegin,
                    CSMsg_VFS_WritePart,
                    CSMsg_VFS_WriteEnd,
                    CSMsg_VFS_WriteComplete
                >
                ( True{"lock group"} );
    }

} // AE::Networking

#endif // AE_ENABLE_NETWORKING
