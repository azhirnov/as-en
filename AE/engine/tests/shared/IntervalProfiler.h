// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "base/Algorithms/StringUtils.h"
#include "base/Platforms/Platform.h"
#include "base/Utils/Threading.h"
#include "threading/Primitives/Synchronized.h"
#include "UnitTest_Shared.h"


struct IntervalProfiler
{
// types
public:
	enum class EFlags
	{
		Unknown					= 0,

		// sort
		SortByName				= 1 << 0,
		SortByTime				= 1 << 1,
		SortByUserData			= 1 << 2,		// using string lexical compare

		// include
		IncludeDelta			= 1 << 8,		// difference from previous to current, depends on sorting
		IncludeTime				= 1 << 9,
		IncludeDiffFromFastest	= 1 << 10,		// difference from fastest to current

		_BITOPS_				= 0
	};

private:
	using Clock_t		= std::chrono::high_resolution_clock;
	using TimePoint_t	= Clock_t::time_point;
	using Duration_t	= Clock_t::duration;
	using AddInfoFn_t	= Function< String (Duration_t) >;

	struct TestInfo
	{
		TimePoint_t			lastStartPoint	= Default;
		String				name;
		Array< Duration_t >	iterations;
		Duration_t			average			= Default;
		bool				isComplete		= false;
		AddInfoFn_t			addInfoFn;
	};

	static constexpr EFlags		c_DefaultFlags = EFlags::SortByTime | EFlags::IncludeTime | EFlags::IncludeDiffFromFastest;


// variables
private:
	Array< TestInfo >	_tests;
	String				_testName;
	EFlags				_flags		= Default;


// methods
public:
	explicit IntervalProfiler (StringView name, EFlags flags = c_DefaultFlags) :
		_testName{name}, _flags{flags} {}

	~IntervalProfiler ()	{ PrintAndReset(); }

	void  BeginTest (StringView name);
	void  BeginTest (StringView name, AddInfoFn_t fn);
	void  EndTest ();

	void  BeginIteration ();
	void  EndIteration ();

	void  PrintAndReset ();

private:
	static void  _FormatTable (ArrayView<String> lines, INOUT String &str);

	static bool  _UserDataCmp (StringView lhs, StringView rhs);
	static Pair<double, StringView>  _ParseNumber (StringView str);
};

using TsIntervalProfiler = AE::Threading::Synchronized< Mutex, IntervalProfiler >;


/*
=================================================
	PrintAndReset
=================================================
*/
inline void  IntervalProfiler::PrintAndReset ()
{
	if ( _tests.empty() )
		return;

	Duration_t				min_time	{MaxValue<slong>()};
	bool					sorted		= false;
	Array<TestInfo const*>	sorted_tests;
	sorted_tests.resize( _tests.size() );

	if ( AllBits( _flags, EFlags::SortByUserData ) and not sorted )
	{
		Array<Pair<usize, String>>	user_data;
		for (auto [t, i] : WithIndex(_tests))
		{
			String	tmp;
			if ( t.addInfoFn )
				tmp = t.addInfoFn( t.average );

			user_data.emplace_back( i, RVRef(tmp) );
		}

		std::sort( user_data.begin(), user_data.end(), [](const auto& lhs, const auto& rhs) { return _UserDataCmp( lhs.second, rhs.second ); });

		for (auto [i_str, dst_idx] : WithIndex(user_data)) {
			sorted_tests[dst_idx] = &_tests[ i_str.first ];
		}
		sorted = true;
	}

	if ( not sorted )
	{
		for (auto [t, i] : WithIndex(_tests)) {
			sorted_tests[i] = &t;
		}
	}

	if ( AllBits( _flags, EFlags::SortByTime ) and not sorted )
	{
		std::sort( sorted_tests.begin(), sorted_tests.end(), [](const auto& lhs, const auto& rhs) { return lhs->average < rhs->average; });
		min_time = (*sorted_tests.begin())->average;
		sorted	 = true;
	}
	else
	{
		for (auto& t : _tests)
			min_time = Min( min_time, t.average );
	}

	if ( AllBits( _flags, EFlags::SortByName ) and not sorted )
	{
		std::sort( sorted_tests.begin(), sorted_tests.end(), [](const auto& lhs, const auto& rhs) { return lhs->name < rhs->name; });		// TODO: lexical compare?
		sorted = true;
	}

	String	str;
	str << '\n' << _testName << ':';

	const auto	ToDouble = [] (Duration_t dt) {{ return TimeCast<secondsd>( dt ).count(); }};

	Duration_t		prev	{-1};
	Array<String>	lines;

	for (const auto* t : sorted_tests)
	{
		auto&	line = lines.emplace_back();
		line << t->name << '|';

		if ( AnyBits( _flags, EFlags::IncludeTime ))
		{
			line << ToString(t->average) << '|';
		}

		// perf diff
		if ( AnyBits( _flags, EFlags::IncludeDiffFromFastest ))
		{
			double	fract	= min_time.count() == 0 ? 0.0 : (ToDouble( t->average - min_time ) / ToDouble( min_time )) * 100.0;
					fract	= Round( Abs( fract ));

			if ( fract != 0.0 )
			{
				String	s = ToString( fract, 1 );
				s.pop_back();
				s.pop_back();
				line << '+' << s << '%';
			}else
				line << '-';

			line << '|';
		}

		// diff from previous
		if ( AnyBits( _flags, EFlags::IncludeDelta ))
		{
			double	fract	= prev.count() < 0 ? 0.0 :  (ToDouble( t->average - prev ) / ToDouble( prev )) * 100.0;
					fract	= Round( Abs( fract ));
					prev	= t->average;

			if ( fract != 0.0 )
			{
				String	s = ToString( fract, 1 );
				s.pop_back();
				s.pop_back();
				line << (Sign(fract) > 0 ? '+' : '-') << s << '%';
			}else
				line << '-';

			line << '|';
		}

		if ( t->addInfoFn )
			line << t->addInfoFn( t->average );
	}

	_FormatTable( lines, INOUT str );
	AE_LOGI( str );

	_tests.clear();
}

/*
=================================================
	_UserDataCmp
=================================================
*/
inline Pair<double, StringView>  IntervalProfiler::_ParseNumber (StringView str)
{
	double	num		= 1.0;
	usize	pos		= 0;

	// parse number
	for (; pos < str.size(); ++pos)
	{
		char	c = str[pos];
		if ( c >= '0' and c <= '9' )
			num = num * 10.0 + double(c - '0');
		else
			break;
	}

	// parse fractional
	if ( pos < str.size() and str[pos] == '.' )
	{
		++pos;
		for (double scale = 0.1; pos < str.size(); ++pos)
		{
			char	c = str[pos];
			if ( c >= '0' and c <= '9' )
			{
				num = num + double(c - '0') * scale;
				scale *= 0.1;
			}else
				break;
		}
	}

	// parse suffix
	if ( pos < str.size() )
	{
		char	c = str[pos];
		++pos;
		switch ( c )
		{
			case 'T' :	num *= 1.0e+12; break;
			case 'G' :	num *= 1.0e+9;	break;
			case 'M' :	num *= 1.0e+6;	break;
			case 'K' :	num *= 1.0e+3;	break;
			case 'm' :	num *= 1.0e-3;	break;
			case 'u' :	num *= 1.0e-6;	break;
			case 'n' :	num *= 1.0e-9;	break;
			case 'p' :	num *= 1.0e-12;	break;
			default :	--pos;			break;
		}
	}

	return { num, SubString( str, pos )};
}


inline bool  IntervalProfiler::_UserDataCmp (StringView lhs, StringView rhs)
{
	auto	[l_val, l_sfx] = _ParseNumber( lhs );
	auto	[r_val, r_sfx] = _ParseNumber( rhs );

	if ( l_val == r_val )
		return Base::StringLessThan( r_sfx, l_sfx );

	return l_val > r_val;
}

/*
=================================================
	BeginTest
----
	warning: 'fn' executed in destructor, so references may expire
=================================================
*/
forceinline void  IntervalProfiler::BeginTest (StringView name)
{
	auto&	dst = _tests.emplace_back();
	dst.name	= name;
	dst.iterations.reserve( 128 );
}

forceinline void  IntervalProfiler::BeginTest (StringView name, AddInfoFn_t fn)
{
	auto&	dst = _tests.emplace_back();
	dst.name		= name;
	dst.addInfoFn	= RVRef(fn);
	dst.iterations.reserve( 128 );
}

/*
=================================================
	EndTest
=================================================
*/
forceinline void  IntervalProfiler::EndTest ()
{
	CHECK_ERRV( not _tests.empty() );

	auto&	test = _tests.back();
	CHECK_ERRV( not test.isComplete );
	test.isComplete = true;

	CHECK_ERRV( not test.iterations.empty() );

	Duration_t	sum = {};
	for (auto& dt : test.iterations) {
		sum += dt;
	}
	test.average = sum / test.iterations.size();

	test.iterations.clear();
}

/*
=================================================
	BeginIteration
=================================================
*/
forceinline void  IntervalProfiler::BeginIteration ()
{
	CHECK_ERRV( not _tests.empty() );

	// don't reorder instructions
	CompilerBarrier( EMemoryOrder::Acquire );

	auto&	test = _tests.back();
	CHECK_ERRV( not test.isComplete );

	test.iterations.emplace_back();
	test.lastStartPoint = Clock_t::now();

	// don't reorder instructions
	CompilerBarrier( EMemoryOrder::Release );
}

/*
=================================================
	EndIteration
=================================================
*/
forceinline void  IntervalProfiler::EndIteration ()
{
	// don't reorder instructions
	CompilerBarrier( EMemoryOrder::Acquire );

	TimePoint_t	end_time = Clock_t::now();

	CHECK_ERRV( not _tests.empty() );

	auto&	test = _tests.back();
	CHECK_ERRV( not test.isComplete );

	ASSERT( end_time >= test.lastStartPoint );
	ASSERT( not test.iterations.empty() );

	test.iterations.back() = end_time - test.lastStartPoint;

	// don't reorder instructions
	CompilerBarrier( EMemoryOrder::Release );
}

/*
=================================================
	_FormatTable
=================================================
*/
inline void  IntervalProfiler::_FormatTable (ArrayView<String> lines, INOUT String &str)
{
	constexpr uint	max_cols = 16;
	using Columns_t = StaticArray< ushort, max_cols >;

	Array< Columns_t >	row_cols;
	Columns_t			max_sizes = {};
	row_cols.resize( lines.size() );

	// find max sizes
	for (usize i = 0; i < lines.size(); ++i)
	{
		StringView	src = lines[i];
		auto&		dst = row_cols[i];
		usize		pos	= 0;
		ushort		prev = 0;

		dst.fill( ushort(src.size()) );
		for (uint j = 0; (j < max_cols) and (pos != StringView::npos); ++j)
		{
			pos				= src.find( '|', pos+1 );
			dst[j]			= ushort( Min( pos, src.size() ));
			max_sizes[j]	= Max( max_sizes[j], ushort(dst[j] - prev) );
			prev			= dst[j] + 1;
		}
	}

	// header
	for (uint j = 0; (j < max_cols) and (max_sizes[j] != 0); ++j)
	{
		if ( j == 0 )
			str << "\n|-";

		AppendToString( INOUT str, max_sizes[j], '-' );
		str << "-|-";
	}
	str.pop_back();

	// format table
	for (usize i = 0; i < lines.size(); ++i)
	{
		StringView		line	= lines[i];
		const auto&		sizes	= row_cols[i];
		ushort			prev	= 0;

		str << "\n| ";

		for (uint j = 0; j < max_cols; ++j)
		{
			StringView	part = SubStringBE( line, prev, sizes[j] );
			ASSERT( not part.empty() );

			str << part;

			AppendToString( INOUT str, usize{max_sizes[j]} - part.size() );

			str << " | ";
			prev = sizes[j] + 1;

			if ( prev >= line.size() )
				break;
		}
	}
	str.pop_back();
}

