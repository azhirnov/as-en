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


// methods
public:
	explicit IntervalProfiler (StringView name) : _testName{name}	{}
	~IntervalProfiler ()											{ PrintAndReset(); }

	void  BeginTest (StringView name);
	void  BeginTest (StringView name, AddInfoFn_t fn);
	void  EndTest ();

	void  BeginIteration ();
	void  EndIteration ();

	void  PrintAndReset ();
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

	std::sort( _tests.begin(), _tests.end(), [](const auto& lhs, const auto& rhs) { return lhs.medium < rhs.medium; });

	String	str;
	str << '\n' << _testName << ':';

	const auto	ToDouble = [] (Duration_t dt) {{ return TimeCast<secondsd>( dt ).count(); }};

	const Duration_t	first	= _tests.begin()->medium;
	const usize			max_pad	= 12;
	usize				max_len	= 0;

	for (auto& t : _tests) {
		max_len = Max( max_len, t.name.length() );
	}

	for (auto& t : _tests)
	{
		double	fract = first.count() == 0 ? 0.0 : (ToDouble( t.medium - first ) / ToDouble( first )) * 100.0;

		str << "\n  " << t.name;
		AppendToString( INOUT str, max_len - t.name.length() );
		str << ": " << ToString(t.medium);

		const usize	pos = str.length();

		if ( fract > 1.0e-10 )
			str << "  +" << ToString( fract, 1 ) << '%';

		if ( t.addInfoFn )
		{
			AppendToString( INOUT str, max_pad - Min( max_pad, str.length() - pos ));
			str << t.addInfoFn( t.medium );
		}
	}
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
