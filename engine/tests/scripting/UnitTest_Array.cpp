// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "UnitTest_Common.h"

namespace
{
	static void  ScriptArray_Test1 (const ScriptEnginePtr &se)
	{
		const char	script[] = R"#(
			int ASmain () {
				array<int16>	arr;
				arr.push_back( 1 );
				arr.push_back( 2 );
				arr.insert( /*pos*/1, 3 );
				arr.pop_back();
				return arr[1];
			}
		)#";

		int	res = 0;
		TEST( Run< int() >( se, script, "ASmain", OUT res ));
		TEST( res == 3 );
	}


	static void  ScriptArray_Test2 (const ScriptEnginePtr &se)
	{
		struct Utils {
			static void CheckArray (ScriptArray<int> &arr)
			{
				TEST( arr.size() == 2 );
				TEST( arr[0] == 1 );
				TEST( arr[1] == 2 );

				for (auto& e : arr) {
					Unused( e );
				}
			};
		};

		const char	script[] = R"#(
			void ASmain () {
				array<int>	arr;
				arr.push_back( 1 );
				arr.push_back( 2 );
				CheckArray( arr );
			}
		)#";

		TEST_NOTHROW(
			se->AddFunction( &Utils::CheckArray, "CheckArray" );
		)
		TEST( Run< void() >( se, script, "ASmain" ));
	}


	static void  ScriptArray_Test3 (const ScriptEnginePtr &se)
	{
		struct Utils {
			static void CheckArray (ScriptArray<String> &arr)
			{
				TEST( arr.size() == 2 );
				TEST( arr[0] == "1" );
				TEST( arr[1] == "2" );
				arr.push_back( "3" );
				
				TEST( arr.size() == 3 );
				TEST( arr[0] == "1" );
				TEST( arr[1] == "2" );
				TEST( arr[2] == "3" );

				for (auto& e : arr) {
					Unused( e );
				}
			};
		};

		const char	script[] = R"#(
			int ASmain () {
				array<string>	arr;
				arr.push_back( "1" );
				arr.push_back( "2" );
				CheckArray( arr );
				if ( arr[2] != "3" )
					return 0;
				return arr.size();
			}
		)#";
		
		TEST_NOTHROW(
			se->AddFunction( &Utils::CheckArray, "CheckArray" );
		)

		int	res = 0;
		TEST( Run< int() >( se, script, "ASmain", OUT res ));
		TEST( res == 3 );
	}
}


extern void UnitTest_Array ()
{
	TEST_NOTHROW(
		auto	se = MakeRC<ScriptEngine>();
		TEST( se->Create() );

		CoreBindings::BindArray( se );
		CoreBindings::BindString( se );

		ScriptArray_Test1( se );
		ScriptArray_Test2( se );
		ScriptArray_Test3( se );

		TEST_PASSED();
	)
}
