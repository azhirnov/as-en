// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "UnitTest_Common.h"


namespace
{
	static void  ScriptMath_Test1 (const ScriptEnginePtr &se)
	{
		int script_res;
		bool res = Run< int() >( se, R"#(
			int main() {
				return Max( 2, -4 );
			})#", "main", OUT script_res );
		TEST( res );
		TEST( script_res == 2 );
	}


	static void  ScriptMath_Test2 (const ScriptEnginePtr &se)
	{
		int script_res;
		bool res = Run< int() >( se, R"#(
			int main() {
				uint2 a( 1, 2 );
				uint2 b = a + 4;
				uint2 c = b & 1;
				return c.x == 1 ? 1 : c.y == 1 ? 2 : 0;
			})#", "main", OUT script_res );
		TEST( res );
		TEST( script_res == 1 );
	}
}


extern void UnitTest_MathFunc ()
{
	auto	se = MakeRC<ScriptEngine>();
	TEST( se->Create() );

	CoreBindings::BindScalarMath( se );
	CoreBindings::BindVectorMath( se );
	CoreBindings::BindColor( se );

	ScriptMath_Test1( se );
	ScriptMath_Test2( se );

	TEST_PASSED();
}
