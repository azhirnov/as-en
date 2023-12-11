// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorage.h"

namespace AE::VFS
{

/*
=================================================
    Open
=================================================
*/
    bool  NetworkStorage::Open (OUT RC<RStream> &stream, FileNameRef name) C_NE___
    {
        auto    async = _client.OpenForRead( name );
        if_likely( async ) {
            stream = MakeRC<SyncRStream2>( RVRef(async) );
            return true;
        }
        return false;
    }

    bool  NetworkStorage::Open (OUT RC<RDataSource> &ds, FileNameRef name) C_NE___
    {
        auto    async = _client.OpenForRead( name );
        if_likely( async ) {
            ds = MakeRC<SyncRDataSource>( RVRef(async) );
            return true;
        }
        return false;
    }

    bool  NetworkStorage::Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name) C_NE___
    {
        ds = _client.OpenForRead( name );
        return ds != null;
    }

    bool  NetworkStorage::Open (OUT RC<WStream> &stream, FileNameRef name) C_NE___
    {
        auto    async = _client.OpenForWrite( name );
        if_likely( async ) {
            stream = MakeRC<SyncWStream2>( RVRef(async) );
            return true;
        }
        return false;
    }

    bool  NetworkStorage::Open (OUT RC<WDataSource> &ds, FileNameRef name) C_NE___
    {
        auto    async = _client.OpenForWrite( name );
        if_likely( async ) {
            ds = MakeRC<SyncWDataSource>( RVRef(async) );
            return true;
        }
        return false;
    }

    bool  NetworkStorage::Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name) C_NE___
    {
        ds = _client.OpenForWrite( name );
        return ds != null;
    }

/*
=================================================
    CreateFile
=================================================
*/
    bool  NetworkStorage::CreateFile (OUT FileName &, const Path &) C_NE___
    {
        // TODO ?
        return false;
    }

/*
=================================================
    CreateUniqueFile
=================================================
*/
    bool  NetworkStorage::CreateUniqueFile (OUT FileName &, INOUT Path &) C_NE___
    {
        // TODO ?
        return false;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  NetworkStorage::Exists (FileNameRef) C_NE___
    {
        // TODO ?
        return false;
    }

    bool  NetworkStorage::Exists (FileGroupNameRef) C_NE___
    {
        // TODO ?
        return false;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CreateNetworkStorage
=================================================
*/
    RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateNetworkStorage (Networking::ClientServerBase &cs) __NE___
    {
        auto    result = RC<NetworkStorage>{ new NetworkStorage{}};

        if_unlikely( not (cs.Add( result->_client.GetMessageConsumer().GetRC() ) and
                          cs.Add( result->_client.GetMessageProducer().GetRC() )) )
        {
            cs.Remove( &result->_client.GetMessageConsumer() );
            cs.Remove( &result->_client.GetMessageProducer() );
            return null;
        }

        return result;
    }


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
