// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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
		}{
			StringView	fn_sig	= "void AA::Callback(void (__cdecl *)(int), float)";
			StringView	fn_name	= SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "Callback" );
		}{
			StringView	fn_sig	= "void AA::Callback(void (__cdecl *)(int), float, int (__cdecl *)())";
			StringView	fn_name	= SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "Callback" );
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
		}{
			StringView	fn_sig	= "class AE::_Coro_::ScheduledInlinePromise<class std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > >,1> __cdecl AE::Graphics::ShaderDebugger::_Parse(class AE::_Coro_::BaseCoro<class AE::_Coro_::AsyncPromiseImpl<struct AE::Base::ArrayView<unsigned char,unsigned __int64> > >,struct AE::Threading::CoSafe<void const *>,bool (__cdecl *)(const void *,const void *,struct AE::Base::TByte<unsigned __int64>,enum AE::Graphics::ShaderDebugger::ELogFormat,class std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > > > &) noexcept,enum AE::Graphics::ShaderDebugger::ELogFormat)";
			StringView	fn_name	= SourceLoc::_ExtractFnName( fn_sig );
			TEST_Eq( fn_name, "_Parse" );
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
		#if defined(AE_COMPILER_GCC)
			TEST_Eq( loc.FunctionSignature(), "AE::Base::SourceLoc {anonymous}::SomeFunction(const char*, int)" );

		#elif defined(AE_COMPILER_CLANG)
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
		#if defined(AE_COMPILER_GCC)
			TEST_Eq( loc.FunctionSignature(), "AE::Base::SourceLoc {anonymous}::SomeTemplateFn(T) [with T = int]" );

		#elif defined(AE_COMPILER_CLANG)
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


	static constexpr int	c_Fn3Line = __LINE__;
	template <typename T>
	static SourceLoc  SomeCbFn (int (*)(float), int, void (T::*)(int) const)
	{
		return SourceLoc::current();
	}

	static void  SourceLoc_Test4 ()
	{
		struct A {};
		SourceLoc   loc = SomeCbFn<A>(null, 0, null);

		TEST( Path{loc.FileName()}.filename() == "UnitTest_SourceLoc.cpp" );
		#if defined(AE_COMPILER_GCC)
			TEST_Eq( loc.FunctionSignature(), "" );

		#elif defined(AE_COMPILER_CLANG)
			TEST_Eq( loc.FunctionSignature(), "" );

		#elif defined(AE_COMPILER_CLANG_CL)
			TEST_Eq( loc.FunctionSignature(), "" );

		#elif defined(AE_COMPILER_MSVC)
			TEST_Eq( loc.FunctionSignature(), "struct AE::Base::SourceLoc __cdecl `anonymous-namespace'::SomeCbFn<struct `anonymous-namespace'::SourceLoc_Test4::A>(int (__cdecl *)(float),int,void (__cdecl `anonymous-namespace'::SourceLoc_Test4::A::* )(int) const)" );
		#else
		#	error not implemented!
		#endif

		TEST_Eq( loc.FunctionName(), "SomeCbFn" );
		TEST_Eq( loc.Line(), c_Fn3Line+4 );
	}


	static void  EnumToString_Test1 ()
	{
		enum class ETest
		{
			T0,
			T1,
			_Count
		};

		StaticAssert( EnumToString<ETest>::ToString(ETest::T0) == "T0" );
		StaticAssert( EnumToString<ETest>::ToString(ETest::T1) == "T1" );
		StaticAssert( EnumToString<ETest>::ToString(ETest::_Count) == Default );
		StaticAssert( EnumToString<ETest>::ToString(ETest(10)) == Default );
	}


	static void  EnumToString_Test2 ()
	{
		enum class ETestBits
		{
			Bit0		= 1 << 0,
			Bit1		= 1 << 1,
			Bit2		= 1 << 2,
			_Last,
			_BITOPS_	= 0,
			Bits123		= Bit0 | Bit1 | Bit2,
		};

		constexpr ETestBits	All = CT_AllBitMask<ETestBits>;

		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits::Bit0 ) == "Bit0" );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits::Bit1 ) == "Bit1" );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits::Bit2 ) == "Bit2" );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits::Bits123 ) == Default );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits::_Last ) == Default );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( ETestBits(1<<3) ) == Default );
		StaticAssert( BitEnumToString<ETestBits>::BitToString( All ) == Default );

		TEST_Eq( BitEnumToString<ETestBits>::ToString( All ), "Bit0 | Bit1 | Bit2" );
	}


	static void  EnumToString_Test3 ()
	{
		enum class ETestBits
		{
			Bit0		= 1 << 0,
			Bit1		= 1 << 1,
			Bit2		= 1 << 2,
			_Last,
			All			= CT_AllBitMask2<ETestBits>,
			_BITOPS_	= 0,
			Bits12		= Bit0 | Bit1,
		};

		struct Converter : _EnumToStringConverter<ETestBits>
		{
			using _EnumToStringConverter<ETestBits>::Convert;
		};

		StaticAssert( Converter::Convert< ETestBits::Bits12 >() == "Bits12" );
		StaticAssert( Converter::Convert< ETestBits::All >() == "All" );
	}
}

namespace AE::Base
{
	enum class ETestEnum
	{
		T0,
		T1,
		T256 = 64,
		_Count
	};

	Nd__In StringView  ToString (ETestEnum value) __NE___
	{
		switch_enum( value )
		{
			case ETestEnum::T0 :		return "t0";
			case ETestEnum::T1 :		return "t1";
			case ETestEnum::T256 :		return "t256";
			case ETestEnum::_Count :	break;
		}
		switch_end
		return "";
	}
}

namespace
{
	static void  EnumToString_Test4 ()
	{
		StringView	s = ToString( ETestEnum::T256 );
		TEST( s == "t256" );
	}
}


extern void UnitTest_SourceLoc ()
{
	SourceLoc_Test1();
	SourceLoc_Test2();
	SourceLoc_Test3();
	SourceLoc_Test4();

	EnumToString_Test1();
	EnumToString_Test2();
	EnumToString_Test3();
	EnumToString_Test4();

	TEST_PASSED();
}
