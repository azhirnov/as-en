// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{

	//
	// Source Code Location
	//
	struct SourceLoc
	{
	// variables
	private:
		StringView		_file;
		StringView		_fnName;
		StringView		_fnSign;
		uint			_line		= 0;
		uint			_column		= 0;

	// methods
	public:
		__Cx__ SourceLoc ()											__NE___	{}
		__Cx__ SourceLoc (const std::source_location &loc,
						  StringView functionName = {},
						  StringView functionSignature = {})       	__NE___;

		__Cx__ explicit SourceLoc (StringView file,
								   uint line = 0,
								   uint column = 0)					__NE___	:
			 _file{file}, _line{line}, _column{column}
		{}

		__Cx__ SourceLoc (StringView file, StringView fnName,
						  unsigned int line, unsigned int column,
						  StringView fnSign = {})	    			__NE___	:
			_file{file}, _fnName{fnName}, _fnSign{fnSign},
			_line{line}, _column{column}
		{}

		NdCx__ StringView		FunctionName ()				    	C_NE___	{ return _fnName; }
		NdCx__ StringView		FunctionSignature ()				C_NE___	{ return _fnSign; }
		NdCx__ StringView		FileName ()				    		C_NE___	{ return _file; }
		NdCx__ StringView		FileStem ()				    		C_NE___	{ return _ExtractStem( _file ); }
		NdCx__ uint				Column ()							C_NE___	{ return _column; }
		NdCx__ uint				Line ()								C_NE___	{ return _line; }

		NdCx__ static SourceLoc  current (const std::source_location &loc = std::source_location::current()) __NE___ { return SourceLoc{loc}; }

	//private:  // for tests
		NdCx__ static StringView  _ExtractFnName (StringView)		__NE___;
		NdCx__ static StringView  _ExtractStem (StringView)			__NE___;
	};


/*
=================================================
	constructor
----
	TODO: obfuscate for release version
=================================================
*/
	__CxIn SourceLoc::SourceLoc (const std::source_location &loc, StringView functionName, StringView functionSignature) __NE___ :
		_file{loc.file_name()}, _fnName{functionName}, _fnSign{functionSignature}, _line{loc.line()}, _column{loc.column()}
	{
		if ( functionName.empty() )
		{
		#ifdef AE_COMPILER_MSVC
		# if not _USE_DETAILED_FUNCTION_NAME_IN_SOURCE_LOCATION
			_fnName = loc.function_name();
		# else
			_fnName = _ExtractFnName( loc.function_name() );
		# endif
		#else
			_fnName = _ExtractFnName( loc.function_name() );
		#endif
		}
		if ( functionSignature.empty() )
		{
			_fnSign = loc.function_name();
		}
	}

/*
=================================================
	_ExtractFnName
=================================================
*/
	__CxIn StringView  SourceLoc::_ExtractFnName (StringView fnSign) __NE___
	{
		usize   end = fnSign.rfind( '(' );

		if ( end >= fnSign.size() )
			return {};  // error

		--end;

		// remove template specialization
		#if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL)
		if ( fnSign[end] == '>' )
		{
			int	br_count = 1;
			--end;
			for (; br_count > 0 and end < fnSign.size();)
			{
				char	c = fnSign[end];
				--end;
				if ( c == '>' )
					++br_count;
				if ( c == '<' )
					--br_count;
			}
			if ( br_count != 0 )
				return {};  // error
		}
		#endif

		usize   begin = end;

		for (; begin != 0 and begin < fnSign.size(); --begin)
		{
			char	c = fnSign[begin];
			if ( c == ' ' or c == ':' )
			{
				++begin;
				break;
			}
		}

		if ( begin <= end and end < fnSign.size() )
		{
			// 'operator ()'
			if ( begin > 9 and fnSign[begin-1] == ' ' )
			{
				constexpr StringView	op {"operator "};

				if ( StringView{ fnSign.data() + begin-9, 9 } == op )
					return StringView{ fnSign.data() + begin-9, fnSign.data() + end+1 };
			}

			return StringView{ fnSign.data() + begin, fnSign.data() + end+1 };
		}
		return {};  // error
	}

/*
=================================================
	_ExtractStem
=================================================
*/
	__CxIn StringView  SourceLoc::_ExtractStem (StringView fname) __NE___
	{
		usize	end		= std::min( fname.rfind( '.' ), fname.size() );
		usize	begin	= fname.find_last_of( "/\\", end );

		if ( begin < end )
			return StringView{ fname.data() + begin+1, fname.data() + end };

		return {};  // error
	}

} // AE::Base
