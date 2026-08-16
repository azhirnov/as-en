// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
		String		_fnName;
		String 		_fnSign;
		uint		_line		= 0;
		uint		_column		= 0;

	// methods
	public:
		SourceLocCopy ()									__NE___	{}
		SourceLocCopy (String file, uint line)				__NE___	: _file{RVRef(file)}, _line{line} {}
		explicit SourceLocCopy (const SourceLoc &loc)		__Th___	: _file{loc.FileName()}, _fnName{loc.FunctionName()}, _fnSign{loc.FunctionSignature()}, _line{loc.Line()}, _column{loc.Column()} {}

		SourceLocCopy (const SourceLocCopy &)				__Th___ = default;
		SourceLocCopy (SourceLocCopy &&)					__NE___	= default;

		SourceLocCopy&  operator = (const SourceLocCopy &)	__Th___	= default;
		SourceLocCopy&  operator = (SourceLocCopy &&)		__NE___	= default;

		operator SourceLoc ()								C_NE___	{ return SourceLoc{ _file, _fnName, _line, _column, _fnSign }; }

		ND_ StringView		FunctionName ()				    C_NE___	{ return _fnName; }
		ND_ StringView		FunctionSignature ()			C_NE___	{ return _fnSign; }
		ND_ StringView		FileName ()				    	C_NE___	{ return _file; }
		ND_ uint			Column ()						C_NE___	{ return _column; }
		ND_ uint			Line ()							C_NE___	{ return _line; }

		ND_ static SourceLocCopy  current (const SourceLoc &loc = SourceLoc::current()) __Th___	{ return SourceLocCopy{loc}; }
	};


	template <> struct TTriviallyDestructible< SourceLoc > : CT_True {};

} // AE::Base
