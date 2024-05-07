// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/WindowsFile.h"
#include "base/DataSource/UnixFile.h"

namespace AE::Base
{

#ifdef AE_PLATFORM_WINDOWS
	using FileRStream		= WinFileRStream;
	using FileWStream		= WinFileWStream;
	using FileRDataSource	= WinFileRDataSource;
	using FileWDataSource	= WinFileWDataSource;
#endif

#ifdef AE_PLATFORM_UNIX_BASED
	using FileRStream		= UnixFileRStream;
	using FileWStream		= UnixFileWStream;
	using FileRDataSource	= UnixFileRDataSource;
	using FileWDataSource	= UnixFileWDataSource;
#endif

} // AE::Base
