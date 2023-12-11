// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

    //
    // Disk Static Storage
    //

    class DiskStaticStorage final : public IVirtualFileStorage
    {
    // types
    private:
        using FileMap_t     = FlatHashMap< FileName::Optimized_t, StringView >;
        using Allocator_t   = LinearAllocator<>;


    // variables
    private:
        FileMap_t       _map;
        Path            _folder;
        Allocator_t     _allocator;

        DEBUG_ONLY(
          NamedID_HashCollisionCheck    _hashCollisionCheck;
        )
        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:

      // IVirtualFileStorage //
        bool  Open (OUT RC<RStream> &stream, FileNameRef name)                                  C_NE_OV;
        bool  Open (OUT RC<RDataSource> &ds, FileNameRef name)                                  C_NE_OV;
        bool  Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name)                             C_NE_OV;

        using IVirtualFileStorage::Open;

        bool  Exists (FileNameRef name)                                                         C_NE_OV;
        bool  Exists (FileGroupNameRef name)                                                    C_NE_OV;


    private:
        void  _Append (INOUT GlobalFileMap_t &)                                                 C_Th_OV;

        bool  _OpenByIter (OUT RC<RStream> &stream, FileNameRef name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<RDataSource> &ds, FileNameRef name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, FileNameRef name, const void* ref)     C_NE_OV;

        bool  _OpenByIter (OUT RC<WStream> &stream, FileNameRef name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<WDataSource> &ds, FileNameRef name, const void* ref)          C_NE_OV;
        bool  _OpenByIter (OUT RC<AsyncWDataSource> &ds, FileNameRef name, const void* ref)     C_NE_OV;

        using IVirtualFileStorage::_OpenByIter;

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open (OUT ResultType &, FileNameRef name)                                    C_NE___;

        template <typename ImplType, typename ResultType>
        ND_ bool  _OpenByIter2 (OUT ResultType &, FileNameRef name, const void* ref)            C_NE___;


    private:
        friend class VirtualFileStorageFactory;
        DiskStaticStorage ()                                                                    __NE___ {}
        ~DiskStaticStorage ()                                                                   __NE_OV {}

        ND_ bool  _Create (const Path &folder, StringView prefix)                               __NE___;
        ND_ bool  _Create2 (const Path &folder, StringView prefix)                              __Th___;
    };


} // AE::VFS
