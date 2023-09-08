// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"
#include <ctime>

#include "base/Utils/Date.h"
#include "base/Algorithms/Parser.h"
#include "base/Algorithms/StringUtils.h"

// 'localtime_s' in C has different signature
#ifdef AE_COMPILER_MSVC
#   define localtime2   localtime_s
#else
namespace
{
    ND_ static int  localtime2 (OUT struct tm* const Tm, time_t const* const Time)
    {
        if ( const tm* res = std::localtime( Time ))
        {
            *Tm = *res;
            return 0;
        }
        return -1;
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
        ZeroMem( OUT this, Sizeof(*this) );
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
        ulong   ms = _millis;

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
    Date&  Date::_SetMilliseconds (uint startYear, ulong ms)
    {
        _millis     = uint(ms % MillisInSecond());
        _second     = (ms / MillisInSecond()) % 60;
        _minute     = (ms / MillisInMinute()) % 60;
        _hour       = (ms / MillisInHour())   % 24;

        ulong days  = ms / MillisInDay();
        uint  year  = startYear;
        uint  max_d = _MaxDaysOfYear( year );

        for (; days >= max_d;)
        {
            days -= max_d;
            max_d = _MaxDaysOfYear( ++year );
        }

        _dayOfYear  = uint(days);
        _year       = year;

        uint    m, dm;
        _CalcMonthAndDayOfMonth( _year, _dayOfYear, OUT m, OUT dm );

        _month      = m;
        _dayOfMonth = dm;
        _dayOfWeek  = _CalcDayOfWeek( _year, _month, _dayOfMonth );

        return *this;
    }

/*
=================================================
    ToString
=================================================
*/
    String  Date::ToString (StringView fmt) C_NE___
    {
        // yy - year short
        // yyyy - year full
        // mm - month
        // mmm - month short name
        // mmmm... - month name
        // we - week of year
        // dm - day of month
        // de - day of year
        // dee - day of year aligned
        // dw - day of week
        // dww - day of week short name
        // dwww... - day of week full name
        // hh - hour
        // mi - minute
        // ss - second
        // ms - millisecond

        String  str;

        try{
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
                    str << (_month+1 ? "0"s : ""s) << Base::ToString( _month+1 );
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
                    // TODO: it may be unsuported token, need some way to detect it
                    str << t;
            }
        }
        catch (...){}

        return str;
    }

/*
=================================================
    operator ==
=================================================
*/
    bool  Date::operator == (const Date &other) C_NE___
    {
        return  Year()          == other.Year()         and
                DayOfYear()     == other.DayOfYear()    and
                Hour()          == other.Hour()         and
                Minute()        == other.Minute()       and
                Second()        == other.Second()       and
                Milliseconds()  == other.Milliseconds();
    }

/*
=================================================
    operator >
=================================================
*/
    bool  Date::operator > (const Date &other) C_NE___
    {
        return  Year()          != other.Year()         ?   Year()      > other.Year()      :
                DayOfYear()     != other.DayOfYear()    ?   DayOfYear() > other.DayOfYear() :
                Hour()          != other.Hour()         ?   Hour()      > other.Hour()      :
                Minute()        != other.Minute()       ?   Minute()    > other.Minute()    :
                Second()        != other.Second()       ?   Second()    > other.Second()    :
                Milliseconds()  <  other.Milliseconds();
    }

/*
=================================================
    operator <
=================================================
*/
    bool  Date::operator < (const Date &other) C_NE___
    {
        return  Year()          != other.Year()         ?   Year()      < other.Year()      :
                DayOfYear()     != other.DayOfYear()    ?   DayOfYear() < other.DayOfYear() :
                Hour()          != other.Hour()         ?   Hour()      < other.Hour()      :
                Minute()        != other.Minute()       ?   Minute()    < other.Minute()    :
                Second()        != other.Second()       ?   Second()    < other.Second()    :
                Milliseconds()  <  other.Milliseconds();
    }

/*
=================================================
    _IsLeapYear
=================================================
*/
    bool  Date::_IsLeapYear (uint year)
    {
        return ((year & 0x3) == 0 and year % 100 != 0);
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
        int m = month;
        int y = year;

        if ( m <= 0 )
        {
            m += 12;
            y--;
        }

        int n = (dayOfMonth + (13 * m - 1) / 5 + (y % 100) + (y / 100) / 4 + (y % 100) / 4 - 2 * (y / 100)) % 7;

        if ( n < 0 )
            n += 7;

        return n == 0 ? 6 : n-1;
    }

/*
=================================================
    _CalcMonthAndDayOfMonth
=================================================
*/
    void  Date::_CalcMonthAndDayOfMonth (uint year, uint dayOfYear, OUT uint &month, OUT uint &dayOfMonth)
    {
        uint days_in_month = _DaysInMonth( year, 0 );

        for (month = 0; dayOfYear >= days_in_month; ++month)
        {
            dayOfYear       -= days_in_month;
            days_in_month    = _DaysInMonth( year, month );
        }

        dayOfMonth = dayOfYear;
    }

/*
=================================================
    Now
=================================================
*/
    Date  Date::Now () __NE___
    {
        Date    res;

        #if 0 //defined(__cpp_lib_chrono) and (__cpp_lib_chrono >= 201907L)
        {
            //AE_LOGI( std::chrono::get_tzdb().current_zone()->name() );

            const auto                          tp          = std::chrono::utc_clock::now();
            const std::chrono::year_month_day   yy_mm_dd    {Cast< std::chrono::days >(tp)};
            const std::chrono::hh_mm_ss         hh_mm_ss    {tp.time_since_epoch()};
            const std::chrono::weekday          wd          {Cast< std::chrono::days >(tp)};

            res._year       = int{yy_mm_dd.year()};
            res._month      = uint{yy_mm_dd.month()}-1;         // 1..12 convert to 0..11
            res._dayOfWeek  = wd.iso_encoding()-1;
            res._dayOfMonth = uint{yy_mm_dd.day()};
            res._dayOfYear  = _CalcDayOfYear( res._year, res._month, res._dayOfMonth );

            res._hour       = hh_mm_ss.hours().count();
            res._minute     = hh_mm_ss.minutes().count();
            res._second     = hh_mm_ss.seconds().count();
            res._millis     = 0;
        }
        #endif

        #if 1
        {
            std::time_t t   = std::time(0);
            std::tm     now;
            Unused( localtime2( OUT &now, &t ));    // TODO: check error?

            res._year       = now.tm_year + 1900;
            res._month      = now.tm_mon;
            res._dayOfMonth = now.tm_mday - 1;
            res._dayOfWeek  = now.tm_wday;
            res._dayOfYear  = now.tm_yday;
            res._hour       = now.tm_hour;
            res._minute     = now.tm_min;
            res._second     = now.tm_sec;
            res._millis     = 0;
        }
        #endif

        #if 0
        {
            SYSTEMTIME   time = {0};
            ::GetLocalTime( &time );

            res._year       = time.wYear;
            res._month      = time.wMonth-1;                                // 1..12 convert to 0..11
            res._dayOfWeek  = time.wDayOfWeek == 0 ? 6 : time.wDayOfWeek-1; // 0..6, Sun == 0 convert to mon..sun
            res._dayOfMonth = time.wDay-1;                                  // 1..31 convert to 0..30
            res._dayOfYear  = _CalcDayOfYear( res._year, res._month, res._dayOfMonth );

            res._hour       = time.wHour;                                   // 0..23
            res._minute     = time.wMinute;                                 // 0..59
            res._second     = time.wSecond;                                 // 0..59
            res._millis     = time.wMilliseconds;                           // 0..999
        }
        #endif

        //ASSERT( res._dayOfWeek == _CalcDayOfWeek( res._year, res._month, res._dayOfMonth ));
        ASSERT_Eq( res._dayOfYear, _CalcDayOfYear( res._year, res._month, res._dayOfMonth ));

        return res;
    }

} // AE::Base
