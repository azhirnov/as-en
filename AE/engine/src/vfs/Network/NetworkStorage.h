// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorageClient.h"

namespace AE::VFS
{

    //
    // Network Virtual File Storage
    //

    class NetworkStorage final : public IVirtualFileStorage
    {
    // variables
    private:
        mutable NetworkStorageClient    _client;


    // methods
    public:

      // IVirtualFileStorage //
        bool  Open (OUT RC<RStream> &stream, FileNameRef name)                      C_NE_OV;
        bool  Open (OUT RC<RDataSource> &ds, FileNameRef name)                      C_NE_OV;
        bool  Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name)                 C_NE_OV;

        bool  Open (OUT RC<WStream> &stream, FileNameRef name)                      C_NE_OV;
        bool  Open (OUT RC<WDataSource> &ds, FileNameRef name)                      C_NE_OV;
        bool  Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name)                 C_NE_OV;

        bool  CreateFile (OUT FileName &name, const Path &path)                     C_NE_OV;
        bool  CreateUniqueFile (OUT FileName &name, INOUT Path &path)               C_NE_OV;

        bool  Exists (FileNameRef name)                                             C_NE_OV;
        bool  Exists (FileGroupNameRef name)                                        C_NE_OV;


    private:
        void  _Append (INOUT GlobalFileMap_t &)                                     C_Th_OV {}

        bool  _OpenByIter (OUT RC<RStream>&, FileNameRef, const void*)              C_NE_OV { DBG_WARNING("not supported");  return false; }
        bool  _OpenByIter (OUT RC<RDataSource>&, FileNameRef, const void*)          C_NE_OV { DBG_WARNING("not supported");  return false; }
        bool  _OpenByIter (OUT RC<AsyncRDataSource>&, FileNameRef, const void*)     C_NE_OV { DBG_WARNING("not supported");  return false; }

        using IVirtualFileStorage::_OpenByIter;


    private:
        friend class VirtualFileStorageFactory;
        NetworkStorage ()                                                           __NE___ {}
        ~NetworkStorage ()                                                          __NE_OV {}
    };


} // AE::VFS

#endif // AE_ENABLE_NETWORKING
