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

				usize i = 0;
				for (auto& e : arr) {
					switch ( i ) {
						case 0 :	TEST( e == 1 );	break;
						case 1 :	TEST( e == 2 );	break;
					}
					++i;
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
				
				usize i = 0;
				for (auto& e : arr) {
					switch ( i ) {
						case 0 :	TEST( e == "1" );	break;
						case 1 :	TEST( e == "2" );	break;
						case 2 :	TEST( e == "3" );	break;
					}
					++i;
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

	
	static void  ScriptArray_Test4 (const ScriptEnginePtr &se)
	{
		struct Utils {
			static void CheckArray (ScriptArray<packed_int4> &arr)
			{
				TEST( arr.size() == 2 );
				TEST( All( arr[0] == packed_int4( 0, 1, 2, 3 ) ));
				TEST( All( arr[1] == packed_int4( 10, 11, 22, 33 ) ));
				
				usize i = 0;
				for (auto& e : arr) {
					switch ( i ) {
						case 0 :	TEST( All( e == packed_int4( 0, 1, 2, 3 ) ));		break;
						case 1 :	TEST( All( e == packed_int4( 10, 11, 22, 33 ) ));	break;
					}
					++i;
				}
			};
		};

		const char	script[] = R"#(
			void ASmain () {
				array<int4>	arr;
				arr.push_back( int4( 0, 1, 2, 3 ));
				arr.push_back( int4( 10, 11, 22, 33 ));
				CheckArray( arr );
			}
		)#";
		TEST_NOTHROW(
			se->AddFunction( &Utils::CheckArray, "CheckArray" );
		)
		TEST( Run< void() >( se, script, "ASmain" ));
	}


	static void  ScriptArray_Test5 (const ScriptEnginePtr &se)
	{
		struct Utils {
			static void CheckArray (const ScriptArray<packed_float2x2> &arr)
			{
				TEST( arr.size() == 2 );
				TEST( All( arr[0][0] == packed_float2{1.1f, 2.2f} ));
				TEST( All( arr[0][1] == packed_float2{3.3f, 4.4f} ));

				TEST( All( arr[1][0] == packed_float2{5.5f, 6.6f} ));
				TEST( All( arr[1][1] == packed_float2{7.7f, 8.8f} ));
				
				usize i = 0;
				for (auto& e : arr) {
					switch ( i ) {
						case 0 :
							TEST( All( e[0] == packed_float2{1.1f, 2.2f} ));
							TEST( All( e[1] == packed_float2{3.3f, 4.4f} ));
							break;
						case 1 :
							TEST( All( e[0] == packed_float2{5.5f, 6.6f} ));
							TEST( All( e[1] == packed_float2{7.7f, 8.8f} ));
							break;
					}
					++i;
				}
			};
		};

		const char	script[] = R"#(
			void ASmain () {
				array<float2x2>	arr;
				arr.push_back( float2x2( float2(1.1, 2.2), float2(3.3, 4.4) ));
				arr.push_back( float2x2( float2(5.5, 6.6), float2(7.7, 8.8) ));
				CheckArray( arr );
			}
		)#";
		TEST_NOTHROW(
			se->AddFunction( &Utils::CheckArray, "CheckArray" );
		)
		TEST( Run< void() >( se, script, "ASmain" ));
	}
}


extern void UnitTest_Array ()
{
	TEST_NOTHROW(
		auto	se = MakeRC<ScriptEngine>();
		TEST( se->Create() );

		CoreBindings::BindArray( se );
		CoreBindings::BindString( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindMatrixMath( se );

		ScriptArray_Test1( se );
		ScriptArray_Test2( se );
		ScriptArray_Test3( se );
		ScriptArray_Test4( se );
		ScriptArray_Test5( se );

		TEST_PASSED();
	)
}
