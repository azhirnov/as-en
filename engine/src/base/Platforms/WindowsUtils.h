// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/NtStringView.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

	//
	// WinAPI Utils
	//

	struct WindowsUtils final
	{
	// types
		struct MemoryPageInfo
		{
			Bytes	pageSize;
			Bytes	allocationGranularity;
		};


	// functions

		// Errors //
		ND_ static uint  GetErrorCode ();
		ND_ static uint  GetNetworkErrorCode ();

			static bool  CheckError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckError (uint err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			
			static bool  CheckNetworkError (StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);
			static bool  CheckNetworkError (uint err, StringView msg, StringView file, int line, ELogLevel level = ELogLevel::Error);

		// Memory //
		ND_ static MemoryPageInfo  GetMemoryPageInfo ();

	private:
		ND_ static bool  _CheckError (uint err, StringView msg, StringView file, int line, ELogLevel level, ELogScope scope);
	};

}	// AE::Base

#endif // AE_PLATFORM_WINDOWS
