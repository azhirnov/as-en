// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  SourceLoc_Test1 ()
	{
		{
			StringView	fn_sig  = "";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "" );
		}{
			StringView	fn_sig  = "fn(";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "fn" );
		}{
			StringView	fn_sig  = "fn(int)";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "fn" );
		}{
			StringView	fn_sig  = "fn<11>(";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "fn" );
		}{
			StringView	fn_sig  = "11>(";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "" );
		}{
			StringView	fn_sig  = "ns fn<11>(";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "fn" );
		}{
			StringView	fn_sig  = "void AA::fn<11>(";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "fn" );
		}{
			StringView	fn_sig  = "void AA::operator ()<11>(int) const";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "operator ()" );
		}{
			StringView	fn_sig  = "void AA::operator +<11>(int, int) const";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "operator +" );
		}

		// MSVC style signature
		{
			StringView	fn_sig  = "struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl> __cdecl AE::Graphics::CommandBatch::SubmitAsTask<struct AE::Base::ArrayView<struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl>,unsigned __int64>>(const struct AE::Base::Tuple<struct AE::Base::ArrayView<struct AE::Base::RC<class AE::_Coro_::AsyncTaskImpl>,unsigned __int64> > &) noexcept";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SubmitAsTask" );
		}{
			StringView	fn_sig  = "struct AE::Base::SourceLoc __cdecl `anonymous-namespace'::SomeFunction(const char *,int)";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SomeFunction" );
		}{
			StringView	fn_sig  = "class AE::_Coro_::ScheduledInlineCoro<1> __cdecl `anonymous-namespace'::Db6_CopyTask::<lambda_1>::operator ()<class AE::_Coro_::BaseCoro<class AE::_Coro_::AsyncPromiseImpl<struct AE::Graphics::ImageMemView> >,class AE::_Coro_::BaseCoro<class AE::_Coro_::AsyncPromiseImpl<class std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > > >,struct `anonymous-namespace'::Db6_TestData>(class AE::_Coro_::BaseCoro<class AE::_Coro_::AsyncPromiseImpl<struct AE::Graphics::ImageMemView> >,class AE::_Coro_::BaseCoro<class AE::_Coro_::AsyncPromiseImpl<class std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > > >,struct `anonymous-namespace'::Db6_TestData &) const";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "operator ()" );
		}

		// MSVC+Clang style signature
		{
			StringView	fn_sig  = "SourceLoc __cdecl (anonymous namespace)::SomeFunction(const char *, int)";
			StringView	fn_name = SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "SomeFunction" );
		}
	}


	static constexpr int	c_Fn1Line = __LINE__;
	static SourceLoc  SomeFunction (const char* = null, int = 0)
	{
		return SourceLoc::current();
	}

	static void  SourceLoc_Test2 ()
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

	static void  SourceLoc_Test3 ()
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
}

extern void UnitTest_SourceLoc ()
{
	SourceLoc_Test1();
	SourceLoc_Test2();
	SourceLoc_Test3();

	TEST_PASSED();
}
