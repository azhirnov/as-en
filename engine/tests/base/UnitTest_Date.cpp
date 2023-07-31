// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"


extern void UnitTest_Date ()
{
    STATIC_ASSERT( sizeof(Date) == sizeof(ulong) );

    Date    d = Date::Now();

    Date    d0 = d;
    TEST( d0 == d );

    ThreadUtils::Sleep( milliseconds{1000} );
    d0 = Date::Now();

    TEST( d0 != d  );
    TEST( d0 >  d  );
    TEST( d  <  d0 );

    d0.SetMillisecondsSince1970( d.ToMillisecondsSince1970() );
    TEST( d0 == d );

    const char format1[] = "yyyy/mm.dm - hh:mi:ss ms";
    const char format2[] = "yy//mm/dm (dee), dww|mmm  dwww -> mmmm";

    AE_LOGI( "Date with format \""s << format1 << "\":\n" << d.ToString( format1 ));
    AE_LOGI( "Date with format \""s << format2 << "\":\n" << d.ToString( format2 ));

    TEST_PASSED();
}
