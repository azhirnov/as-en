// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Math/Random.h"
#include "Perf_Common.h"
#include "Perf_AllocCounter.h"

namespace
{

	inline bool  Bool_IsOperator (char c) __NE___
	{
		return	(c == '-') or (c == '+') or (c == '*') or (c == '/') or
				(c == '|') or (c == '&') or (c == '^') or (c == '<') or
				(c == '>') or (c == '?') or (c == ':') or (c == ';') or
				(c == ',') or (c == '.') or (c == '!') or (c == '~') or
				(c == '[') or (c == ']') or (c == '(') or (c == ')') or
				(c == '=') or (c == '%') or (c == '"') or (c == '#') or
				(c == '{') or (c == '}') or (c == '\'') or (c == '\\');
	}

	inline bool  Bool_IsBinaryOperator (char p, char c) __NE___
	{
	#	define PAIR_CMP( _pair_ )	(( (p == _pair_[0]) and (c == _pair_[1]) ))

		return	PAIR_CMP( "//" ) or PAIR_CMP( "/*" ) or PAIR_CMP( "*/" ) or
				PAIR_CMP( "&&" ) or PAIR_CMP( "||" ) or PAIR_CMP( "^^" ) or
				PAIR_CMP( "::" ) or PAIR_CMP( "==" ) or PAIR_CMP( "!=" ) or
				PAIR_CMP( ">=" ) or PAIR_CMP( "<=" ) or PAIR_CMP( "##" ) or
				PAIR_CMP( ">>" ) or PAIR_CMP( "<<" ) or PAIR_CMP( "|=" ) or
				PAIR_CMP( "&=" ) or PAIR_CMP( "^=" ) or PAIR_CMP( "+=" ) or
				PAIR_CMP( "-=" ) or PAIR_CMP( "*=" ) or PAIR_CMP( "/=" ) or
				PAIR_CMP( "%=" );

	#	undef PAIR_CMP
	}

	inline bool  Bool_IsTernaryOperator (char pp, char p, char c) __NE___
	{
	#	define TRIPLE_CMP( _triple_ )	(( (pp == _triple_[0]) and (p == _triple_[1]) and (c == _triple_[2]) ))

		return	TRIPLE_CMP( ">>=" ) or TRIPLE_CMP( "<<=" );

	#	undef TRIPLE_CMP
	}



	inline bool  Bit_IsOperator (char c) __NE___
	{
		return	(c == '-') | (c == '+') | (c == '*') | (c == '/') |
				(c == '|') | (c == '&') | (c == '^') | (c == '<') |
				(c == '>') | (c == '?') | (c == ':') | (c == ';') |
				(c == ',') | (c == '.') | (c == '!') | (c == '~') |
				(c == '[') | (c == ']') | (c == '(') | (c == ')') |
				(c == '=') | (c == '%') | (c == '"') | (c == '#') |
				(c == '{') | (c == '}') | (c == '\'') | (c == '\\');
	}

	inline bool  Bit_IsBinaryOperator (char p, char c) __NE___
	{
	#	define PAIR_CMP( _pair_ )	(( (p == _pair_[0]) & (c == _pair_[1]) ))

		return	PAIR_CMP( "//" ) | PAIR_CMP( "/*" ) | PAIR_CMP( "*/" ) |
				PAIR_CMP( "&&" ) | PAIR_CMP( "||" ) | PAIR_CMP( "^^" ) |
				PAIR_CMP( "::" ) | PAIR_CMP( "==" ) | PAIR_CMP( "!=" ) |
				PAIR_CMP( ">=" ) | PAIR_CMP( "<=" ) | PAIR_CMP( "##" ) |
				PAIR_CMP( ">>" ) | PAIR_CMP( "<<" ) | PAIR_CMP( "|=" ) |
				PAIR_CMP( "&=" ) | PAIR_CMP( "^=" ) | PAIR_CMP( "+=" ) |
				PAIR_CMP( "-=" ) | PAIR_CMP( "*=" ) | PAIR_CMP( "/=" ) |
				PAIR_CMP( "%=" );

	#	undef PAIR_CMP
	}

	inline bool  Bit_IsTernaryOperator (char pp, char p, char c) __NE___
	{
	#	define TRIPLE_CMP( _triple_ )	(( (pp == _triple_[0]) & (p == _triple_[1]) & (c == _triple_[2]) ))

		return	TRIPLE_CMP( ">>=" ) | TRIPLE_CMP( "<<=" );

	#	undef TRIPLE_CMP
	}


	static void LogicOp_Test ()
	{
		IntervalProfiler		profiler{ "LogicOp vs BitOp" };
		String					large_str;
		const uint				N = 1000;
		StaticArray< ulong, 8 >	sum = {};

		large_str.resize( 1000'000 );

		profiler.BeginTest( "Bool_IsOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (char c : large_str)
					sum[0] += ulong{Bool_IsOperator( c )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		profiler.BeginTest( "Bool_IsBinaryOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (usize j = 1; j < large_str.size(); ++j)
					sum[1] += ulong{Bool_IsBinaryOperator( large_str[j-1], large_str[j] )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		profiler.BeginTest( "Bool_IsTernaryOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (usize j = 2; j < large_str.size(); ++j)
					sum[2] += ulong{Bool_IsTernaryOperator( large_str[j-2], large_str[j-1], large_str[j] )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();


		profiler.BeginTest( "Bit_IsOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (char c : large_str)
					sum[3] += ulong{Bit_IsOperator( c )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		profiler.BeginTest( "Bit_IsBinaryOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (usize j = 1; j < large_str.size(); ++j)
					sum[4] += ulong{Bit_IsBinaryOperator( large_str[j-1], large_str[j] )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		profiler.BeginTest( "Bit_IsTernaryOperator" );
		profiler.BeginIteration();
		{
			for (uint i = 0; i < N; ++i)
			{
				for_likely (usize j = 2; j < large_str.size(); ++j)
					sum[5] += ulong{Bit_IsTernaryOperator( large_str[j-2], large_str[j-1], large_str[j] )};
			}
		}
		profiler.EndIteration();
		profiler.EndTest();

		CHECK( sum[0] == sum[3] );
		CHECK( sum[1] == sum[4] );
		CHECK( sum[2] == sum[5] );
	}
}


extern void PerfTest_LogicOp ()
{
	LogicOp_Test();

	TEST_PASSED();
}
