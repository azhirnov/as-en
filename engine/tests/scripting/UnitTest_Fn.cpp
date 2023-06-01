// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "UnitTest_Common.h"

namespace
{
	static void  GenericFn1 (ScriptArgList args)
	{
		if ( args.Is< int (int, float) >())
		{
			TEST( args.Arg<int>(0) == 2 );
			TEST( args.Arg<float>(1) == 2.22f );
			args.Return( 1 );
		}
		else
		if ( args.Is< int (double, ulong) >())
		{
			TEST( args.Arg<double>(0) == 1.11 );
			TEST( args.Arg<ulong>(1) == 1 );
			args.Return( 10 );
		}
		else
			throw AE::Exception{""};
	}

	static void  ScriptFn_Test1 (const ScriptEnginePtr &se)
	{
		const char	script[] = R"#(
			int ASmain () {
				return	gen( double(1.11), ulong(1) ) +
						gen( sint(2), float(2.22) );
			}
		)#";

		se->AddGenericFn< int (int, float) >( &GenericFn1, "gen" );
		se->AddGenericFn< int (double, ulong) >( &GenericFn1, "gen" );

		int	res = 0;
		TEST( Run< int() >( se, script, "ASmain", OUT res ));
		TEST( res == 11 );
	}

	
	static void  GenericFn2 (ScriptArgList args)
	{
		if ( args.Is< int (const String &) >())
		{
			TEST_Eq( args.Arg<const String &>(0), "qwerty" );
			args.Return( 11 );
		}
		else
		if ( args.Is< String () >())
		{
			args.Return( String{"1234"} );
		}
		else
			throw AE::Exception{""};
	}

	static void  ScriptFn_Test2 (const ScriptEnginePtr &se)
	{
		const char	script[] = R"#(
			int ASmain () {
				return	gen( "qwerty" ) +
						int(gen() == "1234" ? 1 : 2);
			}
		)#";

		se->AddGenericFn< int (const String &) >( &GenericFn2, "gen" );
		se->AddGenericFn< String () >( &GenericFn2, "gen" );

		int	res = 0;
		TEST( Run< int() >( se, script, "ASmain", OUT res ));
		TEST( res == 12 );
	}
}


extern void UnitTest_Fn ()
{
	TEST_NOTHROW(
		auto	se = MakeRC<ScriptEngine>();
		TEST( se->Create() );

		CoreBindings::BindStdTypes( se );
		ScriptFn_Test1( se );
		
		CoreBindings::BindString( se );
		ScriptFn_Test2( se );

		TEST_PASSED();
	)
}
