// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    File - any data source with fixed size.

    FileGroup - list of file which can be preloaded or preload file headers.

    Storage - collection of files and file groups.
----

    RStream, RDataSource, AsyncRDataSource
        Thread-safe:    depends on implementation

    WStream, WDataSource, AsyncWDataSource
        Thread-safe:    depends on implementation

    IVirtualFileStorage
        Thread-safe:    yes (interface methods only)

    VirtualFileSystem
        Thread-safe:    yes (const methods only)
----

    [docs](https://github.com/azhirnov/as-en/blob/dev/AE/engine/docs/ru/VirtualFileSystem.md)
*/

#pragma once

#include "vfs/Common.h"

namespace AE { VFS::VirtualFileSystem&  GetVFS () __NE___; }

namespace AE::VFS
{

    /*enum class EVFStorageType : uint
    {
        Unknown     = 0,
        Dynamic     = 1 << 0,       // allows to create files and rewrite existing, by default all storages are static
        Remote      = 1 << 1,       // removable disk, network, etc
        Cache       = 1 << 2,       // files may be lost
        Temporary   = 1 << 3,       // files will be deleted at exit
    };
    AE_BIT_OPERATORS( EVFStorageType );*/



    //
    // Virtual File System Storage interface
    //
    class IVirtualFileStorage : public EnableRC<IVirtualFileStorage>
    {
        friend class VirtualFileSystem;

    // types
    protected:
        struct GlobalFileRef
        {
            IVirtualFileStorage const*  storage;
            void const*                 ref;        // map iterator, only for static storage
        };

        using GlobalFileMap_t   = FlatHashMap< FileName::Optimized_t, GlobalFileRef >;

      #if AE_OPTIMIZE_IDS
        using FileNameRef       = FileName::Optimized_t;
        using FileGroupNameRef  = FileGroupName::Optimized_t;
      #else
        using FileNameRef       = FileName const&;
        using FileGroupNameRef  = FileGroupName const&;
      #endif


    // interface
    public:
        ND_ virtual bool  Open (OUT RC<RStream> &stream, FileNameRef name)                                  C_NE___ = 0;
        ND_ virtual bool  Open (OUT RC<RDataSource> &ds, FileNameRef name)                                  C_NE___ = 0;
        ND_ virtual bool  Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name)                             C_NE___ = 0;

        ND_ virtual bool  Open (OUT RC<WStream> &stream, FileNameRef name)                                  C_NE___ { Unused( stream, name );  return false; }
        ND_ virtual bool  Open (OUT RC<WDataSource> &ds, FileNameRef name)                                  C_NE___ { Unused( ds, name );  return false; }
        ND_ virtual bool  Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name)                             C_NE___ { Unused( ds, name );  return false; }

        ND_ virtual bool  CreateFile (OUT FileName &name, const Path &path)                                 C_NE___ { Unused( name, path );  return false; }
        ND_ virtual bool  CreateUniqueFile (OUT FileName &name, INOUT Path &path)                           C_NE___ { Unused( name, path );  return false; }

        ND_ virtual bool  Exists (FileNameRef name)                                                         C_NE___ = 0;
        ND_ virtual bool  Exists (FileGroupNameRef name)                                                    C_NE___ = 0;


    protected:
            virtual void  _Append (INOUT GlobalFileMap_t &)                                                 C_Th___ = 0;

        ND_ virtual bool  _OpenByIter (OUT RC<RStream> &stream, FileNameRef name, const void* ref)          C_NE___ = 0;
        ND_ virtual bool  _OpenByIter (OUT RC<RDataSource> &ds, FileNameRef name, const void* ref)          C_NE___ = 0;
        ND_ virtual bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, FileNameRef name, const void* ref)     C_NE___ = 0;

        ND_ virtual bool  _OpenByIter (OUT RC<WStream> &stream, FileNameRef name, const void* ref)          C_NE___ { Unused( stream, name, ref );  return false; }
        ND_ virtual bool  _OpenByIter (OUT RC<WDataSource> &ds, FileNameRef name, const void* ref)          C_NE___ { Unused( ds, name, ref );  return false; }
        ND_ virtual bool  _OpenByIter (OUT RC<AsyncWDataSource> &ds, FileNameRef name, const void* ref)     C_NE___ { Unused( ds, name, ref );  return false; }
    };



    //
    // Virtual File System
    //
    class VirtualFileSystem final
    {
        friend struct InPlace<VirtualFileSystem>;

    // types
    private:
        using GlobalFileMap_t   = IVirtualFileStorage::GlobalFileMap_t;
        using FileNameRef       = IVirtualFileStorage::FileNameRef;
        using FileGroupNameRef  = IVirtualFileStorage::FileGroupNameRef;

        using StorageArray_t    = FixedMap< StorageName::Optimized_t, RC<IVirtualFileStorage>, 16 >;


    // variables
    private:
        GlobalFileMap_t             _globalMap;
        StorageArray_t              _storageMap;

        DRC_ONLY( RWDataRaceCheck   _drCheck;)


    // methods
    public:
            static void  CreateInstance ()                                                      __NE___;
            static void  DestroyInstance ()                                                     __NE___;

        // Add VF storage to the VFS.
        // All static files will be added to the global map.
        // FileName hash must be unique for all files in the VFS.
        // Must be externally synchronized with 'Open()' and 'Exists()' methods.
        //
        ND_ bool  AddStorage (const StorageName &name, RC<IVirtualFileStorage> storage)         __NE___;
        ND_ bool  AddStorage (RC<IVirtualFileStorage> storage)                                  __NE___;


        ND_ bool  Open (OUT RC<RStream> &stream, FileNameRef name)                              C_NE___;
        ND_ bool  Open (OUT RC<RDataSource> &ds, FileNameRef name)                              C_NE___;
        ND_ bool  Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name)                         C_NE___;
        ND_ bool  Open (OUT RC<AsyncRStream> &stream, FileNameRef name)                         C_NE___;

        ND_ bool  Open (OUT RC<WStream> &stream, FileNameRef name)                              C_NE___;
        ND_ bool  Open (OUT RC<WDataSource> &ds, FileNameRef name)                              C_NE___;
        ND_ bool  Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name)                         C_NE___;
        ND_ bool  Open (OUT RC<AsyncWStream> &stream, FileNameRef name)                         C_NE___;

        template <typename T>
        ND_ RC<T>  Open (FileNameRef name)                                                      C_NE___;

        ND_ bool  CreateFile (const StorageName &storage,
                              OUT FileName      &name,
                              const Path        &path)                                          C_NE___;

        ND_ bool  CreateUniqueFile (const StorageName   &storage,
                                    OUT FileName        &name,
                                    INOUT Path          &path)                                  C_NE___;

        ND_ bool  Exists (FileNameRef name)                                                     C_NE___;
        ND_ bool  Exists (FileGroupNameRef name)                                                C_NE___;


    private:
        VirtualFileSystem ()                                                                    __NE___ {}
        ~VirtualFileSystem ()                                                                   __NE___ {}

        template <typename ResultType>
        ND_ bool  _OpenForRead (OUT ResultType &, FileNameRef name)                             C_NE___;

        template <typename ResultType>
        ND_ bool  _OpenForWrite (OUT ResultType &, FileNameRef name)                            C_NE___;

        friend VirtualFileSystem&       AE::GetVFS ()                                           __NE___;
        ND_ static VirtualFileSystem&   _Instance ()                                            __NE___;
    };



    //
    // Virtual File Storage Factory
    //
    class VirtualFileStorageFactory : public Noninstanceable
    {
    public:
        ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (RC<RDataSource> archive)                           __NE___;
        ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (const Path &filename)                              __NE___;

        ND_ static RC<IVirtualFileStorage>  CreateStaticFolder (const Path &folder, StringView prefix = Default)    __NE___;
        ND_ static RC<IVirtualFileStorage>  CreateDynamicFolder (const Path &folder, StringView prefix = Default)   __NE___;

        ND_ static RC<IVirtualFileStorage>  CreateNetworkStorage (StringView url)                                   __NE___;
    };


/*
=================================================
    Open
=================================================
*/
    template <typename T>
    RC<T>  VirtualFileSystem::Open (FileNameRef name) C_NE___
    {
        RC<T>   result;
        return Open( OUT result, name ) ? RVRef(result) : null;
    }


} // AE::VFS


namespace AE
{
/*
=================================================
    GetVFS
=================================================
*/
    ND_ inline VFS::VirtualFileSystem&  GetVFS () __NE___
    {
        return VFS::VirtualFileSystem::_Instance();
    }

} // AE
