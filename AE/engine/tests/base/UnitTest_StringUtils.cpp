// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  StringUtils_FindChar ()
    {
        String  s  = "dfkfmdasikf1nqaodms;alc,m";
        usize   a0 = FindChar( s.data(), 0, s.size(), '1' );    TEST_Eq( a0, 11 );
        usize   a1 = FindChar( s.data(), 5, s.size(), '1' );    TEST_Eq( a1, 11 );
    }


    static void  StringUtils_IsNullTerminated ()
    {
        const char  s [] = "iwuwei dksam\0sdfsdfsdf";
        StringView  a0 { s, CountOf(s)-2 };
        StringView  a1 { s, 12 };
        TEST( not IsNullTerminated( a0 ));
        TEST( IsNullTerminated( a1 ));
    }


    static void  StringUtils_EqualIC ()
    {
        const char  a0 [] = "11aabbcc";
        const char  a1 [] = "11AaBBcC";

        TEST( EqualIC( StringView{a0}, StringView{a1} ));
        TEST( not EqualIC( StringView{a0}.substr(1), StringView{a1} ));
        TEST( not EqualIC( StringView{a0}.substr(0, 7), StringView{a1} ));
    }


    static void  StringUtils_FindString ()
    {
        const char  a0 [] = "oqkkmxaskjndqkas123-567dn;q'smfadjksajdnalkmsa";
        const char  a1 [] = "123-567";
        const char  a2 [] = "9123-";

        usize   b0 = FindString( a0, a1, 0 );       TEST_Eq( b0, 16 );
        usize   b1 = FindString( a0, a2, 0 );       TEST_Eq( b1, 46 );

        TEST( HasSubString( a0, a1 ));
        TEST( not HasSubString( a0, a2 ));
    }


    static void  StringUtils_FindStringIC ()
    {
        const char  a0 [] = "oqkkmxaskjndqkas-YbFSPD-dn;q'smfadjksajdnalkmsa";
        const char  a1 [] = "yBfsPD";
        const char  a2 [] = "-YbbFS";
        const char  a3 [] = "YbFSPD";

        usize   b0 = FindStringIC( a0, a1, 0 );     TEST_Eq( b0, 17 );
        usize   b1 = FindStringIC( a0, a2, 0 );     TEST_Eq( b1, 47 );
        usize   b2 = FindStringIC( a0, a3, 0 );     TEST_Eq( b2, 17 );

        TEST( HasSubStringIC( a0, a1 ));
        TEST( not HasSubStringIC( a0, a2 ));
        TEST( HasSubStringIC( a0, a3 ));
    }


    static void  StringUtils_StartsWith ()
    {
        const char  a0 [] = "12AbcD567-dlfsdmfls";
        const char  a1 [] = "12AbcD567";
        const char  a2 [] = "12aBCD567";
        const char  a3 [] = "23A";

        TEST( StartsWith( a0, a1 ));
        TEST( not StartsWith( a0, a2 ));
        TEST( not StartsWith( a0, a3 ));

        TEST( StartsWithIC( a0, a1 ));
        TEST( StartsWithIC( a0, a2 ));
        TEST( not StartsWithIC( a0, a3 ));
    }


    static void  StringUtils_EndsWith ()
    {
        const char  a0 [] = "dlfsdmfls-12AbcD567";
        const char  a1 [] = "12AbcD567";
        const char  a2 [] = "12aBCD567";
        const char  a3 [] = "5677";

        TEST( EndsWith( a0, a1 ));
        TEST( not EndsWith( a0, a2 ));
        TEST( not EndsWith( a0, a3 ));

        TEST( EndsWithIC( a0, a1 ));
        TEST( EndsWithIC( a0, a2 ));
        TEST( not EndsWithIC( a0, a3 ));
    }
}


extern void UnitTest_StringUtils ()
{
    StringUtils_FindChar();
    StringUtils_IsNullTerminated();
    StringUtils_EqualIC();
    StringUtils_FindString();
    StringUtils_FindStringIC();
    StringUtils_StartsWith();
    StringUtils_EndsWith();

    TEST_PASSED();
}
