// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

	//
	// Unix Process
	//

	class UnixProcess final
	{
	// types
	public:
		enum class EFlags
		{
			None			= 0,
			NoWindow		= 1 << 0,
			ReadOutput		= 1 << 1,	// optional for sync execution, not compatible with 'NoWindow' flag
			Unknown			= None,
		};


	// variables
	private:
		static constexpr auto	_DefTimeout = milliseconds{60'000};


	// methods
	public:
		static bool  Execute (const String &commandLine, EFlags flags = EFlags::NoWindow, milliseconds timeout = _DefTimeout);
		static bool  Execute (const String &commandLine, INOUT String &output, std::mutex *outputGuard = null, milliseconds timeout = _DefTimeout);
	};
	
	AE_BIT_OPERATORS( UnixProcess::EFlags );


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
