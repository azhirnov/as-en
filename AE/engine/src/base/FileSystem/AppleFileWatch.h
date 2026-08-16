// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"
#if defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS)
# include "base/FileSystem/FileWatchEvent.h"

namespace AE::Base
{

	//
	// File Watch
	//

	class AppleFileWatch
	{
	// types
	public:
		using EventArray_t	= Array< FileWatchEvent >;
		using DirFilter_t	= Function< bool (const Path &dirName) >;


	// variables
	private:


	// methods
	public:
		AppleFileWatch ()												__NE___ {}
		~AppleFileWatch ()												__NE___	{ Stop(); }

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
	};


} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
