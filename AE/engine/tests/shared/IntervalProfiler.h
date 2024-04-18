// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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

	struct TestInfo
	{
		TimePoint_t			lastStartPoint	= Default;
		String				name;
		Array< Duration_t >	iterations;
		Duration_t			medium			= Default;
		bool				isEnded			= false;
	};


// variables
private:
	Array< TestInfo >	_tests;
	String				_testName;


// methods
public:
	explicit IntervalProfiler (StringView name) : _testName{name} {}
	~IntervalProfiler ();

	void  BeginTest (StringView name);
	void  EndTest ();

	void  BeginIteration ();
	void  EndIteration ();
};

using TsIntervalProfiler = AE::Threading::Synchronized< Mutex, IntervalProfiler >;


/*
=================================================
	destructor
=================================================
*/
inline IntervalProfiler::~IntervalProfiler ()
{
	std::sort( _tests.begin(), _tests.end(), [](const auto& lhs, const auto& rhs) { return lhs.medium < rhs.medium; });

	String	str;
	str << '\n' << _testName << ':';

	const auto	ToDouble = [] (Duration_t dt) {{ return TimeCast<secondsd>( dt ).count(); }};

	const Duration_t	first	= _tests.begin()->medium;
	usize				max_len	= 0;

	for (auto& t : _tests) {
		max_len = Max( max_len, t.name.length() );
	}

	for (auto& t : _tests)
	{
		double	frac = first.count() == 0 ? 0.0 : (ToDouble( t.medium - first ) / ToDouble( first )) * 100.0;

		str << "\n  " << t.name;
		AppendToString( INOUT str, max_len - t.name.length() );
		str << ": " << ToString(t.medium);

		if ( frac != 0.0 )
			str << "  +" << ToString( frac, 1 ) << '%';
	}
	AE_LOGI( str );
}

/*
=================================================
	BeginTest
=================================================
*/
forceinline void  IntervalProfiler::BeginTest (StringView name)
{
	_tests.emplace_back().name = name;
}

/*
=================================================
	EndTest
=================================================
*/
forceinline void  IntervalProfiler::EndTest ()
{
	CHECK_ERRV( _tests.size() );

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
	CHECK_ERRV( _tests.size() );

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

	CHECK_ERRV( _tests.size() );

	auto&	test = _tests.back();
	CHECK_ERRV( not test.isEnded );

	ASSERT( end_time >= test.lastStartPoint );
	ASSERT( test.iterations.size() );

	test.iterations.back() = end_time - test.lastStartPoint;

	// don't reorder instructions
	CompilerBarrier( EMemoryOrder::Release );
}
