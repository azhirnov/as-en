// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  Console_Test1 ()
    {
        Console     csl;

        {
            Console::CommandName_t  res;
            TEST( csl.ProcessUserInput( "ae.hulang", OUT res ));

            TEST( res.size() == 2 );
            TEST( res[0] == "ae" );
            TEST( res[1] == "hulang" );
        }{
            Console::CommandName_t  res;
            TEST( csl.ProcessUserInput( "ae.hl.", OUT res ));

            TEST( res.size() == 3 );
            TEST( res[0] == "ae" );
            TEST( res[1] == "hl" );
            TEST( res[2] == "" );
        }{
            Console::CommandName_t  res;
            TEST( csl.ProcessUserInput( "ae..hl.", OUT res ));

            TEST( res.size() == 4 );
            TEST( res[0] == "ae" );
            TEST( res[1] == "" );
            TEST( res[2] == "hl" );
            TEST( res[3] == "" );
        }{
            Console::CommandName_t  res;
            TEST( not csl.ProcessUserInput( "ae::hl", OUT res ));
        }{
            Console::CommandName_t  res;
            TEST( not csl.ProcessUserInput( "ae,hl", OUT res ));
        }
    }


    static void  Console_Test2 ()
    {
        Console     csl;
        TEST( csl.Register( "ae.hulang", ConsoleCmdName{"1"} ));
        TEST( csl.IsRegistred( "ae.hulang" ));
        TEST( not csl.IsRegistred( "a.hulang" ));

        TEST( csl.Register( "ae.hulang.11", ConsoleCmdName{"2"} ));
        TEST( csl.IsRegistred( "ae.hulang.11" ));

        TEST( csl.Register( "eA.ui", ConsoleCmdName{"3"} ));
        TEST( csl.IsRegistred( "eA.ui" ));
        TEST( not csl.IsRegistred( "ea.ui" ));
        TEST( not csl.IsRegistred( "ea.Ui" ));

        {
            Console::Suggestion_t   sug;
            TEST( csl.GetSuggestion( "ae.hulang", OUT sug ));

            TEST( sug.size() == 1 );
            TEST( sug.front().name == "ae.hulang" );
            TEST( sug.front().probability == 1.0f );
            TEST( sug.front().command.IsDefined() );
        }{
            Console::Suggestion_t   sug;
            TEST( csl.GetSuggestion( "Ae.hulang", OUT sug ));

            TEST( sug.size() == 1 );
            TEST( sug.front().name == "ae" );
            TEST( sug.front().probability > 0.0f );
            TEST( sug.front().probability < 1.0f );
            TEST( not sug.front().command.IsDefined() );
        }{
            Console::Suggestion_t   sug;
            TEST( csl.GetSuggestion( "ae.hilang", OUT sug ));

            TEST( sug.size() >= 1 );
            TEST( sug.front().name == "ae.hulang" );
            TEST( sug.front().probability > 0.9f );
            TEST( sug.front().command.IsDefined() );
        }
    }
}


extern void UnitTest_Console ()
{
    Console_Test1();
    Console_Test2();

    TEST_PASSED();
}
