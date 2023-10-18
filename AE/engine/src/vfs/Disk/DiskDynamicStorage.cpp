// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/Disk/DiskDynamicStorage.h"
#include "vfs/Disk/Utils.cpp.h"

namespace AE::VFS
{

/*
=================================================
    Create
=================================================
*/
    bool  DiskDynamicStorage::Create (const Path &folder, StringView prefix)
    {
        {
            auto    file_map = _fileMap.WriteNoLock();
            EXLOCK( file_map );

            CHECK_ERR( _folder.empty() );
            CHECK_ERR( FileSystem::IsDirectory( folder ));

            file_map->map.clear();
            file_map->map.reserve( 128 );
            file_map->lastUpdate = Default;

            _folder = FileSystem::ToAbsolute( folder );
            _prefix = String{prefix};
        }

        CHECK_ERR( _Update() );
        return true;
    }

/*
=================================================
    _Update
=================================================
*/
    bool  DiskDynamicStorage::_Update () C_NE___
    {
        try{
            auto    cur_time = TimePoint_t::clock::now();

            if ( TimeCast<seconds>( cur_time - _fileMap.ConstPtr()->lastUpdate ) < _UpdateInterval )
                return false;

            _fileMap->lastUpdate = cur_time;

            // build file map
            Array< Path >   stack;
            stack.push_back( _folder );

            String  str, name;
            name.reserve( 64 );

            for (; not stack.empty();)
            {
                Path    path = RVRef(stack.back());
                stack.pop_back();

                for (auto& entry : FileSystem::Enum( path ))
                {
                    if ( entry.IsFile() )
                    {
                        auto    file = FileSystem::ToRelative( entry.Get(), _folder );

                        CHECK_ERR( Convert<Path::value_type>( OUT str, file.native() ));

                        name.clear();
                        name << _prefix << str;

                        DEBUG_ONLY( _fileMap->hashCollisionCheck.Add( FileName{name} ));

                        _fileMap->map.emplace( FileName::Optimized_t{name}, RVRef(str) );
                    }
                    else
                    if ( entry.IsDirectory() )
                    {
                        stack.push_back( entry.Get() );
                    }
                }
            }

            _fileMap->lastUpdate = TimePoint_t::clock::now();
        }
        catch (...)
        {}
        return true;
    }

/*
=================================================
    Open
=================================================
*/
    template <typename ImplType, typename ResultType>
    bool  DiskDynamicStorage::_Open2 (OUT ResultType &result, FileNameRef name) C_NE___
    {
        Path    path;
        {
            auto    file_map = _fileMap.ReadNoLock();
            SHAREDLOCK( file_map );

            auto    iter = file_map->map.find( FileName::Optimized_t{name} );
            if_unlikely( iter == file_map->map.end() )
                return false;

            path = _folder / iter->second;
        }

        auto    file = MakeRC<ImplType>( path );
        if_likely( file->IsOpen() )
        {
            result = file;
            return true;
        }
        return false;
    }

    template <typename ImplType, typename ResultType>
    bool  DiskDynamicStorage::_Open (OUT ResultType &result, FileNameRef name) C_NE___
    {
        // first try
        {
            if_likely( _Open2<ImplType>( OUT result, name ))
                return true;
        }

        // update file map and try again
        if ( _Update() )
        {
            if_likely( _Open2<ImplType>( OUT result, name ))
                return true;
        }

        return false;
    }

    bool  DiskDynamicStorage::Open (OUT RC<RStream> &stream, FileNameRef name) C_NE___
    {
        return _Open<FileRStream>( OUT stream, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<RDataSource> &ds, FileNameRef name) C_NE___
    {
        return _Open<FileRDataSource>( OUT ds, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name) C_NE___
    {
    #if defined(AE_PLATFORM_WINDOWS)
        return _Open< Threading::WinAsyncRDataSource >( OUT ds, name );

    #else
        Unused( ds, name );
        return false;
    #endif
    }

/*
=================================================
    Open
=================================================
*/
    bool  DiskDynamicStorage::Open (OUT RC<WStream> &stream, FileNameRef name) C_NE___
    {
        return _Open<FileWStream>( OUT stream, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<WDataSource> &ds, FileNameRef name) C_NE___
    {
        return _Open<FileWDataSource>( OUT ds, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name) C_NE___
    {
    #if defined(AE_PLATFORM_WINDOWS)
        return _Open< Threading::WinAsyncWDataSource >( OUT ds, name );

    #else
        Unused( ds, name );
        return false;
    #endif
    }

/*
=================================================
    CreateFile
=================================================
*/
    bool  DiskDynamicStorage::CreateFile (OUT FileName &name, const Path &inPath) C_NE___
    {
        Path    path = (_folder / inPath).lexically_normal();   // path without '..'
                path = FileSystem::ToRelative( path, _folder );

        String  str;
        CHECK_ERR( Convert<Path::value_type>( OUT str, path.native() ));

        name = FileName{_prefix + str};
        DEBUG_ONLY( _fileMap->hashCollisionCheck.Add( name ));

        _fileMap->map.emplace( FileName::Optimized_t{name}, RVRef(str) );
        return true;
    }

/*
=================================================
    CreateUniqueFile
=================================================
*/
#if 1
    bool  DiskDynamicStorage::CreateUniqueFile (OUT FileName &name, INOUT Path &inoutPath) C_NE___
    {
        Path            path    = (_folder / inoutPath).lexically_normal(); // path without '..'
        String          fname   = ToString( path.filename().replace_extension("") );
        const String    ext     = ToString( path.extension() );
        const usize     len     = fname.length();
                        path    = path.parent_path();

        const auto  BuildName = [&] (OUT Path &p, usize idx)
        {{
            fname.resize( len );
            fname << ToString(idx) << ext;
            p = path / fname;
        }};

        const auto  Consume = [this, &inoutPath, &name] (const Path &p) -> bool
        {{
            inoutPath = FileSystem::ToRelative( p, _folder );

            String  str;
            CHECK_ERR( Convert<Path::value_type>( OUT str, inoutPath.native() ));

            name = FileName{_prefix + str};
            DEBUG_ONLY( _fileMap->hashCollisionCheck.Add( name ));

            _fileMap->map.emplace( FileName::Optimized_t{name}, RVRef(str) );
            return true;
        }};

        FileSystem::FindUnusedFilename( BuildName, Consume );
        return name.IsDefined();
    }
#else

    bool  DiskDynamicStorage::CreateUniqueFile (OUT FileName &name, INOUT Path &inoutPath) C_NE___
    {
        Path    path = (_folder / inoutPath).lexically_normal();    // path without '..'
                path = FileSystem::ToRelative( path, _folder );

        // TODO: search in map
    }
#endif

/*
=================================================
    Exists
=================================================
*/
    bool  DiskDynamicStorage::Exists (FileNameRef name) C_NE___
    {
        // first try
        {
            if_likely( _fileMap.ConstPtr()->map.contains( FileName::Optimized_t{name} ))
                return true;
        }

        // update file map and try again
        if ( _Update() )
        {
            if_likely( _fileMap.ConstPtr()->map.contains( FileName::Optimized_t{name} ))
                return true;
        }

        return false;
    }

    bool  DiskDynamicStorage::Exists (FileGroupNameRef) C_NE___
    {
        // not supported
        return false;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    CreateDynamicFolder
=================================================
*/
    RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateDynamicFolder (const Path &folder, StringView prefix) __NE___
    {
        auto    result = MakeRC<DiskDynamicStorage>();
        CHECK_ERR( result->Create( folder, prefix ));
        return result;
    }


} // AE::VFS
