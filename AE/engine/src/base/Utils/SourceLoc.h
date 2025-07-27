// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/ToString.h"

namespace AE::Base
{

	//
	// Source Code Location
	//
	struct SourceLocCopy
	{
	// variables
	private:
		String		_file;
		String		_fn;
		uint		_line		= 0;
		uint		_column		= 0;

	// methods
	public:
		SourceLocCopy ()									__NE___	{}
		SourceLocCopy (String file, uint line)				__NE___	: _file{RVRef(file)}, _line{line} {}
		explicit SourceLocCopy (const SourceLoc &loc)		__Th___	: _file{ToString(loc.file_name())}, _fn{ToString(loc.function_name())}, _line{loc.line()}, _column{loc.column()} {}

		SourceLocCopy (const SourceLocCopy &)				__Th___ = default;
		SourceLocCopy (SourceLocCopy &&)					__NE___	= default;

		SourceLocCopy&  operator = (const SourceLocCopy &)	__Th___	= default;
		SourceLocCopy&  operator = (SourceLocCopy &&)		__NE___	= default;

		operator SourceLoc ()								C_NE___	{ return SourceLoc{ _file.c_str(), _fn.c_str(), _line, _column }; }

		ND_ const char*		function_name ()				C_NE___	{ return _fn.c_str(); }
		ND_ const char*		file_name ()					C_NE___	{ return _file.c_str(); }
		ND_ uint			column ()						C_NE___	{ return _column; }
		ND_ uint			line ()							C_NE___	{ return _line; }

		ND_ static SourceLocCopy  current (const SourceLoc &loc = SourceLoc::current()) __Th___	{ return SourceLocCopy{loc}; }
	};


	template <> struct TTriviallyDestructible< SourceLoc > : CT_True {};

} // AE::Base
