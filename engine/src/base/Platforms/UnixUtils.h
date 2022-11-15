// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/SourceLoc.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

	//
	// Unix Utils
	//

	struct UnixUtils : Noninstancable
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
		};


	// functions
	
		// Errors //
		ND_ static int   GetErrorCode ()			__NE___;
		ND_ static int   GetNetworkErrorCode ()		__NE___;

			static bool  CheckError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)					__NE___;
			static bool  CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;
			
			static bool  CheckNetworkError (StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)			__NE___;
			static bool  CheckNetworkError (int err, StringView msg, const SourceLoc &loc, ELogLevel level = ELogLevel::Error)	__NE___;
		
		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo () __NE___;

		// Thread //
			// interval > 4000ns
			static bool		NanoSleep (nanoseconds relativeTime)	__NE___;

			static bool		WaitIO (milliseconds relativeTime)		__NE___;
			
			static bool		ThreadYield ()							__NE___;

	private:
		ND_ static bool  _CheckError (int err, StringView msg, const SourceLoc &loc, ELogLevel level, ELogScope scope) __NE___;
	};

} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
