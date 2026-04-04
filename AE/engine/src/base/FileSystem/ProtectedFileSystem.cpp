// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/FileSystem/ProtectedFileSystem.h"

namespace AE::Base
{
#define PFS_IMPL( _fn_ )										\
	bool  ProtectedFileSystem::_fn_ (const Path &p) C_NE___		\
	{															\
		Path	abs_path = FileSystem::ToAbsolute( p );			\
		if_unlikely( not _IsValid( abs_path ))					\
			return false;										\
		return FileSystem::_fn_( abs_path );					\
	}

	PFS_IMPL( DeleteFile )
	PFS_IMPL( DeleteDirectory )
	PFS_IMPL( DeleteEmptyDirectory )
	PFS_IMPL( CreateDirectory )
	PFS_IMPL( CreateDirectories )
	PFS_IMPL( CreateEmptyFile )
	PFS_IMPL( SetCurrentPath )
	PFS_IMPL( IsFile )
	PFS_IMPL( IsDirectory )
	PFS_IMPL( IsFileOrDirectory )
	PFS_IMPL( IsEmptyDirectory )

/*
=================================================
	constructor
=================================================
*/
	ProtectedFileSystem::ProtectedFileSystem (const Path &baseFolder) __NE___ :
		_baseFolder{ FileSystem::ToAbsolute( baseFolder )}
	{
		CHECK_FATAL( FileSystem::SetCurrentPath( _baseFolder ));
	}

/*
=================================================
	_IsValid
=================================================
*/
	bool  ProtectedFileSystem::_IsValid (const Path &absPath) C_NE___
	{
		Path	rel_path = FileSystem::ToRelative( absPath, _baseFolder );
		return	not rel_path.empty() and *rel_path.begin() != ".." ;
	}

/*
=================================================
	Equal
=================================================
*/
	bool  ProtectedFileSystem::Equal (const Path &lhs, const Path &rhs) C_NE___
	{
		Path	abs_lhs = FileSystem::ToAbsolute( lhs );
		Path	abs_rhs = FileSystem::ToAbsolute( rhs );

		CHECK_ERR( _IsValid( abs_lhs ) and _IsValid( abs_rhs ));
		return FileSystem::Equal( abs_lhs, abs_rhs );
	}

/*
=================================================
	LastWriteTime
=================================================
*/
	ProtectedFileSystem::Time_t  ProtectedFileSystem::LastWriteTime (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		if_unlikely( not _IsValid( abs_path ))
			return Default;
		return FileSystem::LastWriteTime( abs_path );
	}

/*
=================================================
	SetLastWriteTime
=================================================
*/
	bool  ProtectedFileSystem::SetLastWriteTime (const Path &p, Time_t t) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		if_unlikely( not _IsValid( abs_path ))
			return false;
		return FileSystem::SetLastWriteTime( abs_path, t );
	}

/*
=================================================
	ToAbsolute
=================================================
*/
	Path  ProtectedFileSystem::ToAbsolute (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return abs_path;
	}

/*
=================================================
	ToRelative
=================================================
*/
	Path  ProtectedFileSystem::ToRelative (const Path &p, const Path &base) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		Path	abs_base = FileSystem::ToAbsolute( base );

		CHECK_ERR( _IsValid( abs_path ) and _IsValid( abs_base ));
		return FileSystem::ToRelative( abs_path, abs_base );
	}

/*
=================================================
	Normalize
=================================================
*/
	Path  ProtectedFileSystem::Normalize (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return FileSystem::Normalize( p );
	}

/*
=================================================
	CurrentPath
=================================================
*/
	ProtectedFileSystem::DirectoryIter  ProtectedFileSystem::Enum (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return FileSystem::Enum( abs_path );
	}

/*
=================================================
	EnumRecursive
=================================================
*/
	ProtectedFileSystem::RecursiveDirectoryIter  ProtectedFileSystem::EnumRecursive (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return FileSystem::EnumRecursive( abs_path );
	}

/*
=================================================
	CopyFile
=================================================
*/
	bool  ProtectedFileSystem::CopyFile (const Path &from, const Path &to) C_NE___
	{
		Path	abs_from = FileSystem::ToAbsolute( from );
		Path	abs_to	 = FileSystem::ToAbsolute( to );

		if_unlikely( not _IsValid( abs_from ) or not _IsValid( abs_to ))
			return false;

		return FileSystem::CopyFile( abs_from, abs_to );
	}

/*
=================================================
	CopyFile
=================================================
*/
	bool  ProtectedFileSystem::CopyDirectory (const Path &from, const Path &to) C_NE___
	{
		Path	abs_from = FileSystem::ToAbsolute( from );
		Path	abs_to	 = FileSystem::ToAbsolute( to );

		if_unlikely( not _IsValid( abs_from ) or not _IsValid( abs_to ))
			return false;

		return FileSystem::CopyDirectory( abs_from, abs_to );
	}

/*
=================================================
	MergeDirectory
=================================================
*/
	bool  ProtectedFileSystem::MergeDirectory (const Path &from, const Path &to, ECopyOpt opt) C_NE___
	{
		Path	abs_from = FileSystem::ToAbsolute( from );
		Path	abs_to	 = FileSystem::ToAbsolute( to );

		if_unlikely( not _IsValid( abs_from ) or not _IsValid( abs_to ))
			return false;

		return FileSystem::MergeDirectory( abs_from, abs_to, opt );
	}

/*
=================================================
	Rename
=================================================
*/
	bool  ProtectedFileSystem::Rename (const Path &oldName, const Path &newName) C_NE___
	{
		Path	abs_old = FileSystem::ToAbsolute( oldName );
		Path	abs_new = FileSystem::ToAbsolute( newName );

		if_unlikely( not _IsValid( abs_old ) or not _IsValid( abs_new ))
			return false;

		return FileSystem::Rename( abs_old, abs_new );
	}

/*
=================================================
	FileSize
=================================================
*/
	Bytes  ProtectedFileSystem::FileSize (const Path &p) C_NE___
	{
		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return FileSystem::FileSize( abs_path );
	}

/*
=================================================
	GetSpace
=================================================
*/
	bool  ProtectedFileSystem::GetSpace (const Path &p, OUT Bytes &total, OUT Bytes &available) C_NE___
	{
		total		= 0_b;
		available	= 0_b;

		Path	abs_path = FileSystem::ToAbsolute( p );
		CHECK_ERR( _IsValid( abs_path ));
		return FileSystem::GetSpace( abs_path, OUT total, OUT available );
	}

} // AE::Base
