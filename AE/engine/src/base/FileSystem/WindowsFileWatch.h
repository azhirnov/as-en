// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "base/FileSystem/FileWatchEvent.h"
# include "base/Memory/UntypedStorage.h"

namespace AE::Base
{

	//
	// File Watch
	//

	class WindowsFileWatch
	{
	// types
	private:
		using Overlapped_t	= UntypedStorage< sizeof(ulong)*4, alignof(ulong) >;	// OVERLAPPED
	public:
		using EventArray_t	= Array< FileWatchEvent >;


	// variables
	private:
		void *				_dir			= null;		// HANDLE
		void *				_event			= null;		// HANDLE
		Overlapped_t		_overlapped;
		Array<uint>			_buffer;
		uint				_filter			= 0;
		bool				_recursive		= false;


	// methods
	public:
		WindowsFileWatch ()												__NE___ {}
		~WindowsFileWatch ()											__NE___	{ Stop(); }

		ND_ bool  Start (const Path				&path,
						 EFileSystemWatchBits	events,
						 Bool					recursive  = True{},
						 Bytes					bufferSize = 64_KiB)	__NE___;
			void  Stop ()												__NE___;

		ND_ bool  GetEvents (OUT EventArray_t &,
							 milliseconds	  timeout = Default)		__NE___;

	private:
		ND_ bool  _Request ()											__NE___;
	};


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
