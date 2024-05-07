// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"
#include <ctime>

#include "base/Time/Date.h"
#include "base/Utils/Threading.h"
#include "base/Algorithms/Parser.h"
#include "base/Algorithms/StringUtils.h"

#include "base/Platforms/WindowsHeader.cpp.h"

// 'localtime_s' in C has different signature
#ifdef AE_COMPILER_MSVC
#	define localtime2( _outBuf_, _inTime_ )		(localtime_s( _outBuf_, _inTime_ ) == 0)
#elif 1
#	define localtime2( _outBuf_, _inTime_ )		(localtime_r( _inTime_, _outBuf_ ) != null)
#else
namespace
{
	ND_ static bool  localtime2 (OUT struct tm* const Tm, time_t const* const Time)
	{
		// from specs:
		//	This function may not be thread-safe.
		//	The structure may be shared between std::gmtime, std::localtime, and std::ctime, and may be overwritten on each invocation.

		if ( const tm* res = std::localtime( Time ))
		{
			*Tm = *res;
			return true;
		}
		return false;
	}
}
#endif

namespace AE::Base
{

/*
=================================================
	constructor
=================================================
*/
	Date::Date () __NE___
	{
		UnsafeZeroMem( *this );
	}

/*
=================================================
	MonthName
=================================================
*/
	StringView  Date::MonthName () C_NE___
	{
		static const char * names[] = {
			"January", "February", "March", "April", "May", "June", "July",
			"August", "September", "October", "November", "December"
		};
		return names[ _month ];
	}

/*
=================================================
	DayOfWeekName
=================================================
*/
	StringView  Date::DayOfWeekName () C_NE___
	{
		static const char * names[] = {
			"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
		};
		return names[ _dayOfWeek ];
	}

/*
=================================================
	_ToMilliseconds
=================================================
*/
	ulong  Date::_ToMilliseconds (uint startYear) const
	{
		ulong	ms = _millis;

		ms += _second * MillisInSecond();
		ms += _minute * MillisInMinute();
		ms += _hour * MillisInHour();
		ms += _dayOfYear * MillisInDay();

		for (uint y = startYear; y < _year; ++y) {
			ms += _MaxDaysOfYear( y ) * MillisInDay();
		}
		return ms;
	}

/*
=================================================
	_SetMilliseconds
=================================================
*/
	Date&  Date::_SetMilliseconds (const uint startYear, const ulong ms)
	{
		_millis		= uint(ms % MillisInSecond());
		_second		= (ms / MillisInSecond()) % 60;
		_minute		= (ms / MillisInMinute()) % 60;
		_hour		= (ms / MillisInHour())   % 24;

		ulong days	= ms / MillisInDay();
		uint  year	= startYear;
		uint  max_d	= _MaxDaysOfYear( year );

		for (; days >= max_d;)
		{
			days -= max_d;
			max_d = _MaxDaysOfYear( ++year );
		}

		_dayOfYear	= uint(days);
		_year		= year;

		uint	m, dm;
		_CalcMonthAndDayOfMonth( _year, _dayOfYear, OUT m, OUT dm );

		_month		= m;
		_dayOfMonth	= dm;
		_dayOfWeek	= _CalcDayOfWeek( _year, _month, _dayOfMonth );

		return *this;
	}

/*
=================================================
	ToString
----
	yy		- year short
	yyyy	- year full
	mm		- month
	mmm		- month short name
	mmmm...	- month name
	we		- week of year
	dm		- day of month
	de		- day of year
	dee		- day of year aligned
	dw		- day of week
	dww		- day of week short name
	dwww...	- day of week full name
	hh		- hour
	mi		- minute
	ss		- second
	ms		- millisecond
=================================================
*/
	String  Date::ToString (StringView fmt) C_NE___
	{
		TRY{
			String				str;
			Array< StringView > tokens;
			Parser::DivideString_Words( fmt, OUT tokens );

			for (auto t : tokens)
			{
				if ( t == "yy" )
					str << FormatAlignedI<10>( Year(), 4, '0' ).substr(2);
				else
				if ( t == "yyyy" )
					str << FormatAlignedI<10>( Year(), 4, '0' );
				else
				if ( t == "mm" )
					str << FormatAlignedI<10>( _month+1, 2, '0' );
				else
				if ( t == "mmm" )
					str << MonthName().substr( 0, 3 );
				else
				if ( StartsWith( t, "mmmm" ))
					str << MonthName();
				else
				if ( t == "we" )
					str << FormatAlignedI<10>( WeekOfYear(), 2, '0' );
				else
				if ( t == "dm" )
					str << FormatAlignedI<10>( DayOfMonth(), 2, '0' );
				else
				if ( t == "de" )
					str << Base::ToString( DayOfYear() );
				else
				if ( t == "dee" )
					str << FormatAlignedI<10>( DayOfYear(), 3, '0' );
				else
				if ( t == "dw" )
					str << FormatAlignedI<10>( uint(DayOfWeek())+1, 2, '0' );
				else
				if ( t == "dww" )
					str << DayOfWeekName().substr( 0, 3 );
				else
				if ( StartsWith( t, "dwww" ))
					str << DayOfWeekName();
				else
				if ( t == "hh" )
					str << FormatAlignedI<10>( Hour(), 2, '0' );
				else
				if ( t == "mi" )
					str << FormatAlignedI<10>( Minute(), 2, '0' );
				else
				if ( t == "ss" )
					str << FormatAlignedI<10>( Second(), 2, '0' );
				else
				if ( t == "ms" )
					str << FormatAlignedI<10>( Milliseconds(), 3, '0' );
				else
					// TODO: it may be unsupported token, need some way to detect it
					str << t;
			}
			return str;
		}
		CATCH_ALL(
			return Default;
		)
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  Date::operator == (const Date &other) C_NE___
	{
		return	(Year()			== other.Year())		and
				(DayOfYear()	== other.DayOfYear())	and
				(Hour()			== other.Hour())		and
				(Minute()		== other.Minute())		and
				(Second()		== other.Second())		and
				(Milliseconds()	== other.Milliseconds());
	}

/*
=================================================
	operator >
=================================================
*/
	bool  Date::operator > (const Date &other) C_NE___
	{
		return	Year()			!= other.Year()			?	Year()		> other.Year()		:
				DayOfYear()		!= other.DayOfYear()	?	DayOfYear()	> other.DayOfYear()	:
				Hour()			!= other.Hour()			?	Hour()		> other.Hour()		:
				Minute()		!= other.Minute()		?	Minute()	> other.Minute()	:
				Second()		!= other.Second()		?	Second()	> other.Second()	:
				Milliseconds()	<  other.Milliseconds();
	}

/*
=================================================
	operator <
=================================================
*/
	bool  Date::operator < (const Date &other) C_NE___
	{
		return	Year()			!= other.Year()			?	Year()		< other.Year()		:
				DayOfYear()		!= other.DayOfYear()	?	DayOfYear()	< other.DayOfYear()	:
				Hour()			!= other.Hour()			?	Hour()		< other.Hour()		:
				Minute()		!= other.Minute()		?	Minute()	< other.Minute()	:
				Second()		!= other.Second()		?	Second()	< other.Second()	:
				Milliseconds()	<  other.Milliseconds();
	}

/*
=================================================
	_IsLeapYear
=================================================
*/
	bool  Date::_IsLeapYear (uint year)
	{
		return ((year % 4 == 0) and (year % 100 != 0)) or (year % 400 == 0);
	}

/*
=================================================
	_MaxDaysOfYear
=================================================
*/
	uint  Date::_MaxDaysOfYear (uint year)
	{
		return 365 + uint(_IsLeapYear( year ));
	}

/*
=================================================
	_DaysInMonth
=================================================
*/
	uint  Date::_DaysInMonth (uint year, uint month)
	{
		const ubyte days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		return days[ month ] + uint( (EMonth(month) == EMonth::Feb) and _IsLeapYear( year ));
	}

/*
=================================================
	_CalcDayOfYear
=================================================
*/
	uint  Date::_CalcDayOfYear (uint year, uint month, uint dayOfMonth)
	{
		uint day = 0;

		for (uint i = 0; i < month; ++i) {
			day += _DaysInMonth( year, i );
		}
		return day + dayOfMonth;
	}

/*
=================================================
	_CalcDayOfWeek
=================================================
*/
	uint  Date::_CalcDayOfWeek (uint year, uint month, uint dayOfMonth)
	{
		const ubyte	t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
		year -= month < 2;
		return (year + year/4 - year/100 + year/400 + t[month] + dayOfMonth) % 7;
	}

/*
=================================================
	_CalcMonthAndDayOfMonth
=================================================
*/
	void  Date::_CalcMonthAndDayOfMonth (uint year, uint dayOfYear, OUT uint &month, OUT uint &dayOfMonth)
	{
		uint days_in_month = _DaysInMonth( year, 0 );

		for (month = 0; dayOfYear >= days_in_month;)
		{
			dayOfYear		-= days_in_month;
			days_in_month	 = _DaysInMonth( year, ++month );
		}

		dayOfMonth = dayOfYear;
	}

/*
=================================================
	Now
=================================================
*/
#if 0 //not defined(DATE_NOW_MODE) and (defined(__cpp_lib_chrono) and (__cpp_lib_chrono >= 201907L))
#	define DATE_NOW_MODE	1	// C++20	// TODO: mem leak
#endif

#if not defined(DATE_NOW_MODE) and defined(AE_PLATFORM_WINDOWS)
#	define DATE_NOW_MODE	2	// WinAPI
#endif

#ifndef DATE_NOW_MODE
#	define DATE_NOW_MODE	0	// cross-platform
#endif

	Date  Date::Now () __NE___
	{
		Date	res;

		#if DATE_NOW_MODE == 0
		{
			std::time_t	t	= std::time(0);
			std::tm		now	= {};
			Unused( localtime2( OUT &now, &t ));

			res._year		= now.tm_year + 1900;
			res._month		= now.tm_mon;
			res._dayOfMonth	= now.tm_mday - 1;
			res._dayOfWeek	= now.tm_wday == 0 ? 6 : now.tm_wday-1;
			res._dayOfYear	= now.tm_yday;
			res._hour		= now.tm_hour;
			res._minute		= now.tm_min;
			res._second		= now.tm_sec;
			res._millis		= 0;
		}
		#elif DATE_NOW_MODE == 1
		{
			using namespace std::chrono;

			const auto				tp			= get_tzdb().current_zone()->to_local( system_clock::now() );
			const year_month_day	yy_mm_dd	{ floor< std::chrono::days >( tp )};
			const hh_mm_ss			hh_mm_ss	{tp.time_since_epoch()};

			res._year		= int{yy_mm_dd.year()};
			res._month		= uint{yy_mm_dd.month()}-1;			// 1..12 convert to 0..11
			res._dayOfMonth	= uint{yy_mm_dd.day()};
			res._dayOfYear	= _CalcDayOfYear( res._year, res._month, res._dayOfMonth );
			res._dayOfWeek	= _CalcDayOfWeek( res._year, res._month, res._dayOfMonth );

			res._hour		= hh_mm_ss.hours().count() % 24;
			res._minute		= hh_mm_ss.minutes().count();
			res._second		= hh_mm_ss.seconds().count();
			res._millis		= hh_mm_ss.subseconds().count();
		}
		#elif DATE_NOW_MODE == 2
		{
			SYSTEMTIME	 time = {};
			::GetLocalTime( &time );

			res._year		= time.wYear;
			res._month		= time.wMonth-1;								// 1..12 convert to 0..11
			res._dayOfWeek	= time.wDayOfWeek == 0 ? 6 : time.wDayOfWeek-1;	// 0..6, Sun == 0 convert to mon..sun
			res._dayOfMonth	= time.wDay-1;									// 1..31 convert to 0..30
			res._dayOfYear	= _CalcDayOfYear( res._year, res._month, res._dayOfMonth );

			res._hour		= time.wHour;									// 0..23
			res._minute		= time.wMinute;									// 0..59
			res._second		= time.wSecond;									// 0..59
			res._millis		= time.wMilliseconds;							// 0..999
		}
		#endif

		ASSERT_Eq( res._dayOfWeek, _CalcDayOfWeek( res._year, res._month, res._dayOfMonth ));
		ASSERT_Eq( res._dayOfYear, _CalcDayOfYear( res._year, res._month, res._dayOfMonth ));

		DEBUG_ONLY({
			uint	m, dm;
			_CalcMonthAndDayOfMonth( res._year, res._dayOfYear, OUT m, OUT dm );
			CHECK( m == res._month );
			CHECK( dm == res._dayOfMonth );
		})
		return res;
	}

#undef DATE_NOW_MODE
//-----------------------------------------------------------------------------



/*
=================================================
	Calc*
=================================================
*/
	Date::Builder&  Date::Builder::CalcDayOfYear () __NE___
	{
		return DayOfYear( Date::_CalcDayOfYear( _date._year, _date._month, _date._dayOfMonth ));
	}

	Date::Builder&  Date::Builder::CalcDayOfWeek () __NE___
	{
		return DayOfWeek( Date::_CalcDayOfWeek( _date._year, _date._month, _date._dayOfMonth ));
	}

	Date::Builder&  Date::Builder::CalcMonthAndDayOfMonth () __NE___
	{
		uint	m, dm;
		Date::_CalcMonthAndDayOfMonth( _date._year, _date._dayOfYear, OUT m, OUT dm );

		_date._month		= m;
		_date._dayOfMonth	= dm;
		return *this;
	}

/*
=================================================
	AddDayOfYear
=================================================
*
	Date::Builder&  Date::Builder::AddDayOfYear (uint value) __NE___
	{
		return *this;
	}

/*
=================================================
	SubDayOfYear
=================================================
*
	Date::Builder&  Date::Builder::SubDayOfYear (uint value) __NE___
	{
	}
*/

} // AE::Base
