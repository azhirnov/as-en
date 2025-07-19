// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

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

		constexpr SourceLoc ()											__NE___	{}
		explicit constexpr SourceLoc (StringView file, uint line = 0)	__NE___	: file{file}, line{line} {}
		explicit constexpr SourceLoc (const std::source_location &loc)	__NE___	: file{loc.file_name()}, line{loc.line()} {}

		SourceLoc (const SourceLoc &)									__NE___ = default;
		SourceLoc (SourceLoc &&)										__NE___	= default;

		SourceLoc&  operator = (const SourceLoc &)						__NE___	= default;
		SourceLoc&  operator = (SourceLoc &&)							__NE___	= default;
	};

	#define SourceLoc_Current()		AE::Base::SourceLoc{ std::source_location::current() }



	//
	// Source Code Location
	//
	struct SourceLoc2
	{
		String		file;
		uint		line	= 0;

		SourceLoc2 ()									__NE___	{}
		SourceLoc2 (String file, uint line)				__NE___	: file{RVRef(file)}, line{line} {}
		explicit SourceLoc2 (const SourceLoc &loc)		__Th___	: file{loc.file}, line{loc.line} {}

		SourceLoc2 (const SourceLoc2 &)					__Th___ = default;
		SourceLoc2 (SourceLoc2 &&)						__NE___	= default;

		SourceLoc2&  operator = (const SourceLoc2 &)	__NE___	= default;
		SourceLoc2&  operator = (SourceLoc2 &&)			__NE___	= default;
	};



	template <> struct TTriviallyDestructible< SourceLoc > : CT_True {};

} // AE::Base
