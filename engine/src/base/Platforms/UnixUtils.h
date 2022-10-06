// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

# include "base/Math/Bytes.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

	//
	// Unix Utils
	//

	struct UnixUtils
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
		};


	// functions
	
		// Errors //
		ND_ static int   GetErrorCode ();
		ND_ static int   GetNetworkErrorCode ();

			static bool  CheckError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckError (int err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			
			static bool  CheckNetworkError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckNetworkError (int err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
		
		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo ();

	private:
		ND_ static bool  _CheckError (int err, StringView msg, StringView file, int line, ELogLevel level, ELogScope scope);
	};

}	// AE::Base

#endif // AE_PLATFORM_UNIX_BASED
