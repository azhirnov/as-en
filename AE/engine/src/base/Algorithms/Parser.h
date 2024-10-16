// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/Utils/Helpers.h"

namespace AE::Base
{

	//
	// String Parser
	//

	class Parser final : Noninstanceable
	{
	public:
			static void  ToEndOfLine	(StringView str, INOUT usize &pos)								__NE___;
			static void  ToBeginOfLine	(StringView str, INOUT usize &pos)								__NE___;
			static void  ToNextLine		(StringView str, INOUT usize &pos)								__NE___;
			static void  ToPrevLine		(StringView str, INOUT usize &pos)								__NE___;

		ND_ static bool  IsBeginOfLine	(StringView str, usize pos)										__NE___;
		ND_ static bool  IsEndOfLine	(StringView str, usize pos)										__NE___;

		ND_ static usize CalculateNumberOfLines (StringView str)										__NE___;

			static bool  MoveToLine (StringView str, INOUT usize &pos, usize lineNumber)				__NE___;

			static void  ReadCurrLine (StringView str, INOUT usize &pos, OUT StringView &result)		__NE___;
			static void  ReadLineToEnd (StringView str, INOUT usize &pos, OUT StringView &result)		__NE___;

			static bool  ReadString (StringView str, INOUT usize &pos, OUT StringView &result)			__NE___;

			static void  DivideLines (StringView str, OUT Array<StringView> &lines)						__Th___;

			static bool  DivideString_CPP (StringView str, OUT Array<StringView> &tokens)				__Th___;
			static bool  DivideString_Words (StringView str, OUT Array<StringView> &tokens)				__Th___;

			static void  Tokenize (StringView str, const char divisor, OUT Array<StringView> &tokens)	__Th___;

			template <typename IfNotEqualFn, typename IfSizeIsNotEqualFn>
			static bool  CompareLineByLine (StringView left, StringView right,
											IfNotEqualFn&& ifNotEqual,
											IfSizeIsNotEqualFn&& ifSizeNotEqual)						__NE___;

			static void  Preprocessor_CPP (StringView str, ArrayView<StringView> defines,
										   OUT Array<StringView> &result)								__Th___;

			static void   ValidateVarName_CPP (StringView name, OUT String &result)						__NE___;
		ND_ static String ValidateVarName_CPP (StringView name)											__NE___;

		ND_ static String TabsToSpaces (StringView src, uint tabSize = 4)								__Th___;
			static void   TabsToSpaces (OUT String &dst, StringView src, uint tabSize = 4)				__Th___;

		ND_ static bool  IsWhiteSpacesOnly (StringView str)												__NE___;

		    static void  Align (INOUT String &str, usize pos, usize lineSize, char c = ' ')				__Th___;

		static constexpr struct _CPPHelper
		{
			ND_ static bool  IsWordBegin (char c)	__NE___;
			ND_ static bool  IsWord (char c)		__NE___;

			ND_ static bool  IsNumberBegin (char c)	__NE___;
			ND_ static bool  IsNumber (char c)		__NE___;

		}	CPP = {};
	};


/*
=================================================
	CompareLineByLine
=================================================
*/
	template <typename IfNotEqualFn, typename IfSizeIsNotEqualFn>
	bool  Parser::CompareLineByLine (StringView				left,
									 StringView				right,
									 IfNotEqualFn &&		ifNotEqual,
									 IfSizeIsNotEqualFn &&	ifSizeNotEqual) __NE___
	{
		usize		l_pos	= 0;
		usize		r_pos	= 0;
		uint		line_number [2] = {};
		StringView	line_str [2];

		const auto	LeftValid	= [&l_pos, &left ] ()	{{ return l_pos < left.length(); }};
		const auto	RightValid	= [&r_pos, &right] ()	{{ return r_pos < right.length(); }};

		const auto	IsEmptyLine	= [] (StringView str)
		{{
			for (auto& c : str) {
				if ( c != '\n' and c != '\r' and c != ' ' and c != '\t' )
					return false;
			}
			return true;
		}};

		// compare line by line
		for (; LeftValid() and RightValid(); )
		{
			// read left line
			do {
				Parser::ReadLineToEnd( left, INOUT l_pos, OUT line_str[0] );
				++line_number[0];
			}
			while ( IsEmptyLine( line_str[0] ) and LeftValid() );

			// read right line
			do {
				Parser::ReadLineToEnd( right, INOUT r_pos, OUT line_str[1] );
				++line_number[1];
			}
			while ( IsEmptyLine( line_str[1] ) and RightValid() );

			if ( line_str[0] != line_str[1] )
			{
				CheckNothrow( IsNoExcept( ifNotEqual( line_number[0], line_str[0], line_number[1], line_str[1] )));

				if ( ifNotEqual( line_number[0], line_str[0],
								 line_number[1], line_str[1] ))
					return false;
			}
		}

		if ( LeftValid() != RightValid() )
		{
			CheckNothrow( IsNoExcept( ifSizeNotEqual() ));

			ifSizeNotEqual();
			return false;
		}
		return true;
	}

/*
=================================================
	_CPPHelper
=================================================
*/
	forceinline bool  Parser::_CPPHelper::IsWordBegin (char c) __NE___
	{
		return	((c >= 'A') and (c <= 'Z')) or
				((c >= 'a') and (c <= 'z')) or
				(c == '_');
	}

	forceinline bool  Parser::_CPPHelper::IsNumberBegin (char c) __NE___
	{
		return	(c >= '0') and (c <= '9');
	}

	forceinline bool  Parser::_CPPHelper::IsWord (char c) __NE___
	{
		return	IsWordBegin( c ) or IsNumberBegin( c );
	}

	forceinline bool  Parser::_CPPHelper::IsNumber (char c) __NE___
	{
		return	((c >= '0') and (c <= '9')) or
				((c >= 'A') and (c <= 'F')) or
				((c >= 'a') and (c <= 'f')) or
				(c == '.')  or  (c == 'x')  or
				(c == 'X');
	}


} // AE::Base
