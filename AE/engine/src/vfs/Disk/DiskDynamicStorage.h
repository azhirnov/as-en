// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

    //
    // Disk Dynamic Storage
    //

    class DiskDynamicStorage final : public IVirtualFileStorage
    {
    // types
    private:
        using FileMap_t     = FlatHashMap< FileName::Optimized_t, String >;
        using TimePoint_t   = std::chrono::high_resolution_clock::time_point;

        static constexpr seconds    _UpdateInterval {10};

        struct SyncData
        {
            FileMap_t       map;
            TimePoint_t     lastUpdate;

            DEBUG_ONLY(
              NamedID_HashCollisionCheck    hashCollisionCheck;
            )
        };
        using SyncData_t = Threading::Synchronized< SharedMutex, SyncData >;


    // variables
    private:
        Path                _folder;
        String              _prefix;
        mutable SyncData_t  _fileMap;


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

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open (OUT ResultType &, FileNameRef name)                        C_NE___;

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open2 (OUT ResultType &, FileNameRef name)                       C_NE___;

        ND_ bool  _Update ()                                                        C_NE___;


    private:
        friend class VirtualFileStorageFactory;
        DiskDynamicStorage ()                                                       __NE___ {}
        ~DiskDynamicStorage ()                                                      __NE_OV {}

        ND_ bool  _Create (const Path &folder, StringView prefix, bool createFolder)__NE___;
    };


} // AE::VFS
