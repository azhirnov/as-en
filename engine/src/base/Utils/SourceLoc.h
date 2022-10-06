// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
// TODO: use std::source_location

#pragma once

#include "base/Common.h"

#ifdef AE_HAS_SOURCE_LOCATION
# include <source_location>
#endif

namespace AE::Base
{

	//
	// Source Code Location
	//

	struct SourceLoc
	{
		StringView	file;
		uint		line	= 0;

		constexpr SourceLoc () {}
		explicit constexpr SourceLoc (StringView file, uint line = 0) : file{file}, line{line} {}
		
		#ifdef AE_HAS_SOURCE_LOCATION
		ND_ static consteval SourceLoc  Current ()
		{
			auto loc = std::source_location::current();
			return SourceLoc{ loc.file_name(), loc.line() };
		}
		#endif
	};

	
	#ifdef AE_HAS_SOURCE_LOCATION
	# define SourceLoc_Current()	SourceLoc::Current()
	#else
	# define SourceLoc_Current()	SourceLoc{ __FILE__, __LINE__ }
	#endif
	

	//
	// Source Code Location
	//

	struct SourceLoc2
	{
		String		file;
		uint		line	= 0;

		SourceLoc2 () {}
		SourceLoc2 (String file, uint line) : file{RVRef(file)}, line{line} {}
		explicit SourceLoc2 (const SourceLoc &loc) : file{loc.file}, line{loc.line} {}
	};

} // AE::Base
