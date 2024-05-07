// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/FileSystem/FileSystem.h"

namespace AE::Base
{

	//
	// Protected File System
	//

	class ProtectedFileSystem final
	{
	// variables
	private:
		const Path		_baseFolder;


	// filesystem
	public:
		ProtectedFileSystem (const Path &baseFolder)			__NE___;
		~ProtectedFileSystem ()									__NE___;

		ND_ bool  LockFile (const Path &p)						__NE___;
		ND_ bool  UnlockFile (const Path &p)					__NE___;

		ND_ bool  LockFolder (const Path &p)					__NE___;
		ND_ bool  UnlockFolder (const Path &p)					__NE___;

		// Remove file or empty directory.
		// Returns 'true' if the file was deleted.
			bool  DeleteFile (const Path &p)					__NE___;

		// Remove directory and all subdirectories.
			bool  DeleteDirectory (const Path &p)				__NE___;

		// Create directory, parent directory must be exists.
			bool  CreateDirectory (const Path &p)				__NE___;

		// Returns 'true' if path refers to a file.
		ND_ bool  IsFile (const Path &p)						C_NE___;

		// Returns 'true' if path refers to a directory.
		ND_ bool  IsDirectory (const Path &p)					C_NE___;

		// Set working directory.
			bool  SetCurrentPath (const Path &p)				__NE___;

		// Returns current path
		ND_ Path  CurrentPath ()								__Th___;
	};


} // AE::Base
