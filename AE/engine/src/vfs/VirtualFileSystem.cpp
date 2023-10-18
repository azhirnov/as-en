// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "vfs/VirtualFileSystem.h"

namespace AE::VFS
{
    using namespace AE::Threading;

/*
=================================================
    _Instance
=================================================
*/
    INTERNAL_LINKAGE( InPlace<VirtualFileSystem>  s_VirtualFileSystem );

    VirtualFileSystem&  VirtualFileSystem::_Instance () __NE___
    {
        return s_VirtualFileSystem.AsRef();
    }

/*
=================================================
    CreateInstance
=================================================
*/
    void  VirtualFileSystem::CreateInstance () __NE___
    {
        s_VirtualFileSystem.Create();

        MemoryBarrier( EMemoryOrder::Release );
    }

/*
=================================================
    DestroyInstance
=================================================
*/
    void  VirtualFileSystem::DestroyInstance () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );

        s_VirtualFileSystem.Destroy();

        MemoryBarrier( EMemoryOrder::Release );
    }

/*
=================================================
    AddStorage
=================================================
*/
    bool  VirtualFileSystem::AddStorage (const StorageName &name, RC<IVirtualFileStorage> storage) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( storage );

        CHECK_ERR( _storageMap.emplace( StorageName::Optimized_t{name}, storage ).second );

        CATCH_ERR( storage->_Append( INOUT _globalMap );)
        return true;
    }

    bool  VirtualFileSystem::AddStorage (RC<IVirtualFileStorage> storage) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( storage );

        StorageName::Optimized_t    st_name {HashVal32{uint(_storageMap.size())}};

        CHECK_ERR( _storageMap.emplace( st_name, storage ).second );

        CATCH_ERR( storage->_Append( INOUT _globalMap );)
        return true;
    }

/*
=================================================
    Open
=================================================
*/
    bool  VirtualFileSystem::Open (OUT RC<RStream> &stream, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT stream, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<RDataSource> &ds, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT ds, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<AsyncRDataSource> &ds, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT ds, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<AsyncRStream> &stream, FileNameRef name) C_NE___
    {
        RC<AsyncRDataSource>    ds;
        if ( Open( OUT ds, name ))
        {
            stream = MakeRC<AsyncRDataSourceAsStream>( RVRef(ds) );
            ASSERT( stream->IsOpen() );
            return true;
        }
        return false;
    }

    bool  VirtualFileSystem::Open (OUT RC<WStream> &stream, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT stream, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<WDataSource> &ds, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT ds, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<AsyncWDataSource> &ds, FileNameRef name) C_NE___
    {
        return _OpenForRead( OUT ds, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<AsyncWStream> &stream, FileNameRef name) C_NE___
    {
        RC<AsyncWDataSource>    ds;
        if ( Open( OUT ds, name ))
        {
            stream = MakeRC<AsyncWDataSourceAsStream>( RVRef(ds) );
            ASSERT( stream->IsOpen() );
            return true;
        }
        return false;
    }

/*
=================================================
    _OpenForRead
=================================================
*/
    template <typename ResultType>
    bool  VirtualFileSystem::_OpenForRead (OUT ResultType &result, FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
            {
                ASSERT( iter->second.storage != null );
                ASSERT( iter->second.ref != null );

                DEBUG_ONLY(
                    bool    found = false;
                    for (auto& st : _storageMap.GetValueArray()) {
                        found |= (st.get() == iter->second.storage);
                    }
                    ASSERT( found );
                )

                return iter->second.storage->_OpenByIter( OUT result, name, iter->second.ref );
            }
        }

        // search in all storages
        for (auto& st : _storageMap.GetValueArray())
        {
            if_unlikely( st->Open( OUT result, name ))
                return true;
        }

        #if not AE_OPTIMIZE_IDS
        DBG_WARNING( "Failed to open VFS file '"s << name.GetName() << "'" );
        #endif
        return false;
    }

/*
=================================================
    _OpenForWrite
=================================================
*/
    template <typename ResultType>
    bool  VirtualFileSystem::_OpenForWrite (OUT ResultType &result, FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
            {
                ASSERT( iter->second.storage != null );
                ASSERT( iter->second.ref != null );

                DEBUG_ONLY(
                    bool    found = false;
                    for (auto& st : _storageMap.GetValueArray()) {
                        found |= (st.get() == iter->second.storage);
                    }
                    ASSERT( found );
                )

                return iter->second.storage->_OpenByIter( OUT result, name, iter->second.ref );
            }
        }

        // search in all storages
        for (auto& st : _storageMap.GetValueArray())
        {
            if_unlikely( st->Open( OUT result, name ))
                return true;
        }

        #if not AE_OPTIMIZE_IDS
        DBG_WARNING( "Failed to open VFS file '"s << name.GetName() << "'" );
        #endif
        return false;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  VirtualFileSystem::Exists (FileNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
                return true;
        }

        // search in all storages
        for (auto& st : _storageMap.GetValueArray())
        {
            if_unlikely( st->Exists( name ))
                return true;
        }

        return false;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  VirtualFileSystem::Exists (FileGroupNameRef name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        for (auto& st : _storageMap.GetValueArray())
        {
            if_unlikely( st->Exists( name ))
                return true;
        }

        return false;
    }

/*
=================================================
    CreateFile
=================================================
*/
    bool  VirtualFileSystem::CreateFile (const StorageName &stName, OUT FileName &name, const Path &path) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _storageMap.find( stName );
        CHECK_ERR( it != _storageMap.end() );

        return it->second->CreateFile( OUT name, path );
    }

/*
=================================================
    CreateUniqueFile
=================================================
*/
    bool  VirtualFileSystem::CreateUniqueFile (const StorageName &stName, OUT FileName &name, INOUT Path &path) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _storageMap.find( stName );
        CHECK_ERR( it != _storageMap.end() );

        return it->second->CreateUniqueFile( OUT name, INOUT path );
    }


} // AE::VFS
