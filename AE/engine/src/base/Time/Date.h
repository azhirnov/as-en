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
				_millis		: 10;	// 1000	(0..1024)	// TODO: change to micro or nano?


	// methods
	public:
		Date ()											__NE___;

		ND_ static Date		Now ()						__NE___;

		ND_ uint			Year ()						C_NE___	{ return _year; }
		ND_ EMonth			Month ()					C_NE___	{ return EMonth( _month ); }
		ND_ uint			WeekOfYear ()				C_NE___	{ return (_dayOfYear / 7) + 1; }
		ND_ uint			DayOfYear ()				C_NE___	{ return _dayOfYear + 1; }
		ND_ uint			DayOfMonth ()				C_NE___	{ return _dayOfMonth + 1; }
		ND_ EWeekDay		DayOfWeek ()				C_NE___	{ return EWeekDay( _dayOfWeek ); }
		ND_ uint			Hour ()						C_NE___	{ return _hour; }
		ND_ uint			Minute ()					C_NE___	{ return _minute; }
		ND_ uint			Second ()					C_NE___	{ return _second; }
		ND_ uint			Milliseconds ()				C_NE___	{ return _millis; }

		ND_ StringView		MonthName ()				C_NE___;
		ND_ StringView		DayOfWeekName ()			C_NE___;

		ND_ static ulong	MillisInDay ()				__NE___	{ return 24 * MillisInHour(); }
		ND_ static ulong	MillisInHour ()				__NE___	{ return 60 * MillisInMinute(); }
		ND_ static ulong	MillisInMinute ()			__NE___	{ return 60 * MillisInSecond(); }
		ND_ static ulong	MillisInSecond ()			__NE___	{ return 1000; }


		ND_ bool			IsLeapYear ()				C_NE___	{ return _IsLeapYear( _year ); }
		ND_ uint			DaysInYear ()				C_NE___	{ return _MaxDaysOfYear( _year ); }
		ND_ uint			DaysInMonth ()				C_NE___	{ return _DaysInMonth( _year, _month ); }


		ND_ ulong	ToMillisecondsSince1970 ()			C_NE___	{ return _ToMilliseconds( 1970 ); }
			Date&	SetMillisecondsSince1970 (ulong ms)	__NE___	{ return _SetMilliseconds( 1970, ms ); }

		ND_ ulong	ToMillisecondsSinceEpoch ()			C_NE___	{ return _ToMilliseconds( 0 ); }
			Date&	SetMillisecondsSinceEpoch (ulong ms)__NE___	{ return _SetMilliseconds( 0, ms ); }


		ND_ String  ToString (StringView fmt)			C_NE___;

		ND_ bool  operator == (const Date &other)		C_NE___;
		ND_ bool  operator >  (const Date &other)		C_NE___;
		ND_ bool  operator <  (const Date &other)		C_NE___;
		ND_ bool  operator != (const Date &other)		C_NE___	{ return not (*this == other); }
		ND_ bool  operator >= (const Date &other)		C_NE___	{ return not (*this < other); }
		ND_ bool  operator <= (const Date &other)		C_NE___	{ return not (*this > other); }


	private:
		ND_ static bool  _IsLeapYear (uint year);
		ND_ static uint  _MaxDaysOfYear (uint year);
		ND_ static uint  _DaysInMonth (uint year, uint month);
		ND_ static uint  _CalcDayOfYear (uint year, uint month, uint dayOfMonth);
		ND_ static uint  _CalcDayOfWeek (uint year, uint month, uint dayOfMonth);
			static void  _CalcMonthAndDayOfMonth (uint year, uint dayOfYear, OUT uint &month, OUT uint &dayOfMonth);

		ND_ ulong  _ToMilliseconds (uint startYear) const;
			Date&  _SetMilliseconds (uint startYear, ulong ms);
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
		Builder ()								__NE___	{}
		explicit Builder (const Date &value)	__NE___ : _date{value} {}

		Builder&  Year (uint value)				__NE___	{ ASSERT( value < 65536 );					_date._year			= value;		return *this; }
		Builder&  Month (uint value)			__NE___	{ ASSERT( value < 12 );						_date._month		= value;		return *this; }
		Builder&  Month (EMonth value)			__NE___	{											_date._month		= uint(value);	return *this; }
		Builder&  DayOfMonth (uint value)		__NE___	{ ASSERT( value < _date.DaysInMonth() );	_date._dayOfMonth	= value;		return *this; }
		Builder&  DayOfYear (uint value)		__NE___	{ ASSERT( value < _date.DaysInYear() );		_date._dayOfYear	= value;		return *this; }
		Builder&  DayOfWeek (uint value)		__NE___	{ ASSERT( value < 7 );						_date._dayOfWeek	= value;		return *this; }
		Builder&  DayOfWeek (EWeekDay value)	__NE___	{											_date._dayOfWeek	= uint(value);	return *this; }
		Builder&  Hour (uint value)				__NE___	{ ASSERT( value < 24 );						_date._hour			= value;		return *this; }
		Builder&  Minute (uint value)			__NE___	{ ASSERT( value < 60 );						_date._minute		= value;		return *this; }
		Builder&  Second (uint value)			__NE___	{ ASSERT( value < 60 );						_date._second		= value;		return *this; }
		Builder&  Milliseconds (uint value)		__NE___	{ ASSERT( value < 1000 );					_date._millis		= value;		return *this; }

		Builder&  AddDayOfYear (uint value)		__NE___;
		Builder&  SubDayOfYear (uint value)		__NE___;

		Builder&  CalcDayOfYear ()				__NE___;	// requires: year, month, dayOfMonth
		Builder&  CalcDayOfWeek ()				__NE___;	// requires: year, month, dayOfMonth
		Builder&  CalcMonthAndDayOfMonth ()		__NE___;	// requires: year, dayOfYear

		ND_ Date		Get ()					C_NE___	{ return _date; }
		ND_ Date const*	operator -> ()			C_NE___	{ return &_date; }

	private:
		void  _ValidateYear ()					__NE___;
	};


} // AE::Base
