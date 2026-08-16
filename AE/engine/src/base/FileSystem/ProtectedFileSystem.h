// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe:  yes
*/

#pragma once

#include "base/FileSystem/FileSystem.h"

namespace AE::Base
{

	//
	// Protected File System
	//

	class ProtectedFileSystem final
	{
	// types
	public:
		using Time_t					= FileSystem::Time_t;
		using ECopyOpt					= FileSystem::ECopyOpt;
		using DirectoryIter				= FileSystem::DirectoryIter;
		using RecursiveDirectoryIter	= FileSystem::RecursiveDirectoryIter;


	// variables
	private:
		const Path		_baseFolder;


	// filesystem
	public:
		explicit ProtectedFileSystem (const Path &baseFolder)		__NE___;

		// Remove file.
		// Returns 'true' if the file was deleted.
		bool  DeleteFile (const Path &p)							C_NE___;

		// Remove directory and all subdirectories.
		bool  DeleteDirectory (const Path &p)						C_NE___;
		bool  DeleteEmptyDirectory (const Path &p)					C_NE___;

		// Create directory, parent directory must be exists.
		// Returns 'false' on error.
		bool  CreateDirectory (const Path &p)						C_NE___;

		// Create all directories that is not exists.
		// Returns 'false' on error.
		bool  CreateDirectories (const Path &p)						C_NE___;

		// Create an empty file.
		bool  CreateEmptyFile (const Path &p)						C_NE___;

		// Set working directory.
		bool  SetCurrentPath (const Path &p)						C_NE___;

		// Returns current path
		ND_ Path  CurrentPath ()									C_NE___;

		// Returns 'true' if path refers to a file.
		ND_ bool  IsFile (const Path &p)							C_NE___;

		// Returns 'true' if path refers to a directory.
		ND_ bool  IsDirectory (const Path &p)						C_NE___;

		// Returns 'true' if path refers to a file or directory.
		ND_ bool  IsFileOrDirectory (const Path &p)					C_NE___;

		// Returns 'true' if path refers to an empty directory.
		ND_ bool  IsEmptyDirectory (const Path &p)					C_NE___;

		// Returns 'true' if 'lhs' and 'rhs' refer to the same file or directory.
		ND_ bool  Equal (const Path &lhs, const Path &rhs)			C_NE___;

		// Returns time of the last modification of file.
		ND_ Time_t  LastWriteTime (const Path &p)					C_NE___;

		// Set time of the last modification of file.
		bool  SetLastWriteTime (const Path &p, Time_t t)			C_NE___;

		// Returns absolute path.
		ND_ Path  ToAbsolute (const Path &p)						C_NE___;

		// Returns relative path.
		ND_ Path  ToRelative (const Path &p, const Path &base)		C_NE___;

		// Returns path without /../
		ND_ Path  Normalize (const Path &p)							C_NE___;

		// Enumerate all files in directory.
		ND_ DirectoryIter  Enum (const Path &p)						C_NE___;

		// Enumerate all files in directory and its subdirectories.
		ND_ RecursiveDirectoryIter  EnumRecursive (const Path &p)	C_NE___;

		// Copy file. Returns 'false' if failed.
		// Will override existing file.
		bool  CopyFile (const Path &from, const Path &to)			C_NE___;

		// Recursive copy directory. Returns 'false' if failed.
		// Will override existing files.
		bool  CopyDirectory (const Path &from, const Path &to)		C_NE___;

		// Recursive merge directories. Returns 'false' if failed.
		bool  MergeDirectory (const Path &from, const Path &to, ECopyOpt opt) C_NE___;

		// Move or rename filesystem object (file/directory).
		bool  Rename (const Path &oldName, const Path &newName)		C_NE___;

		// Returns file size or 0 on error.
		ND_ Bytes  FileSize (const Path &p)							C_NE___;

		// Writes file system capacity and available space.
		bool  GetSpace (const Path &path, OUT Bytes &total, OUT Bytes &available) C_NE___;

	private:
		ND_ bool  _IsValid (const Path &absPath)					C_NE___;
	};


} // AE::Base
