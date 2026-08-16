// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#if defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS)
# include "base/FileSystem/AppleFileWatch.h"

namespace AE::Base
{

	bool  AppleFileWatch::Start (const Path				&path,
								 EFileSystemWatchBits	events,
								 Bool					recursive,
								 Bytes					bufferSize) __NE___
	{
		return false;
	}

	bool  AppleFileWatch::Start (const Path				&path,
								 EFileSystemWatchBits	events,
								 Bool					recursive,
								 DirFilter_t			filter,
								 Bytes					bufferSize) __NE___
	{
		return false;
	}

	void  AppleFileWatch::Stop () __NE___
	{
	}

	bool  AppleFileWatch::GetEvents (OUT EventArray_t &, milliseconds timeout) __NE___
	{
		return false;
	}

} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
