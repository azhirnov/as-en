// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  Preprocessor_Test1 ()
	{
		const char	source[] =
R"(aaaj sdi	kas jnd
trurt qwa	fdgh
#ifdef PART0
	rt u3we		rqs
	fdhrftqa qfgtdhjdsf	rfdgsdf
#    endif
	dsfgdf
sdf dsh fed

asdasd

#if 0
	as df hgsdfc
		f	sdfsad dfasd asdfas
#endif

ssdgadds

# ifdef PART1_1
	sdfs dfhd sq wfd
	#	ifdef PART2
		rgg qeafa
qfw edgedfg
efedhrsd fzsdsfdg		asdasd
"  #  endif PART2 "
sgszf dgd
#endif // PART2
sdfgswdfsa df
adt fg sdfsdf
#	endif // PART1	(PART1_1)

asdasd sdhedtwqfdcdz
		#ifdef PART5
AOILAs,';llkmkljsad
LDKFMSLD;F;'ASL,lksdnmdkjfnslkd
#	if 1
lsdkfmsdlklas akdslmalksd
346iergfd
  #  endif
#			endif // PART5
)";

		const StringView	defines[] = { "PART1", "PART2", "PART5" };
		String				src;

		TEST( ScriptEngine::_Preprocessor2( source, OUT src, defines, Default ));

		const char	expected[] =
R"(aaaj sdi	kas jnd
trurt qwa	fdgh




	dsfgdf
sdf dsh fed

asdasd






ssdgadds














asdasd sdhedtwqfdcdz

AOILAs,';llkmkljsad
LDKFMSLD;F;'ASL,lksdnmdkjfnslkd

lsdkfmsdlklas akdslmalksd
346iergfd


)";

		TEST( expected == src );
	}
	//-----------------------------------------------------


	static void  Preprocessor_Test2 ()
	{
		const char	source[] = R"(
#include <preprocessor>
#include <parser>
sdafjezs zsgesh DFsdzfg
ewgryjkcs eDSfsehyu rdscszdfve
#if PART_5
	fghjdfg
# endif
	// 5yhs

11
)";
		String	src;
		TEST( ScriptEngine::_Preprocessor2( source, OUT src, Default, Default ));

		const char	expected[] = R"(


sdafjezs zsgesh DFsdzfg
ewgryjkcs eDSfsehyu rdscszdfve



	// 5yhs

11
)";
		TEST( expected == src );
	}
	//-----------------------------------------------------


	static void  Preprocessor_Test3 ()
	{
		const char	source[] = R"(
RC<Class> cl = Class();

cl.Method( EType::Value );
)";
		String	src;
		TEST( ScriptEngine::_Preprocessor2( source, OUT src, Default, Default ));

		const char	expected[] = R"(
Class@ cl = Class();

cl.Method( EType_Value );
)";
		TEST( expected == src );
	}
	//-----------------------------------------------------


	static void  Preprocessor_Test4 ()
	{
		const char	source[] = R"(
dfrkdtywb
#if 0
1111
22222
#  if 1
	3333
	4 4 4
#endif
5  5
 6 6
	#endif
7777
888
)";
		String	src;
		TEST( ScriptEngine::_Preprocessor2( source, OUT src, Default, Default ));

		const char	expected[] = R"(
dfrkdtywb










7777
888
)";
		TEST( expected == src );
	}
	//-----------------------------------------------------


	static void  Preprocessor_Test5 ()
	{
		const char	source[] = "// 11\r\n"
"#ifdef __INTELLISENSE__\r\n"
"# 	include <res_editor.as>\r\n"
"#	define SH_RAY_GEN\r\n"
"#	include <aestyle.glsl.h>\r\n"
"#	define PRIMARY_MISS\r\n"
"#	define SHADOW_MISS\r\n"
"#	define PRIMARY_HIT\r\n"
"#	define SHADOW_HIT\r\n"
"#endif\r\n"
"//-----------------------------------------------------------------------------\r\n"
"#ifdef SCRIPT\r\n"
"\r\n"
"	void ASmain ()\r\n"
"	{\r\n"
"	}\r\n"
"\r\n"
"#endif\r\n";
		String	src;
		TEST( ScriptEngine::_Preprocessor2( source, OUT src, ArrayView<StringView>{"SCRIPT"}, Default ));

		FindAndReplace( INOUT src, "\r\n", "\n" );

		const char	expected[] = R"(// 11









//-----------------------------------------------------------------------------


	void ASmain ()
	{
	}


)";
		TEST( expected == src );
	}
	//-----------------------------------------------------


	static void  Preprocessor_Test6 ()
	{
		const char	source[] = R"(
"str1"
#ifdef SCRIPT
	script src1
	""
	script src2
	"1"
	script src3
	"12"
	script src3
	"123"
	script src4
	""""
	script src5
	"\n\r\n\n"
	script src6
#endif

#ifdef SHADER
	shader src
#endif

end)";
		String	src;
		TEST( ScriptEngine::_Preprocessor2( source, OUT src, ArrayView<StringView>{"SCRIPT"}, Default ));

		const char	expected[] = R"(
"str1"

	script src1
	""
	script src2
	"1"
	script src3
	"12"
	script src3
	"123"
	script src4
	""""
	script src5
	"\n\r\n\n"
	script src6






end)";

		TEST( expected == src );
	}
	//-----------------------------------------------------
}


extern void UnitTest_Preprocessor ()
{
	Preprocessor_Test1();
	Preprocessor_Test2();
	Preprocessor_Test3();
	Preprocessor_Test4();
	Preprocessor_Test5();
	Preprocessor_Test6();

	TEST_PASSED();
}
