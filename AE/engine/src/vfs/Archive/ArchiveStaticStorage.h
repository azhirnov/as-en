// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{

    //
    // Archive Static Storage
    //

    class ArchiveStaticStorage final : public IVirtualFileStorage
    {
        friend class ArchivePacker;

    // types
    private:
        struct ArchiveHeader
        {
            uint    name;
            uint    version;
            uint    fileHeadersSize;    // array of FileInfo
        };

        enum class EFileType : uint
        {
            Raw         = 1 << 0,       // SequentialAccess | RandomAccess
            Brotli      = 1 << 1,       // SequentialAccess
            InMemory    = 1 << 2,       // RandomAccess | Buffered
        //  Encrypted   = 1 << 3,       // SequentialAccess
            _Last,
            All         = ((_Last - 1) << 1) - 1,
            Unknown     = Raw,

            BrotliInMemory          = Brotli | InMemory,
        //  BrotliEncrypted         = Brotli | Encrypted,
        //  BrotliEncryptedInMemory = Brotli | Encrypted | InMemory,
        };

        struct FileInfo
        {
            uint            size        = 0;
            packed_ulong    offset;
            EFileType       type        = Default;

            ND_ Bytes  Size ()      const   { return Bytes{size}; }
            ND_ Bytes  Offset ()    const   { return Bytes{ulong{offset}}; }
        };

        struct FileHeader
        {
            FileName::Optimized_t   name;
            FileInfo                info;
        };
        STATIC_ASSERT( sizeof(FileHeader) == 20 );

        using FileMap_t = FlatHashMap< FileName::Optimized_t, FileInfo >;

        static constexpr uint   Name    = "VfsArch"_Hash;
        static constexpr uint   Version = (1 << 12) | (sizeof(FileHeader) & 0xFFF);


    // variables
    private:
        FileMap_t           _map;
        RC<RDataSource>     _archive;

        DRC_ONLY(
            RWDataRaceCheck _drCheck;
        )


    // methods
    public:
        ArchiveStaticStorage ()                                                                 __NE___ {}
        ~ArchiveStaticStorage ()                                                                __NE_OV {}

        ND_ bool  Create (RC<RDataSource> archive);
        ND_ bool  Create (const Path &filename);


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

        using IVirtualFileStorage::_OpenByIter;

        bool  _Open2 (OUT RC<RStream> &stream, const FileInfo &info)                            C_NE___;
        bool  _Open2 (OUT RC<RDataSource> &ds, const FileInfo &info)                            C_NE___;
    //  bool  _Open2 (OUT RC<AsyncRDataSource> &ds, const FileInfo &info)                       C_NE___;

        ND_ bool  _ReadHeader (RDataSource &ds)                                                 __NE___;
    };


} // AE::VFS
