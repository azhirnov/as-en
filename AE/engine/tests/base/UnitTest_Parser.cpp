// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  Parser_ToEndOfLine ()
    {
        usize   pos = 7;
        Parser::ToEndOfLine( "1111\n2222\n3333333", pos );
        TEST( pos == 9 );

        pos = 0;
        Parser::ToEndOfLine( "1111", pos );
        TEST( pos == 4 );

        pos = 0;
        Parser::ToEndOfLine( "1111\r\n2222", pos );
        TEST( pos == 4 );

        pos = 0;
        Parser::ToEndOfLine( "1111\r\n", pos );
        TEST( pos == 4 );
    }


    static void  Parser_ToBeginOfLine ()
    {
        usize   pos = 7;
        Parser::ToBeginOfLine( "1111\n2222\n3333333", pos );
        TEST( pos == 5 );

        pos = 6;
        Parser::ToBeginOfLine( "11\r\n222\r\n33", pos );
        TEST( pos == 4 );
    }


    static void  Parser_ToNextLine ()
    {
        usize   pos = 7;
        Parser::ToNextLine( "1111\n2222\n3333333", pos );
        TEST( pos == 10 );

        pos = 7;
        Parser::ToNextLine( "1111\n2222\r\n\r\n333", pos );
        TEST( pos == 11 );
    }


    static void  Parser_ToPrevLine ()
    {
        usize   pos = 7;
        Parser::ToPrevLine( "1111\n2222\n3333333", pos );
        TEST( pos == 4 );
    }


    static void  Parser_Tokenize_1 ()
    {
        Array< StringView > tokens;
        Parser::Tokenize( "11,22,33,44,55", ',', tokens );

        TEST( tokens.size() == 5 );
        TEST( tokens[0] == "11" );
        TEST( tokens[1] == "22" );
        TEST( tokens[2] == "33" );
        TEST( tokens[3] == "44" );
        TEST( tokens[4] == "55" );
    }


    static void  Parser_Tokenize_2 ()
    {
        Array< StringView > tokens;
        Parser::Tokenize( "1111,2,,4,", ',', tokens );

        TEST( tokens.size() == 5 );
        TEST( tokens[0] == "1111" );
        TEST( tokens[1] == "2" );
        TEST( tokens[2] == "" );
        TEST( tokens[3] == "4" );
        TEST( tokens[4] == "" );
    }


    static void  Parser_CStyleDivideString ()
    {
        Array< StringView > tokens;
        Parser::DivideString_CPP( "a=122; _bc+=5/ 7*34\",,\"--->", OUT tokens );

        TEST( tokens.size() == 19 );
        TEST( tokens[0] == "a" );
        TEST( tokens[1] == "=" );
        TEST( tokens[2] == "122" );
        TEST( tokens[3] == ";" );
        TEST( tokens[4] == "_bc" );
        TEST( tokens[5] == "+=" );
        TEST( tokens[6] == "5" );
        TEST( tokens[7] == "/" );
        TEST( tokens[8] == "7" );
        TEST( tokens[9] == "*" );
        TEST( tokens[10] == "34" );
        TEST( tokens[11] == "\"" );
        TEST( tokens[12] == "," );
        TEST( tokens[13] == "," );
        TEST( tokens[14] == "\"" );
        TEST( tokens[15] == "-" );
        TEST( tokens[16] == "-" );
        TEST( tokens[17] == "-" );
        TEST( tokens[18] == ">" );
    }


    static void  Parser_ReadLine ()
    {
        StringView  str = "01234\r\n5678";
        StringView  line;
        usize       pos = 0;

        Parser::ReadLineToEnd( str, pos, line );

        TEST( line == "01234" );
        TEST( pos == 7 );
    }


    static void  Parser_ReadString ()
    {
        usize       pos = 0;
        StringView  result;
        Parser::ReadString( "include \"123456\" ; ", pos, result );

        TEST( pos == 16 );
        TEST( result == "123456" );
    }


    static void  Parser_CalculateNumberOfLines ()
    {
        usize   lines = Parser::CalculateNumberOfLines( "1\n2\n3\r\n4\r\n5\n6\n7\r8\n9\n10" );
        TEST( lines == 10 );

        lines = Parser::CalculateNumberOfLines( "1" );
        TEST( lines == 1 );

        lines = Parser::CalculateNumberOfLines( "1\n2\n" );
        TEST( lines == 3 );

        const char  source[] = "// 11\r\n"
            "#ifdef __INTELLISENSE__\r\n"
            "#  include <res_editor.as>\r\n"
            "#  define SH_RAY_GEN\r\n"
            "#  include <aestyle.glsl.h>\r\n"
            "#  define PRIMARY_MISS\r\n"
            "#  define SHADOW_MISS\r\n"
            "#  define PRIMARY_HIT\r\n"
            "#  define SHADOW_HIT\r\n"
            "#endif\r\n"
            "//-----------------------------------------------------------------------------\r\n"
            "#ifdef SCRIPT\r\n"
            "\r\n"
            "   void ASmain ()\r\n"
            "   {\r\n"
            "   }\r\n"
            "\r\n"
            "#endif";
        lines = Parser::CalculateNumberOfLines( source );
        TEST( lines == 18 );
    }


    static void  Parser_MoveToLine ()
    {
        usize pos = 0;
        Parser::MoveToLine( "1\n2\n3\r\n4\r\n5\n6\n7\r8\n9\n10", OUT pos, 0 );
        TEST( pos == 0 );

        pos = 0;
        Parser::MoveToLine( "1\n2\n3\r\n4\r\n5\n6\n7\r8\n9\n10", OUT pos, 1 );
        TEST( pos == 2 );

        pos = 0;
        Parser::MoveToLine( "1\n2\n3\r\n4\r\n5\n6\n7\r8\n9\n10", OUT pos, 9 );
        TEST( pos == 20 );
    }


    static void  Parser_Preprocessor_CPP ()
    {
        const char  source[] = 
R"(aaaj sdi kas jnd
trurt qwa   fdgh
#ifdef PART0
    rt u3we     rqs
    fdhrftqa qfgtdhjdsf rfdgsdf
#    endif
    dsfgdf
sdf dsh fed

asdasd

#if 0
    as df hgsdfc
        f   sdfsad dfasd asdfas
#endif

ssdgadds

# ifdef PART1_1
    sdfs dfhd sq wfd
    #   ifdef PART2
        rgg qeafa
qfw edgedfg                 
efedhrsd fzsdsfdg       asdasd
"  #  endif PART2 "
sgszf dgd
#endif // PART2
sdfgswdfsa df
adt fg sdfsdf
#   endif // PART1  (PART1_1)

asdasd sdhedtwqfdcdz
        #ifdef PART5
AOILAs,';llkmkljsad
LDKFMSLD;F;'ASL,lksdnmdkjfnslkd
lsdkfmsdlklas akdslmalksd
#           endif // PART5
)";

        const StringView    defines[] = { "PART1", "PART2", "PART5" };
        Array<StringView>   output;

        Parser::Preprocessor_CPP( source, defines, OUT output );

        const char  expected0[] =
R"(        rgg qeafa
qfw edgedfg                 
efedhrsd fzsdsfdg       asdasd
"  #  endif PART2 "
sgszf dgd)";
        const char  expected1[] =
R"(AOILAs,';llkmkljsad
LDKFMSLD;F;'ASL,lksdnmdkjfnslkd
lsdkfmsdlklas akdslmalksd)";

        TEST( output.size() == 2 );
        TEST( output[0] == expected0 );
        TEST( output[1] == expected1 );
    }


    static void  Parser_TabsToSpaces ()
    {
        {
            const StringView    src = R"(
    1       22  333 4   
    1111    22  333 4
)";         const StringView    ref = R"(
    1       22  333 4   
    1111    22  333 4
)";
            String  dst;
            Parser::TabsToSpaces( OUT dst, src, 4 );
            TEST( ref == dst );
        }
    }
}


extern void UnitTest_Parser ()
{
    Parser_ToEndOfLine();
    Parser_ToBeginOfLine();
    Parser_ToNextLine();
    Parser_ToPrevLine();
    Parser_Tokenize_1();
    Parser_Tokenize_2();
    Parser_CStyleDivideString();
    Parser_ReadLine();
    Parser_ReadString();
    Parser_CalculateNumberOfLines();
    Parser_MoveToLine();
    Parser_Preprocessor_CPP();
    Parser_TabsToSpaces();

    TEST_PASSED();
}
