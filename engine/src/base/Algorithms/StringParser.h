// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/Utils/Helpers.h"

namespace AE::Base
{

	//
	// String Parser
	//

	class StringParser final : Noninstancable
	{
	public:
			static void  ToEndOfLine	(StringView str, INOUT usize &pos);
			static void  ToBeginOfLine	(StringView str, INOUT usize &pos);
			static void  ToNextLine		(StringView str, INOUT usize &pos);
			static void  ToPrevLine		(StringView str, INOUT usize &pos);

		ND_ static bool  IsBeginOfLine	(StringView str, usize pos);
		ND_ static bool  IsEndOfLine	(StringView str, usize pos);

		ND_ static usize CalculateNumberOfLines (StringView str);

			static bool  MoveToLine (StringView str, INOUT usize &pos, usize lineNumber);

			static void  ReadCurrLine (StringView str, INOUT usize &pos, OUT StringView &result);
			static void  ReadLineToEnd (StringView str, INOUT usize &pos, OUT StringView &result);

			static bool  ReadTo (StringView str, StringView endSymbol, INOUT usize &pos, OUT StringView &result);

			static bool  ReadString (StringView str, INOUT usize &pos, OUT StringView &result);
			
			static void  DivideLines (StringView str, OUT Array<StringView> &lines);
			
			static bool  DivideString_CPP (StringView str, OUT Array<StringView> &tokens);
			static bool  DivideString_Words (StringView str, OUT Array<StringView> &tokens);

			static void  Tokenize (StringView str, const char divisor, OUT Array<StringView> &tokens);

			template <typename IfNotEqualFn, typename IfSizeIsNotEqualFn>
			static bool  CompareLineByLine (StringView left, StringView right,
											IfNotEqualFn && ifNotEqual,
											IfSizeIsNotEqualFn && ifSizeNotEqual);
	};
	

/*
=================================================
	CompareLineByLine
=================================================
*/
	template <typename IfNotEqualFn, typename IfSizeIsNotEqualFn>
	inline bool  StringParser::CompareLineByLine (StringView left, StringView right,
												  IfNotEqualFn && ifNotEqual,
												  IfSizeIsNotEqualFn && ifSizeNotEqual)
	{
		usize		l_pos	= 0;
		usize		r_pos	= 0;
		uint2		line_number;
		StringView	line_str[2];

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
				StringParser::ReadLineToEnd( left, INOUT l_pos, OUT line_str[0] );
				++line_number[0];
			}
			while ( IsEmptyLine( line_str[0] ) and LeftValid() );

			// read right line
			do {
				StringParser::ReadLineToEnd( right, INOUT r_pos, OUT line_str[1] );
				++line_number[1];
			}
			while ( IsEmptyLine( line_str[1] ) and RightValid() );

			if ( line_str[0] != line_str[1] )
			{
				ifNotEqual( line_number[0], line_str[0],
							line_number[1], line_str[1] );
				return false;
			}
		}

		if ( LeftValid() != RightValid() )
		{
			ifSizeNotEqual();
			return false;
		}
		return true;
	}

} // AE::Base
