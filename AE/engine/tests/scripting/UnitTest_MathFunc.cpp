// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  ScriptMath_Test1 (const ScriptEnginePtr &se)
	{
		int script_res;
		bool res = Run< int() >( se, R"#(
			int ASmain () {
				return Max( 2, -4 );
			})#", "ASmain", OUT script_res );
		TEST( res );
		TEST_Eq( script_res, 2 );
	}


	static void  ScriptMath_Test2 (const ScriptEnginePtr &se)
	{
		int script_res;
		bool res = Run< int() >( se, R"#(
			int ASmain () {
				const uint2 a( 1, 2 );
				const uint2 b = a + 4;
				const uint2 c = b & 1;  // failed on Android & Linux
				const uint2 d = c + a;
				return d.x != 2 ? 1 : d.y != 2 ? 2 : 0;
			})#", "ASmain", OUT script_res );
		TEST( res );
		TEST_Eq( script_res, 0 );
	}


	static void  ScriptMath_Test3 (const ScriptEnginePtr &se)
	{
		int script_res;
		bool res = Run< int() >( se, R"#(
			int ASmain () {
				const uint2  a( 1, 2 );
				const float2 b = float2(a);
				return (b.x == 1.0f && b.y == 2.0f) ? 1 : 0;
			})#", "ASmain", OUT script_res );
		TEST( res );
		TEST_Eq( script_res, 1 );
	}


	static void  ScriptPhisicalMath_Test1 (const ScriptEnginePtr &se)
	{
		float script_res;
		bool res = Run< float() >( se, R"#(
			float ASmain () {
				Second	s = 1.1;
				Meter	m = 2.0;
				MeterPerSecond	ms = m / s;
				return ms.GetScaled();
			})#", "ASmain", OUT script_res );
		TEST( res );
		TEST( Equal( script_res, (2.f / 1.1f) ));
	}
}


extern void UnitTest_MathFunc ()
{
	auto	se = MakeRC<ScriptEngine>();
	TEST( se->Create() );

	TEST_NOTHROW(
		CoreBindings::BindScalarMath( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindColor( se );
	)

	ScriptMath_Test1( se );
  #ifndef AE_PLATFORM_UNIX_BASED
	ScriptMath_Test2( se );
  #endif
	ScriptMath_Test3( se );

	TEST_NOTHROW(
		CoreBindings::BindPhysicalTypes( se );
	)

  #ifndef AE_PLATFORM_UNIX_BASED
	ScriptPhisicalMath_Test1( se );
  #endif

	TEST_PASSED();
}
