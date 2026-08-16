// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "vfs/Archive/ReloadableArchiveStorage.h"

namespace AE::VFS
{

/*
=================================================
	Reload
=================================================
*/
	bool  ReloadableArchiveStorage::Reload (RC<RDataSource> archive) __NE___
	{
		return Reload( null, RVRef(archive) );
	}

	bool  ReloadableArchiveStorage::Reload (RC<AsyncRDataSource> asyncFile, RC<RDataSource> syncFile) __NE___
	{
		CHECK_ERR( asyncFile or syncFile );

		EXLOCK( _guard );

		_storage = null;

		auto	new_storage = VirtualFileStorageFactory::CreateStaticArchive( RVRef(asyncFile), RVRef(syncFile) );
		CHECK_ERR( new_storage );

		_storage = RC<ArchiveStaticStorage>{RVRef(new_storage)};
		return true;
	}

	bool  ReloadableArchiveStorage::Reload (const Path &filename) __NE___
	{
		EXLOCK( _guard );

		_storage = null;

		auto	new_storage = VirtualFileStorageFactory::CreateStaticArchive( filename );
		CHECK_ERR( new_storage );

		_storage = RC<ArchiveStaticStorage>{RVRef(new_storage)};
		return true;
	}

/*
=================================================
	Reset
=================================================
*/
	void  ReloadableArchiveStorage::Reset () __NE___
	{
		EXLOCK( _guard );

		_storage = null;
	}

/*
=================================================
	Open
=================================================
*/
	bool  ReloadableArchiveStorage::Open (OUT RC<RStream> &stream, FileName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Open( OUT stream, name );
	}

	bool  ReloadableArchiveStorage::Open (OUT RC<RDataSource> &ds, FileName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Open( OUT ds, name );
	}

	bool  ReloadableArchiveStorage::Open (OUT RC<AsyncRStream> &stream, FileName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Open( OUT stream, name );
	}

	bool  ReloadableArchiveStorage::Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Open( OUT ds, name );
	}

/*
=================================================
	Exists
=================================================
*/
	bool  ReloadableArchiveStorage::Exists (FileName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Exists( name );
	}

	bool  ReloadableArchiveStorage::Exists (FileGroupName::Ref name) C_NE___
	{
		SHAREDLOCK( _guard );
		CHECK_ERR( _storage );
		return _storage->Exists( name );
	}


} // AE::VFS
