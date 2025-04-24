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
		Unknown			= 0,

		// sort
		SortByName		= 1 << 0,
		SortByPerf		= 1 << 1,

		// exclude
		ExcludeDelta	= 1 << 8,		// difference from previous to current, depends on sorting
		ExcludeTime		= 1 << 9,
		ExcludePerfDiff	= 1 << 10,		// difference from fastest to current
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
		Duration_t			medium			= Default;
		bool				isEnded			= false;
		AddInfoFn_t			addInfoFn;
	};


// variables
private:
	Array< TestInfo >	_tests;
	String				_testName;
	EFlags				_flags		= Default;


// methods
public:
	explicit IntervalProfiler (StringView name, EFlags flags = EFlags::SortByPerf) :
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
};

AE_BIT_OPERATORS( IntervalProfiler::EFlags );

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

	Duration_t	min_time {MaxValue<slong>()};

	if ( AllBits( _flags, EFlags::SortByPerf ))
	{
		std::sort( _tests.begin(), _tests.end(), [](const auto& lhs, const auto& rhs) { return lhs.medium < rhs.medium; });
		min_time = _tests.begin()->medium;
	}
	else
	{
		for (auto& t : _tests)
			min_time = Min( min_time, t.medium );
	}

	if ( AllBits( _flags, EFlags::SortByName ))
		std::sort( _tests.begin(), _tests.end(), [](const auto& lhs, const auto& rhs) { return lhs.name < rhs.name; });		// TODO: lexical compare?

	String	str;
	str << '\n' << _testName << ':';

	const auto	ToDouble = [] (Duration_t dt) {{ return TimeCast<secondsd>( dt ).count(); }};

	Duration_t		prev	{-1};
	Array<String>	lines;

	for (auto& t : _tests)
	{
		auto&	line = lines.emplace_back();
		line << t.name << '|';
		
		if ( NoBits( _flags, EFlags::ExcludeTime ))
		{
			line << ToString(t.medium) << '|';
		}

		// perf diff
		if ( NoBits( _flags, EFlags::ExcludePerfDiff ))
		{
			double	fract	= min_time.count() == 0 ? 0.0 : (ToDouble( t.medium - min_time ) / ToDouble( min_time )) * 100.0;
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
		if ( NoBits( _flags, EFlags::ExcludeDelta ))
		{
			double	fract	= prev.count() < 0 ? 0.0 :  (ToDouble( t.medium - prev ) / ToDouble( prev )) * 100.0;
					fract	= Round( Abs( fract ));
					prev	= t.medium;
					
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

		if ( t.addInfoFn )
			line << t.addInfoFn( t.medium );
	}

	_FormatTable( lines, INOUT str );
	AE_LOGI( str );

	_tests.clear();
}

/*
=================================================
	BeginTest
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
	CHECK_ERRV( not test.isEnded );
	test.isEnded = true;

	CHECK_ERRV( not test.iterations.empty() );

	Duration_t	sum = {};
	for (auto& dt : test.iterations) {
		sum += dt;
	}
	test.medium = sum / test.iterations.size();
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
	CHECK_ERRV( not test.isEnded );

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
	CHECK_ERRV( not test.isEnded );

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
			StringView	part = SubString2( line, prev, sizes[j] );
			str << part;

			AppendToString( INOUT str, usize{max_sizes[j]} - part.size() );

			str << " | ";
			prev = sizes[j] + 1;

			if ( sizes[j] >= line.size() )
				break;
		}
	}
	str.pop_back();
}

