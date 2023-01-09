// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
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

		TEST( ScriptEngine::_Preprocessor( source, OUT src, defines ));

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
		TEST( ScriptEngine::_Preprocessor( source, OUT src, Default ));

		const char	expected[] = R"(


sdafjezs zsgesh DFsdzfg
ewgryjkcs eDSfsehyu rdscszdfve



	// 5yhs

11
)";
		TEST( expected == src );
	}

	
	static void  Preprocessor_Test3 ()
	{
		const char	source[] = R"(
RC<Class> cl = Class();

cl.Method( EType::Value );
)";
		String	src;
		TEST( ScriptEngine::_Preprocessor( source, OUT src, Default ));

		const char	expected[] = R"(
Class@ cl = Class();

cl.Method( EType_Value );
)";
		TEST( expected == src );
	}

	
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
		TEST( ScriptEngine::_Preprocessor( source, OUT src, Default ));

		const char	expected[] = R"(
dfrkdtywb










7777
888
)";
		TEST( expected == src );
	}
}


extern void UnitTest_Preprocessor ()
{
	Preprocessor_Test2();
	Preprocessor_Test3();
	Preprocessor_Test4();
	Preprocessor_Test1();

	TEST_PASSED();
}
