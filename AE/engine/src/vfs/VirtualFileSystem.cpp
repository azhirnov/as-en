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
	InstanceCtor
=================================================
*/
	void  VirtualFileSystem::InstanceCtor::Create () __NE___
	{
		s_VirtualFileSystem.Create();

		MemoryBarrier( EMemoryOrder::Release );
	}

	void  VirtualFileSystem::InstanceCtor::Destroy () __NE___
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
	bool  VirtualFileSystem::AddStorage (StorageName::Ref name, RC<IVirtualFileStorage> storage) __NE___
	{
		return _AddStorage( StorageName::Optimized_t{name}, RVRef(storage) );
	}

	bool  VirtualFileSystem::AddStorage (RC<IVirtualFileStorage> storage) __NE___
	{
		DRC_EXLOCK( _drCheck );

		StorageName::Optimized_t	st_name {HashVal32{uint(_storageMap.size())}};

		return _AddStorage( st_name, RVRef(storage) );
	}

	bool  VirtualFileSystem::_AddStorage (StorageName::Optimized_t name, RC<IVirtualFileStorage> storage) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( not _isImmutable.load() );
		CHECK_ERR( storage );

		auto [it, inserted] = _storageMap.emplace( name, RVRef(storage) );
		CHECK_ERR( inserted );

		NOTHROW_ERR( it->second->_Append( INOUT _globalMap ));
		return true;
	}

/*
=================================================
	MakeImmutable
=================================================
*/
	bool  VirtualFileSystem::MakeImmutable () __NE___
	{
		DRC_EXLOCK( _drCheck );
		return not _isImmutable.exchange( true );
	}

/*
=================================================
	Open
=================================================
*/
	bool  VirtualFileSystem::Open (OUT RC<RStream> &stream, FileName::Ref name) C_NE___
	{
		return _OpenForRead( OUT stream, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<RDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _OpenForRead( OUT ds, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _OpenForRead( OUT ds, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<AsyncRStream> &stream, FileName::Ref name) C_NE___
	{
		RC<AsyncRDataSource>	ds;
		if ( Open( OUT ds, name ))
		{
			stream = MakeRC<AsyncRDataSourceAsStream>( RVRef(ds) );
			ASSERT( stream->IsOpen() );
			return true;
		}
		return false;
	}

	bool  VirtualFileSystem::Open (OUT RC<WStream> &stream, FileName::Ref name) C_NE___
	{
		return _OpenForWrite( OUT stream, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<WDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _OpenForWrite( OUT ds, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<AsyncWDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _OpenForWrite( OUT ds, name );
	}

	bool  VirtualFileSystem::Open (OUT RC<AsyncWStream> &stream, FileName::Ref name) C_NE___
	{
		RC<AsyncWDataSource>	ds;
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
	bool  VirtualFileSystem::_OpenForRead (OUT ResultType &result, FileName::Ref name) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

		// find in global map
		{
			auto	iter = _globalMap.find( FileName::Optimized_t{name} );
			if_likely( iter != _globalMap.end() )
			{
				NonNull( iter->second.storage );
				NonNull( iter->second.ref );

				DEBUG_ONLY(
					bool	found = false;
					for (auto& st : _storageMap.GetValueArray()) {
						found |= (st.get() == iter->second.storage);
					}
					ASSERT_MSG( found, "memory corruption - iterator contains pointer to storage which is not exists" );
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
	bool  VirtualFileSystem::_OpenForWrite (OUT ResultType &result, FileName::Ref name) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

		// find in global map
		{
			auto	iter = _globalMap.find( FileName::Optimized_t{name} );
			if_likely( iter != _globalMap.end() )
			{
				NonNull( iter->second.storage );
				NonNull( iter->second.ref );

				DEBUG_ONLY(
					bool	found = false;
					for (auto& st : _storageMap.GetValueArray()) {
						found |= (st.get() == iter->second.storage);
					}
					ASSERT_MSG( found, "memory corruption - iterator contains pointer to storage which is not exists" );
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
	bool  VirtualFileSystem::Exists (FileName::Ref name) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

		// find in global map
		{
			auto	iter = _globalMap.find( FileName::Optimized_t{name} );
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
	bool  VirtualFileSystem::Exists (FileGroupName::Ref name) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

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
	bool  VirtualFileSystem::CreateFile (OUT FileName &name, const Path &path, StorageName::Ref stName) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

		auto	it = _storageMap.find( stName );
		CHECK_ERR( it != _storageMap.end() );

		return it->second->CreateFile( OUT name, path );
	}

/*
=================================================
	CreateUniqueFile
=================================================
*/
	bool  VirtualFileSystem::CreateUniqueFile (OUT FileName &name, INOUT Path &path, StorageName::Ref stName) C_NE___
	{
		CHECK_ERR( _isImmutable.load() );

		auto	it = _storageMap.find( stName );
		CHECK_ERR( it != _storageMap.end() );

		return it->second->CreateUniqueFile( OUT name, INOUT path );
	}


} // AE::VFS
