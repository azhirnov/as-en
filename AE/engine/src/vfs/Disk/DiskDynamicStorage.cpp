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
            EXLOCK( _mapGuard );

            CHECK_ERR( _folder.empty() );
            CHECK_ERR( FileSystem::IsDirectory( folder ));

            _map.clear();
            _map.reserve( 128 );

            _folder     = FileSystem::ToAbsolute( folder );
            _lastUpdate = Default;
            _prefix     = String{prefix};
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
            Threading::DeferExLock      lock {_mapGuard};
            if ( not lock.try_lock() )
                return false;

            auto    cur_time = TimePoint_t::clock::now();

            if ( TimeCast<seconds>( cur_time - _lastUpdate ) < _UpdateInterval )
                return false;

            _lastUpdate = cur_time;

            // build file map
            Array< Path >   stack;
            stack.push_back( _folder );

            String  str, name;
            str.reserve( 64 );
            name.reserve( 64 );

            for (; not stack.empty();)
            {
                Path    path = RVRef(stack.back());
                stack.pop_back();

                for (auto& entry : FileSystem::Enum( path ))
                {
                    if ( not entry.IsDirectory() )
                    {
                        auto    file = FileSystem::ToRelative( entry.Get(), _folder );

                        CHECK_ERR( Convert<Path::value_type>( OUT str, file.native() ));

                        name.clear();
                        name << _prefix << str;

                        DEBUG_ONLY( _hashCollisionCheck.Add( FileName{name} ));

                        _map.emplace( FileName::Optimized_t{name}, str );
                    }
                    else
                    {
                        stack.push_back( entry.Get() );
                    }
                }
            }
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
    bool  DiskDynamicStorage::_Open2 (OUT ResultType &result, const FileName &name) C_NE___
    {
        auto    iter = _map.find( FileName::Optimized_t{name} );
        if_likely( iter != _map.end() )
        {
            Path    path{ _folder };
            path /= Path{ iter->second };

            auto    file = MakeRC<ImplType>( path );
            if_likely( file->IsOpen() )
            {
                result = file;
                return true;
            }
        }
        return false;
    }

    template <typename ImplType, typename ResultType>
    bool  DiskDynamicStorage::_Open (OUT ResultType &result, const FileName &name) C_NE___
    {
        // first try
        {
            SHAREDLOCK( _mapGuard );

            if_likely( _Open2<ImplType>( OUT result, name ))
                return true;
        }

        // update file map and try again
        if ( _Update() )
        {
            SHAREDLOCK( _mapGuard );

            if_likely( _Open2<ImplType>( OUT result, name ))
                return true;
        }

        return false;
    }

    bool  DiskDynamicStorage::Open (OUT RC<RStream> &stream, const FileName &name) C_NE___
    {
        return _Open<FileRStream>( OUT stream, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<RDataSource> &ds, const FileName &name) C_NE___
    {
        return _Open<FileRDataSource>( OUT ds, name );
    }

    bool  DiskDynamicStorage::Open (OUT RC<AsyncRDataSource> &ds, const FileName &name) C_NE___
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
    LoadAsync
=================================================
*
    Promise<void>  DiskDynamicStorage::LoadAsync (const FileGroupName &name) const
    {
        // not supported
        return Default;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  DiskDynamicStorage::_Exists (const FileName &name) C_NE___
    {
        auto    iter = _map.find( FileName::Optimized_t{name} );
        return iter != _map.end();
    }

    bool  DiskDynamicStorage::Exists (const FileName &name) C_NE___
    {
        // first try
        {
            SHAREDLOCK( _mapGuard );

            if_likely( _Exists( name ))
                return true;
        }

        // update file map and try again
        if ( _Update() )
        {
            SHAREDLOCK( _mapGuard );

            if_likely( _Exists( name ))
                return true;
        }

        return false;

    }

    bool  DiskDynamicStorage::Exists (const FileGroupName &) C_NE___
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
