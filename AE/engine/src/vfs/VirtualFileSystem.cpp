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
    bool  VirtualFileSystem::AddStorage (RC<IVirtualFileStorage> storage) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( storage );

        CATCH_ERR(
            _storageArr.push_back( storage );       // throw

            storage->_Append( INOUT _globalMap );   // throw
            return true;
        )
    }

/*
=================================================
    Open***
=================================================
*/
    auto  VirtualFileSystem::OpenAsStream (const FileName &name) C_NE___ -> RC<RStream>
    {
        RC<RStream>  result;
        return Open( OUT result, name ) ? RVRef(result) : Default;
    }

    auto  VirtualFileSystem::OpenAsSource (const FileName &name) C_NE___ -> RC<RDataSource>
    {
        RC<RDataSource>  result;
        return Open( OUT result, name ) ? RVRef(result) : Default;
    }

    auto  VirtualFileSystem::OpenAsAsyncDS (const FileName &name) C_NE___ -> RC<AsyncRDataSource>
    {
        RC<AsyncRDataSource>  result;
        return Open( OUT result, name ) ? RVRef(result) : Default;
    }

    bool  VirtualFileSystem::Open (OUT RC<RStream> &stream, const FileName &name) C_NE___
    {
        return _Open( OUT stream, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<RDataSource> &ds, const FileName &name) C_NE___
    {
        return _Open( OUT ds, name );
    }

    bool  VirtualFileSystem::Open (OUT RC<AsyncRDataSource> &ds, const FileName &name) C_NE___
    {
        return _Open( OUT ds, name );
    }

/*
=================================================
    _Open
=================================================
*/
    template <typename ResultType>
    bool  VirtualFileSystem::_Open (OUT ResultType &result, const FileName &name) C_NE___
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
                    for (auto& st : _storageArr) {
                        found |= (st.get() == iter->second.storage);
                    }
                    ASSERT( found );
                )

                return iter->second.storage->_OpenByIter( OUT result, name, iter->second.ref );
            }
        }

        // search in all storages
        for (auto& st : _storageArr)
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
    LoadAsync
=================================================
*
    Promise<void>  VirtualFileSystem::LoadAsync (const FileGroupName &name) const
    {
        // TODO
        return Default;
    }

/*
=================================================
    Exists
=================================================
*/
    bool  VirtualFileSystem::Exists (const FileName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        // find in global map
        {
            auto    iter = _globalMap.find( FileName::Optimized_t{name} );
            if_likely( iter != _globalMap.end() )
            {
                return true;
            }
        }

        // search in all storages
        for (auto& st : _storageArr)
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
    bool  VirtualFileSystem::Exists (const FileGroupName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        for (auto& st : _storageArr)
        {
            if_unlikely( st->Exists( name ))
                return true;
        }

        return false;
    }


} // AE::VFS
