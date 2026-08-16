// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"
#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
# include "base/FileSystem/FileWatchEvent.h"

namespace AE::Base
{

	//
	// File Watch
	//

	class LinuxFileWatch
	{
	// types
	public:
		using EventArray_t	= Array< FileWatchEvent >;
		using DirFilter_t	= Function< bool (const Path &dirName) >;

	private:
		enum class WatchDirId : int		// result of inotify_add_watch
		{
			Unknown = -1
		};

		struct WatchDir
		{
			WatchDirId		parent;
			Path			path;		// relative
		};
		using WatchDirMap_t	= FlatHashMap< WatchDirId, WatchDir >;


	// variables
	private:
		int					_inotifyFd		= -1;
		uint				_filter			= 0;
		bool				_recursive		= false;

		Path				_baseDir;

		WatchDirMap_t		_watchMap;
		Array<uint>			_buffer;


	// methods
	public:
		LinuxFileWatch ()												__NE___ {}
		~LinuxFileWatch ()												__NE___	{ Stop(); }

		ND_ bool  Start (const Path				&path,
						 EFileSystemWatchBits	events,
						 Bool					recursive  = True{},
						 Bytes					bufferSize = 64_KiB)	__NE___;

		ND_ bool  Start (const Path				&path,
						 EFileSystemWatchBits	events,
						 Bool					recursive,
						 DirFilter_t			filter,			// optional
						 Bytes					bufferSize = 64_KiB)	__NE___;

			void  Stop ()												__NE___;

		ND_ bool  GetEvents (OUT EventArray_t &,
							 milliseconds	  timeout = Default)		__NE___;

	private:
		ND_ bool  _Request ()											__NE___;
	};


} // AE::Base

#endif // AE_PLATFORM_LINUX or AE_PLATFORM_ANDROID
