// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Disk/DiskStaticStorage.h"
#include "vfs/Disk/Utils.cpp.h"

namespace AE::VFS
{

/*
=================================================
    Create
=================================================
*/
    bool  DiskStaticStorage::Create (const Path &folder, StringView prefix) __NE___
    {
        CATCH_ERR(
            return _Create( folder, prefix );
        )
    }

    bool  DiskStaticStorage::_Create (const Path &folder, StringView prefix) __Th___
    {
        DRC_EXLOCK( _drCheck );

        CHECK_ERR( _folder.empty() );
        CHECK_ERR( FileSystem::IsDirectory( folder ));

        _allocator.Discard();

        _map.clear();
        _map.reserve( 128 );    // throw

        _folder = FileSystem::ToAbsolute( folder );

        // build file map
        Array< Path >   stack;
        stack.push_back( _folder ); // throw

        String  name;
        name.reserve( 64 );

        for (; not stack.empty();)
        {
            Path    path = RVRef(stack.back());
            stack.pop_back();

            for (auto& entry : FileSystem::Enum( path ))
            {
                if ( entry.IsFile() )
                {
                    auto        file = FileSystem::ToRelative( entry.Get(), _folder );
                    StringView  str;

                    CHECK_ERR( Convert<Path::value_type>( OUT str, file.native(), _allocator ));

                    name.clear();
                    name.reserve( prefix.size() + str.size() + 1 );

                    name << prefix << str;
                    FindAndReplace( INOUT name, '\\', '/' );

                    DEBUG_ONLY( _hashCollisionCheck.Add( FileName{name} ));

                    CHECK_ERR( _map.emplace( FileName::Optimized_t{name}, str ).second );   // throw
                }
                else
                if ( entry.IsDirectory() )
                {
                    stack.push_back( entry.Get() ); // throw
                }
            }
        }
        ASSERT( not _hashCollisionCheck.HasCollisions() );

        return true;
    }

/*
=================================================
    Open
=================================================
*/
    bool  DiskStaticStorage::Open (OUT RC<RStream> &outStream, FileNameRef name) C_NE___
    {
        return _Open<FileRStream>( OUT outStream, name );
    }

    bool  DiskStaticStorage::Open (OUT RC<RDataSource> &outDS, FileNameRef name) C_NE___
    {
        return _Open<FileRDataSource>( OUT outDS, name );
    }

    bool  DiskStaticStorage::Open (OUT RC<AsyncRDataSource> &outDS, FileNameRef name) C_NE___
    {
    #if defined(AE_PLATFORM_WINDOWS)
        return _Open< Threading::WinAsyncRDataSource >( OUT outDS, name );

    #else
        Unused( outDS, name );
        return false;
    #endif
    }

/*
=================================================
    _Open
=================================================
*/
    template <typename ImplType, typename ResultType>
    bool  DiskStaticStorage::_Open (OUT ResultType &result, FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _map.find( FileName::Optimized_t{name} );
        if_likely( iter != _map.end() )
        {
            Path    path{ _folder };
            path /= Path{ iter->second };

            auto    file = MakeRC<ImplType>( path );    // throw
            if_likely( file->IsOpen() )
            {
                result = file;
                return true;
            }
        }
        return false;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  DiskStaticStorage::Exists (FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    iter = _map.find( FileName::Optimized_t{name} );
        return iter != _map.end();
    }

    bool  DiskStaticStorage::Exists (FileGroupNameRef) C_NE___
    {
        // not supported
        return false;
    }

/*
=================================================
    _Append
=================================================
*/
    void  DiskStaticStorage::_Append (INOUT GlobalFileMap_t &map) C_Th___
    {
        DRC_SHAREDLOCK( _drCheck );

        for (auto& [name, file] : _map)
        {
            map.emplace( name, GlobalFileRef{ this, &file });   // throw
        }
    }

/*
=================================================
    _OpenByIter
=================================================
*/
    bool  DiskStaticStorage::_OpenByIter (OUT RC<RStream> &stream, FileNameRef name, const void* ref) C_NE___
    {
        return _OpenByIter2<FileRStream>( OUT stream, name, ref );
    }

    bool  DiskStaticStorage::_OpenByIter (OUT RC<RDataSource> &ds, FileNameRef name, const void* ref) C_NE___
    {
        return _OpenByIter2<FileRDataSource>( OUT ds, name, ref );
    }

    bool  DiskStaticStorage::_OpenByIter (OUT RC<AsyncRDataSource> &ds, FileNameRef name, const void* ref) C_NE___
    {
    #if defined(AE_PLATFORM_WINDOWS)
        return _OpenByIter2< Threading::WinAsyncRDataSource >( OUT ds, name, ref );

    #else
        Unused( ds, name, ref );
        return false;
    #endif
    }

/*
=================================================
    _OpenByIter2
=================================================
*/
    template <typename ImplType, typename ResultType>
    bool  DiskStaticStorage::_OpenByIter2 (OUT ResultType &result, FileNameRef name, const void* ref) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        DEBUG_ONLY(
            auto    iter = _map.find( FileName::Optimized_t{name} );
            CHECK_ERR( iter != _map.end() );
            CHECK_ERR( &iter->second == ref );
        )
        Unused( name );

        Path    path{ _folder };
        path /= Path{ *Cast<StringView>( ref )};

        auto    file = MakeRC<ImplType>( path );
        if_likely( file->IsOpen() )
            result = file;

        return result != null;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CreateStaticFolder
=================================================
*/
    RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateStaticFolder (const Path &folder, StringView prefix) __NE___
    {
        auto    result = MakeRC<DiskStaticStorage>();
        CHECK_ERR( result->Create( folder, prefix ));
        return result;
    }


} // AE::VFS
