// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  ScriptException_Test1 (const ScriptEnginePtr &se)
	{
		static const int	 line		= __LINE__ + 1;
		static const char	script[]	= R"#(
			bool ASmain () {
				LogFatal( "aaaa" );
				return true;
			}
		)#";

		bool	res = false;
		TEST( Run< bool() >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ) == false );
		TEST_Eq( res, false );
	}
}


extern void UnitTest_Exceptions ()
{
#if defined(AE_PLATFORM_ANDROID) and defined(AE_CPU_ARCH_ARM32)
	// crash
#else
	TEST_NOTHROW(
		auto	se = MakeRC<ScriptEngine>();
		TEST( se->Create() );

		CoreBindings::BindString( se );
		CoreBindings::BindLog( se );

		ScriptException_Test1( se );

		TEST_PASSED();
	)
#endif
}
