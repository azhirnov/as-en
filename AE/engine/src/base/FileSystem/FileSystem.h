// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Wrapper for std::filesystem that disable all exceptions.

	Thread safe:  yes for independent paths
*/

#pragma once

#include "base/Math/Byte.h"
#include "base/Utils/SourceLoc.h"
#include "base/Pointers/RefCounter.h"

namespace AE::Base
{

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
		using Time_t = std::filesystem::file_time_type;

		struct DirectoryEntry;
		struct DirectoryIter;
		struct RecursiveDirectoryIter;

		enum class ECopyOpt : ubyte
		{
			ReplaceAll,		// delete 'dst' and put 'src'
			FileReplace,	// use 'src' file
			FileKeep,		// keep 'dst' file
			FileNewest,		// compare last modification time
			FileLargest,	// compare file size
			FileKeepBoth,	// keep 'dst' file, rename 'src' file
		};


	// filesystem
	public:

		// Remove file.
		// Returns 'true' if the file was deleted.
		static bool  DeleteFile (const Path &p)													__NE___	{ return IsFile( p ) and _Remove( p ); }

		// Remove directory and all subdirectories.
		static bool  DeleteDirectory (const Path &p)											__NE___	{ return IsDirectory( p ) and _RemoveAll( p ); }
		static bool  DeleteEmptyDirectory (const Path &p)										__NE___	{ return IsDirectory( p ) and _Remove( p ); }

		// Create directory, parent directory must be exists.
		// Returns 'false' on error.
		static bool  CreateDirectory (const Path &p)											__NE___;

		// Create all directories that is not exists.
		// Returns 'false' on error.
		static bool  CreateDirectories (const Path &p)											__NE___;

		// Create an empty file.
		static bool  CreateEmptyFile (const Path &p)											__NE___;

		// Set working directory.
		static bool  SetCurrentPath (const Path &p)												__NE___;

		// Returns current path
		ND_ static Path  CurrentPath ()															__NE___;

		// Returns 'true' if path refers to a file.
		ND_ static bool  IsFile (const Path &p)													__NE___;

		// Returns 'true' if path refers to a directory.
		ND_ static bool  IsDirectory (const Path &p)											__NE___;

		// Returns 'true' if path refers to a file or directory.
		ND_ static bool  IsFileOrDirectory (const Path &p)										__NE___	{ return _Exists( p ); }

		// Returns 'true' if path refers to an empty directory.
		ND_ static bool  IsEmptyDirectory (const Path &p)										__NE___	{ return _IsEmpty( p ) and IsDirectory( p ); }

		// Returns 'true' if 'lhs' and 'rhs' refer to the same file or directory.
		ND_ static bool  Equal (const Path &lhs, const Path &rhs)								__NE___;

		// Returns time of the last modification of file.
		ND_ static Time_t  LastWriteTime (const Path &p)										__NE___;

		// Set time of the last modification of file.
		static bool  SetLastWriteTime (const Path &p, Time_t t)									__NE___;

		// Returns absolute path.
		ND_ static Path  ToAbsolute (const Path &p)												__NE___;

		// Returns relative path.
		ND_ static Path  ToRelative (const Path &p, const Path &base)							__NE___;

		// Check that 'p' is subpath of 'base' or equal to 'base'.
		// Optionally return subpath in 'outSubPath'.
		ND_ static bool  IsSubPath (const Path &p, const Path &base, OUT Path* outSubPath = null)__NE___;

		// Returns path without /../
		ND_ static Path  Normalize (const Path &p)												__NE___;

		// Enumerate all files in directory.
		ND_ static auto  Enum (const Path &p)													__NE___;

		// Enumerate all files in directory and its subdirectories.
		ND_ static auto  EnumRecursive (const Path &p)											__NE___;

		// Copy file. Returns 'false' if failed.
		// Will override existing file.
		static bool  CopyFile (const Path &from, const Path &to)								__NE___;

		// Recursive copy directory. Returns 'false' if failed.
		// Will override existing files.
		static bool  CopyDirectory (const Path &from, const Path &to)							__NE___;

		// Recursive merge directories. Returns 'false' if failed.
		static bool  MergeDirectory (const Path &from, const Path &to, ECopyOpt opt)			__NE___;

		// Move or rename filesystem object (file/directory).
		static bool  Rename (const Path &oldName, const Path &newName)							__NE___;

		// Returns file size or 0 on error.
		ND_ static Bytes  FileSize (const Path &p)												__NE___;

		// Writes file system capacity and available space.
		static bool  GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available)		__NE___;

		// Replace unsupported symbols.
		// Returns 'true' if name is modified.
		template <typename T>
		static bool  ValidateFileName (INOUT BasicString<T> &name)								__NE___;

		// Calculate hash of path.
		ND_ static HashVal  Hash (const Path &p)												__NE___	{ ASSERT( p.is_absolute() );  return HashVal{ std::filesystem::hash_value( p )}; }


	// utils
	public:
		// Searches for a path for which 'IsDirectory( ref )' returns 'true'.
		static bool  FindAndSetCurrent (const Path &ref, uint depth)														__Th___;
		static bool  FindAndSetCurrent (const Path &base, const Path &ref, uint depth)										__Th___;

		static bool  SearchBackward (const Path &ref, uint depth, OUT Path &result)											__Th___;
		static bool  SearchBackward (const Path &base, const Path &ref, uint depth, OUT Path &result)						__Th___;

		static bool  SearchForward (const Path &ref, uint depth, OUT Path &result)											__Th___;
		static bool  SearchForward (const Path &base, const Path &ref, uint depth, OUT Path &result)						__Th___;

		static bool  Search (const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result)						__Th___;
		static bool  Search (const Path &base, const Path &ref, uint backwardDepth, uint forwardDepth, OUT Path &result)	__Th___;

		//
		static bool  FindUnusedFilename (const Function< void (OUT Path &, usize idx) >	&buildName,
										 const Function< bool (const Path &) >			&consume,
										 uint											maxStep = 1000)						__Th___;

		static bool  MakeUniqueName (INOUT Path &p)																			__Th___;

		ND_ static StringView  ToShortPath (StringView file, uint maxParts = 3)												__NE___;

		// Will rewrite existing file, but if file in use will try name with '-number' suffix.
		template <typename FileType, typename ModeType>
		ND_ static RC<FileType>  OpenUnusedFile (INOUT Path &p, ModeType mode, uint maxAttempts = 100)						__NE___;

		ND_ static Path  ReplaceFirstFolder (const Path &input, const Path &replacement)									__NE___;


	// platform dependent
	public:
	  #ifdef AE_PLATFORM_WINDOWS
		// Returns path like a 'C:\Windows'
		ND_ static Path  GetWindowsPath ()								__NE___;

		ND_ static Array<Path>	GetLogicalDrives ()						__NE___;
	  #endif

	  #ifdef AE_PLATFORM_LINUX
		ND_ static Array<Path>	GetMountPoints ()						__NE___;
	  #endif

	private:
		ND_ static bool  _Exists (const Path &p)						__NE___;
		ND_ static bool  _IsEmpty (const Path &p)						__NE___;

			static bool  _Remove (const Path &p)						__NE___;
			static bool  _RemoveAll (const Path &p)						__NE___;
	};
//-----------------------------------------------------------------------------



	//
	// Directory iterator
	//
	struct FileSystem::DirectoryIter
	{
		friend class FileSystem;
	private:
		std::filesystem::directory_iterator		_it;

		DirectoryIter (std::filesystem::directory_iterator it)	__NE___	: _it{it} {}
	public:
		DirectoryIter ()										__NE___	= default;
		DirectoryIter (DirectoryIter &&)						__NE___	= default;
		DirectoryIter (const DirectoryIter &)					__NE___	= default;
		~DirectoryIter ()										__NE___	= default;

		DirectoryIter&  operator = (const DirectoryIter &)		__NE___ = default;
		DirectoryIter&  operator = (DirectoryIter &&)			__NE___ = default;

		ND_ DirectoryEntry const&	operator *  ()				C_NE___;
		ND_ DirectoryEntry const*	operator -> ()				C_NE___;

			DirectoryIter &			operator ++ ()				__NE___;

		ND_ bool  operator == (const DirectoryIter &rhs)		C_NE___	{ return _it == rhs._it; }
		ND_ bool  operator != (const DirectoryIter &rhs)		C_NE___	{ return _it != rhs._it; }
	};
	StaticAssert( sizeof(FileSystem::DirectoryIter) == sizeof(std::filesystem::directory_iterator) );



	//
	// Recursive Directory iterator
	//
	struct FileSystem::RecursiveDirectoryIter
	{
		friend class FileSystem;
	private:
		std::filesystem::recursive_directory_iterator		_it;

		RecursiveDirectoryIter (std::filesystem::recursive_directory_iterator it)	__NE___	: _it{it} {}
	public:
		RecursiveDirectoryIter ()													__NE___	= default;
		RecursiveDirectoryIter (RecursiveDirectoryIter &&)							__NE___	= default;
		RecursiveDirectoryIter (const RecursiveDirectoryIter &)						__NE___	= default;
		~RecursiveDirectoryIter ()													__NE___	= default;

		RecursiveDirectoryIter&  operator = (const RecursiveDirectoryIter &)		__NE___ = default;
		RecursiveDirectoryIter&  operator = (RecursiveDirectoryIter &&)				__NE___ = default;

		ND_ DirectoryEntry const&	operator *  ()									C_NE___;
		ND_ DirectoryEntry const*	operator -> ()									C_NE___;

			RecursiveDirectoryIter&	operator ++ ()									__NE___;

		ND_ bool  operator == (const RecursiveDirectoryIter &rhs)					C_NE___	{ return _it == rhs._it; }
		ND_ bool  operator != (const RecursiveDirectoryIter &rhs)					C_NE___	{ return _it != rhs._it; }
	};
	StaticAssert( sizeof(FileSystem::RecursiveDirectoryIter) == sizeof(std::filesystem::recursive_directory_iterator) );



	//
	// Directory Entry
	//
	struct FileSystem::DirectoryEntry
	{
		friend struct FileSystem::DirectoryIter;
		friend struct FileSystem::RecursiveDirectoryIter;

	private:
		std::filesystem::directory_entry	_entry;

		DirectoryEntry (std::filesystem::directory_entry e)	__NE___	: _entry{RVRef(e)} {}
	public:
		DirectoryEntry ()									__NE___	= default;
		DirectoryEntry (const DirectoryEntry &)						= default;
		DirectoryEntry (DirectoryEntry &&)					__NE___ = default;

		DirectoryEntry&  operator = (const DirectoryEntry &)__NE___	= default;
		DirectoryEntry&	 operator = (DirectoryEntry &&)		__NE___	= default;

		operator const Path & ()							C_NE___	{ return _entry.path(); }

		ND_ Path const&		Get ()							C_NE___	{ return _entry.path(); }
		ND_ bool			Exists ()						C_NE___	{ std::error_code ec;  return _entry.exists( OUT ec ); }
		ND_ bool			IsDirectory ()					C_NE___	{ std::error_code ec;  return _entry.is_directory( OUT ec ); }
		ND_ bool			IsFile ()						C_NE___	{ std::error_code ec;  return _entry.is_regular_file( OUT ec ); }
		ND_ Bytes			FileSize ()						C_NE___	{ std::error_code ec;  return Bytes{_entry.file_size( OUT ec )}; }
		ND_ Time_t			LastWriteTime ()				C_NE___	{ std::error_code ec;  return _entry.last_write_time( OUT ec ); }
		ND_ auto			Status ()						C_NE___	{ std::error_code ec;  return _entry.status( OUT ec ); }

		ND_ bool  operator == (const DirectoryEntry &rhs)	C_NE___	{ return _entry == rhs._entry; }
		ND_ bool  operator != (const DirectoryEntry &rhs)	C_NE___	{ return _entry != rhs._entry; }
		ND_ bool  operator <  (const DirectoryEntry &rhs)	C_NE___	{ return _entry <  rhs._entry; }
		ND_ bool  operator >  (const DirectoryEntry &rhs)	C_NE___	{ return _entry >  rhs._entry; }
		ND_ bool  operator <= (const DirectoryEntry &rhs)	C_NE___	{ return _entry <= rhs._entry; }
		ND_ bool  operator >= (const DirectoryEntry &rhs)	C_NE___	{ return _entry >= rhs._entry; }
	};
	StaticAssert( sizeof(FileSystem::DirectoryEntry) == sizeof(std::filesystem::directory_entry) );
//-----------------------------------------------------------------------------



	//
	// Path and Line
	//
	struct PathAndLine
	{
		Path		path;
		uint		line	= 0;

		PathAndLine ()									__NE___	{}
		explicit PathAndLine (Path path, uint line = 0)	__NE___	: path{RVRef(path)}, line{line} {}
		explicit PathAndLine (const SourceLoc &loc)		__NE___ : path{loc.FileName()}, line{loc.Line()} {}
		explicit PathAndLine (const SourceLocCopy &loc)	__NE___ : path{loc.FileName()}, line{loc.Line()} {}

		PathAndLine (const PathAndLine &)				__NE___ = default;
		PathAndLine (PathAndLine &&)					__NE___	= default;

		PathAndLine&  operator = (const PathAndLine &)	__NE___	= default;
		PathAndLine&  operator = (PathAndLine &&)		__NE___	= default;
	};
//-----------------------------------------------------------------------------


	//
	// Path Hasher
	//
	struct PathHasher
	{
		ND_ usize  operator () (const Path &p) C_NE___
		{
			return usize(FileSystem::Hash( p ));
		}
	};
//-----------------------------------------------------------------------------

	#ifdef AE_CFG_DEBUG
	#	define FS_MSG( _expr_, _msg_ )		if(bool{_expr_}){}else{AE_LOGW( _msg_ );}
	#else
	#	define FS_MSG( _expr_, _msg_ )		{}
	#endif



	inline FileSystem::DirectoryEntry const&  FileSystem::DirectoryIter::operator *  ()			C_NE___ { return reinterpret_cast<DirectoryEntry const&>( _it.operator* () ); }
	inline FileSystem::DirectoryEntry const*  FileSystem::DirectoryIter::operator -> ()			C_NE___ { return reinterpret_cast<DirectoryEntry const*>( _it.operator->() ); }

	Nd__In FileSystem::DirectoryIter  begin (FileSystem::DirectoryIter it)						__NE___ { return it; }
	Nd__In FileSystem::DirectoryIter  end   (FileSystem::DirectoryIter)							__NE___ { return {}; }

	inline FileSystem::DirectoryIter&  FileSystem::DirectoryIter::operator ++ ()				__NE___
	{
		std::error_code ec;
		_it.increment( OUT ec );	// throw 'std::bad_alloc'
		FS_MSG( not ec, "DirectoryIterator++ error: " + ec.message() );
		if_unlikely( ec ) *this = {}; // end
		return *this;
	}


	inline FileSystem::DirectoryEntry const&  FileSystem::RecursiveDirectoryIter::operator * ()	C_NE___ { return reinterpret_cast<DirectoryEntry const&>( _it.operator* () ); }
	inline FileSystem::DirectoryEntry const*  FileSystem::RecursiveDirectoryIter::operator ->()	C_NE___ { return reinterpret_cast<DirectoryEntry const*>( _it.operator->() ); }

	Nd__In FileSystem::RecursiveDirectoryIter  begin (FileSystem::RecursiveDirectoryIter it)	__NE___ { return it; }
	Nd__In FileSystem::RecursiveDirectoryIter  end   (FileSystem::RecursiveDirectoryIter)		__NE___ { return {}; }

	inline FileSystem::RecursiveDirectoryIter&  FileSystem::RecursiveDirectoryIter::operator ++ () __NE___
	{
		std::error_code ec;
		_it.increment( OUT ec );	// throw 'std::bad_alloc'
		FS_MSG( not ec, "RecursiveDirectoryIterator++ error: " + ec.message() );
		if_unlikely( ec ) *this = {}; // end
		return *this;
	}


	inline bool  FileSystem::_Remove (const Path &p) __NE___
	{
		std::error_code	ec;
		bool	res = std::filesystem::remove( p, OUT ec );
		FS_MSG( not ec, "Remove('" + p.string() + "'): " + ec.message() );
		return res;
	}

	inline bool  FileSystem::_RemoveAll (const Path &p) __NE___
	{
		std::error_code	ec;
		std::filesystem::remove_all( p, OUT ec );
		FS_MSG( not ec, "RemoveAll('" + p.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::CreateDirectory (const Path &p) __NE___
	{
		if ( p.empty() )
			return true;

		std::error_code	ec;
		std::filesystem::create_directory( p, OUT ec );
		FS_MSG( not ec, "CreateDirectory('" + p.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::CreateDirectories (const Path &p) __NE___
	{
		if ( p.empty() )
			return true;

		std::error_code	ec;
		std::filesystem::create_directories( p, OUT ec );
		FS_MSG( not ec, "CreateDirectories('" + p.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::SetCurrentPath (const Path &p) __NE___
	{
		if ( p.empty() )
			return true;

		std::error_code	ec;
		std::filesystem::current_path( p, OUT ec );
		FS_MSG( not ec, "SetCurrentPath('" + p.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::_Exists (const Path &p) __NE___
	{
		std::error_code	ec;
		return std::filesystem::exists( p, OUT ec );
	}

	inline FileSystem::Time_t  FileSystem::LastWriteTime (const Path &p) __NE___
	{
		std::error_code	ec;
		return std::filesystem::last_write_time( p, OUT ec );
	}

	inline bool  FileSystem::SetLastWriteTime (const Path &p, Time_t t) __NE___
	{
		std::error_code	ec;
		std::filesystem::last_write_time( p, t, OUT ec );
		FS_MSG( not ec, "SetLastWriteTime('" + p.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::IsDirectory (const Path &p) __NE___
	{
		std::error_code	ec;
		return std::filesystem::is_directory( p, OUT ec );
	}

	inline bool  FileSystem::IsFile (const Path &p) __NE___
	{
		std::error_code	ec;
		return std::filesystem::is_regular_file( p, OUT ec );
	}

	inline bool  FileSystem::_IsEmpty (const Path &p) __NE___
	{
		std::error_code	ec;
		return std::filesystem::is_empty( p, OUT ec );
	}

	inline Path  FileSystem::CurrentPath () __NE___
	{
		std::error_code	ec;
		return std::filesystem::current_path( OUT ec );
	}

	inline Path  FileSystem::ToAbsolute (const Path &p) __NE___
	{
		if ( p.empty() )
			return CurrentPath();

		std::error_code	ec;
		return std::filesystem::absolute( p, OUT ec ).lexically_normal();
	}

	inline Path  FileSystem::ToRelative (const Path &p, const Path &base) __NE___
	{
		std::error_code	ec;
		return std::filesystem::relative( p, base, OUT ec );
	}

	inline Path  FileSystem::Normalize (const Path &p) __NE___
	{
		return p.lexically_normal();
	}


	inline auto  FileSystem::Enum (const Path &p) __NE___
	{
		ASSERT( not p.empty() );	// use "." instead
		std::error_code	ec;
		return DirectoryIter{ std::filesystem::directory_iterator{ p, std::filesystem::directory_options::skip_permission_denied, OUT ec }};
	}

	inline auto  FileSystem::EnumRecursive (const Path &p) __NE___
	{
		ASSERT( not p.empty() );	// use "." instead
		std::error_code	ec;
		return RecursiveDirectoryIter{ std::filesystem::recursive_directory_iterator{ p, std::filesystem::directory_options::skip_permission_denied, OUT ec }};
	}

	inline bool  FileSystem::CopyFile (const Path &from, const Path &to) __NE___
	{
		std::error_code	ec;
		const auto		opt = std::filesystem::copy_options::overwrite_existing;
		bool			res = std::filesystem::copy_file( from, to, opt, OUT ec );
		FS_MSG( not ec, "CopyFile('" + from.string() + "', '" + to.string() + "'): " + ec.message() );
		return res;
	}

	inline bool  FileSystem::CopyDirectory (const Path &from, const Path &to) __NE___
	{
		std::error_code	ec;
		const auto		opt = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
		std::filesystem::copy( from, to, opt, OUT ec );
		FS_MSG( not ec, "CopyDirectory('" + from.string() + "', '" + to.string() + "'): " + ec.message() );
		return not ec;
	}

	inline bool  FileSystem::Rename (const Path &oldName, const Path &newName) __NE___
	{
		std::error_code	ec;
		std::filesystem::rename( oldName, newName, OUT ec );
		FS_MSG( not ec, "Rename('" + oldName.string() + "', '" + newName.string() + "'): " + ec.message() );
		return not ec;
	}

	inline Bytes  FileSystem::FileSize (const Path &p) __NE___
	{
		std::error_code	ec;
		auto	size = std::filesystem::file_size( p );
		FS_MSG( not ec, "FileSize('" + p.string() + "'): " + ec.message() );
		return not ec ? Bytes{size} : 0_b;
	}

	inline bool  FileSystem::GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available) __NE___
	{
		std::error_code	ec;
		auto	space = std::filesystem::space( path, OUT ec );
		FS_MSG( not ec, "GetSpace('" + path.string() + "'): " + ec.message() );

		if_likely( not ec )
		{
			total		= Bytes{ space.capacity };
			available	= Bytes{ space.available };
			return true;
		}

		total		= 0_b;
		available	= 0_b;
		return false;
	}

	template <typename T>
	bool  FileSystem::ValidateFileName (INOUT BasicString<T> &name) __NE___
	{
		bool	res = false;
		for (usize i = 0; i < name.size(); ++i)
		{
			T&	c = name[i];

		  #if defined(AE_PLATFORM_WINDOWS)
			if ( (c == T('/')) or (c == T('\\')) or (c == T('?')) or (c == T('%')) or (c == T('*')) or
				 (c == T('|')) or (c == T(':'))  or (c == T('"')) or (c == T('<')) or (c == T('>')) )
			{
				res = true;
				c = T('_');
			}
		  #elif defined(AE_PLATFORM_UNIX_BASED)
			if ( (c == T('/')) )
			{
				res = true;
				c = T('_');
			}
		  #else
			#error Not implemented
		  #endif
		}
		return res;
	}


	Nd__In bool  IsAnsiPath (const Path &path) __NE___
	{
		for (auto& c : path.native())
		{
			if ( ulong(c) > 127 )
				return false;
		}
		return true;
	}

	template <typename FileType, typename ModeType>
	RC<FileType>  FileSystem::OpenUnusedFile (INOUT Path &path, ModeType mode, uint maxAttempts) __NE___
	{
		const String	ext		= path.extension().string();
		const Path		name	= path.stem();

		for (uint i = 0; i < maxAttempts; ++i)
		{
			auto	file = MakeRC<FileType>( path, mode );

			if ( file and file->IsOpen() )
				return file;

			path.replace_filename( ((Path{name} += '-') += std::to_string(i)) += ext );
		}
		return null;
	}

	inline bool  FileSystem::Equal (const Path &lhs, const Path &rhs) __NE___
	{
		std::error_code	ec;
		return std::filesystem::equivalent( lhs, rhs, OUT ec );
	}


	#undef FS_MSG

} // AE::Base
