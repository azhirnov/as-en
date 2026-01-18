// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static constexpr int	c_Fn1Line = __LINE__;
	static SourceLoc  SomeFunction (const char* = null, int = 0)
	{
		return SourceLoc::current();
	}

	static void  SourceLoc_Test1 ()
	{
		SourceLoc   loc = SomeFunction();

		TEST( Path{loc.FileName()}.filename() == "UnitTest_SourceLoc.cpp" );
		#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
			TEST_Eq( loc.FunctionSignature(), "SourceLoc (anonymous namespace)::SomeFunction(const char *, int)" );

		#elif defined(AE_COMPILER_CLANG_CL)
			TEST_Eq( loc.FunctionSignature(), "SourceLoc __cdecl (anonymous namespace)::SomeFunction(const char *, int)" );

		#elif defined(AE_COMPILER_MSVC)
			TEST_Eq( loc.FunctionSignature(), "struct AE::Base::SourceLoc __cdecl `anonymous-namespace'::SomeFunction(const char *,int)" );
		#else
		#	error not implemented!
		#endif
		TEST_Eq( loc.FunctionName(), "SomeFunction" );
		TEST_Eq( loc.Line(), c_Fn1Line+3 );
	}


	static constexpr int	c_Fn2Line = __LINE__;
	template <typename T>
	static SourceLoc  SomeTemplateFn (T t = {})
	{
		Unused( t );
		return SourceLoc::current();
	}

	static void  SourceLoc_Test2 ()
	{
		SourceLoc   loc = SomeTemplateFn<int>();

		TEST( Path{loc.FileName()}.filename() == "UnitTest_SourceLoc.cpp" );
		#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
			TEST_Eq( loc.FunctionSignature(), "SourceLoc (anonymous namespace)::SomeTemplateFn(T) [T = int]" );

		#elif defined(AE_COMPILER_CLANG_CL)
			TEST_Eq( loc.FunctionSignature(), "SourceLoc __cdecl (anonymous namespace)::SomeTemplateFn(T) [T = int]" );

		#elif defined(AE_COMPILER_MSVC)
			TEST_Eq( loc.FunctionSignature(), "struct AE::Base::SourceLoc __cdecl `anonymous-namespace'::SomeTemplateFn<int>(int)" );
		#else
		#	error not implemented!
		#endif
		TEST_Eq( loc.FunctionName(), "SomeTemplateFn" );
		TEST_Eq( loc.Line(), c_Fn2Line+5 );
	}


	static void  SourceLoc_Test3 ()
	{
		// MSVC style signature
		{
			StringView	fn_sig  = "struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl> __cdecl AE::Graphics::CommandBatch::SubmitAsTask<struct AE::Base::ArrayView<struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl>,unsigned __int64>>(const struct AE::Base::Tuple<struct AE::Base::ArrayView<struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl>,unsigned __int64> > &) noexcept";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SubmitAsTask" );
		}{
			StringView	fn_sig  = "struct AE::Base::SourceLoc __cdecl `anonymous-namespace'::SomeFunction(const char *,int)";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SomeFunction" );
		}

		// MSVC+Clang style signature
		{
			StringView	fn_sig  = "SourceLoc __cdecl (anonymous namespace)::SomeFunction(const char *, int)";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SomeFunction" );
		}
	}
}

extern void UnitTest_SourceLoc ()
{
	SourceLoc_Test1();
	SourceLoc_Test2();

	TEST_PASSED();
}
