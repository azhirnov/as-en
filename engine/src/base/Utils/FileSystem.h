// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Wrapper for std::filesystem that disable all exceptions except std::bad_alloc
*/

#pragma once

#include "base/Math/Bytes.h"

#ifdef AE_ENABLE_GFS
#  include "filesystem.hpp"
	namespace _ae_fs_ = ghc::filesystem;
#else
#  include <filesystem>
	namespace _ae_fs_ = std::filesystem;
#endif

namespace AE::Base
{
	using Path = _ae_fs_::path;


	//
	// File System
	//

	class FileSystem final
	{
	private:
		FileSystem () = delete;
		~FileSystem () = delete;

	// types
	public:
		using DirectoryIter	= _ae_fs_::directory_iterator;


	// filesystem
	public:
		// remove file or empty directory.
		// returns 'true' if the file was deleted.
		static bool  Remove (const Path &p);

		// remove directory and all subdirectories.
		static bool  RemoveAll (const Path &p);

		// create directory, parent directory must be exists
		static bool  CreateDirectory (const Path &p);

		// create all directories that is not exists
		static bool  CreateDirectories (const Path &p);

		// set working directory
		static bool  SetCurrentPath (const Path &p);

		// returns 'true' if file or directory is exists
		ND_ static bool  Exists (const Path &p);
		
		// returns 'true' if path refers to a file
		ND_ static bool  IsFile (const Path &p);

		// returns 'true' if path refers to a directory
		ND_ static bool  IsDirectory (const Path &p);

		// returns current path
		ND_ static Path  CurrentPath ();

		// returns absolute path
		ND_ static Path  ToAbsolute (const Path &p);

		// returns relative path
		ND_ static Path  ToRelative (const Path &p, const Path &base);

		// enumerate all files in directory
		ND_ static DirectoryIter  Enum (const Path &p);

		// 
		static bool  CopyFile (const Path &from, const Path &to);
		static bool  CopyDirectory (const Path &from, const Path &to);

		// writes file system capacity and available space
		static bool  GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available);

		// replace unsupported symbols.
		// returns 'true' if name is not modified.
		template <typename T>
		static bool  ValidateFileName (INOUT BasicString<T> &name);

		
	// utils
	public:
		// searches for a directory for which 'Exists( ref )' returns 'true'
		static bool  FindAndSetCurrent (const Path &ref, uint depth);
		static bool  FindAndSetCurrent (const Path &base, const Path &ref, uint depth);

		static bool  SearchBackward (const Path &ref, uint depth, OUT Path &result);
		static bool  SearchBackward (const Path &base, const Path &ref, uint depth, OUT Path &result);

		static bool  SearchForward (const Path &ref, uint depth, OUT Path &result);
		static bool  SearchForward (const Path &base, const Path &ref, uint depth, OUT Path &result);

		static bool  Search (const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result);
		static bool  Search (const Path &base, const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result);


	// platform dependent
	public:
		#ifdef AE_PLATFORM_WINDOWS
		ND_ static Path  GetWindowsPath ();
		#endif
	};

	

	inline bool  FileSystem::Remove (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::remove( p, OUT ec );
	}
	
	inline bool  FileSystem::RemoveAll (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::remove_all( p, OUT ec ) != UMax;
	}
	
	inline bool  FileSystem::CreateDirectory (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::create_directory( p, OUT ec );
	}
	
	inline bool  FileSystem::CreateDirectories (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::create_directories( p, OUT ec );
	}
	
	inline bool  FileSystem::SetCurrentPath (const Path &p)
	{
		std::error_code	ec;
		_ae_fs_::current_path( p, OUT ec );
		return ec == Default;
	}
		
	inline bool  FileSystem::Exists (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::exists( p, OUT ec );
	}
	
	inline bool  FileSystem::IsFile (const Path &p)
	{
		std::error_code	ec;
		return not _ae_fs_::is_directory( p, OUT ec );	// TODO
	}

	inline bool  FileSystem::IsDirectory (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::is_directory( p, OUT ec );
	}

	inline Path  FileSystem::CurrentPath ()
	{
		std::error_code	ec;
		return _ae_fs_::current_path( OUT ec );
	}
	
	inline Path  FileSystem::ToAbsolute (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::absolute( p, OUT ec );
	}
	
	inline Path  FileSystem::ToRelative (const Path &p, const Path &base)
	{
		std::error_code	ec;
		return _ae_fs_::relative( p, base, OUT ec );
	}
	
	inline FileSystem::DirectoryIter  FileSystem::Enum (const Path &p)
	{
		std::error_code	ec;
		return _ae_fs_::directory_iterator{ p, OUT ec };
	}
	
	inline bool  FileSystem::CopyFile (const Path &from, const Path &to)
	{
		std::error_code	ec;
		return _ae_fs_::copy_file( from, to, _ae_fs_::copy_options::overwrite_existing, OUT ec );
	}

	inline bool  FileSystem::CopyDirectory (const Path &from, const Path &to)
	{
		std::error_code	ec;
		_ae_fs_::copy( from, to, _ae_fs_::copy_options::recursive, OUT ec );
		return not ec;
	}
	
	inline bool  FileSystem::GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available)
	{
		std::error_code	ec;
		auto	space = _ae_fs_::space( path, OUT ec );
		if ( not ec )
		{
			total		= Bytes{ space.capacity };
			available	= Bytes{ space.available };
			return true;
		}
		return false;
	}
	
	template <typename T>
	inline bool  FileSystem::ValidateFileName (INOUT BasicString<T> &name)
	{
		bool	res = true;
		for (usize i = 0; i < name.size(); ++i)
		{
			T&	c = name[i];

		#ifdef AE_PLATFORM_WINDOWS
			if ( (c == T('/')) | (c == T('\\')) | (c == T('?')) | (c == T('%')) | (c == T('*')) |
				 (c == T('|')) | (c == T(':'))  | (c == T('"')) | (c == T('<')) | (c == T('>')) )
			{
				res = false;
				c = T('_');
			}
		#elif defined(AE_PLATFORM_UNIX_BASED)
			if ( (c == T('/')) )
			{
				res = false;
				c = T('_');
			}
		#else
			#error Not implemented
		#endif
		}
		return res;
	}


	ND_ inline bool  IsAnsiPath (const Path &path)
	{
		for (auto& c : path.native())
		{
			if ( ulong(c) > 127 )
				return false;
		}
		return true;
	}

}	// AE::Base


namespace std
{
	template <>
	struct hash< AE::Base::Path >
	{
		ND_ size_t  operator () (const AE::Base::Path &value) const
		{
			ASSERT( value.is_absolute() );
			return size_t(AE::Base::HashOf( value.native() ));
		}
	};

}	// std
