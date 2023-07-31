// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

    //
    // Archive Dynamic Storage
    //

    class ArchiveDynamicStorage final : public IVirtualFileStorage
    {
    // types
    private:
        using FileMap_t     = FlatHashMap< FileName::Optimized_t, StringView >;
        using Allocator_t   = LinearAllocator<>;


    // variables
    private:

        DEBUG_ONLY(
          NamedID_HashCollisionCheck    _hashCollisionCheck;
        )
        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        ArchiveDynamicStorage ()                                                                __NE___ {}
        ~ArchiveDynamicStorage ()                                                               __NE_OV {}

        ND_ bool  Create (RC<RStream> archive);


      // IVirtualFileStorage //
        bool  Open (OUT RC<RStream> &stream, const FileName &name)                              C_NE_OV;
        bool  Open (OUT RC<RDataSource> &ds, const FileName &name)                              C_NE_OV;
        bool  Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)                         C_NE_OV;

        bool  Exists (const FileName &name)                                                     C_NE_OV;
        bool  Exists (const FileGroupName &name)                                                C_NE_OV;

    private:
        void  _Append (INOUT GlobalFileMap_t &)                                                 C_Th_OV;

        bool  _OpenByIter (OUT RC<RStream> &stream, const FileName &name, const void* ref)      C_NE_OV;
        bool  _OpenByIter (OUT RC<RDataSource> &ds, const FileName &name, const void* ref)      C_NE_OV;
        bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, const FileName &name, const void* ref) C_NE_OV;
    };


} // AE::VFS
