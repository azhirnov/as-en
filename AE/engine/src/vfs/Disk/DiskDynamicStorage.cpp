// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/FileAsyncDataSource.h"

#include "vfs/Disk/DiskDynamicStorage.h"
#include "vfs/Disk/Utils.cpp.h"

namespace AE::VFS
{

/*
=================================================
	_Create
=================================================
*/
	bool  DiskDynamicStorage::_Create (const Path &folder, StringView prefix, bool createFolder) __NE___
	{
		TRY{
			CHECK_ERR( _folder.empty() );

			_folder = FileSystem::ToAbsolute( folder );

			if ( createFolder )
				FileSystem::CreateDirectories( _folder );

			CHECK_ERR( FileSystem::IsDirectory( _folder ));

			auto	file_map = _fileMap.WriteLock();
			file_map->map.clear();
			file_map->map.reserve( 128 );	// throw
			file_map->lastUpdate = Default;

			_prefix	= String{prefix};
		}
		CATCH_ALL(
			return false;
		)

		Unused( _Update() );
		return true;
	}

/*
=================================================
	_Update
=================================================
*/
	bool  DiskDynamicStorage::_Update () C_NE___
	{
		TRY{
			auto	cur_time = TimePoint_t::clock::now();

			if ( TimeCast<seconds>( cur_time - _fileMap.ConstPtr()->lastUpdate ) < _UpdateInterval )
				return false;

			_fileMap->lastUpdate = cur_time;

			// build file map
			Array< Path >	stack;
			stack.push_back( _folder );

			String	str, name;
			name.reserve( 64 );

			for (; not stack.empty();)
			{
				Path	path = RVRef(stack.back());
				stack.pop_back();

				for (auto& entry : FileSystem::Enum( path ))
				{
					if ( entry.IsFile() )
					{
						auto	file = FileSystem::ToRelative( entry.Get(), _folder );

						CHECK_ERR( Convert( OUT str, file ));

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
		CATCH_ALL()

		return true;
	}

/*
=================================================
	Open
=================================================
*/
	template <typename ImplType, typename ResultType>
	bool  DiskDynamicStorage::_Open2 (OUT ResultType &result, FileName::Ref name) C_NE___
	{
		Path	path;
		{
			auto	file_map = _fileMap.ReadLock();
			auto	iter	 = file_map->map.find( FileName::Optimized_t{name} );

			if_unlikely( iter == file_map->map.end() )
				return false;

			path = _folder / iter->second;
		}

		auto	file = MakeRC<ImplType>( path );
		if_likely( file->IsOpen() )
		{
			result = file;
			return true;
		}
		return false;
	}

	template <typename ImplType, typename ResultType>
	bool  DiskDynamicStorage::_Open (OUT ResultType &result, FileName::Ref name) C_NE___
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

	bool  DiskDynamicStorage::Open (OUT RC<RStream> &stream, FileName::Ref name) C_NE___
	{
		return _Open<FileRStream>( OUT stream, name );
	}

	bool  DiskDynamicStorage::Open (OUT RC<RDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _Open<FileRDataSource>( OUT ds, name );
	}

	bool  DiskDynamicStorage::Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _Open< Threading::FileAsyncRDataSource >( OUT ds, name );
	}

/*
=================================================
	Open
=================================================
*/
	bool  DiskDynamicStorage::Open (OUT RC<WStream> &stream, FileName::Ref name) C_NE___
	{
		return _Open<FileWStream>( OUT stream, name );
	}

	bool  DiskDynamicStorage::Open (OUT RC<WDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _Open<FileWDataSource>( OUT ds, name );
	}

	bool  DiskDynamicStorage::Open (OUT RC<AsyncWDataSource> &ds, FileName::Ref name) C_NE___
	{
		return _Open< Threading::FileAsyncWDataSource >( OUT ds, name );
	}

/*
=================================================
	CreateFile
=================================================
*/
	bool  DiskDynamicStorage::CreateFile (OUT FileName &name, const Path &inPath) C_NE___
	{
		TRY{
			const Path	abs_path = (_folder / inPath).lexically_normal();	// path without '..'
			const Path	rel_path = FileSystem::ToRelative( abs_path, _folder );

			CHECK_ERR( *rel_path.begin() != ".." );
			FileSystem::CreateDirectories( abs_path.parent_path() );

			String	str;
			CHECK_ERR( Convert( OUT str, rel_path ));

			name = FileName{_prefix + str};
			DEBUG_ONLY( _fileMap->hashCollisionCheck.Add( name ));

			_fileMap->map.emplace( FileName::Optimized_t{name}, RVRef(str) );
			return true;
		}
		CATCH_ALL(
			return false;
		)
	}

/*
=================================================
	CreateUniqueFile
=================================================
*/
#if 1
	bool  DiskDynamicStorage::CreateUniqueFile (OUT FileName &name, INOUT Path &inoutPath) C_NE___
	{
		TRY{
			const Path		abs_path	= (_folder / inoutPath).lexically_normal();	// path without '..'
			String			fname		= ToString( abs_path.stem() );
			const String	ext			= ToString( abs_path.extension() );
			const usize		len			= fname.length();
			Path			path		= abs_path.parent_path();

			{
				const Path	rel_path = FileSystem::ToRelative( abs_path, _folder );
				CHECK_ERR( *rel_path.begin() != ".." );
				FileSystem::CreateDirectories( abs_path.parent_path() );
			}

			const auto	BuildName = [&] (OUT Path &p, usize idx) __Th___
			{{
				fname.resize( len );
				fname << ToString(idx) << ext;
				p = path / fname;
			}};

			const auto	Consume = [this, &inoutPath, &name] (const Path &p) __Th___ -> bool
			{{
				inoutPath = FileSystem::ToRelative( p, _folder );

				String	str;
				CHECK_ERR( Convert( OUT str, inoutPath ));

				name = FileName{_prefix + str};
				DEBUG_ONLY( _fileMap->hashCollisionCheck.Add( name ));

				_fileMap->map.emplace( FileName::Optimized_t{name}, RVRef(str) );
				return true;
			}};

			FileSystem::FindUnusedFilename( BuildName, Consume );
			return name.IsDefined();
		}
		CATCH_ALL(
			return false;
		)
	}
#else

	bool  DiskDynamicStorage::CreateUniqueFile (OUT FileName &name, INOUT Path &inoutPath) C_NE___
	{
		Path	path = (_folder / inoutPath).lexically_normal();	// path without '..'
				path = FileSystem::ToRelative( path, _folder );

		// TODO: search in map
	}
#endif

/*
=================================================
	Exists
=================================================
*/
	bool  DiskDynamicStorage::Exists (FileName::Ref name) C_NE___
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

	bool  DiskDynamicStorage::Exists (FileGroupName::Ref) C_NE___
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
	RC<IVirtualFileStorage>  VirtualFileStorageFactory::CreateDynamicFolder (const Path &folder, StringView prefix, Bool createFolder) __NE___
	{
		auto	result = RC<DiskDynamicStorage>{ new DiskDynamicStorage{}};
		CHECK_ERR( result->_Create( folder, prefix, createFolder ));
		return result;
	}


} // AE::VFS
