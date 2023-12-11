// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  Date_Test1 ()
    {
        StaticAssert( sizeof(Date) == sizeof(ulong) );

        Date    d = Date::Now();

        Date    d0 = d;
        TEST( d0 == d );

        ThreadUtils::MilliSleep( milliseconds{1000} );
        d0 = Date::Now();

        TEST( d0 != d  );
        TEST( d0 >  d  );
        TEST( d  <  d0 );

        d0.SetMillisecondsSince1970( d.ToMillisecondsSince1970() );
        TEST( d0 == d );
        TEST( d0.Month() == d.Month() );
        TEST_Eq( d0.DayOfMonth(), d.DayOfMonth() );
        TEST_Eq( d0.WeekOfYear(), d.WeekOfYear() );
        TEST_Eq( d0.DayOfYear(), d.DayOfYear() );
        TEST( d0.DayOfWeek() == d.DayOfWeek() );
    }


    static void  Date_Test2 ()
    {
        const char      format1[]   = "yyyy/mm.dm - hh:mi:ss ms";
        const char      format2[]   = "yy//mm/dm (dee), dww|mmm  dwww -> mmmm";

        const ulong     min_ms      = Date::MillisInMinute();
        const ulong     hour_ms     = Date::MillisInHour();
        const ulong     day_ms      = Date::MillisInDay();
        const ulong     y2024_ms    = ulong(365*54 + 13) * day_ms;

        {
            Date    d;
            d.SetMillisecondsSince1970( y2024_ms );

            TEST_Eq( "2024/01.01 - 00:00:00 000", d.ToString( format1 ));
            TEST_Eq( "24//01/01 (001), Mon|Jan  Monday -> January", d.ToString( format2 ));
        }{
            Date    d;
            d.SetMillisecondsSince1970( y2024_ms + 2*day_ms + 12*hour_ms + 59*min_ms + 22'000 + 999 );

            TEST_Eq( "2024/01.03 - 12:59:22 999", d.ToString( format1 ));
            TEST_Eq( "24//01/03 (003), Wed|Jan  Wednesday -> January", d.ToString( format2 ));
        }
    }


    static void  Date_Test3 ()
    {
        const ulong     y2024_ms = ulong(365*54 + 13) * Date::MillisInDay();
        Date            d;  d.SetMillisecondsSince1970( y2024_ms );

        TEST( d.Year() == 2024 );
        TEST( d.Month() == Date::EMonth::Jan );
        TEST( d.DayOfMonth() == 1 );
        TEST( d.DayOfYear() == 1 );
        TEST( d.DayOfWeek() == Date::EWeekDay::Mon );
        TEST( d.IsLeapYear() );

        Date::Builder   b {d};

        for (uint dw = 1, dy = 1;; ++dw, ++dy)
        {
            if ( dw >= uint(Date::EWeekDay::_Count) )
                dw = uint(Date::EWeekDay::Mon);

            if ( dy >= b->DaysInYear() )
                break;

            b.DayOfYear( dy );
            b.CalcMonthAndDayOfMonth();
            b.CalcDayOfWeek();

            TEST( b->DayOfWeek() == Date::EWeekDay(dw) );
        }
    }
}


extern void UnitTest_Date ()
{
    Date_Test1();
    Date_Test2();
    Date_Test3();

    TEST_PASSED();
}
