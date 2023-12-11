// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Archive/ArchiveStaticStorage.h"

namespace AE::VFS
{
    using ArchiveDataSource_t   = RDataSourceRange< RC<RDataSource> >;
    using ArchiveStream_t       = RDataSourceAsStream< RC<RDataSource> >;
    using TSFileRDataSource_t   = Threading::TsRDataSource< AsPointer< FileRDataSource >>;
    using TSRDataSource_t       = Threading::TsRDataSource< RC<RDataSource> >;

/*
=================================================
    _Create
=================================================
*/
    bool  ArchiveStaticStorage::_Create (RC<RDataSource> archive) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( archive and archive->IsOpen() );
        CHECK_ERR( not _archive );

        if ( not archive->IsThreadSafe() )
            archive = MakeRC<TSRDataSource_t>( RVRef(archive) );

        CHECK_ERR( _ReadHeader( *archive ));

        _archive = RVRef(archive);
        return true;
    }

/*
=================================================
    _Create
=================================================
*/
    bool  ArchiveStaticStorage::_Create (const Path &filename) __NE___
    {
        auto    file = MakeRC<TSFileRDataSource_t>( filename );
        CHECK_ERR( file );
        return _Create( file );
    }

/*
=================================================
    _ReadHeader
=================================================
*/
    bool  ArchiveStaticStorage::_ReadHeader (RDataSource &inDS) __NE___
    {
        TRY{
            ArchiveHeader   hdr;
            CHECK_ERR( inDS.ReadBlock( 0_b, OUT &hdr, Sizeof(hdr) ) == Sizeof(hdr) );

            CHECK_ERR(  hdr.name == Name            and
                        hdr.version == Version      and
                        hdr.fileHeadersSize > 0     and
                        IsMultipleOf( hdr.fileHeadersSize, sizeof(FileHeader) ));

            const uint  file_count = hdr.fileHeadersSize / sizeof(FileHeader);
            _map.reserve( file_count );

            auto    mem = MakeRC<MemRStream>();
            CHECK_ERR( mem->Load( inDS, Sizeof(hdr), Bytes{hdr.fileHeadersSize} ));

            DEBUG_ONLY( const Bytes  ds_size = inDS.Size();)

            FastRStream stream {mem};
            for (uint i = 0; i < file_count; ++i)
            {
                FileHeader  fhdr;
                CHECK_ERR( stream.Read( OUT fhdr ));
                CHECK_ERR( _map.emplace( fhdr.name, fhdr.info ).second );

                ASSERT( fhdr.info.Offset() < ds_size );
                ASSERT( (fhdr.info.Offset() + fhdr.info.size) <= ds_size );
            }
            return true;
        }
        CATCH_ALL(
            return false;
        )
    }

/*
=================================================
    Open (RStream)
=================================================
*/
    bool  ArchiveStaticStorage::Open (OUT RC<RStream> &outStream, FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _map.find( FileName::Optimized_t{name} );
        if_likely( iter != _map.end() )
            return _Open2( OUT outStream, iter->second );

        return false;
    }

    bool  ArchiveStaticStorage::_Open2 (OUT RC<RStream> &outStream, const FileInfo &info) C_NE___
    {
        auto    substream = MakeRC<ArchiveStream_t>( _archive, info.Offset(), info.Size() );

        BEGIN_ENUM_CHECKS();
        switch ( info.type )
        {
            case EFileType::Raw :
            {
                outStream = RVRef(substream);
                return true;
            }

            case EFileType::InMemory :
            {
                auto    result = MakeRC<MemRStream>();
                CHECK_ERR( result->LoadAll( *substream ));

                outStream = RVRef(result);
                return true;
            }

          #ifdef AE_ENABLE_BROTLI
            case EFileType::Brotli :
            {
                outStream = MakeRC<BrotliRStream>( substream );
                return true;
            }

            case EFileType::BrotliInMemory :
            {
                BrotliRStream   brotli{ substream };
                auto            result = MakeRC<MemRStream>();

                CHECK_ERR( result->Decompress( brotli ));
                outStream = RVRef(result);
                return true;
            }
          #else

            case EFileType::Brotli :
            case EFileType::BrotliInMemory :
                break;
          #endif

            case EFileType::_Last :
            case EFileType::All :
                break;
        }
        END_ENUM_CHECKS();

        return false;
    }

/*
=================================================
    Open (RDataSource)
=================================================
*/
    bool  ArchiveStaticStorage::Open (OUT RC<RDataSource> &outDS, FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _map.find( FileName::Optimized_t{name} );
        if_likely( iter != _map.end() )
            return _Open2( OUT outDS, iter->second );

        return false;
    }

    bool  ArchiveStaticStorage::_Open2 (OUT RC<RDataSource> &outDS, const FileInfo &info) C_NE___
    {
        auto    ds = MakeRC<ArchiveDataSource_t>( _archive, info.Offset(), info.Size() );

        BEGIN_ENUM_CHECKS();
        switch ( info.type )
        {
            case EFileType::Raw :
            {
                outDS = RVRef(ds);
                return true;
            }

            case EFileType::InMemory :
            {
                auto    result = MakeRC<MemRDataSource>();
                CHECK_ERR( result->LoadAll( *ds ));

                outDS = RVRef(result);
                return true;
            }

          #ifdef AE_ENABLE_BROTLI
            case EFileType::Brotli :
                break;  // not supported

            case EFileType::BrotliInMemory :
            {
                auto            stream = MakeRC<ArchiveStream_t>( _archive, info.Offset(), info.Size() );
                BrotliRStream   brotli{ stream };
                auto            result = MakeRC<MemRDataSource>();

                CHECK_ERR( result->Decompress( brotli ));
                outDS = RVRef(result);
                return true;
            }
          #else

            case EFileType::Brotli :
            case EFileType::BrotliInMemory :
                break;
          #endif

            case EFileType::_Last :
            case EFileType::All :
                break;
        }
        END_ENUM_CHECKS();

        return false;
    }

/*
=================================================
    Open (AsyncRDataSource)
=================================================
*/
    bool  ArchiveStaticStorage::Open (OUT RC<AsyncRDataSource> &, FileNameRef) C_NE___
    {
        // TODO: replace '_archive' by asyncDS
        return false;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  ArchiveStaticStorage::Exists (FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _map.find( FileName::Optimized_t{name} );
        return iter != _map.end();
    }

/*
=================================================
    Exists
=================================================
*/
    bool  ArchiveStaticStorage::Exists (FileGroupNameRef) C_NE___
    {
        // not supported
        return false;
    }

/*
=================================================
    _Append
=================================================
*/
    void  ArchiveStaticStorage::_Append (INOUT GlobalFileMap_t &map) C_Th___
    {
        DRC_SHAREDLOCK( _drCheck );

        for (auto& [name, info] : _map)
        {
            map.emplace( name, GlobalFileRef{ this, &info });   // throw
        }
    }

/*
=================================================
    _OpenByIter (RStream)
=================================================
*/
    bool  ArchiveStaticStorage::_OpenByIter (OUT RC<RStream> &outStream, FileNameRef name, const void* ref) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        DEBUG_ONLY(
            auto    iter = _map.find( FileName::Optimized_t{name} );
            CHECK_ERR( iter != _map.end() );
            CHECK_ERR( &iter->second == ref );
        )
        Unused( name );

        return _Open2( OUT outStream, *Cast<FileInfo>( ref ));
    }

/*
=================================================
    _OpenByIter (RDataSource)
=================================================
*/
    bool  ArchiveStaticStorage::_OpenByIter (OUT RC<RDataSource> &outDS, FileNameRef name, const void* ref) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        DEBUG_ONLY(
            auto    iter = _map.find( FileName::Optimized_t{name} );
            CHECK_ERR( iter != _map.end() );
            CHECK_ERR( &iter->second == ref );
        )
        Unused( name );

        return _Open2( OUT outDS, *Cast<FileInfo>( ref ));
    }

/*
=================================================
    _OpenByIter (AsyncRDataSource)
=================================================
*/
    bool  ArchiveStaticStorage::_OpenByIter (OUT RC<AsyncRDataSource> &, FileNameRef, const void*) C_NE___
    {
        // TODO: replace '_archive' by asyncDS
        return false;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CreateStaticArchive
=================================================
*/
    RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticArchive (RC<RDataSource> archive) __NE___
    {
        auto    result = RC<ArchiveStaticStorage>{ new ArchiveStaticStorage{}};
        CHECK_ERR( result->_Create( RVRef(archive) ));
        return result;
    }

    RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticArchive (const Path &filename) __NE___
    {
        auto    result = RC<ArchiveStaticStorage>{ new ArchiveStaticStorage{}};
        CHECK_ERR( result->_Create( filename ));
        return result;
    }


} // AE::VFS
