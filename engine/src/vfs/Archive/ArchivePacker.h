// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "vfs/Archive/ArchiveStaticStorage.h"

namespace AE::VFS
{

    //
    // Archive Packer
    //

    class ArchivePacker
    {
    // types
    public:
        using EFileType         = ArchiveStaticStorage::EFileType;

    private:
        using ArchiveHeader     = ArchiveStaticStorage::ArchiveHeader;
        using FileInfo          = ArchiveStaticStorage::FileInfo;
        using FileHeader        = ArchiveStaticStorage::FileHeader;
        using FileMap_t         = ArchiveStaticStorage::FileMap_t;

        static constexpr uint   Name    = ArchiveStaticStorage::Name;
        static constexpr uint   Version = ArchiveStaticStorage::Version;


    // variables
    private:
        FileMap_t       _map;
        Path            _tempFile;
        RC<WStream>     _archive;

        NamedID_HashCollisionCheck  _hashCollisionCheck;

        DRC_ONLY(
            DataRaceCheck   _drCheck;
        )


    // methods
    public:
        ArchivePacker ();
        ~ArchivePacker ();

        ND_ bool  Create (const Path &tempFile);

        ND_ bool  Store (WStream &dstStream);
        ND_ bool  Store (const Path &filename);

        ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, Bytes size, EFileType type);
        ND_ bool  Add (const FileName::WithString_t &name, RStream &stream, EFileType type);
        ND_ bool  Add (const FileName::WithString_t &name, const Path &filename, EFileType type);

        ND_ bool  AddArchive (const Path &filename);
        ND_ bool  AddArchive (RC<RDataSource> archive);

    private:
        ND_ bool  _AddFile (FileName::Optimized_t name, const FileInfo &info);
        ND_ bool  _Store (WStream &dstStream, Bytes archiveSize);
        ND_ uint  _BrotliCompression (RStream &stream, const FileName::WithString_t &name, FileInfo &info, Bytes startPos, Bytes size);

        ND_ bool  _AddArchive (ArchiveStaticStorage &);
    };


} // AE::VFS
