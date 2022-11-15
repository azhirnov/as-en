// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/StdInclude.h"
#ifdef AE_HAS_SOURCE_LOCATION
# include <source_location>
#endif
#include "base/Common.h"

namespace AE::Base
{

	//
	// Source Code Location
	//

	struct SourceLoc
	{
		StringView	file;
		uint		line	= 0;

		// TODO: file_name, column ?

		constexpr SourceLoc () {}
		explicit constexpr SourceLoc (StringView file, uint line = 0) : file{file}, line{line} {}
		
		#ifdef AE_HAS_SOURCE_LOCATION
		explicit constexpr SourceLoc (const std::source_location &loc) : file{loc.file_name()}, line{loc.line()} {}
		#endif
	};

	
	#ifdef AE_HAS_SOURCE_LOCATION
	# define SourceLoc_Current()	AE::Base::SourceLoc{ std::source_location::current() }
	#else
	# define SourceLoc_Current()	AE::Base::SourceLoc{ __FILE__, __LINE__ }
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
