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


    // variables
    private:
        uint    _year       : 16,   // **** (0..65536)
                _month      : 4,    // 12   (0..16)
                _dayOfWeek  : 3,    // 7    (0..8)
                _dayOfYear  : 9,    // 366  (0..512)
                _dayOfMonth : 5,    // 31   (0..32)
                _hour       : 5,    // 24   (0..32)
                _minute     : 6,    // 60   (0..64)
                _second     : 6,    // 60   (0..64)
                _millis     : 10;   // 1000 (0..1024)   // TODO: change to micro or nano?


    // methods
    public:
        Date ()                                         __NE___;

        ND_ static Date     Now ()                      __NE___;

        ND_ uint            Year ()                     C_NE___ { return _year; }
        ND_ EMonth          Month ()                    C_NE___ { return EMonth( _month ); }
        ND_ uint            WeekOfYear ()               C_NE___ { return (_dayOfYear / 7) + 1; }
        ND_ uint            DayOfYear ()                C_NE___ { return _dayOfYear + 1; }
        ND_ uint            DayOfMonth ()               C_NE___ { return _dayOfMonth + 1; }
        ND_ EWeekDay        DayOfWeek ()                C_NE___ { return EWeekDay( _dayOfWeek ); }
        ND_ uint            Hour ()                     C_NE___ { return _hour; }
        ND_ uint            Minute ()                   C_NE___ { return _minute; }
        ND_ uint            Second ()                   C_NE___ { return _second; }
        ND_ uint            Milliseconds ()             C_NE___ { return _millis; }

        ND_ StringView      MonthName ()                C_NE___;
        ND_ StringView      DayOfWeekName ()            C_NE___;

        ND_ static ulong    MillisInDay ()              __NE___ { return 24 * MillisInHour(); }
        ND_ static ulong    MillisInHour ()             __NE___ { return 60 * MillisInMinute(); }
        ND_ static ulong    MillisInMinute ()           __NE___ { return 60 * MillisInSecond(); }
        ND_ static ulong    MillisInSecond ()           __NE___ { return 1000; }


        ND_ bool            IsLeapYear ()               C_NE___ { return _IsLeapYear( _year ); }
        ND_ uint            DaysInYear ()               C_NE___ { return _MaxDaysOfYear( _year ); }
        ND_ uint            DaysInMonth ()              C_NE___ { return _DaysInMonth( _year, _month ); }


        ND_ ulong   ToMillisecondsSince1970 ()          C_NE___ { return _ToMilliseconds( 1970 ); }
            Date&   SetMillisecondsSince1970 (ulong ms) __NE___ { return _SetMilliseconds( 1970, ms ); }

        ND_ ulong   ToMillisecondsSinceEpoch ()         C_NE___ { return _ToMilliseconds( 0 ); }
            Date&   SetMillisecondsSinceEpoch (ulong ms)__NE___ { return _SetMilliseconds( 0, ms ); }

            Date&   SetYear (uint value)                __NE___ { _year = value; return *this; }
            Date&   SetMonth (uint value)               __NE___ { _month = value; return *this; }


        ND_ String  ToString (StringView fmt)           C_NE___;

        ND_ bool  operator == (const Date &other)       C_NE___;
        ND_ bool  operator >  (const Date &other)       C_NE___;
        ND_ bool  operator <  (const Date &other)       C_NE___;
        ND_ bool  operator != (const Date &other)       C_NE___ { return not (*this == other); }
        ND_ bool  operator >= (const Date &other)       C_NE___ { return not (*this < other); }
        ND_ bool  operator <= (const Date &other)       C_NE___ { return not (*this > other); }


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


} // AE::Base
