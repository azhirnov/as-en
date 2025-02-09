// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Date
	//

	struct Date final
	{
	// types
	public:
		enum class EWeekDay
		{
			Mon = 0,
			Tue,
			Wed,
			Thu,
			Fri,
			Sat,
			Sun,
			_Count
		};

		enum class EMonth
		{
			Jan = 0,
			Feb,
			Mar,
			Apr,
			May,
			Jun,
			Jul,
			Aug,
			Sep,
			Oct,
			Nov,
			Dec,
			_Count
		};

		struct Builder;


	// variables
	private:
		uint	_year		: 16,	// ****	(0..65536)
				_month		: 4,	// 12	(0..16)
				_dayOfWeek	: 3,	// 7	(0..8)
				_dayOfYear	: 9,	// 366	(0..512)
				_dayOfMonth	: 5,	// 31	(0..32)
				_hour		: 5,	// 24	(0..32)
				_minute		: 6,	// 60	(0..64)
				_second		: 6,	// 60	(0..64)
				_millis		: 10;	// 1000	(0..1024)


	// methods
	public:
		__Cx__ Date ()											__NE___;

		__Cx__ Date (const Date &)								__NE___ = default;
		__Cx__ Date&		operator = (const Date &)			__NE___	= default;

		ND_ static Date		Now ()								__NE___;

		NdCx__ uint			Year ()								C_NE___	{ return _year; }
		NdCx__ EMonth		Month ()							C_NE___	{ return EMonth( _month ); }
		NdCx__ uint			MonthIndex ()						C_NE___	{ return _month+1; }
		NdCx__ uint			WeekOfYear ()						C_NE___	{ return (_dayOfYear / 7) + 1; }
		NdCx__ uint			DayOfYear ()						C_NE___	{ return _dayOfYear + 1; }
		NdCx__ uint			DayOfMonth ()						C_NE___	{ return _dayOfMonth + 1; }
		NdCx__ EWeekDay		DayOfWeek ()						C_NE___	{ return EWeekDay( _dayOfWeek ); }
		NdCx__ uint			Hour ()								C_NE___	{ return _hour; }
		NdCx__ uint			Minute ()							C_NE___	{ return _minute; }
		NdCx__ uint			Second ()							C_NE___	{ return _second; }
		NdCx__ uint			Milliseconds ()						C_NE___	{ return _millis; }

		Nd____ StringView	MonthName ()						C_NE___;
		Nd____ StringView	DayOfWeekName ()					C_NE___;

		NdCe__ static ulong	MillisInDay ()						__NE___	{ return 24 * MillisInHour(); }
		NdCe__ static ulong	MillisInHour ()						__NE___	{ return 60 * MillisInMinute(); }
		NdCe__ static ulong	MillisInMinute ()					__NE___	{ return 60 * MillisInSecond(); }
		NdCe__ static ulong	MillisInSecond ()					__NE___	{ return 1000; }


		NdCx__ bool			IsLeapYear ()						C_NE___	{ return _IsLeapYear( _year ); }
		NdCx__ uint			DaysInYear ()						C_NE___	{ return _MaxDaysOfYear( _year ); }
		NdCx__ uint			DaysInMonth ()						C_NE___	{ return _DaysInMonth( _year, _month ); }


		NdCx__ ulong	ToMillisecondsSince1970 ()				C_NE___	{ return _ToMilliseconds( 1970 ); }
		__Cx__ Date&	SetMillisecondsSince1970 (ulong ms)		__NE___	{ return _SetMilliseconds( 1970, ms ); }

		NdCx__ ulong	ToMillisecondsSinceEpoch ()				C_NE___	{ return _ToMilliseconds( 0 ); }
		__Cx__ Date&	SetMillisecondsSinceEpoch (ulong ms)	__NE___	{ return _SetMilliseconds( 0, ms ); }


		ND_ String  ToString (StringView fmt)					C_NE___;

		NdCx__ bool  operator == (const Date &other)			C_NE___;
		NdCx__ bool  operator <  (const Date &other)			C_NE___;
		NdCx__ bool  operator >  (const Date &other)			C_NE___	{ return other < *this; }
		NdCx__ bool  operator != (const Date &other)			C_NE___	{ return not (*this == other); }
		NdCx__ bool  operator >= (const Date &other)			C_NE___	{ return not (*this < other); }
		NdCx__ bool  operator <= (const Date &other)			C_NE___	{ return not (*this > other); }


	private:
		NdCx__ static bool  _IsLeapYear (uint year);
		NdCx__ static uint  _MaxDaysOfYear (uint year);
		NdCx__ static uint  _DaysInMonth (uint year, uint month);
		NdCx__ static uint  _CalcDayOfYear (uint year, uint month, uint dayOfMonth);
		NdCx__ static uint  _CalcDayOfWeek (uint year, uint month, uint dayOfMonth);
		__Cx__ static void  _CalcMonthAndDayOfMonth (uint year, uint dayOfYear, OUT uint &month, OUT uint &dayOfMonth);

		NdCx__ ulong  _ToMilliseconds (uint startYear) const;
		__Cx__ Date&  _SetMilliseconds (uint startYear, ulong ms);
	};



	//
	// Date Builder
	//

	struct Date::Builder
	{
	// variables
	private:
		Date	_date;


	// methods
	public:
		__Cx__ Builder ()								__NE___	{}
		__Cx__ explicit Builder (const Date &value)		__NE___ : _date{value} {}

		__Cx__ Builder&  Year (uint value)				__NE___	{ ASSERT_Cx( value < 65536 );				_date._year			= value;		return *this; }
		__Cx__ Builder&  Month (uint value)				__NE___	{ ASSERT_Cx( value < 12 );					_date._month		= value;		return *this; }
		__Cx__ Builder&  Month (EMonth value)			__NE___	{											_date._month		= uint(value);	return *this; }
		__Cx__ Builder&  DayOfMonth (uint value)		__NE___	{ ASSERT_Cx( value < _date.DaysInMonth() );	_date._dayOfMonth	= value;		return *this; }
		__Cx__ Builder&  DayOfYear (uint value)			__NE___	{ ASSERT_Cx( value < _date.DaysInYear() );	_date._dayOfYear	= value;		return *this; }
		__Cx__ Builder&  DayOfWeek (uint value)			__NE___	{ ASSERT_Cx( value < 7 );					_date._dayOfWeek	= value;		return *this; }
		__Cx__ Builder&  DayOfWeek (EWeekDay value)		__NE___	{											_date._dayOfWeek	= uint(value);	return *this; }
		__Cx__ Builder&  Hour (uint value)				__NE___	{ ASSERT_Cx( value < 24 );					_date._hour			= value;		return *this; }
		__Cx__ Builder&  Minute (uint value)			__NE___	{ ASSERT_Cx( value < 60 );					_date._minute		= value;		return *this; }
		__Cx__ Builder&  Second (uint value)			__NE___	{ ASSERT_Cx( value < 60 );					_date._second		= value;		return *this; }
		__Cx__ Builder&  Milliseconds (uint value)		__NE___	{ ASSERT_Cx( value < 1000 );				_date._millis		= value;		return *this; }

		__Cx__ Builder&  AddDayOfYear (uint value)		__NE___;
		__Cx__ Builder&  SubDayOfYear (uint value)		__NE___;

		__Cx__ Builder&  CalcDayOfYear ()				__NE___;	// requires: year, month, dayOfMonth
		__Cx__ Builder&  CalcDayOfWeek ()				__NE___;	// requires: year, month, dayOfMonth
		__Cx__ Builder&  CalcMonthAndDayOfMonth ()		__NE___;	// requires: year, dayOfYear

		NdCx__ Date			Get ()						C_NE___	{ return _date; }
		NdCx__ Date const*	operator -> ()				C_NE___	{ return &_date; }

	private:
		__Cx__ void  _ValidateYear ()					__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	__CxIn Date::Date () __NE___ :
		_year{0}, _month{0}, _dayOfWeek{0}, _dayOfYear{0}, _dayOfMonth{0}, _hour{0}, _minute{0}, _second{0}, _millis{0}
	{}

/*
=================================================
	operator ==
=================================================
*/
	__CxIn bool  Date::operator == (const Date &other) C_NE___
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
	operator <
=================================================
*/
	__CxIn bool  Date::operator < (const Date &other) C_NE___
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
	__CxIn bool  Date::_IsLeapYear (uint year)
	{
		return ((year % 4 == 0) and (year % 100 != 0)) or (year % 400 == 0);
	}

/*
=================================================
	_MaxDaysOfYear
=================================================
*/
	__CxIn uint  Date::_MaxDaysOfYear (uint year)
	{
		return 365 + uint(_IsLeapYear( year ));
	}

/*
=================================================
	_DaysInMonth
=================================================
*/
	__CxIn uint  Date::_DaysInMonth (uint year, uint month)
	{
		const ubyte days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		return days[ month ] + uint( (EMonth(month) == EMonth::Feb) and _IsLeapYear( year ));
	}

/*
=================================================
	_CalcDayOfYear
=================================================
*/
	__CxIn uint  Date::_CalcDayOfYear (uint year, uint month, uint dayOfMonth)
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
	__CxIn uint  Date::_CalcDayOfWeek (uint year, uint month, uint dayOfMonth)
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
	__CxIn void  Date::_CalcMonthAndDayOfMonth (uint year, uint dayOfYear, OUT uint &month, OUT uint &dayOfMonth)
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
	_ToMilliseconds
=================================================
*/
	__CxIn ulong  Date::_ToMilliseconds (uint startYear) const
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
	__CxIn Date&  Date::_SetMilliseconds (const uint startYear, const ulong ms)
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

		uint	m = 0, dm = 0;
		_CalcMonthAndDayOfMonth( _year, _dayOfYear, OUT m, OUT dm );

		_month		= m;
		_dayOfMonth	= dm;
		_dayOfWeek	= _CalcDayOfWeek( _year, _month, _dayOfMonth );

		return *this;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Calc*
=================================================
*/
	__CxIn Date::Builder&  Date::Builder::CalcDayOfYear () __NE___
	{
		return DayOfYear( Date::_CalcDayOfYear( _date._year, _date._month, _date._dayOfMonth ));
	}

	__CxIn Date::Builder&  Date::Builder::CalcDayOfWeek () __NE___
	{
		return DayOfWeek( Date::_CalcDayOfWeek( _date._year, _date._month, _date._dayOfMonth ));
	}

	__CxIn Date::Builder&  Date::Builder::CalcMonthAndDayOfMonth () __NE___
	{
		uint	m = 0, dm = 0;
		Date::_CalcMonthAndDayOfMonth( _date._year, _date._dayOfYear, OUT m, OUT dm );

		_date._month		= m;
		_date._dayOfMonth	= dm;
		return *this;
	}


} // AE::Base
