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


    // variables
    private:
        mutable SharedMutex     _mapGuard;
        mutable FileMap_t       _map;
        mutable TimePoint_t     _lastUpdate;
        Path                    _folder;
        String                  _prefix;

        DEBUG_ONLY(
          mutable NamedID_HashCollisionCheck    _hashCollisionCheck;
        )


    // methods
    public:
        DiskDynamicStorage ()                                                       __NE___ {}
        ~DiskDynamicStorage ()                                                      __NE_OV {}

        ND_ bool  Create (const Path &folder, StringView prefix = Default);


      // IVirtualFileStorage //
        bool  Open (OUT RC<RStream> &stream, const FileName &name)                  C_NE_OV;
        bool  Open (OUT RC<RDataSource> &ds, const FileName &name)                  C_NE_OV;
        bool  Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)             C_NE_OV;

        bool  Exists (const FileName &name)                                         C_NE_OV;
        bool  Exists (const FileGroupName &name)                                    C_NE_OV;

    private:
        void  _Append (INOUT GlobalFileMap_t &)                                     C_Th_OV {}

        bool  _OpenByIter (OUT RC<RStream>&, const FileName &, const void*)         C_NE_OV { DBG_WARNING("not supported");  return false; }
        bool  _OpenByIter (OUT RC<RDataSource>&, const FileName &, const void*)     C_NE_OV { DBG_WARNING("not supported");  return false; }
        bool  _OpenByIter (OUT RC<AsyncRDataSource>&, const FileName &, const void*)C_NE_OV { DBG_WARNING("not supported");  return false; }

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open (OUT ResultType &, const FileName &name)                    C_NE___;

        template <typename ImplType, typename ResultType>
        ND_ bool  _Open2 (OUT ResultType &, const FileName &name)                   C_NE___;

        ND_ bool  _Exists (const FileName &name)                                    C_NE___;

        ND_ bool  _Update ()                                                        C_NE___;
    };


} // AE::VFS