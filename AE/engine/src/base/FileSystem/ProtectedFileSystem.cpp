// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/FileSystem/ProtectedFileSystem.h"

namespace AE::Base
{

	ProtectedFileSystem::ProtectedFileSystem (const Path &) __NE___
	{
		UNTESTED
	}

	ProtectedFileSystem::~ProtectedFileSystem () __NE___
	{}

	bool  ProtectedFileSystem::LockFile (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::UnlockFile (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::LockFolder (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::UnlockFolder (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::DeleteFile (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::DeleteDirectory (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::CreateDirectory (const Path &) __NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::IsFile (const Path &) C_NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::IsDirectory (const Path &) C_NE___
	{
		return false;
	}

	bool  ProtectedFileSystem::SetCurrentPath (const Path &) __NE___
	{
		return false;
	}

	Path  ProtectedFileSystem::CurrentPath () __Th___
	{
		return Default;
	}

} // AE::Base
